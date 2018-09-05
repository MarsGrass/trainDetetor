/*
* =====================================================================================
*   
*   文 件 名：TCPClient.cpp
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

#include "TCPClient.h"
#include <QDateTime>
#include <QEventLoop>
#include <QTimer>


TCPClient::TCPClient(QObject * parent)
:QObject(parent)
{
	m_bServerConnected = false;  

    m_socket = nullptr;
	/*
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(ConnectServer())); 
	m_timer.setInterval(3000);  
	m_timer.start();  */
}

TCPClient::~TCPClient()
{
	//m_timer.stop();
	if (m_bServerConnected)
	{
		m_socket->disconnectFromHost();
		m_socket->waitForDisconnected(1000);
	}

	if (m_socket)
	{
		delete m_socket;
	}
}

void TCPClient::SetConfig(QString ip, QString port)
{
	m_ip = ip;
	m_port = port.toInt();
}

bool TCPClient::GetOpenStatus()
{
	return m_bServerConnected;
}

void TCPClient::OpenSlots()
{
	try
	{
        if (m_socket == nullptr)
		{
			m_socket = new QTcpSocket();
			m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, "1"); //设置保持连接状态 
			connect(m_socket, SIGNAL(connected()), this, SLOT(OnSocketConnected()));
			connect(m_socket, SIGNAL(disconnected()), this, SLOT(OnSocketDisconnected()));
			connect(m_socket, SIGNAL(readyRead()), this, SLOT(OnReceiveData()));
			connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnSocketError(QAbstractSocket::SocketError)));
		}

		if (!m_bServerConnected)
		{
            m_socket->connectToHost(m_ip, static_cast<quint16>(m_port));
			m_socket->waitForConnected(1000);
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

void TCPClient::CloseSlots()
{
	if (m_bServerConnected)
	{
		m_socket->disconnectFromHost();
		m_socket->waitForDisconnected(1000);
	}
}

void TCPClient::SendDataSlots(QByteArray data)
{
	if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
	{
		return;
	}
	
	try
	{
		m_socket->write(data);
	}
	catch(QString msg)
	{
		qWarning("%s %s:%d", msg.toUtf8().data(), m_ip.toLatin1().data(), m_port);
	}
	catch(...)
	{
		qCritical("%s", QObject::tr("error").arg(m_ip).arg(m_port).toUtf8().data());
	}
}

// 槽函数读发来的数据
void TCPClient::OnReceiveData()
{
	try
	{
		if (m_socket->bytesAvailable() > 0)
		{
            emit ReportStatusSignals(2);
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

void TCPClient::OnSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << m_socket->errorString() << error;
}

void TCPClient::OnSocketConnected()  
{  
	m_bServerConnected = true;
	emit ReportStatusSignals(1);
}  

void TCPClient::OnSocketDisconnected()  
{    
	m_bServerConnected = false; 
	emit ReportStatusSignals(0);
}   

QByteArray TCPClient::GetReadData()
{
    QByteArray byteArray;
    if(m_socket)
     byteArray = m_socket->readAll();

    return byteArray;
}

#include "moc_TCPClient.cpp"
