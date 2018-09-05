
#include "qtMessage.h"
#include "qtMessagePool.h"
#include "IoSession.h"

qtMessage::qtMessage(qtMessagePool* pMessagePool, QObject* qObj)
    : QObject(qObj)
{
    m_nSize = 1024;
    m_data.resize(m_nSize);
    memset(m_data.data(), 0, 1024);
    m_nWritePos = 0;

    m_pSession = NULL;
    m_pMessagePool = pMessagePool;
}


qtMessage::~qtMessage()
{

}

void qtMessage::setSession(IOSession* pSession)
{
    m_pSession = pSession;
}

IOSession* qtMessage::getSession()
{
    return m_pSession;
}

void qtMessage::Release()
{
    m_pMessagePool->ReleaseQtMessage(this);

    memset(m_data.data(), 0, m_nSize);
    m_nWritePos = 0;
    m_pSession = NULL;
}

void qtMessage::WritePos(int nByte)
{
    m_nWritePos += nByte;
    if(m_nWritePos > m_nSize)
    {
        QByteArray add;
        add.resize(m_nWritePos - m_nSize + 1);
        m_data.append(add);
        m_nSize = m_nWritePos;
    }
}

char* qtMessage::WritePosRef()
{
    if(m_nWritePos > m_nSize)
    {
        return NULL;
    }
    return m_data.data() + m_nWritePos;
}
