
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <QString>
#include <QStringList>

#include "message/qtMessagePool.h"
#include "message/qtMessageQueue.h"
#include "message/qtMessageWorkManage.h"

class ImageDataEvent{
  public:
    virtual void ProcessCvMat(const cv::Mat& mat) = 0;
};

class Camera
{
public:
    Camera();
    virtual ~Camera();

    virtual int open() = 0;

    virtual void close() = 0;

    virtual void acquire(cv::Mat& I) = 0;

    virtual void startSnap() = 0;

    virtual void stopSnap() = 0;

    virtual float getBlackLevel() = 0;

    virtual QString getCameraInfo() = 0;

    virtual float getExposure() = 0;

    virtual float getFrameRate() = 0;

    virtual float getGain() = 0;

    virtual float getGamma() = 0;

    virtual float setBlackLevel(float blacklevel_value = 0.0) = 0;

    virtual bool setAutoExposure(bool isAutoExposure = true) = 0;

    virtual float setExposure(float exposure_value = 0.0) = 0;

    virtual float setMaxExposure(float maxExposure = 0.0) = 0;

    virtual float setMinExposure(float minExposure = 0.0) = 0;

    virtual float setGain(float gain_value = 0.0) = 0;

    virtual float setFrameRate(float frame_rate = 20) = 0;

    virtual float setGamma(float gamma_value = 1.0) = 0;

    virtual unsigned int getHeight() const = 0;

    virtual unsigned int getWidth() const = 0;

    void SetImageDataEvent(ImageDataEvent* pDataEvent);

    QString getType();

    void setType(const QString& type);

    QString getSerial();

    void setSerial(const QString& serial);


private:
    QString m_type; //!< Type of the camera.
    QString m_serial; //!< Serial of the camera.

protected:
    bool m_IsOpen;
    ImageDataEvent* m_pDataEvent;
};
