
#include <QSettings>
#include <QCoreApplication>
#include <QFile>

#include "ifiServer.h"

ImageServiceWork::ImageServiceWork()
{

}

ImageServiceWork::~ImageServiceWork()
{

}

bool ImageServiceWork::Work(qtMessage* pMsg)
{
    return true;
}


CImageService::CImageService(int port)
    :server_(port)
{

}


CImageService::~CImageService(void)
{
}

int CImageService::Start()
{
    m_process.Start(&queue_);

    server_.init(&m_decode, &queue_, &pool_);
    server_.run();

    return 0;
}

int CImageService::Stop()
{
    return 0;
}

