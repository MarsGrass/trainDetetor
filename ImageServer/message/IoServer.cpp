
#include "IoServer.h"
#include <QDebug>

IOServer::IOServer(short port, int io_service_pool_size, QObject* qObj)
    : QObject(qObj), io_service_pool_(io_service_pool_size)
    , io_service_work_pool_(io_service_pool_size)
    , acceptor_(io_service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
    , m_nPort(port)
{
    pMessageParse_ = NULL;
    pMessageQueue_ = NULL;
    pMessagePool_  = NULL;

    m_nSessionSize = 20;

    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));


    for(int i = 0; i < m_nSessionSize; i++)
    {
        //session_ptr new_session(new IOSession(io_service_work_pool_.get_io_service(), io_service_pool_.get_io_service()));
        IOSession* session = new IOSession(io_service_work_pool_.get_io_service(), io_service_pool_.get_io_service());
        listSession.push_back(session);
    }

}

IOServer::~IOServer()
{

}

//session_ptr IOServer::GetSession()
//{
//    qDebug() << "sesssion pool size: " << listSession.size() << "--";
//    session_ptr session = listSession.front();
//    listSession.pop_front();
//    return session;
//}

//void IOServer::ReleaseSession(session_ptr session)
//{
//    qDebug() << "sesssion pool size: " << listSession.size() << "++";
//    listSession.push_back(session);
//}

IOSession* IOServer::GetSession()
{
    if(listSession.size() == 0)
    {

        IOSession* session = new IOSession(io_service_work_pool_.get_io_service(), io_service_pool_.get_io_service());
        session->SetServer(this);

        qDebug() << "create a new sesssion index:" << session->m_nIndex;
        return session;
    }
    qDebug() << "sesssion pool size: " << listSession.size() << "--";
    IOSession* session = listSession.front();
    qDebug() << session->m_nIndex;
    listSession.pop_front();

    return session;
}

void IOServer::ReleaseSession(IOSession* session)
{

//        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_nPort);
//        acceptor_.open(endpoint.protocol());
//        acceptor_.bind(endpoint);
//        acceptor_.listen();

    qDebug() << "sesssion pool size: " << listSession.size() << "++";
    qDebug() << session->m_nIndex;
    session->socket().close();
    listSession.push_back(session);
}


void IOServer::init(QtMessageParse* pMessageParse, qtMessageQueue* pMessageQueue, qtMessagePool* pMessagePool)
{
    pMessageParse_ = pMessageParse;
    pMessageQueue_ = pMessageQueue;
    pMessagePool_ = pMessagePool;

    for(int i = 0; i < m_nSessionSize; i++)
    {
        listSession[i]->SetServer(this);
    }

    //session_ptr new_session = this->GetSession();
    IOSession* session = this->GetSession();
    acceptor_.async_accept(session->socket(),
        boost::bind(&IOServer::handle_accept, this, session,
        boost::asio::placeholders::error));
}

void IOServer::handle_accept(IOSession* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        //new_session.reset(new IOSession(io_service_work_pool_.get_io_service(), io_service_pool_.get_io_service()));
        new_session = this->GetSession();
        qDebug() << "getsession id: " << new_session->m_nIndex;
        if(new_session)
        {
            acceptor_.async_accept(new_session->socket(),
                            boost::bind(&IOServer::handle_accept, this, new_session,
                            boost::asio::placeholders::error));
        }
        else
        {
            //acceptor_.close();
            qDebug() << "handle_accept session is is NULL";
        }
    }
    else
    {
        qDebug() << "accept error";
        qDebug() << error.value() << " msg :" << error.message().c_str();
        acceptor_.close();

       // acceptor_.open();
    }
}

//void IOServer::handle_accept(session_ptr new_session, const boost::system::error_code& error)
//{
//    if (!error)
//    {
//        new_session->start();
//        //new_session.reset(new IOSession(io_service_work_pool_.get_io_service(), io_service_pool_.get_io_service()));
//        new_session = this->GetSession();
//        acceptor_.async_accept(new_session->socket(),
//                        boost::bind(&IOServer::handle_accept, this, new_session,
//                        boost::asio::placeholders::error));
//    }
//    else
//    {
//        //ReleaseSession(new_session);
//    }
//}

void IOServer::AsyncWrite(qtMessage* message)
{
    IOSession* pSession = message->getSession();
    if(pSession == NULL)
    {
        qDebug() << "Session is NULL";
        return;
    }

    if(pSession->socket().is_open() == false)
    {
        qDebug() << "Session socket is closed";
        return;
    }
}

void IOServer::run()
{
    io_thread_.reset(new boost::thread(boost::bind(&IOServicePool::run
        , &io_service_pool_)));
    work_thread_.reset(new boost::thread(boost::bind(&IOServicePool::run
        , &io_service_work_pool_)));
}

void IOServer::stop()
{
    io_service_pool_.stop();
    io_service_work_pool_.stop();

    io_thread_->join();
    work_thread_->join();
}

