
#pragma once

#include <QObject>
#include <QVector>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

class IOServicePool : public QObject
{
    Q_OBJECT

public:
    IOServicePool(int nThreadSize = 4, QObject* qObj = 0);
    ~IOServicePool();


    // Run all io_service objects in the pool.
    void run();

    // Stop all io_service objects in the pool
    void stop();

    // Get an io_service to use
    boost::asio::io_service& get_io_service();


public:
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
    typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

    /// The pool of io_services.
    QVector<io_service_ptr> io_services_;

    /// The work that keeps the io_services running.
    QVector<work_ptr> work_;

    /// The next io_service to use for a connection.
    int next_io_service_;
};
