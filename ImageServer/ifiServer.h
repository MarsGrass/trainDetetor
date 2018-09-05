
#pragma once

#include <QObject>
#include <QThread>
#include "Common/TCPServer.h"

class CifiServer : public QObject
{
    Q_OBJECT

public:
    CifiServer(QObject* parent = 0);
    ~CifiServer();

    void InitServer();

signals:
    void OpenSignals();
    void CloseSignals();
    void SendDataSignals(QByteArray, int);

    void CallbackSignals(int, int, const QString&);

public slots:
    void ReportStatusSlots(int nCmdId, int nSocketId);


private:
    TCPServer* pObjSocket;
    QThread* m_pThread;

    QString m_strName;
    QString m_strServerIP;
    QString m_strServerPort;
    QString m_strFilePath;

    QByteArray m_strRecvData;

};
