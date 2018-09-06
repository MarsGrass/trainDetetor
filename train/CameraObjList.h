

#pragma once

#include <QObject>
#include "CameraObj.h"
#include <QList>
#include <QDateTime>
#include <QStringList>
#include <QMap>

#include "common/configJson.h"

class CCameraObjList ;


class CCameraObjList : public QObject
{
    Q_OBJECT

public:
    CCameraObjList(QObject* parent = 0);
    ~CCameraObjList();

    int Init(const QString& strConfigName = "configName.json");

    void Uninit();

    int Start();

    int Stop();

public:  
    QList<CCameraObj*> m_listCamera;

    CConfigJson _config;

    static QDateTime g_datetime;
    static QString g_strCurrentPlate;
    static QStringList g_listSerial;
    static QString g_strFileDir;
    static QString g_strFileDirSuccess;
    static float g_fMeanValue;
};

