
#pragma once

#include <QObject>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "qtMessageParse.h"
#include "qtMessageQueue.h"
#include "qtMessagePool.h"

#include <QTime>

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;

class IOServer;

class IOSession : public QObject, public boost::enable_shared_from_this<IOSession>
{
    Q_OBJECT

public:
    IOSession(boost::asio::io_service& work_service, boost::asio::io_service& io_service, QObject* qObj = 0);
    ~IOSession();

    void SetServer(IOServer* pServer);

    tcp::socket& socket();

    void asynWrite(qtMessage* pMsg);

    void start();

    void stop();

    //读取消息头
    void handle_read_head(size_t nTransSize, const boost::system::error_code& error);

    //读取消息体
    void handle_read_body(size_t nTransSize, const boost::system::error_code& error);


    void handle_write(qtMessage* pMsg, size_t nTransSize, const boost::system::error_code& error);


    void handle_deadline();


private:
    // The io_service used to finish the work.
    boost::asio::io_service& io_work_service;

    // The socket used to communicate with the client.
    tcp::socket socket_;
    deadline_timer recv_timeout_;

    // Buffer used to store data received from the client.
    //boost::array<char, 1024> data_;

    // The allocator to use for handler-based custom memory allocation.
    //handler_allocator allocator_;

    boost::mutex        m_mutex;

    qtMessage* m_pMessage;

public:
    int m_nIndex;
    static int m_sCount;

    IOServer* pServer_;
};
