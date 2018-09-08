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


    imageServer->Start();
    initial_();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnDown_clicked()
{
    QTime time1;

    time1.start();

    cv::Mat image = cv::imread("33.bmp");

    Mat img_h, img_s, img_v, imghsv;
    vector<cv::Mat> hsv_vec;
    cvtColor(image, imghsv, CV_BGR2HSV);
    // 分割hsv通道
    split(imghsv, hsv_vec);
    img_h = hsv_vec[0];
    int nl = img_h.rows; //行数
    int nc = img_h.cols * img_h.channels();
    for (int j = 0; j<nl; j++)
    {
        uchar* data = img_h.ptr<uchar>(j);
        for (int i = 0; i<nc; i++)
        {
            if (data[i]>120)
            {
                data[i] = 255;
            }
            else
            {
                data[i] = 0;
            }

        }
    }

    cv::Mat img_h4;
    cv::resize(img_h, img_h4, cv::Size(0, 0), 0.25, 0.25);

    cv::Mat binaryimage_mat;
    Mat element_1 = getStructuringElement(MORPH_RECT, Size(10, 5));
    morphologyEx(img_h4,binaryimage_mat, MORPH_DILATE, element_1);

    //cv::imshow("ROI_DILATE_image", binaryimage_mat);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryimage_mat, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

    vector<cv::Rect> plateVector;
    for(int i = 0; i < contours.size(); i++)
    {
          cv::Rect rect = cv::boundingRect(cv::Mat(contours[i]));

          float h = rect.height;
          float w = rect.width;
          float ratio_R = h / w;
          float S_R = h*w;

          if (ratio_R > 0.10 && ratio_R < 0.5 && S_R > 3800 && S_R < 6000)
          {
              rect.x = rect.x*4;
              rect.y = rect.y*4;
              rect.height = rect.height*4;
              rect.width = rect.width*4;
              plateVector.push_back(rect);
          }
    }

    for(int i = 0; i < plateVector.size(); i++)
    {
        //cv::rectangle(image, plateVector[i], cv::Scalar(0,255,255), 2);

        cv::Mat roiMat = img_h(plateVector[i]);
        Mat element_t = getStructuringElement(MORPH_RECT, Size(1, 1));
        morphologyEx(roiMat,roiMat, MORPH_DILATE, element_t);

        cv::Mat roiMatBin;
        cv::threshold(roiMat, roiMatBin, 1, 255, CV_THRESH_OTSU);

        //连通域计算
        cv::Mat  labels, stats, centroids, img_color, img_gray, img_gray1, img_gray2;
        int nccomps = cv::connectedComponentsWithStats(
            roiMatBin, //二值图像
            labels,     //和原图一样大的标记图
            stats, //nccomps×5的矩阵 表示每个连通区域的外接矩形和面积（pixel）
            centroids //nccomps×2的矩阵 表示每个连通区域的质心
            );

        qDebug() << "nccomps:" << nccomps;

        //去除过小区域，初始化颜色表
        vector<cv::Vec3b> colors(nccomps);
        colors[0] = cv::Vec3b(0, 0, 0); // background pixels remain black.
        for (int i1 = 1; i1 < nccomps; i1++)
        {
            colors[i1] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
            //去除面积小于100的连通域
            if (stats.at<int>(i1, cv::CC_STAT_AREA) < 350)//调整参数
                colors[i1] = cv::Vec3b(0, 0, 0); // small regions are painted with black too.
        }

        //按照label值，对不同的连通域进行着色
        img_color = cv::Mat::zeros(roiMatBin.size(), CV_8UC3);
        for (int y = 0; y < img_color.rows; y++)
            for (int x = 0; x < img_color.cols; x++)
            {
                int label = labels.at<int>(y, x);
                CV_Assert(0 <= label && label <= nccomps);
                img_color.at<cv::Vec3b>(y, x) = colors[label];
            }

        //统计降噪后的连通区域
        cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(img_gray, img_gray, 0, 1, cv::THRESH_BINARY);//注意二值化数值

        nccomps = cv::connectedComponentsWithStats(img_gray, labels, stats, centroids);

        vector<R_rect>char_rect;
        int num_contour = 0;
        for (int i2 = 0; i2 < nccomps; i2++)
        {
            CvRect aRect_;
            aRect_.x = stats.at<int>(i2, cv::CC_STAT_LEFT);
            aRect_.y = stats.at<int>(i2, cv::CC_STAT_TOP);
            aRect_.width = stats.at<int>(i2, cv::CC_STAT_WIDTH);
            aRect_.height = stats.at<int>(i2, cv::CC_STAT_HEIGHT);
            float ratio_ = (float)aRect_.height / (float)aRect_.width;
            float S_ = aRect_.height*aRect_.width;

            R_rect tempR;
            tempR.rect = aRect_;
            tempR.label = i2;
            if (ratio_ > 1.1&&ratio_ < 4.5&&S_>1100 && S_ < 10000)//设定参数
            {
                char_rect.push_back(tempR);
                num_contour++;
            }
        }

        qDebug() << num_contour;

        cv::imshow("image", roiMatBin);
    }



//    char port;
//    vector<char> plate;
//    bool IsSuccess;

//    cv::Mat dealImage;

//    IplImage* pImage = &IplImage(image);

//    platereco_HSV(pImage, plate, port);
//    QString strPlate;
//    foreach (char c, plate) {
//        strPlate.append(c);
//    }

//    if(strPlate.isEmpty())
//    {
//        IsSuccess = false;
//    }
//    else
//    {
//        if((int)port == 1 || (int)port == 2 || port == '1' || port == '2')
//        {
//            strPlate = strPlate.left(strPlate.length() - (int)(port-'0'));
//            qDebug() << "Plate:" << strPlate << "success!";
//            IsSuccess = true;
//        }
//    }

    qDebug() << "spend time: " << time1.elapsed();

}
