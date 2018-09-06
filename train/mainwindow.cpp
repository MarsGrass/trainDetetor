#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camera/cameraFactory.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnInit->setEnabled(true);
    ui->btnUninit->setEnabled(false);
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);

    list = new CCameraObjList();

    m_model = new QStringListModel();
    ui->listView->setModel(m_model);  

    foreach (CConfigJson::ConfigFile file, list->_config._listConfigFile) {
        ui->comboBox->addItem(file._strName);
        if(file._isEnable)
        {
            ui->comboBox->setCurrentText(file._strName);
        }
    }
}

MainWindow::~MainWindow()
{
    list->Stop();

    list->Uninit();

    CameraFactory::DeleteCameraFacotry();

    delete ui;
}

void MainWindow::on_btnInit_clicked()
{
    list->Init(ui->comboBox->currentText());
    m_model->setStringList(CCameraObjList::g_listSerial);

    ui->btnInit->setEnabled(false);
    ui->btnUninit->setEnabled(true);
    ui->btnStart->setEnabled(true);

    ui->comboBox->setEnabled(false);
}

void MainWindow::on_btnUninit_clicked()
{
    list->Uninit();
    m_model->setStringList(CCameraObjList::g_listSerial);
    ui->btnInit->setEnabled(true);
    ui->btnUninit->setEnabled(false);
    ui->btnStart->setEnabled(false);

    ui->comboBox->setEnabled(true);
}

void MainWindow::on_btnStart_clicked()
{
    list->Start();

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->btnUninit->setEnabled(false);
}

void MainWindow::on_btnStop_clicked()
{
    list->Stop();

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->btnUninit->setEnabled(true);
}
