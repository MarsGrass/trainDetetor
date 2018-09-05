
#include "qtMessageWorkManage.h"

int qtMessageWorkManage::s_nIndex = 0;

qtMessageWorkManage::qtMessageWorkManage()
{
    m_pMsgQueue = NULL;
    m_nThreadSize = 0;
    m_bExitThread = false;
}

qtMessageWorkManage::~qtMessageWorkManage()
{
    this->Stop();
}


int qtMessageWorkManage::Start(qtMessageQueue* pMsgQueue, unsigned int nWorkerSize)
{
    m_pMsgQueue = pMsgQueue;
    if (m_pMsgQueue == NULL)
    {
        return -1;
    }

    if (nWorkerSize >= 10)
    {
        return -1;
    }

    m_nThreadSize = nWorkerSize;
    m_bExitThread = false;

    for(size_t i = 0; i < m_nThreadSize; i++)
    {
        qtMessageWork* pMessageWork = new qtMessageWork();
        pMessageWork->Start(this);
        m_listWorker.push_back(pMessageWork);
    }

    m_pMsgDispatchThread = new boost::thread(boost::bind(&qtMessageWorkManage::WorkLoop, this));

    return 0;
}


void qtMessageWorkManage:: Stop(void)
{
    m_bExitThread = true;
    if (m_pMsgQueue != NULL)
    {
        m_pMsgQueue->NotifyAll();
        m_pMsgQueue = NULL;
    }

    for(size_t i = 0; i < m_nThreadSize; i++)
    {
        qtMessageWork* pMessageWork = m_listWorker[i];
        pMessageWork->Stop();
        delete pMessageWork;
        pMessageWork = NULL;
    }
    m_listWorker.clear();
}


void qtMessageWorkManage::AddMessage(qtMessage* pMsg)
{
    if (NULL == pMsg || m_pMsgQueue == NULL)
    {
        return;
    }

    m_pMsgQueue->SubmitMessage(pMsg);
}

void qtMessageWorkManage::WorkLoop(void)
{
    while (!m_bExitThread)
    {
        //获取消息
        qtMessage* pMsg = m_pMsgQueue->GetMessage();

        if(pMsg == NULL)
            continue;

        //单个线程，直接处理，不需要派发
        if (m_nThreadSize == 1)
        {
            //由基类对消息进行处理
            if (Work(pMsg))
            {
                //释放消息块
                pMsg->Release();
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            //派发消息
            int i = (s_nIndex++) % m_nThreadSize;
            m_listWorker[i]->AddMessage(pMsg);
        }
    }

    //线程退出标记
    m_bExitThread = true;
}
