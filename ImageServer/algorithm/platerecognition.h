#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv/cv.h"
#include "lpr.h"


using namespace std;
using namespace cv;

typedef unsigned char Uint8;
typedef unsigned int  Uint32;

typedef struct R_rect
{
	CvPoint center;
	CvRect rect;
	int label;//luyong20151214 合并框个数
}R_rect;

typedef struct T_rect
{
	int x;
	int y;
	int w;
	int h;
	int weight;//luyong20151214 合并框个数
	float score;
}T_rect;



//按照X坐标排序  
bool rect_rank_x(vector<R_rect> &vec_rects);

void initial_();

void platereco(IplImage *srcimagee, vector<char>&plate,char &port);

void platereco_HSV(IplImage *srcimagee, vector<char>&plate, char &port);

void portreco(IplImage *srcimage,  char &port);

