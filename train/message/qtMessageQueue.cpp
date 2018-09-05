
#include "qtMessageQueue.h"
#include <QDebug>

qtMessageQueue::qtMessageQueue(QObject* qObj)
    : QObject(qObj)
{
    m_nSize = 2048;
    m_bExitWait = false;
}

qtMessageQueue::~qtMessageQueue()
{

}

//提交消息
int qtMessageQueue::SubmitMessage(qtMessage* pMessage)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (m_listMessage.size() > m_nSize)
    {
        qDebug() << "message queue size is too big";
        return -1;
    }
    m_listMessage.push_back(pMessage);
    m_workToBeDone.notify_all();

    return 0;
}

//获取消息
qtMessage* qtMessageQueue::GetMessage(void)
{
    boost::mutex::scoped_lock lock(m_mutex);

    while (m_listMessage.size() == 0 && !m_bExitWait)
        m_workToBeDone.wait(lock);

    if (m_bExitWait)
        return NULL;

    qtMessage* pMessage = m_listMessage.front();
    m_listMessage.pop_front();
    return pMessage;
}

//通知等待消息的线程
void qtMessageQueue::NotifyAll(void)
{
    m_bExitWait = true;
    m_workToBeDone.notify_all();
}
