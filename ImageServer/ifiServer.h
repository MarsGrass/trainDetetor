
#pragma once

#include <QObject>
#include <QThread>
#include "Common/TCPServer.h"

#include "message/IoServer.h"
#include "message/qtMessageWorkManage.h"

#include "algorithm/platerecognition.h"

#ifdef MATCH_TOOL
#include <opencv2/dnn.hpp>
#endif

using namespace boost::asio;

#ifdef MATCH_TOOL
using namespace cv::dnn;
#endif

class ImageServiceMsgDecode : public QtMessageParse
{
public:
    ImageServiceMsgDecode(){}
    virtual ~ImageServiceMsgDecode(){}

    //得到定长头的长度
    virtual int GetConstHeaderLength(void){return 8;}

    //得到包的长度
    virtual int GetPackLength(qtMessage* pMsg)
    {
        QString strData = pMsg->m_data;

        int nPackLength = strData.left(8).toInt();
        return nPackLength+8;
    }

    //得到包体的长度
    virtual int GetBodyLength(qtMessage* pMsg)
    {
        QString strData = pMsg->m_data;

        int nPackLength = strData.left(8).toInt();
        return nPackLength;
    }
};

class ImageServiceWork : public qtMessageWorkManage
{
public:
    ImageServiceWork();
    virtual ~ImageServiceWork();

    virtual bool Work(qtMessage* pMsg);


    bool FindPlate(const QString& strPath, QString& strPlate);

    bool matchTool(const QString& strPath, QString& strName);

public:
#ifdef MATCH_TOOL
    dnn::Net net;
    vector<string> classNamesVec;
#endif

};

class CImageService
{
public:
    CImageService(int port = 8064);
    ~CImageService(void);

public:
    //启动服务
    int Start();

    //停止服务
    int Stop();

public:
    //处理函数
    ImageServiceWork m_process;
    //解析函数
    ImageServiceMsgDecode m_decode;

    qtMessageQueue queue_;

    qtMessagePool pool_;

    IOServer server_;
};

