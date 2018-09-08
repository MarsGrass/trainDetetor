#-------------------------------------------------
#
# Project created by QtCreator 2018-08-07T11:23:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageServer
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

DEFINES += FIND_PLATE
#DEFINES += MATCH_TOOL


SOURCES += main.cpp\
        mainwindow.cpp \
    algorithm/grabimage.cpp \
    algorithm/lpr.cpp \
    algorithm/platerecognition_HSV.cpp \
    common/LogQt.cpp \
    common/TCPClient.cpp \
    common/TCPServer.cpp \
    ifiServer.cpp \
    message/IoServer.cpp \
    message/IoServicePool.cpp \
    message/IoSession.cpp \
    message/IoSessionPool.cpp \
    message/qtMessage.cpp \
    message/qtMessageParse.cpp \
    message/qtMessagePool.cpp \
    message/qtMessageQueue.cpp \
    message/qtMessageWork.cpp \
    message/qtMessageWorkManage.cpp

HEADERS  += mainwindow.h \
    algorithm/grabimage.h \
    algorithm/lpr.h \
    algorithm/platerecognition.h \
    common/LogQt.h \
    common/TCPClient.h \
    common/TCPServer.h \
    ifiServer.h \
    message/IoServer.h \
    message/IoServicePool.h \
    message/IoSession.h \
    message/IoSessionPool.h \
    message/qtMessage.h \
    message/qtMessageParse.h \
    message/qtMessagePool.h \
    message/qtMessageQueue.h \
    message/qtMessageWork.h \
    message/qtMessageWorkManage.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\Qt\opencv\build\include\
INCLUDEPATH += C:\Boost\include\boost-1_66

CONFIG(release,debug|release){
    LIBS += C:\Qt\opencv\build\x64\vc14\lib\opencv_world341.lib
    LIBS += -LC:\Boost\lib -llibboost_thread-vc140-mt-x64-1_66

    DEFINES += _WIN32_WINNT=0x0501
}
CONFIG(debug,debug|release){
    LIBS += C:\Qt\opencv\build\x64\vc14\lib\opencv_world320d.lib
    LIBS += -LC:\Boost\lib -llibboost_thread-vc140-mt-gd-x64-1_66

    DEFINES += -D_WIN32_WINNT=0x0501
}
