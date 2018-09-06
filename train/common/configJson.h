
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

    class ConfigFile{
    public:
        QString _strName;
        bool _isEnable;
    };
public:
     CConfigJson();
     ~CConfigJson();

     void Load();

     void Load(const QString& strConfigFileName);

     bool findCameraBySerialNo(const QString& strSerialNo, CameraConfig& config);

public:
    float _fMeanValue;
    QString _strFileDirSuccess;
    QString _strFileDir;
    QList<CameraConfig> _listCameraConfig;
    QList<ConfigFile> _listConfigFile;
};
