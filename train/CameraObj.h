

#pragma once

#include <QObject>
#include "Camera/camera.h"
#include <QThread>
#include <QDateTime>
#include "common/TCPClient.h"

#include "message/qtMessagePool.h"
#include "message/qtMessageQueue.h"

class CCameraObj;

class TrainMessageManage : public qtMessageWorkManage
{
public:
    TrainMessageManage(CCameraObj* obj);
    virtual ~TrainMessageManage();

    virtual bool Work(qtMessage* pMsg);

protected:
    bool DoMessage(qtMessage* pMsg);

public:
    CCameraObj* m_obj;
};

class CUserDataEvent : public ImageDataEvent
{
public:
    virtual void ProcessCvMat(const cv::Mat& mat);

    void SetCameraObj(CCameraObj* obj);

public:
    CCameraObj* m_obj;
};

class CCameraObj : public QObject
{
    Q_OBJECT

public:
    CCameraObj(QObject* parent = 0);
    ~CCameraObj();

    void Init(); 

    int Create();

    void StartSnap();

    void StopSnap();

    int Capture();

    void SetAutoExposure(bool isAutoExposure);
    void SetExposure(double fExposure);
    void SetMaxExposure(double maxAutoExposure);
    void SetMinExposure(double minAutoExposure);

    void SetBlackLevel(double fBlackLevel);
    void SetGain(double fGain);
    void SetFrameRate(double fFrameRate);
    void SetGamma(double fGamma);

signals:
    void OpenSignals();
    void CloseSignals();
    void SendDataSignals(QByteArray data);

public slots:
    void ReportStatusSlots(int nCmd);

public:
    Camera* _camera;

    QString m_strInfo;
    QString m_strSerial;
    QString m_strType;
    double m_fExposure;
    double m_fBlackLevel;
    double m_fGain;
    double m_fFrameRate;
    double m_fGamma;

    bool m_IsOpen;
    bool m_IsInit;

    bool m_IsSocketConnect;

    cv::Mat m_CaptureImage;

    CUserDataEvent* pDataEvent_;

    qtMessageQueue* pQueue_;
    qtMessagePool* pPool_;
    TrainMessageManage* pProcess_;

    TCPClient* tcpClient_;
};

