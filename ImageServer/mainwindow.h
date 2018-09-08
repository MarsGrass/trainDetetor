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
    void on_btnDown_clicked();

private:
    Ui::MainWindow *ui;

    CImageService* imageServer;
};

#endif // MAINWINDOW_H
