#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

#include "CameraObjList.h"

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
    void on_btnStart_clicked();
    void on_btnStop_clicked();

private:
    Ui::MainWindow *ui;

    CCameraObjList* list;

    QStringListModel* m_model;
};

#endif // MAINWINDOW_H
