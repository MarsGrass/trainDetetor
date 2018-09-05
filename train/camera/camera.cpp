
#include "camera.h"

Camera::Camera(){
    m_pDataEvent = nullptr;
}

Camera::~Camera(){}

QString Camera::getType()
{
    return m_type;
}

void Camera::setType(const QString& type)
{
    m_type = type;
}

QString Camera::getSerial()
{
    return m_serial;
}

void Camera::setSerial(const QString& serial)
{
    m_serial = serial;
}

void Camera::SetImageDataEvent(ImageDataEvent* pDataEvent)
{
    m_pDataEvent = pDataEvent;
}
