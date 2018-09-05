
#include <QSettings>
#include <QCoreApplication>
#include <QFile>

#include "ifiServer.h"

CifiServer::CifiServer(QObject* parent)
    : QObject(parent)
{
    InitServer();
}

CifiServer::~CifiServer()
{
}

void CifiServer::InitServer()
{
    m_strServerIP = "0.0.0.0";
    m_strServerPort = "8064";

    pObjSocket = new TCPServer();
    m_pThread = new QThread();
    pObjSocket->moveToThread(m_pThread);

    connect(this, SIGNAL(OpenSignals()), pObjSocket, SLOT(OpenSlots()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(CloseSignals()), pObjSocket, SLOT(CloseSlots()));
    connect(this, SIGNAL(SendDataSignals(QByteArray, int)), pObjSocket, SLOT(SendDataSlots(QByteArray, int)));
    connect(pObjSocket, SIGNAL(ReportStatusSignals(int, int)), this, SLOT(ReportStatusSlots(int, int)));

    m_pThread->start();
    pObjSocket->SetConfig(m_strServerIP, m_strServerPort);
    emit OpenSignals();
}


void CifiServer::ReportStatusSlots(int nCmdId, int nSocketId)
{
    if(nCmdId == 0)
    {
        CallbackSignals(nCmdId, nSocketId, "");
    }
    else if(nCmdId == 1)
    {
        CallbackSignals(nCmdId, nSocketId, "");
    }
    else
    {
        m_strRecvData = pObjSocket->m_listConnect[nSocketId]->getData();
        CallbackSignals(nCmdId, nSocketId, m_strRecvData);
    }
}
