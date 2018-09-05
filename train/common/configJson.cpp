
#include "configJson.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonDocument>

CConfigJson::CConfigJson()
{
    Load();
}

CConfigJson::~CConfigJson()
{

}

void CConfigJson::Load()
{
    QString strConfig = QCoreApplication::applicationDirPath() + "/config/config.json";

    QFile file(strConfig);
    if(file.exists())
    {
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            QByteArray byte_array = file.readAll();
            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array, &json_error);
            if(json_error.error != QJsonParseError::NoError){
              return ;
            }
            QJsonObject qObj = parse_doucment.object();

            if(qObj.empty())
              return;

           _fMeanValue = qObj["meanValue"].toDouble();
            _strFileDirSuccess = qObj["fileDirSuccess"].toString();
            _strFileDir = qObj["fileDir"].toString();

            QJsonArray arrayCamera = qObj["cameras"].toArray();

            foreach (QJsonValue cameraJ, arrayCamera) {
                QJsonObject camera = cameraJ.toObject();
                CameraConfig cameraConfig;
                cameraConfig._strSerial = camera["serialNo"].toString();
                cameraConfig._isEnable = camera["enable"].toBool();
                cameraConfig._isAutoExposure = camera["autoExposure"].toBool();
                cameraConfig._dValueExposure = camera["valueExposure"].toDouble();
                cameraConfig._dMaxExposure = camera["maxAutoExposure"].toDouble();
                cameraConfig._dMinExposure = camera["minAutoExposure"].toDouble();

                _listCameraConfig.push_back(cameraConfig);
            }
        }

    }
}

bool CConfigJson::findCameraBySerialNo(const QString& strSerialNo, CameraConfig& config)
{
    foreach (CameraConfig cameraConfig, _listCameraConfig) {
        if(cameraConfig._strSerial == strSerialNo){
            config = cameraConfig;
            return true;
        }
    }
    return false;
}
