
#pragma once

#include <QJsonArray>

#include "camera.h"

class CameraFactory
{
public:
    static Camera* CreateCamera(const QString& type, const QString& serial);

    static Camera* CreateCamera(const QString& type, const QString& serial, ImageDataEvent* pDataEvent);

    static void DestroyCamera(Camera* pCamera);

    static QStringList listTypes();

    static void DeleteCameraFacotry();

    static QJsonArray getCameraTypes();

    static QJsonArray getCameraSerials();

    static void SetType(const QString& strType);

public:
    static bool g_IsInitFactory;

    static QString m_strCurrentCameraType;
};
