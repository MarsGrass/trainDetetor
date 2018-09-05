

#pragma once

#include <QObject>
#include <QByteArray>

class IOSession;
class qtMessagePool;

class qtMessage : public QObject
{
    Q_OBJECT

public:
    qtMessage(qtMessagePool* pMessagePool, QObject* qObj = 0);
    ~qtMessage();

    void setSession(IOSession* pSession);

    IOSession* getSession();

    void Release();

    void WritePos(int nByte);

    char* WritePosRef();

public:
    QByteArray m_data;
    int m_nSize;
    int m_nWritePos;

    IOSession* m_pSession;
    qtMessagePool* m_pMessagePool;
};
