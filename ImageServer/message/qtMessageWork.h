
#pragma once

#include "qtMessageQueue.h"

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

class qtMessageWorkManage;

class qtMessageWork : public QObject
{
    Q_OBJECT

public:
    qtMessageWork(QObject* qObj = 0);
    ~qtMessageWork();

    void    Start(qtMessageWorkManage *pMessageWorker);
    void    Stop(void);

    //添加消息块
    void    AddMessage(qtMessage* pMsg);

    //线程循环
    void    WorkLoop(void);

protected:
    qtMessageQueue   m_MsgQueue;
    boost::thread*   m_pThread;
    qtMessageWorkManage*  m_pMessageWorker;
    volatile bool    m_bExitThread;
};
