#include <QDebug>
#include <QDataStream>

#include "cameraBaslerGige.h"

#include <QTime>
#include <QMutex>
#include <QDir>
#include <QEventLoop>
#include <QTimer>


void CImageEventPrinter::SetUserEvent(ImageDataEvent* pDataEvent)
{
    m_pDataEvent = pDataEvent;
}

void CImageEventPrinter::OnImagesSkipped( CInstantCamera& camera, size_t countOfSkippedImages)
{
    qDebug() << "OnImagesSkipped event for device " << camera.GetDeviceInfo().GetSerialNumber().c_str();
    qDebug() << countOfSkippedImages  << " images have been skipped.";
}

void CImageEventPrinter::OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
    //qDebug() << "OnImageGrabbed event for device " << camera.GetDeviceInfo().GetSerialNumber() ;

    if (ptrGrabResult->GrabSucceeded())
    {
        //CPylonImage image;
        //image.AttachGrabResultBuffer(ptrGrabResult);
        //image.Save(EImageFileFormat::ImageFileFormat_Bmp, strPth.toLatin1().data());
        cv::Mat imageMat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)ptrGrabResult->GetBuffer());

        if(m_pDataEvent)
        {
            m_pDataEvent->ProcessCvMat(imageMat);
        }
    }
    else
    {
        qDebug() << camera.GetDeviceInfo().GetSerialNumber().c_str();
        qDebug() << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription();
    }
}

CameraBaslerGige::CameraBaslerGige()
{
    m_IsOpen = false;
    m_pImageEvent = new CImageEventPrinter();
}

CameraBaslerGige::~CameraBaslerGige()
{
    if(m_Camera.IsOpen() == true)
    {
       m_Camera.Close();
    }
}

int CameraBaslerGige::open()
{
    if(m_IsOpen)
    {
        return 0;
    }

    try
    {
        Pylon::CTlFactory &TlFactory = Pylon::CTlFactory::GetInstance();
        Pylon::DeviceInfoList_t lstDevices;
        Pylon::DeviceInfoList_t filter; // Filter for GigE cameras.
        Pylon::CBaslerGigEDeviceInfo gige_devinfo;
        filter.push_back(gige_devinfo);
        TlFactory.EnumerateDevices(lstDevices, filter);

        for(int i = 0; i < lstDevices.size(); i++)
        {
            Pylon::IPylonDevice* device = TlFactory.CreateDevice(lstDevices[i]);
            QString serial = device->GetDeviceInfo().GetSerialNumber();

            //TlFactory.CreateDevice(lstDevices[i])->GetDeviceInfo().GetSerialNumber().c_str();

            if(getSerial() != serial)
            {
                continue;
            }
            else
            {
                m_Camera.Attach(device);
                if(m_Camera.IsOpen() == false)
                {

                    //m_Camera.RegisterConfiguration( new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

                    // For demonstration purposes only, add a sample configuration event handler to print out information
                    // about camera use.
                    m_Camera.RegisterConfiguration( new CConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);

                    // The image event printer serves as sample image processing.
                    // When using the grab loop thread provided by the Instant Camera object, an image event handler processing the grab
                    // results must be created and registered.
                    m_Camera.RegisterImageEventHandler( m_pImageEvent, RegistrationMode_Append, Cleanup_Delete);

                    // For demonstration purposes only, register another image event handler.
                    //m_Camera.RegisterImageEventHandler( new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);

                   m_Camera.Open();

                   if(m_pDataEvent)
                   {
                       m_pImageEvent->SetUserEvent(m_pDataEvent);
                   }

//                   // Get a camera nodemap in order to access camera parameters
//                   GenApi::INodeMap &nodemap = m_Camera.GetNodeMap();
//                   // 设置相机触发模式、触发源等
//                   GenApi::CEnumerationPtr prtTriggerSel = nodemap.GetNode("TriggerSelector");
//                   prtTriggerSel->FromString("FrameStart");

//                   GenApi::CEnumerationPtr prtTrigger = nodemap.GetNode("TriggerMode");
//                   prtTrigger->SetIntValue(1);

//                   GenApi::CEnumerationPtr ptrTriggerSource = nodemap.GetNode("TriggerSource");
//                   ptrTriggerSource->FromString("Line1");

                }
                m_IsOpen = true;
                return 0;
            }
        }
    }
    catch (const GenericException &e)
    {
        // Error handling.
        qWarning() << "An exception occurred.";
        qWarning() << e.GetDescription();
    }
    qWarning() << "No camera present";
    return -1;
}

void CameraBaslerGige::startSnap()
{
    if (m_Camera.CanWaitForFrameTriggerReady())
    {
        // Start the grabbing using the grab loop thread, by setting the grabLoopType parameter
        // to GrabLoop_ProvidedByInstantCamera. The grab results are delivered to the image event handlers.
        // The GrabStrategy_OneByOne default grab strategy is used.
        m_Camera.StartGrabbing( GrabStrategy_LatestImages, GrabLoop_ProvidedByInstantCamera);
    }
    else
    {
        // See the documentation of CInstantCamera::CanWaitForFrameTriggerReady() for more information.
    }

    //m_Camera.ExecuteSoftwareTrigger();
}

void CameraBaslerGige::stopSnap()
{
    m_Camera.StopGrabbing();
}

void CameraBaslerGige::acquire(cv::Mat& I)
{
    try
    {
        if (!m_Camera.IsGrabbing()) {
          m_Camera.StartGrabbing(1);
        }

        Pylon::CGrabResultPtr ptrGrabResult;

        m_Camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {
            int nType = CV_8UC1;
            I.create(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), nType);
            memcpy(I.data, (uint8_t *)ptrGrabResult->GetBuffer(), I.total() * I.elemSize());
        }
        else
        {
            qWarning() << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
        }
    }
    catch (const GenericException &e)
    {
        // Error handling.
        qWarning() << "An exception occurred.";
        qWarning() << e.GetDescription();
    }
}

void CameraBaslerGige::close()
{
    if(m_IsOpen)
    {
        m_Camera.Close();
        m_Camera.DetachDevice();
        m_IsOpen = false;
    }
}

QString CameraBaslerGige::getCameraInfo()
{
    QDataStream os;

    QString Info;

    if(m_IsOpen){
        Pylon::CDeviceInfo deviceInfo = m_Camera.GetDeviceInfo();
        // Get the camera control object.
        GenApi::INodeMap &control = m_Camera.GetNodeMap();

        GenApi::CIntegerPtr widthMax = control.GetNode("WidthMax");
        GenApi::CIntegerPtr heightMax = control.GetNode("HeightMax");

        Info = QString("Camera information:  \r\n Serial number      : %1\r\n Camera model       : %2\r\n"\
                       " Camera vendor      : %3\r\n Resolution         : %4 x %5\r\n Firmware version   : %6\r\n")
                .arg(deviceInfo.GetSerialNumber().c_str()).arg(deviceInfo.GetModelName().c_str()).arg(deviceInfo.GetVendorName().c_str())
                .arg(widthMax->GetValue()).arg(heightMax->GetValue()).arg(deviceInfo.GetDeviceVersion().c_str());

    }

    return Info;
}

float CameraBaslerGige::getBlackLevel()
{
    if(m_IsOpen){
        if (GenApi::IsReadable(m_Camera.BlackLevelAbs))
          return m_Camera.BlackLevelAbs.GetValue() * 0.001;
        else if (GenApi::IsReadable(m_Camera.BlackLevelRaw))
          return m_Camera.BlackLevelRaw.GetValue();
        else
          return -1.0;
    }

    return -1.0;
}


float CameraBaslerGige::getExposure()
{
    if(m_IsOpen){
        if (GenApi::IsReadable(m_Camera.ExposureTimeAbs))
          return m_Camera.ExposureTimeAbs.GetValue() * 0.001;
        else if (GenApi::IsReadable(m_Camera.ExposureTimeRaw))
          return m_Camera.ExposureTimeRaw.GetValue();
        else
          return -1.0;
    }

    return -1.0;
}

float CameraBaslerGige::getFrameRate()
{
    float frame_rate = m_Camera.AcquisitionFrameRateAbs.GetValue();
    return frame_rate;
}

float CameraBaslerGige::getGain()
{
    if(m_IsOpen){
        if (GenApi::IsReadable(m_Camera.GainAbs))
          return m_Camera.GainAbs.GetValue() * 0.001;
        else if (GenApi::IsReadable(m_Camera.GainRaw))
          return m_Camera.GainRaw.GetValue();
        else
          return -1.0;
    }

    return -1.0;
}

float CameraBaslerGige::getGamma()
{
    float gamma = m_Camera.Gamma.GetValue();
    return gamma;
}

float CameraBaslerGige::setBlackLevel(float blacklevel_value)
{
    if(m_IsOpen){
        if (GenApi::IsWritable(m_Camera.BlackLevelAbs)) {
          m_Camera.BlackLevelAbs.SetValue(blacklevel_value);
          return m_Camera.BlackLevelAbs.GetValue();
        } else if (GenApi::IsWritable(m_Camera.BlackLevelRaw)) {
          m_Camera.BlackLevelRaw.SetValue(blacklevel_value);
          return m_Camera.BlackLevelRaw.GetValue();
        }
    }
    return -1.0;
}

float CameraBaslerGige::setExposure(float exposure_value)
{
    if(m_IsOpen){
        m_Camera.ExposureAuto.SetValue(Basler_GigECameraParams::ExposureAuto_Off);
        if (GenApi::IsWritable(m_Camera.ExposureTimeAbs)) {
          m_Camera.ExposureTimeAbs.SetValue(exposure_value);
          return m_Camera.ExposureTimeAbs.GetValue();
        } else if (GenApi::IsWritable(m_Camera.ExposureTimeRaw)) {
          m_Camera.ExposureTimeRaw.SetValue(exposure_value);
          return m_Camera.ExposureTimeRaw.GetValue();
        }
    }
    return -1.0;
}

bool CameraBaslerGige::setAutoExposure(bool isAutoExposure)
{
    if(m_IsOpen){
        if(isAutoExposure){
            m_Camera.ExposureAuto.SetValue(Basler_GigECameraParams::ExposureAuto_Continuous);
        }else{
            m_Camera.ExposureAuto.SetValue(Basler_GigECameraParams::ExposureAuto_Off);
        }
        return true;
    }
    return false;
}

float CameraBaslerGige::setMaxExposure(float maxExposure)
{
    if(m_IsOpen){
        if (GenApi::IsWritable(m_Camera.AutoExposureTimeAbsUpperLimit)) {
          m_Camera.AutoExposureTimeAbsUpperLimit.SetValue(maxExposure);
          return m_Camera.AutoExposureTimeAbsUpperLimit.GetValue();
        }
    }
    return -1.0;
}

float CameraBaslerGige::setMinExposure(float minExposure)
{
    if(m_IsOpen){
        if (GenApi::IsWritable(m_Camera.AutoExposureTimeAbsLowerLimit)) {
          m_Camera.AutoExposureTimeAbsLowerLimit.SetValue(minExposure);
          return m_Camera.AutoExposureTimeAbsLowerLimit.GetValue();
        }
    }
    return -1.0;
}


float CameraBaslerGige::setGain(float gain_value)
{
    if(m_IsOpen){
        if (GenApi::IsWritable(m_Camera.GainAbs)) {
          m_Camera.GainAbs.SetValue(gain_value);
          return m_Camera.GainAbs.GetValue();
        } else if (GenApi::IsWritable(m_Camera.GainRaw)) {
          m_Camera.GainRaw.SetValue(gain_value);
          return m_Camera.GainRaw.GetValue();
        }
    }
    return -1.0;
}

float CameraBaslerGige::setFrameRate(float frame_rate)
{
    m_Camera.AcquisitionFrameRateAbs.SetValue(frame_rate);

    return m_Camera.AcquisitionFrameRateAbs.GetValue();
}

float CameraBaslerGige::setGamma(float gamma_value)
{
    if(m_IsOpen){
        if (GenApi::IsWritable(m_Camera.Gamma)) {
          m_Camera.Gamma.SetValue(gamma_value);
          return m_Camera.Gamma.GetValue();
        }
    }
    return -1.0;
}

unsigned int CameraBaslerGige::getHeight() const
{
    return 0;
}

unsigned int CameraBaslerGige::getWidth() const
{
    return 0;
}



