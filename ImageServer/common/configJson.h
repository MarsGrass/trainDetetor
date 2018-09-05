
#include <QJsonObject>
#include <QList>

class  CConfigJson
{
public:
    class CameraConfig{
    public:
        QString _strSerial;
        bool _isEnable;
        bool _isAutoExposure;
        double _dValueExposure;
        double _dMaxExposure;
        double _dMinExposure;
    };
public:
     CConfigJson();
     ~CConfigJson();

     void Load();

     bool findCameraBySerialNo(const QString& strSerialNo, CameraConfig& config);

public:
    float _fMeanValue;
    QString _strFileDirSuccess;
    QString _strFileDir;
    QList<CameraConfig> _listCameraConfig;
};
