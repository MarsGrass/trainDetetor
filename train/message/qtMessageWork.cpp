
#include "qtMessageWork.h"
#include "qtMessageWorkManage.h"

qtMessageWork::qtMessageWork(QObject* qObj)
    : QObject(qObj)
{
    m_pMessageWorker = NULL;
    m_pThread = NULL;
    m_bExitThread = false;
}

qtMessageWork::~qtMessageWork()
{
    this->Stop();
}

void qtMessageWork::Start(qtMessageWorkManage *pMessageWorker)
{
    m_pMessageWorker = pMessageWorker;
    m_pThread = new boost::thread(boost::bind(&qtMessageWork::WorkLoop, this));
}

void qtMessageWork::Stop(void)
{
    m_bExitThread = true;
    m_pThread->join();
    m_pMessageWorker = NULL;
    m_pThread = NULL;
    m_bExitThread = false;
}

//添加消息块
void qtMessageWork::AddMessage(qtMessage* pMsg)
{
    m_MsgQueue.SubmitMessage(pMsg);
}

//线程循环
void qtMessageWork::WorkLoop(void)
{
    while (!m_bExitThread)
    {
        //获取消息
        qtMessage* pMsg = m_MsgQueue.GetMessage();
        if(pMsg == NULL)
            continue;

        //由基类对消息进行处理
        m_pMessageWorker->Work(pMsg);

        //释放消息块
        pMsg->Release();
    }

    //线程退出标记
    m_bExitThread = true;
}
