
#include "cameraFactory.h"
#include <QDebug>
#include "cameraBaslerGige.h"


bool CameraFactory::g_IsInitFactory = false;
QString CameraFactory::m_strCurrentCameraType = "Basler_GIGE";

QStringList CameraFactory::listTypes()
{
    QStringList types;
    types <<  "Basler_GIGE" << "Basler_USB";

    return types;
}

Camera* CameraFactory::CreateCamera(const QString& type, const QString& serial)
{
    Camera* pCamera = NULL;

    if(type == "Basler_GIGE")
    {
        pCamera = new CameraBaslerGige();
    }
    else if(type == "Basler_USB")
    {
        //pCamera = new CameraBaslerUsb();
    }
    else
    {

    }

    if(pCamera)
    {
        pCamera->setSerial(serial);

        int nRes = pCamera->open();
        if(nRes != 0)
        {
            delete pCamera;
            return NULL;
        }
    }

    return pCamera;
}

Camera* CameraFactory::CreateCamera(const QString& type, const QString& serial, ImageDataEvent* pDataEvent)
{
    Camera* pCamera = NULL;

    if(type == "Basler_GIGE")
    {
        pCamera = new CameraBaslerGige();
    }
    else if(type == "Basler_USB")
    {
        //pCamera = new CameraBaslerUsb();
    }
    else
    {

    }

    if(pCamera)
    {
        pCamera->setSerial(serial);
        pCamera->SetImageDataEvent(pDataEvent);

        int nRes = pCamera->open();
        if(nRes != 0)
        {
            delete pCamera;
            return NULL;
        }

    }

    return pCamera;
}

void CameraFactory::DestroyCamera(Camera* pCamera)
{
    if(pCamera)
    {
        pCamera->close();
        delete pCamera;
    }
}

void CameraFactory::DeleteCameraFacotry()
{
    if(g_IsInitFactory)
    {
        Pylon::PylonTerminate();
    }
}

QJsonArray CameraFactory::getCameraTypes()
{
    QJsonArray array;
    array.append("Basler_GIGE");
    array.append("Basler_USB");
    return array;
}

QJsonArray CameraFactory::getCameraSerials()
{
    QJsonArray array;

    if(g_IsInitFactory == false)
    {
        Pylon::PylonInitialize();
        g_IsInitFactory = true;
    }

    Pylon::CTlFactory &TlFactory = Pylon::CTlFactory::GetInstance();
    Pylon::DeviceInfoList_t lstDevices;
    Pylon::DeviceInfoList_t filter; // Filter for GigE cameras.
    if(m_strCurrentCameraType == "Basler_GIGE")
    {
        Pylon::CBaslerGigEDeviceInfo gige_devinfo;
        filter.push_back(gige_devinfo);
        TlFactory.EnumerateDevices(lstDevices, filter);
        for(int i = 0; i < lstDevices.size(); i++)
        {
            Pylon::IPylonDevice* device = TlFactory.CreateDevice(lstDevices[i]);
            QString serial = device->GetDeviceInfo().GetSerialNumber();
            array.append(serial);
        }
    }
    else if(m_strCurrentCameraType == "Basler_USB")
    {
//        Pylon::CBaslerUsbDeviceInfo usb_devinfo;
//        filter.push_back(usb_devinfo);
//        TlFactory.EnumerateDevices(lstDevices, filter);
//        for(int i = 0; i < lstDevices.size(); i++)
//        {
//            Pylon::IPylonDevice* device = TlFactory.CreateDevice(lstDevices[i]);
//            QString serial = device->GetDeviceInfo().GetSerialNumber();
//            array.append(serial);
//        }
    }

    return array;
}

void CameraFactory::SetType(const QString& strType)
{
    m_strCurrentCameraType = strType;
}


