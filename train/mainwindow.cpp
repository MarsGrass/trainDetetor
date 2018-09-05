#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camera/cameraFactory.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);

    list = new CCameraObjList();

    list->Init();
    m_model = new QStringListModel();
    ui->listView->setModel(m_model);

    m_model->setStringList(CCameraObjList::g_listSerial);
}

MainWindow::~MainWindow()
{
    list->Stop();

    list->Uninit();

    CameraFactory::DeleteCameraFacotry();

    delete ui;
}


void MainWindow::on_btnStart_clicked()
{
    list->Start();

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);


}

void MainWindow::on_btnStop_clicked()
{
    list->Stop();

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
}
