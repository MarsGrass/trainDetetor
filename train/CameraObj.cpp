
#include <QDebug>

#include <QEventLoop>
#include <QTimer>
#include <QDir>
#include <QMutex>

#include "CameraObj.h"
#include "Camera/cameraFactory.h"
#include <pylon/PylonIncludes.h>

#include "CameraObjList.h"
#include "algorithm/grabimage.h"
#include "algorithm/platerecognition.h"

CCameraObj::CCameraObj(QObject* parent)
    : QObject(parent)
{
    _camera = NULL;
    m_strType = "Basler_GIGE";
    m_strSerial = "12345678";
    m_fExposure = 5;
    m_fBlackLevel = 0.0;
    m_fGain = 1.0;
    m_fFrameRate = 10;
    m_fGamma = 1;
    m_IsOpen = false;
    m_IsInit = false;

    Init();
}

CCameraObj::~CCameraObj()
{
    if(_camera)
    {
        CameraFactory::DestroyCamera(_camera);
        _camera = NULL;
    }
}

void CCameraObj::Init()
{
    m_IsInit = true;

    pPool_ = new qtMessagePool(300);
    pQueue_ = new qtMessageQueue();

    pProcess_ = new TrainMessageManage();
    pProcess_->Start(pQueue_, 4);

    pDataEvent_ = new CUserDataEvent();
    pDataEvent_->SetCameraObj(this);
}

int CCameraObj::Create()
{
    _camera = CameraFactory::CreateCamera(m_strType, m_strSerial, pDataEvent_);
    if(_camera == NULL)
    {
        return -1;
    }

    if(m_IsInit)
    {
        //_camera->setExposure(m_fExposure);
        //_camera->setBlackLevel(m_fBlackLevel);
        //_camera->setGain(m_fGain);
        //_camera->setFrameRate(m_fFrameRate);
        //_camera->setGamma(m_fGamma);
    }else
    {
        //m_fExposure = _camera->getExposure();
        //m_fBlackLevel = _camera->getBlackLevel();
        //m_fGain = _camera->getGain();
        //m_fFrameRate = _camera->getFrameRate();
        //m_fGamma = _camera->getGamma();
        m_IsInit = true;
    }
    //m_strInfo = _camera->getCameraInfo();

    m_IsOpen = true;
    return 0;
}

void CCameraObj::StartSnap()
{
    try
    {
        if(_camera && m_IsOpen){
            _camera->startSnap();
        }
    }
    catch (const GenericException &e)
    {
        // Error handling
        qDebug() << "An exception occurred." <<  e.GetDescription();

    }
}

void CCameraObj::StopSnap()
{
    try
    {
        if(_camera && m_IsOpen){
            _camera->stopSnap();
        }
    }
    catch (const GenericException &e)
    {
        // Error handling
        qDebug() << "An exception occurred." <<  e.GetDescription();

    }
}

int CCameraObj::Capture()
{
    try
    {
        if(_camera && m_IsOpen){
            qtMessage* pMessage = pPool_->GetQtMessage();

            if(pMessage)
            {
                pMessage->setSerial(m_strSerial);
                _camera->acquire(pMessage->m_Image);
                pQueue_->SubmitMessage(pMessage);
            }

            return 0;
        }
    }
    catch (const GenericException &e)
    {
        // Error handling
        qDebug() << "An exception occurred." <<  e.GetDescription();

    }
    return -1;
}

void CCameraObj::SetExposure(double fExposure)
{
    try{
        if(_camera && m_IsOpen)
        {
            if(m_fExposure != fExposure){
                _camera->setExposure(fExposure);
                m_fExposure = fExposure;
            }
        }
    }
    catch(...)
    {
       qDebug() << "set Exposure failed";
    }

}

void CCameraObj::SetAutoExposure(bool isAutoExposure)
{
    try{
        if(_camera && m_IsOpen)
        {
            _camera->setAutoExposure(isAutoExposure);
        }
    }
    catch(...)
    {
       qDebug() << "set Auto Exposure failed";
    }
}

void CCameraObj::SetMaxExposure(double maxAutoExposure)
{
    try{
        if(_camera && m_IsOpen)
        {
            _camera->setMaxExposure(maxAutoExposure);
        }
    }
    catch(...)
    {
       qDebug() << "set Max Exposure failed";
    }
}

void CCameraObj::SetMinExposure(double minAutoExposure)
{
    try{
        if(_camera && m_IsOpen)
        {
            _camera->setMinExposure(minAutoExposure);
        }
    }
    catch(...)
    {
       qDebug() << "set Min Exposure failed";
    }
}

void CCameraObj::SetBlackLevel(double fBlackLevel)
{
    try{
        if(_camera && m_IsOpen)
        {
            if(m_fBlackLevel != fBlackLevel){
                _camera->setBlackLevel(fBlackLevel);
                m_fBlackLevel = fBlackLevel;
            }
        }
    }
    catch(...)
    {
       qDebug() << "set BlackLevel failed";
    }
}
void CCameraObj::SetGain(double fGain)
{
    try{
        if(_camera && m_IsOpen)
        {
            if(m_fGain != fGain){
                _camera->setGain(fGain);
                m_fGain = fGain;
            }
        }
    }
    catch(...)
    {
        qDebug() << "set Gain failed";
    }
}
void CCameraObj::SetFrameRate(double fFrameRate)
{
    try{
        if(_camera && m_IsOpen)
        {
            if(m_fFrameRate != fFrameRate){
                _camera->setFrameRate(fFrameRate);
                m_fFrameRate = fFrameRate;
            }
        }
    }
    catch(...)
    {
        qDebug() << "set FrameRate failed";
    }
}
void CCameraObj::SetGamma(double fGamma)
{
    try{
        if(_camera && m_IsOpen)
        {
            if(m_fGamma != fGamma){
                _camera->setGamma(fGamma);
                m_fGamma = fGamma;
            }
        }
    }
    catch(...)
    {
        qDebug() << "set Gamma failed";
    }
}

void CUserDataEvent::SetCameraObj(CCameraObj* obj)
{
    m_obj = obj;
}


void CUserDataEvent::ProcessCvMat(const cv::Mat& mat)
{
    cv::Scalar meanValue = cv::mean(mat);
    float MyMeanValue = meanValue.val[0];//.val[0]表示第一个通道的均值

    if(MyMeanValue > 0)
    {
        QDateTime current_ = QDateTime::currentDateTime();
        //if(pMsg->getSerial().contains("22625824"))
        {
            if(CCameraObjList::g_datetime.secsTo(current_) > 300) //5min
            {
                QMutex mutex;
                mutex.lock();
                CCameraObjList::g_datetime = current_;
                CCameraObjList::g_strCurrentPlate.clear();
                mutex.unlock();

                QString strSubDir = current_.toString("yyyy-MM-dd-HH-mm");

                QDir dir(CCameraObjList::g_strFileDir);
                if(dir.exists(strSubDir))
                {
                }
                else{
                   dir.mkdir(strSubDir);
                   QDir dir1(CCameraObjList::g_strFileDir+strSubDir);
                   for(int i = 0; i < CCameraObjList::g_listSerial.size(); i++)
                   {
                       dir1.mkdir(CCameraObjList::g_listSerial.at(i));
                   }
                }

                QDir dirSuccess(CCameraObjList::g_strFileDirSuccess);
                if(dirSuccess.exists(strSubDir))
                {
                }
                else{
                   dirSuccess.mkdir(strSubDir);
                   QDir dir1(CCameraObjList::g_strFileDirSuccess+strSubDir);
                   for(int i = 0; i < CCameraObjList::g_listSerial.size(); i++)
                   {
                       dir1.mkdir(CCameraObjList::g_listSerial.at(i));
                   }
                }
            }
        }

        if(CCameraObjList::g_datetime.secsTo(current_) <= 10 && CCameraObjList::g_datetime.secsTo(current_) >= 0)
        {
            if(m_obj)
            {
                qtMessage* pMessage = m_obj->pPool_->GetQtMessage();

                if(pMessage)
                {
                    pMessage->setSerial(m_obj->m_strSerial);

                    pMessage->m_Image = mat.clone();
                    m_obj->pQueue_->SubmitMessage(pMessage);
                }
            }

        }
    }
}

TrainMessageManage::TrainMessageManage()
{

}

TrainMessageManage::~TrainMessageManage()
{

}

bool TrainMessageManage::Work(qtMessage* pMsg)
{
    if(pMsg->m_strSerial.isEmpty())
    {
        return false;
    }

    bool bRes = false;

    QString path = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz-");
    path += pMsg->getSerial();
    path += ".jpg";
    QString strPth;
    QString strSubDir1 = CCameraObjList::g_datetime.toString("yyyy-MM-dd-HH-mm");
    strSubDir1 = strSubDir1 + "/" + pMsg->getSerial() + "/"+ path;

    if(bRes)
    {
        qDebug() << path;
        strPth = CCameraObjList::g_strFileDirSuccess + strSubDir1;
    }
    else
    {
        qDebug() << path;
        strPth = CCameraObjList::g_strFileDir + strSubDir1;
    }

//    QEventLoop loop;
//    QTimer::singleShot(500, &loop, SLOT(quit()));
//    loop.exec();

    cv::imwrite(strPth.toLatin1().data(), pMsg->m_Image);

    return true;
}

/**
flag = 1 底部感应线圈  684  728
flag = 2 底部机车标签  708
flag = 3 速度感应器   707  726
*/

bool TrainMessageManage::DoMessage(qtMessage* pMsg)
{
    QString strSerialNo = pMsg->getSerial();
    if(strSerialNo.isEmpty())
    {
        return false;
    }

    bool IsSuccess = false;

    if(strSerialNo.contains("22637684") || strSerialNo.contains("22648728"))
    {
        IsSuccess = grabimagefun(pMsg->m_Image, 1);
    }
    else if(strSerialNo.contains("22648708"))
    {
        IsSuccess = grabimagefun(pMsg->m_Image, 2);
    }
    else if(strSerialNo.contains("22648707") || strSerialNo.contains("22648726"))
    {
        IsSuccess = grabimagefun(pMsg->m_Image, 3);
    }
    else if(strSerialNo.contains("22625824") || strSerialNo.contains("22625830"))
    {
        char port;
        vector<char> plate;

        //cv::Mat dealImage;
        //cv::cvtColor(pMsg->m_Image, dealImage, CV_BayerRG2BGR);

        //IplImage* pImage = &IplImage(dealImage);

//        platereco_HSV(pImage, plate, port);
//        QString strPlate;
//        foreach (char c, plate) {
//            strPlate.append(c);
//        }

//        if(strPlate.isEmpty())
//        {
//            IsSuccess = false;
//        }
//        else
//        {
//            if((int)port == 1 || (int)port == 2)
//            {
//                strPlate = strPlate.left(strPlate.length() - (int)port);
//                listCamera_->m_strCurrentPlate = strPlate;
//                qDebug() << "Plate:" << strPlate << "success!";
//                IsSuccess = true;
//            }
//        }
    }

    if(IsSuccess)
    {
        qDebug() << strSerialNo << "success!";
    }
    return IsSuccess;
}
