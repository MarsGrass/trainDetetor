#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QStringList>

#include "common/LogQt.h"


int main(int argc, char *argv[])
{
    qInstallMessageHandler(CLogQt::myMessageOutput);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
