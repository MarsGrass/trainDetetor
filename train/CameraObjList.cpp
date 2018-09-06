
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMutex>
#include <QDateTime>

#include "CameraObjList.h"
#include "camera/cameraFactory.h"

#include "camera/cameraBaslerGige.h"

QDateTime CCameraObjList::g_datetime = QDateTime::currentDateTime().addSecs(-300);
QString CCameraObjList::g_strCurrentPlate;
QStringList CCameraObjList::g_listSerial;
QString CCameraObjList::g_strFileDir;
QString CCameraObjList::g_strFileDirSuccess;
float CCameraObjList::g_fMeanValue = 0.0;


CCameraObjList::CCameraObjList(QObject* parent)
    : QObject(parent)
{
}

CCameraObjList::~CCameraObjList()
{
    Uninit();
}


int CCameraObjList::Init(const QString& strConfigName)
{
    _config.Load(strConfigName);

    g_strFileDir = _config._strFileDir;
    g_strFileDirSuccess = _config._strFileDirSuccess;
    g_fMeanValue = _config._fMeanValue;

    QJsonArray arrayCamera = CameraFactory::getCameraSerials();

    int nCameraSize = arrayCamera.size();
    if(nCameraSize == 0){
        return -1;
    }

    qDebug() << "Cameras Info: " << arrayCamera;

    for(int i = 0; i < nCameraSize; i++)
    {
        QString strSerialNo = arrayCamera[i].toString();

        CConfigJson::CameraConfig cameraConfig;
        if(_config.findCameraBySerialNo(strSerialNo, cameraConfig))
        {
            if(cameraConfig._isEnable)
            {
                CCameraObj* cameraObj = new CCameraObj();
                cameraObj->m_strSerial = strSerialNo;

                int nRes = cameraObj->Create();
                if(nRes != 0)
                {
                    qDebug() << "Create the camera Error: " << strSerialNo;
                    delete cameraObj;
                    continue;
                }

                if(cameraConfig._isAutoExposure)
                {
                    cameraObj->SetAutoExposure(true);
                    cameraObj->SetMaxExposure(cameraConfig._dMaxExposure);
                    cameraObj->SetMinExposure(cameraConfig._dMinExposure);
                }
                else
                {
                    cameraObj->SetAutoExposure(false);
                    cameraObj->SetExposure(cameraConfig._dValueExposure);
                }

                g_listSerial.append(strSerialNo);
                m_listCamera.push_back(cameraObj);
            }
        }
    }

    return 0;
}

void CCameraObjList::Uninit()
{
    foreach (CCameraObj* cameraObj, m_listCamera) {
        delete cameraObj;
    }
    m_listCamera.clear();
    g_listSerial.clear();
    return;
}

int CCameraObjList::Start()
{
    foreach (CCameraObj* cameraObj, m_listCamera) {
        cameraObj->StartSnap();
    }

    return 0;
}

int CCameraObjList::Stop()
{
    foreach (CCameraObj* cameraObj, m_listCamera) {
        cameraObj->StopSnap();
    }

    return 0;
}
