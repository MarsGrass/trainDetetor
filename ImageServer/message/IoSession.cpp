
#include "IoSession.h"
#include "IoServer.h"
#include <QDebug>

int IOSession::m_sCount = 0;

IOSession::IOSession(boost::asio::io_service& work_service, boost::asio::io_service& io_service, QObject* qObj)
    :  QObject(qObj), socket_(io_service), io_work_service(work_service), recv_timeout_(io_service)
{
    pServer_ = NULL;

    m_nIndex = m_sCount++;

    m_pMessage = NULL;
}

IOSession::~IOSession()
{

}

void IOSession::handle_deadline()
{
    if(socket_.is_open() == false)
        return;

    if (recv_timeout_.expires_at() <= deadline_timer::traits_type::now())
    {// 真正的等待超时

        socket_.close();// 关闭对应的socket 包括连接超时/recv超时

        recv_timeout_.expires_at(boost::posix_time::pos_infin);// 定时器 设定为永不超时/不可用状态
    }

    // 如果不是真正的超时，定是其操作已成功/用户重新设置了定时器
    // 重新启动定时器
    recv_timeout_.async_wait(boost::bind(&IOSession::handle_deadline, this));
}

tcp::socket& IOSession::socket()
{
    return socket_;
}

void IOSession::SetServer(IOServer* pServer)
{
    pServer_ = pServer;
}


void IOSession::start()
{
    boost::system::error_code error;

    socket_.set_option(boost::asio::ip::tcp::no_delay(true));
    socket_.set_option(boost::asio::socket_base::keep_alive(true));


    //handle_write(error);

//    socket_.async_read_some(boost::asio::buffer(data_),
//        make_custom_alloc_handler(allocator_,
//        boost::bind(&session::handle_read,
//        shared_from_this(),
//        boost::asio::placeholders::error,
//        boost::asio::placeholders::bytes_transferred)));

    m_pMessage = pServer_->pMessagePool_->GetQtMessage();
    if (m_pMessage == NULL)
    {
        pServer_->ReleaseSession(this);
        return;
    }
    m_pMessage->setSession(this);
    //请求数据包
//    socket_.async_receive(boost::asio::buffer(pMessage->WritePosRef(), pMessageParse_->GetConstHeaderLength()),
//        boost::bind(&IOSession::handle_read_head, this,
//        pMessage, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));

     recv_timeout_.expires_from_now(boost::posix_time::seconds(60));

     boost::asio::async_read(socket_, boost::asio::buffer(m_pMessage->WritePosRef(), pServer_->pMessageParse_->GetConstHeaderLength()),
                             boost::bind(&IOSession::handle_read_head, this,
                             boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));

     recv_timeout_.async_wait(boost::bind(&IOSession::handle_deadline, this));

}

//读取消息头
void IOSession::handle_read_head(size_t nTransSize, const boost::system::error_code& error)
{
    if (!error)
    {
        m_pMessage->WritePos((int)nTransSize);
//        socket_.async_receive(boost::asio::buffer(pMsg->WritePosRef(), pMessageParse_->GetBodyLength(pMsg)),
//            boost::bind(&IOSession::handle_read_body, this,
//            pMsg, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));

        recv_timeout_.expires_from_now(boost::posix_time::seconds(60));

        boost::asio::async_read(socket_, boost::asio::buffer(m_pMessage->WritePosRef(), pServer_->pMessageParse_->GetBodyLength(m_pMessage)),
                                boost::bind(&IOSession::handle_read_body, this,
                                boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
    }
    else
    {    
        qDebug() << "socket close:" << error.value() << error.message().c_str();
        stop();
    }
}

//读取消息体
void IOSession::handle_read_body(size_t nTransSize, const boost::system::error_code& error)
{
    if (!error)
    {
        m_pMessage->WritePos((int)nTransSize);
        pServer_->pMessageQueue_->SubmitMessage(m_pMessage);

        m_pMessage = pServer_->pMessagePool_->GetQtMessage();
        m_pMessage->setSession(this);

        //请求数据包
//        socket_.async_receive(boost::asio::buffer(pMessage->WritePosRef(), pServer_->pMessageParse_->GetConstHeaderLength()),
//            boost::bind(&IOSession::handle_read_head, this,
//            pMessage, boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));

        recv_timeout_.expires_from_now(boost::posix_time::seconds(60));

        boost::asio::async_read(socket_, boost::asio::buffer(m_pMessage->WritePosRef(), pServer_->pMessageParse_->GetConstHeaderLength()),
                                boost::bind(&IOSession::handle_read_head, this,
                                boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error));
    }
    else
    { 
        qDebug() << "socket close:" << error.value() << error.message().c_str();
        stop();
    }
}

void IOSession::stop()
{    
    boost::system::error_code ignored_ec;
    socket_.close(ignored_ec);// 关闭socket

    if(m_pMessage)
    {
        m_pMessage->Release();
    }
    pServer_->ReleaseSession(this);

    recv_timeout_.cancel();// 取消定时器
}

void IOSession::asynWrite(qtMessage* pMsg)
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::asio::async_write(socket_, boost::asio::buffer(pMsg->m_data.data(), pMsg->m_nWritePos),
                              boost::asio::transfer_at_least(pMsg->m_nWritePos),
                              boost::bind(&IOSession::handle_write,
                                          this,  pMsg,
                                          boost::asio::placeholders::bytes_transferred,
                                          boost::asio::placeholders::error ));
}

void IOSession::handle_write(qtMessage* pMsg, size_t nTransSize, const boost::system::error_code& error)
{
    pMsg->Release();
    if (!error)
    {
    }
    else
    {
        qDebug() << "socket close:" << error.value() << error.message().c_str();
        stop();
    }
}


