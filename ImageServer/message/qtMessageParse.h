
#pragma once

#include <QObject>

class qtMessage;

class QtMessageParse
{
public:
    QtMessageParse();
    virtual ~QtMessageParse();

    //得到定长头的长度
    virtual int GetConstHeaderLength(void) = 0;

    //得到包的长度
    virtual int GetPackLength(qtMessage* pMsg) = 0;

    //得到包体的长度
    virtual int GetBodyLength(qtMessage* pMsg) = 0;

    //virtual unsigned int GetMessgeType(qtMessage* pMsg) = 0;
};
