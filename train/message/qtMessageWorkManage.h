
#pragma once

#include "qtMessageWork.h"

class qtMessageWorkManage
{
public:
    qtMessageWorkManage();
    virtual ~qtMessageWorkManage();

public:
    //线程开始
    int     Start(qtMessageQueue* pMsgQueue, unsigned int nWorkerSize = 10);

    //线程结束
    void    Stop(void);

    //添加消息块
    void    AddMessage(qtMessage* pMsg);

public:
    //派生类需要重载此虚函数,以完成工作.
    virtual bool    Work(qtMessage* pMsg) = 0;

protected:
    //线程循环
    void    WorkLoop(void);

protected:
    qtMessageQueue*   m_pMsgQueue;
    QList<qtMessageWork*>  m_listWorker;
    unsigned int           m_nThreadSize;
    volatile bool   m_bExitThread;
    boost::thread*  m_pMsgDispatchThread;

    static int s_nIndex;
};
