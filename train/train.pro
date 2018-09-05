#-------------------------------------------------
#
# Project created by QtCreator 2018-07-27T17:58:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = train
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    camera/camera.cpp \
    camera/cameraBaslerGige.cpp \
    camera/cameraFactory.cpp \
    CameraObj.cpp \
    CameraObjList.cpp \
    message/qtMessage.cpp \
    message/qtMessagePool.cpp \
    message/qtMessageQueue.cpp \
    message/qtMessageWork.cpp \
    message/qtMessageWorkManage.cpp \
    common/LogQt.cpp \
    algorithm/grabimage.cpp \
    algorithm/lpr.cpp \
    algorithm/platerecognition_HSV.cpp \
    common/configJson.cpp

HEADERS  += mainwindow.h \
    camera/camera.h \
    camera/cameraBaslerGige.h \
    camera/cameraFactory.h \
    CameraObj.h \
    CameraObjList.h \
    message/qtMessage.h \
    message/qtMessagePool.h \
    message/qtMessageQueue.h \
    message/qtMessageWork.h \
    message/qtMessageWorkManage.h \
    common/LogQt.h \
    algorithm/grabimage.h \
    algorithm/lpr.h \
    algorithm/platerecognition.h \
    common/configJson.h

FORMS    += mainwindow.ui


INCLUDEPATH += C:\Qt\opencv\build\include\
INCLUDEPATH += C:\Boost\include\boost-1_66

CONFIG(release,debug|release){
    LIBS += C:\Qt\opencv\build\x64\vc14\lib\opencv_world320.lib
    LIBS += -LC:\Boost\lib -llibboost_thread-vc140-mt-x64-1_66

    DEFINES += _WIN32_WINNT=0x0501
}
CONFIG(debug,debug|release){
    LIBS += C:\Qt\opencv\build\x64\vc14\lib\opencv_world320d.lib
    LIBS += -LC:\Boost\lib -llibboost_thread-vc140-mt-gd-x64-1_66

    DEFINES += -D_WIN32_WINNT=0x0501
}

INCLUDEPATH += $$quote(C:\Program Files\Basler\pylon 5\Development\include )
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib )
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib)
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\PylonBase_MD_VC120_v5_0.lib)
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\PylonC_MD_VC120.lib)
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\PylonGUI_MD_VC120_v5_0.lib )
LIBS += $$quote(C:\Program Files\Basler\pylon 5\Development\lib\x64\PylonUtility_MD_VC120_v5_0.lib )



