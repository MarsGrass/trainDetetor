
#include "IoServicePool.h"

IOServicePool::IOServicePool(int nThreadSize, QObject* qObj)
    : QObject(qObj)
{
    next_io_service_ = 0;

    for (int i = 0; i < nThreadSize; ++i)
    {
        io_service_ptr io_service(new boost::asio::io_service);
        work_ptr work(new boost::asio::io_service::work(*io_service));
        io_services_.push_back(io_service);
        work_.push_back(work);
    }
}

IOServicePool::~IOServicePool()
{

}

// Run all io_service objects in the pool.
void IOServicePool::run()
{
    // Create a pool of threads to run all of the io_services.
    QVector<boost::shared_ptr<boost::thread>> threads;
    for (std::size_t i = 0; i < io_services_.size(); ++i)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, io_services_[i])));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
}

// Stop all io_service objects in the pool
void IOServicePool::stop()
{
    // Explicitly stop all io_services.
    for (std::size_t i = 0; i < io_services_.size(); ++i)
        io_services_[i]->stop();
}

// Get an io_service to use
boost::asio::io_service& IOServicePool::get_io_service()
{
    // Use a round-robin scheme to choose the next io_service to use.
    boost::asio::io_service& io_service = *io_services_[next_io_service_];
    ++next_io_service_;
    if (next_io_service_ == io_services_.size())
        next_io_service_ = 0;
    return io_service;
}
