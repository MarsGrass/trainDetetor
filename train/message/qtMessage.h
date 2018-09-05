

#pragma once

#include <QObject>
#include <QByteArray>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class qtMessagePool;

class qtMessage : public QObject
{
    Q_OBJECT

public:
    qtMessage(qtMessagePool* pMessagePool, QObject* qObj = 0);
    ~qtMessage();

    void setSerial(const QString& strSerial);

    const QString getSerial();

    void Release();


public:
    cv::Mat m_Image;
    QString m_strSerial;
    qtMessagePool* m_pMessagePool;
};
