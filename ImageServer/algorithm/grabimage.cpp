#include "grabimage.h"

#include <QDebug>

bool grabimagefun(Mat src,int flag)
{

    Mat img ,templ, result;
    resize(src, img, Size(src.cols / 4, src.rows / 4), 0, 0, INTER_LINEAR);
    if (flag == 1)
    {
        templ = imread("tempimage//1-4.png");
    }
    if (flag == 2)
    {
        templ = imread("tempimage//2-4.png");
    }
    if (flag == 3)
    {
        templ = imread("tempimage//3-4.png");
    }


    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create(result_cols, result_rows, CV_32FC1);

    cv::cvtColor(templ, templ, CV_BGR2GRAY);


    matchTemplate(img, templ, result, CV_TM_SQDIFF_NORMED);//这里我们使用的匹配算法是标准平方差匹配 method=CV_TM_SQDIFF_NORMED，数值越小匹配度越好
    //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    qDebug() << "匹配度：" << minVal ;
    matchLoc = minLoc;
    printf("matchLoc.x = %d\n", matchLoc.x);
    printf("matchLoc.y = %d\n", matchLoc.y);
    printf("maxx = %d\n", matchLoc.x + templ.cols);
    printf("maxy = %d\n", matchLoc.y + templ.rows);

    rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 255, 0), 2, 8, 0);

    //imshow("templ", templ);
    //imshow("img", img);
    //waitKey(0);

    if (minVal<0.09)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
