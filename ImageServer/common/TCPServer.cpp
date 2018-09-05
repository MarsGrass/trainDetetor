/*
* =====================================================================================
*   
*   文 件 名：TCPServer.cpp
*
*   描    述：定义
*
*   创建时间：
*
*   更新历史：
*
*   版权所有：
* 
* =====================================================================================
*/

#include "TCPServer.h"

NetConnect::NetConnect()
{
	m_nSocketId = -1;
    m_pSocket = nullptr;
    m_pServer = nullptr;
}

NetConnect::~NetConnect()
{
    if (m_pSocket != nullptr)
	{
		delete m_pSocket;
	}
}

void NetConnect::InitConnect(QTcpSocket *pSocket, int nSocketId, TCPServer *pServer)
{
	m_pSocket = pSocket;
	if (m_pSocket)
	{
		m_pSocket->setParent(this);
		m_pSocket->setSocketOption(QAbstractSocket::KeepAliveOption, "1"); //设置保持连接状态 
	}
	m_nSocketId = nSocketId;
	m_pServer = pServer;

}

void NetConnect::DeleteConnect()
{
	m_pSocket->disconnect();
    m_pSocket = nullptr;
	m_nSocketId = -1;
    m_pServer = nullptr;
}

void NetConnect::OnReadReady()
{
	try
	{
        if (m_pSocket->bytesAvailable() > 0)
		{
            m_pServer->ReportStatusSignals(2, m_nSocketId);
		}
	}
	catch (QString msg)
	{
		qWarning("%s,%d,%s", __FILE__, __LINE__, msg.toUtf8().data());
	}
	catch (...)
	{
		qCritical("%s,%d,%s", __FILE__, __LINE__, QObject::tr("error").toUtf8().data());
	}
}

 QByteArray NetConnect::getData()
 {
     return m_pSocket->readAll();
 }

bool NetConnect::sendData(QByteArray &data)
{
	if (data.isEmpty())
	{
		return false;
	}

	try
	{
		if (m_pSocket)
		{
			m_pSocket->write(data.data());
			return true;
		}
	}
	catch (QString msg)
	{
		qWarning("%s", msg.toUtf8().data());
	}
	catch (...)
	{
		qCritical("%s", QObject::tr("error").toUtf8().data());
	}
	return false;
}

TCPServer::TCPServer(QObject * parent, int nMaxConnect)
: QObject(parent)
{
	m_nNumId = 1;
	m_nMaxConnect = nMaxConnect;
    m_bServerListening = false;
    m_pServer = nullptr;
}

TCPServer::~TCPServer(void)
{
    foreach (NetConnect* s, m_listConnect)
    {
        if(s != nullptr)
        {
            delete s;
        }
    }
    m_listConnect.clear();

    if (m_pServer != nullptr)
	{
		delete m_pServer;
	}
}

void TCPServer::SetConfig(QString ip, QString port)
{
	m_ip = ip;
	m_port = port.toInt();
}

bool TCPServer::GetOpenStatus()
{
    return m_bServerListening;
}

void TCPServer::OpenSlots()
{
	try
	{
        if (m_bServerListening)
		{
			return;
		}

        if(!m_pServer)
        {
            m_pServer = new QTcpServer();

            for(int i = 0; i < m_nMaxConnect; i++)
            {
                NetConnect* pConnect = new NetConnect();
                m_listConnect.push_back(pConnect);
            }
            connect(m_pServer, SIGNAL(newConnection()), this, SLOT(OnNewConnect()));
        }

        if (m_pServer->listen(QHostAddress(m_ip), static_cast<quint16>(m_port)))
		{
			qDebug("%s", QObject::tr("listen %1:%2 success").arg(m_ip).arg(m_port).toLatin1().data());
            m_bServerListening = true;
		}
		else
		{
			qDebug("%s", QObject::tr("listen %1:%2 failed").arg(m_ip).arg(m_port).toLatin1().data());
		}
	}
	catch (QString msg)
	{
		qWarning("%s", msg.toLatin1().data());
	}
	catch (...)
	{
		qCritical("%s", QObject::tr("connect%1:%2error").arg(m_ip).arg(m_port).toLatin1().data());
	}
}

void TCPServer::CloseSlots()
{
	if (m_pServer)
	{
		m_pServer->close();
        m_bServerListening = false;
	}
}

void TCPServer::OnNewConnect()
{
	int i;
	for (i = 0; i < m_nMaxConnect; i++)
	{
        if (m_listConnect[i]->m_nSocketId == -1)
		{
			break;
		}
	}

	if (i >= m_nMaxConnect)
	{
		return;
	}

	m_ConnectMutex.lock();
    m_listConnect[i]->InitConnect(m_pServer->nextPendingConnection(), i, this);
    connect(m_listConnect[i]->m_pSocket, SIGNAL(readyRead()), m_listConnect[i], SLOT(OnReadReady()));
    connect(m_listConnect[i]->m_pSocket, SIGNAL(disconnected()), this, SLOT(OnDisConnect()));
	m_ConnectMutex.unlock();

    emit ReportStatusSignals(1, i);

	if (m_nNumId == m_nMaxConnect)
	{
		m_pServer->close();
	}

	m_nNumId++;
}

void TCPServer::OnDisConnect()
{
	QTcpSocket* pSocket = qobject_cast<QTcpSocket*>(sender());
    if (pSocket == nullptr)
		return;

	NetConnect* pNetConnect = qobject_cast<NetConnect*>(pSocket->parent());
    if (pNetConnect == nullptr)
		return;

	m_ConnectMutex.lock();
	pNetConnect->DeleteConnect();
	m_ConnectMutex.unlock();

    int nIndex = m_listConnect.indexOf(pNetConnect);
    if(nIndex >= 0)
    {
        emit ReportStatusSignals(0, nIndex);
    }

	m_nNumId--;

	if (m_nNumId == m_nMaxConnect)
	{
        m_pServer->listen(QHostAddress(m_ip), static_cast<quint16>(m_port));
	}
}

void TCPServer::SendDataSlots(QByteArray data, int nIndexClient)
{
    if(m_listConnect[nIndexClient]->m_nSocketId >= 0)
    {
        m_listConnect[nIndexClient]->sendData(data);
    }
    else
    {
        qWarning() << "socket send error";
    }
}

QStringList TCPServer::GetClientList()
{
    QStringList list;

    foreach (NetConnect* pConnect, m_listConnect)
    {
        if(pConnect->m_nSocketId >= 0)
        {
            QString strClientInfo;
            strClientInfo = pConnect->m_pSocket->peerAddress().toString();
            strClientInfo.append(":");
            strClientInfo += QString::number(pConnect->m_pSocket->peerPort());
            list.append(strClientInfo);
        }
    }

    return list;
}

int TCPServer::GetIndexByStringName(QString strName)
{
    for(int i = 0; i < m_nNumId; i++)
    {
        if (m_listConnect[i]->m_nSocketId >= 0)
        {
            QString strClientInfo;
            strClientInfo = m_listConnect[i]->m_pSocket->peerAddress().toString();
            strClientInfo.append(":");
            strClientInfo += QString::number(m_listConnect[i]->m_pSocket->peerPort());
            if(strName == strClientInfo)
            {
                return i;
            }
        }
    }
    return -1;
}

#include "moc_TCPServer.cpp"
