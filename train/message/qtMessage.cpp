
#include "qtMessage.h"
#include "qtMessagePool.h"

qtMessage::qtMessage(qtMessagePool* pMessagePool, QObject* qObj)
    : QObject(qObj)
{
    m_pMessagePool = pMessagePool;
    m_Image.create(1024, 1280, CV_8UC1);
}


qtMessage::~qtMessage()
{

}

void qtMessage::setSerial(const QString& strSerial)
{
    m_strSerial = strSerial;
}

const QString qtMessage:: getSerial()
{
    return m_strSerial;
}

void qtMessage::Release()
{
    m_pMessagePool->ReleaseQtMessage(this);

    m_strSerial = "";
}

