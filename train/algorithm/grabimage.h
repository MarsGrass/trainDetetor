#ifndef GRABIMAGE_H
#define GRABIMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/**
flag = 1 底部感应线圈  684  728
flag = 2 底部机车标签  708
flag = 3 速度感应器   707  726
*/
bool grabimagefun(Mat src,int flag);
#endif // GRABIMAGE_H
