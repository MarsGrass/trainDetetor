
#pragma once

#include "qtMessage.h"

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

class qtMessageQueue : public QObject
{
    Q_OBJECT

public:
    qtMessageQueue(QObject* qObj = 0);
    ~qtMessageQueue();

    //提交消息
    int SubmitMessage(qtMessage* pMessage);

    //获取消息
    qtMessage* GetMessage(void);

    //通知等待消息的线程
    void NotifyAll(void);

private:
    bool                m_bExitWait;
    int                 m_nSize;
    QList<qtMessage*>   m_listMessage;
    boost::mutex        m_mutex;
    boost::condition    m_workToBeDone;
};
