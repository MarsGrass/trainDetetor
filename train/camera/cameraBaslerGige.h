
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <QString>
#include <QStringList>
#include "camera.h"

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/cameralink/BaslerCameraLinkInstantCamera.h>

using namespace Pylon;

class CImageEventPrinter : public CImageEventHandler
{
public:

    virtual void OnImagesSkipped( CInstantCamera& camera, size_t countOfSkippedImages);

    virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);  

    void SetUserEvent(ImageDataEvent* pDataEvent);

public:
    ImageDataEvent* m_pDataEvent;
};

class CConfigurationEventPrinter : public CConfigurationEventHandler
{
public:
    void OnAttach( CInstantCamera& /*camera*/)
    {
        qDebug() << "OnAttach event" ;
    }

    void OnAttached( CInstantCamera& camera)
    {
        qDebug() << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnOpen( CInstantCamera& camera)
    {
        qDebug() << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnOpened( CInstantCamera& camera)
    {
        qDebug() << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnGrabStart( CInstantCamera& camera)
    {
        qDebug() << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnGrabStarted( CInstantCamera& camera)
    {
        qDebug() << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnGrabStop( CInstantCamera& camera)
    {
        qDebug() << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnGrabStopped( CInstantCamera& camera)
    {
        qDebug() << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnClose( CInstantCamera& camera)
    {
        qDebug() << "OnClose event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnClosed( CInstantCamera& camera)
    {
        qDebug() << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnDestroy( CInstantCamera& camera)
    {
        qDebug() << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnDestroyed( CInstantCamera& /*camera*/)
    {
        qDebug() << "OnDestroyed event" ;
    }

    void OnDetach( CInstantCamera& camera)
    {
        qDebug() << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnDetached( CInstantCamera& camera)
    {
        qDebug() << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName() ;
    }

    void OnGrabError( CInstantCamera& camera, const String_t errorMessage)
    {
        qDebug() << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName() ;
        qDebug() << "Error Message: " << errorMessage  ;
    }

    void OnCameraDeviceRemoved( CInstantCamera& camera)
    {
        qDebug() << "OnCameraDeviceRemoved event for device " << camera.GetDeviceInfo().GetModelName() ;
    }
};


class CameraBaslerGige : public Camera
{
public:
    CameraBaslerGige();
    virtual ~CameraBaslerGige();

    virtual void acquire(cv::Mat& I);

    virtual int open();

    virtual void startSnap();

    virtual void stopSnap();

    virtual void close();

    virtual float getBlackLevel();

    virtual QString getCameraInfo();

    virtual float getExposure();

    virtual float getFrameRate();

    virtual float getGain();

    virtual float getGamma();

    virtual float setBlackLevel(float blacklevel_value = 0.0);

    virtual bool setAutoExposure(bool isAutoExposure = true);

    virtual float setExposure(float exposure_value = 0.0);

    virtual float setMaxExposure(float maxExposure = 0.0);

    virtual float setMinExposure(float minExposure = 0.0);

    virtual float setGain(float gain_value = 0.0);

    virtual float setFrameRate(float frame_rate = 20);

    virtual float setGamma(float gamma_value = 1.0);

    virtual unsigned int getHeight() const;

    virtual unsigned int getWidth() const;

public:
    Pylon::CBaslerGigEInstantCamera m_Camera;

    CImageEventPrinter* m_pImageEvent;
};
