#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "algorithm/platerecognition.h"

#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imageServer = new CImageService();

    initial_();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ServerDataSlots(int nCmdId, int nSocketId, const QString& strData)
{
    if(nCmdId == 2)
    {
        ui->plainTextEdit->appendPlainText(strData);
        ui->plainTextEdit->updateGeometry();
    }
}

void MainWindow::on_btnDown_clicked()
{
    QTime time1;

    time1.start();

    cv::Mat image = cv::imread("33.bmp");

    char port;
    vector<char> plate;
    bool IsSuccess;

    cv::Mat dealImage;

    IplImage* pImage = &IplImage(image);

    platereco_HSV(pImage, plate, port);
    QString strPlate;
    foreach (char c, plate) {
        strPlate.append(c);
    }

    if(strPlate.isEmpty())
    {
        IsSuccess = false;
    }
    else
    {
        if((int)port == 1 || (int)port == 2 || port == '1' || port == '2')
        {
            strPlate = strPlate.left(strPlate.length() - (int)(port-'0'));
            qDebug() << "Plate:" << strPlate << "success!";
            IsSuccess = true;
        }
    }

    qDebug() << "spend time: " << time1.elapsed();

}
