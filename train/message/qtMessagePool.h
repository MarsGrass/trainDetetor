
#pragma once

#include <QList>

#include <boost/thread/mutex.hpp>

#include "qtMessage.h"


class qtMessagePool : public QObject
{
    Q_OBJECT

public:
    qtMessagePool(int nCount = 1024, QObject* qObj = 0);
    ~qtMessagePool();

    qtMessage* GetQtMessage();

    void ReleaseQtMessage(qtMessage* pMessage);


public:
    QList<qtMessage*> m_listMessage;

    boost::mutex        m_mutex;
};

