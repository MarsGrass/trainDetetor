#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ifiServer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void ServerDataSlots(int nCmdId, int nSocketId, const QString& strData);

    void on_btnDown_clicked();

private:
    Ui::MainWindow *ui;

    CifiServer* ifiserver;
};

#endif // MAINWINDOW_H
