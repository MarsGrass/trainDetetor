
#pragma once

#include <QObject>

#include "IoServicePool.h"
#include "IoSession.h"

typedef boost::shared_ptr<IOSession> session_ptr;

class IOServer : public QObject
{
    Q_OBJECT

public:
    IOServer(short port, int io_service_pool_size = 4, QObject* qObj = 0);

    ~IOServer();

    void init(QtMessageParse* pMessageParse, qtMessageQueue* pMessageQueue, qtMessagePool* pMessagePool);

    //void handle_accept(session_ptr new_session, const boost::system::error_code& error);

    void handle_accept(IOSession* new_session, const boost::system::error_code& error);

    void run();

    void stop();

    IOSession* GetSession();
    void ReleaseSession(IOSession* session);

    void AsyncWrite(qtMessage* message);



private:
//    session_ptr GetSession();
//    void ReleaseSession(session_ptr session);


public:
    boost::shared_ptr<boost::thread> io_thread_;
    boost::shared_ptr<boost::thread> work_thread_;
    IOServicePool io_service_pool_;
    IOServicePool io_service_work_pool_;
    tcp::acceptor acceptor_;

    //QList<session_ptr> listSession;
    QList<IOSession*> listSession;

    QtMessageParse* pMessageParse_;
    qtMessageQueue* pMessageQueue_;
    qtMessagePool*  pMessagePool_;

    boost::mutex m_mutex;

    int m_nSessionSize;
    int m_nPort;
};
