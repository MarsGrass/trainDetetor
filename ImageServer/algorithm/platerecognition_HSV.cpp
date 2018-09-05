#include "platerecognition.h"


//#define DEBUG_
#define OPENCV_
//#define MY_

IplImage *curframetrgb = NULL;
IplImage *curframet = NULL;
IplImage *curframet_port = NULL;
IplImage *image_save = NULL;
IplImage *image_dif = NULL;
IplImage *ROI_image = NULL;
IplImage *ROI_show_image = NULL;
IplImage *ROI_image_R = NULL;
IplImage *ROI_image_G = NULL;
IplImage *ROI_image_B = NULL;
IplImage *ROI_grayimage = NULL;
IplImage *ROI_binaryimage = NULL;
IplImage *ROI_binary3image = NULL;
IplImage *ROI_tesimage = NULL;
IplImage *ROI_BLACK_HAT_image = NULL;
IplImage *ROI_DILATE_image = NULL;
IplImage *ROI_ERODE_image = NULL;
IplImage *ROI_CLOSE_image = NULL;
IplImage *ROI_mask_image = NULL;
IplImage *ROI_image_temp = NULL;
IplImage *ROI_binary_temp = NULL;
IplImage *gray_image = NULL;
IplImage *binary_image = NULL;
IplImage *image_save_port = NULL;
IplImage *ROI_image_port = NULL;
IplImage *ROI_grayimage_port = NULL;
IplImage *ROI_binaryimage_port = NULL;
IplImage *inputimage = NULL;
IplImage *maskimage = NULL;
IplImage *maskimage_resize = NULL;

vector<CvRect>plate_p;//候选区域
float coef = 4.0;
float coef_port = 2.0;
char showstr[1024];
unsigned char *Char_m;

IplConvKernel* element = cvCreateStructuringElementEx(10, 3, 0.5, 0.5, CV_SHAPE_RECT, 0);
CvFont font;
CvFont font_show;

Uint8 tempBW[800][800];
Uint8 **BW = NULL;
//***************************************************//
//*********************连通域*************************//
CvMemStorage * storage = cvCreateMemStorage(0);
CvMemStorage * storage_temp = cvCreateMemStorage(0);
IplImage* dst = NULL;

T_rect ROI_rect_M = { 100, 100, 300, 100 };

T_rect ROI_rect_M_port = { 100, 100, 300, 100 };

//************************************************************************************************************************************************
void sk_image_Imdilate(const unsigned char * src_A, int row, int col, const int *Se, const int *Ap, unsigned char * dst_B)
{
	int i = 0, j = 0, m = 0, n = 0;
	int tmp1 = 0, tmp2 = 0;

	memset(dst_B, 0, row*col * sizeof(unsigned char));

	for (i = 0; i<row; ++i)   //对第一行和第一列单独处理
	{
		if (src_A[i*col]>0)
		{
			for (m = 0; m<Se[0]; ++m)
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = i + m - Ap[0];
					tmp2 = n - Ap[1];
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
						dst_B[tmp1*col + tmp2] = 1;
				}
			}
		}
	}
	for (j = 0; j<col; ++j)
	{
		if (src_A[j]>0)
		{
			for (m = 0; m<Se[0]; ++m)
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = m - Ap[0];
					tmp2 = j + n - Ap[1];
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
						dst_B[tmp1*col + tmp2] = 1;
				}
			}
		}
	}

	for (i = 1; i<row; ++i)
	{
		for (j = 1; j<col; ++j)
		{
			//当前像素点左上邻点均为零时，则用膨胀矩阵对该点进行膨胀
			if ((src_A[i*col + j]>0) && (src_A[(i - 1)*col + j] == 0) && (src_A[i*col + j - 1] == 0))
			{
				for (m = 0; m<Se[0]; ++m)
				{
					for (n = 0; n<Se[1]; ++n)
					{
						tmp1 = i + m - Ap[0];
						tmp2 = j + n - Ap[1];
						if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
							dst_B[tmp1*col + tmp2] = 1;
					}
				}
			}
			//当前像素点左邻点有值时，左邻点已被膨胀矩阵作用过，则只需要在左邻点膨胀后的基础上，向右处理一列数据
			if ((src_A[i*col + j]>0) && (src_A[i*col + j - 1]>0))
			{
				for (m = 0; m<Se[0]; ++m)
				{
					tmp1 = i + m - Ap[0];
					tmp2 = j + Se[1] - Ap[1] - 1;
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
						dst_B[tmp1*col + tmp2] = 1;
				}
			}
			//当前像素点上邻点有值，而左邻点无值时，上邻点已被膨胀，则只需要在上邻点膨胀后的基础上，向下处理一行数据
			if ((src_A[i*col + j]>0) && (src_A[(i - 1)*col + j]>0) && (src_A[i*col + j - 1] == 0))
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = i + Se[0] - Ap[0] - 1;
					tmp2 = j + n - Ap[1];
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
						dst_B[tmp1*col + tmp2] = 1;
				}
			}

		}
	}
}
//************************************************************************************************************************************************
//************************************************************************************************************************************************
void sk_image_Imerode(const unsigned char * src_A, int row, int col, const int *Se, const int *Ap, unsigned char * dst_B)
{
	register int i = 0, j = 0, m = 0, n = 0;
	int tmp1 = 0, tmp2 = 0;
	memset(dst_B, 0, row*col * sizeof(unsigned char));

	for (i = 0; i<row*col; ++i)
		dst_B[i] = src_A[i];

	for (i = 0; i<row; ++i)     //处理图像的第一列
	{
		if (src_A[i*col]>0)
		{
			for (m = 0; m<Se[0]; ++m)
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = i - Ap[0] + m;
					tmp2 = n - Ap[1];
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
					{
						if (src_A[tmp1*col + tmp2]<1)
						{
							dst_B[i*col] = 0;
							break;
						}
					}
					else
					{
						if ((tmp1 >= row) || (tmp2 >= col))
						{
							dst_B[i*col] = 0;
							break;
						}
					}
				}
				if (dst_B[i*col] == 0)	break;
			}
		}
	}

	for (j = 0; j<col; ++j)       //处理图像的第一行
	{
		if (src_A[j]>0)
		{
			for (m = 0; m<Se[0]; ++m)
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = m - Ap[0];
					tmp2 = j - Ap[1] + n;
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
					{
						if (src_A[tmp1*col + tmp2]<1)
						{
							dst_B[j] = 0;
							break;
						}
					}
					else
					{
						if ((tmp1 >= row) || (tmp2 >= col))
						{
							dst_B[j] = 0;
							break;
						}
					}
				}
				if (dst_B[j] == 0)	break;
			}
		}
	}

	for (i = 1; i<row; ++i)
	{
		for (j = 1; j<col; ++j)
		{
			//当前点上邻点和左邻点为0时，利用腐蚀矩阵对当前点进行腐蚀判断
			if ((src_A[i*col + j]>0) && (dst_B[(i - 1)*col + j] == 0) && (dst_B[i*col + j - 1] == 0))
			{
				for (m = 0; m<Se[0]; ++m)
				{
					for (n = 0; n<Se[1]; ++n)
					{
						tmp1 = i - Ap[0] + m;
						tmp2 = j - Ap[1] + n;
						if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
						{
							if (src_A[tmp1*col + tmp2]<1)
							{
								dst_B[i*col + j] = 0;
								break;
							}
						}
						else
						{
							if ((tmp1 >= row) || (tmp2 >= col))
							{
								dst_B[i*col + j] = 0;
								break;
							}
						}
					}
					if (dst_B[i*col + j] == 0)	break;
				}
			}
			//当前点左邻点值为1时，说明在腐蚀矩阵在前一点的作用域内均为1，此时只需要判断最后一列数据是否存在零值
			if ((src_A[i*col + j]>0) && (dst_B[i*col + j - 1]>0))
			{
				for (m = 0; m<Se[0]; ++m)
				{
					tmp1 = i - Ap[0] + m;
					tmp2 = j - Ap[1] + Se[1] - 1;
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
					{
						if (src_A[tmp1*col + tmp2]<1)
						{
							dst_B[i*col + j] = 0;
							break;
						}
					}
					else
					{
						if ((tmp1 >= row) || (tmp2 >= col))
						{
							dst_B[i*col + j] = 0;
							break;
						}
					}
				}
			}
			//当前像素点上邻点有值，而左邻点无值时，说明腐蚀矩阵在上邻点的作用域内均为1，只需判断最后一行数据是否存在零值
			if ((src_A[i*col + j]>0) && (dst_B[(i - 1)*col + j]>0) && (dst_B[i*col + j - 1] == 0))
			{
				for (n = 0; n<Se[1]; ++n)
				{
					tmp1 = i - Ap[0] + Se[0] - 1;
					tmp2 = j - Ap[1] + n;
					if ((tmp1>-1) && (tmp2>-1) && (tmp1<row) && (tmp2<col))
					{
						if (src_A[tmp1*col + tmp2]<1)
						{
							dst_B[i*col + j] = 0;
							break;
						}
					}
					else
					{
						if ((tmp1 >= row) || (tmp2 >= col))
						{
							dst_B[i*col + j] = 0;
							break;
						}
					}
				}
			}
		}
	}
}


bool rect_rank_x(vector<R_rect> &vec_rects)
{
	R_rect vec_temp;
	for (int l = 1; l < vec_rects.size(); l++)
	{
		for (int m = vec_rects.size() - 1; m >= l; m--)
		{
			if (vec_rects[m].rect.x < vec_rects[m - 1].rect.x)
			{
				vec_temp = vec_rects[m - 1];
				vec_rects[m - 1] = vec_rects[m];
				vec_rects[m] = vec_temp;
			}
		}
	}
	return true;
}

void initial_()
{
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 0, 0.1, 1);
	cvInitFont(&font_show, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 0, 1, 8);
	int width = 1600;
	int height = 1200;
	ROI_rect_M.w = (int)(width / coef) / 4 * 4;//test
	ROI_rect_M.h = (int)(height / coef) / 4 * 4;
	inputimage = cvCreateImage(cvSize(width, height), 8, 1);
	maskimage = cvCreateImage(cvSize(width, height), 8, 1);
	curframet = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	maskimage_resize = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	curframetrgb = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 3);
	ROI_rect_M.x = (int)(curframet->width*0.0);//ADB
	ROI_rect_M.y = (int)(curframet->height*0.0);
	ROI_rect_M.w = ((int)(curframet->width*1.0 / 4)) * 4;
	ROI_rect_M.h = ((int)(curframet->height*1.0 / 4)) * 4;//有效区域最终缩放后图像中选择有效区域
	InitiateMEM(curframet->height, curframet->width);
	Char_m = new unsigned char[resize_COL*resize_ROW];
	ROI_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, curframet->nChannels);
	ROI_show_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, curframet->nChannels);
	ROI_grayimage = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_image_R = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_image_G = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_image_B = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_binaryimage = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_binary3image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 3);
	ROI_BLACK_HAT_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_ERODE_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_DILATE_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_CLOSE_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);
	ROI_mask_image = cvCreateImage(cvSize(ROI_rect_M.w, ROI_rect_M.h), 8, 1);

	gray_image = cvCreateImage(cvSize(curframet->width, curframet->height), 8, 1);
	binary_image = cvCreateImage(cvSize(curframet->width, curframet->height), 8, 1);
	image_save = cvCreateImage(cvSize(curframet->width, curframet->height), curframet->depth, curframet->nChannels);
	image_dif = cvCreateImage(cvSize(curframet->width, curframet->height), curframet->depth, curframet->nChannels);

	dst = cvCreateImage(cvGetSize(ROI_binary3image), 8, 3);

	ROI_rect_M_port.w = (int)(width / coef_port) / 4 * 4;//test
	ROI_rect_M_port.h = (int)(height / coef_port) / 4 * 4;

	curframet_port = cvCreateImage(cvSize(ROI_rect_M_port.w, ROI_rect_M_port.h), 8, 1);
	ROI_rect_M_port.x = (int)(curframet_port->width*0.2);//ADB
	ROI_rect_M_port.y = (int)(curframet_port->height*0.0);
	ROI_rect_M_port.w = ((int)(curframet_port->width*0.6 / 4)) * 4;
	ROI_rect_M_port.h = ((int)(curframet_port->height*0.2 / 4)) * 4;//有效区域最终缩放后图像中选择有效区域
	image_save_port = cvCreateImage(cvSize(curframet_port->width, curframet_port->height), curframet_port->depth, curframet_port->nChannels);
	ROI_image_port = cvCreateImage(cvSize(ROI_rect_M_port.w, ROI_rect_M_port.h), 8, curframet_port->nChannels);
	ROI_grayimage_port = cvCreateImage(cvSize(ROI_rect_M_port.w, ROI_rect_M_port.h), 8, 1);
	ROI_binaryimage_port = cvCreateImage(cvSize(ROI_rect_M_port.w, ROI_rect_M_port.h), 8, 1);
}

// Normalizes a given image into a value range between 0 and 255.
Mat norm_0_255(const Mat& src) {
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

void platereco_HSV(IplImage *srcimage, vector<char>&plate, char &port)
{

	
//#define GAMMA_
#ifdef GAMMA_

	Mat image = cvarrToMat(srcimage);
	Mat X;
	image.convertTo(X, CV_32FC1);
	//image.convertTo(X, CV_32F);
	// Start preprocessing:
	Mat I;
	float gamma = 5.0;
	pow(X, gamma, I);
	I = norm_0_255(I);
	//// Draw it on screen:
	//imshow("Original Image", image);
	//imshow("Gamma correction image", I);
	//// Show the images:
	//waitKey(10);

	srcimage = &IplImage(I);
	//cvShowImage("inputimage", inputimage);
	//cvWaitKey(0);
#endif

	Mat image = cvarrToMat(srcimage);
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
	//Mat imgh;
	//resize(img_h, imgh, Size(img_h.cols / coef, img_h.rows / coef), 0, 0, INTER_LINEAR);

	if (srcimage->nChannels == 3)
	{
		
//#ifdef GAMMA_
//		for (int iy = 0; iy < srcimage->height; iy++)
//		{
//			for (int ix = 0; ix < srcimage->width; ix++)
//			{
//				inputimage->imageData[iy*inputimage->width + ix] = srcimage->imageData[iy*srcimage->widthStep + ix * 3 + 2];
//			}
//		}
//#else
//		cvCvtColor(srcimage,inputimage,CV_BGR2GRAY);
//#endif

#define RGB_
#ifdef RGB_
		//for (int iy = 0; iy < srcimage->height; iy++)
		//{
		//	for (int ix = 0; ix < srcimage->width; ix++)
		//	{
		//		inputimage->imageData[iy*inputimage->width + ix] = srcimage->imageData[iy*srcimage->widthStep + ix * 3 + 2];

		//		int r = (uchar)srcimage->imageData[iy*srcimage->widthStep + ix * 3 + 2];
		//		int g = (uchar)srcimage->imageData[iy*srcimage->widthStep + ix * 3 + 1];
		//		int b = (uchar)srcimage->imageData[iy*srcimage->widthStep + ix * 3];
		//		int rg = r - g;
		//		int rb = r - b;
		//		maskimage->imageData[iy*maskimage->width + ix] = 0;
		//		if (rg > 5 && rb > 5)
		//		{
		//			maskimage->imageData[iy*maskimage->width + ix] = 255;
		//		}
		//		//else
		//		//{
		//		//	maskimage->imageData[iy*maskimage->width + ix] = 0;
		//		//}

		//	}
		//}


		maskimage = &IplImage(img_h);
		cvResize(maskimage, maskimage_resize);
/*		cvShowImage("maskimage_resize", maskimage_resize);
		cvWaitKey(0)*/;


		Mat binaryimage_mat_ = cvarrToMat(maskimage_resize);
		Mat binaryimage_mat;
		Mat element_1 = getStructuringElement(MORPH_RECT, Size(10, 5));
		morphologyEx(binaryimage_mat_,binaryimage_mat, MORPH_DILATE, element_1);
		ROI_DILATE_image = &IplImage(binaryimage_mat);
		//cvShowImage("ROI_DILATE_image", ROI_DILATE_image);
		//cvWaitKey(0);

		CvSeq * contour = 0;
		CvSeq *contmax = 0;
		cvFindContours(ROI_DILATE_image, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		cvZero(dst);//清空数组 
		for (; contour != 0; contour = contour->h_next)
		{
			bool flag_small = false;
			CvRect aRect = cvBoundingRect(contour, 0);
			float h = aRect.height;
			float w = aRect.width;
			float ratio_R = h / w;
			float S_R = h*w;

#ifdef DEBUG_

			printf("ratio_R = %f;S_R = %f\n", ratio_R, S_R);
#endif
			if (ratio_R > 0.10&&ratio_R < 0.5&&S_R>3800 && S_R < 6000)//0609
																	   //if (ratio_R > 0.20&&ratio_R<0.35&&S_R>800 && S_R<2500)//删除宽高比例小于设定值的轮廓
																	   //if (ratio_R > 0.20&&ratio_R<0.35/*&&S_R>800 && S_R<2500*/)//chepai
			{
				plate_p.push_back(aRect);
				cvRectangle(ROI_image, cvPoint(aRect.x, aRect.y), cvPoint(aRect.x + aRect.width, aRect.y + aRect.height), CV_RGB(0, 255, 255), 1);

				//****************************候选区域处理*****************************//
				ROI_image_temp = cvCreateImage(cvSize(aRect.width*coef, aRect.height*coef), inputimage->depth, inputimage->nChannels);
				ROI_binary_temp = cvCreateImage(cvSize(aRect.width*coef, aRect.height*coef), inputimage->depth, inputimage->nChannels);
				//cvSetImageROI(inputimage, cvRect(aRect.x*coef, aRect.y*coef, aRect.width*coef, aRect.height*coef));
				//cvCopy(inputimage, ROI_image_temp);
				//cvResetImageROI(inputimage);

				cvSetImageROI(maskimage, cvRect(aRect.x*coef, aRect.y*coef, aRect.width*coef, aRect.height*coef));
				cvCopy(maskimage, ROI_image_temp);
				cvResetImageROI(maskimage);
				//cvShowImage("ROI_image_temp", ROI_image_temp);
#ifdef OPENCV_
				cv::Mat  img_grayt, img_graytt;
				img_grayt = cvarrToMat(ROI_image_temp);
				Mat element_t = getStructuringElement(MORPH_RECT, Size(1, 1));
				morphologyEx(img_grayt, img_graytt, MORPH_DILATE, element_t);
				//cv::imshow("tt", img_graytt);
#endif

				//******************有效区域提取*********************************//
				CvSeq * contour_temp = 0;
				cvThreshold(ROI_image_temp, ROI_binary_temp, 1, 255, CV_THRESH_OTSU);
				//cvThreshold(ROI_image_temp, ROI_binary_temp, 150, 255, CV_THRESH_BINARY);
				Mat ROI_char = cvarrToMat(ROI_binary_temp);
				Mat ROI_binary_char;
				ROI_char.copyTo(ROI_binary_char);

				ROI_char = img_graytt;
				//*****************************************************************//
				//连通域计算  
				cv::Mat  labels, stats, centroids, img_color, img_gray, img_gray1, img_gray2;
				int nccomps = cv::connectedComponentsWithStats(
					ROI_char, //二值图像  
					labels,     //和原图一样大的标记图  
					stats, //nccomps×5的矩阵 表示每个连通区域的外接矩形和面积（pixel）  
					centroids //nccomps×2的矩阵 表示每个连通区域的质心  
					);
				//显示原图统计结果  
				char title[1024];
				//sprintf(title, "原图中连通区域数：%d\n", nccomps);
				//cv::String num_connect(title);
				//cv::imshow("t", ROI_char);

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
				img_color = cv::Mat::zeros(ROI_char.size(), CV_8UC3);
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
				//cv::threshold(img_gray, img_gray, 0, 255, cv::THRESH_BINARY);//注意二值化数值
			    //************************图像预处理*******************************//
#ifdef OPENCV_
				//Mat element_ = getStructuringElement(MORPH_RECT, Size(3, 2));
				//morphologyEx(img_gray2, img_gray, MORPH_OPEN, element_);
#endif

#ifdef MY_

				IplImage* pword_ = &IplImage(img_gray);
				int Se[2] = { 12,12 };
				int Ap[2] = { 2,2 };
				sk_image_Imerode((unsigned char*)pword_->imageData, pword_->height, pword_->width, Se, Ap, (unsigned char*)ROI_binary_temp->imageData);

				for (int i = 0; i < ROI_binary_temp->height*ROI_binary_temp->width; ++i)
				{
					if (ROI_binary_temp->imageData[i])    ROI_binary_temp->imageData[i] = (char)255;
				}

				//cvShowImage("erode", ROI_binary_temp);
				//cvWaitKey(0);
				img_gray = cvarrToMat(ROI_binary_temp);

#endif

#ifdef DEBUG_
				//cv::imshow("预处理", img_gray);
				//cv::waitKey(0);
#endif
				//****************************************************************//

				nccomps = cv::connectedComponentsWithStats(img_gray, labels, stats, centroids);

				//sprintf(title, "过滤小目标后的连通区域数量：%d\n", nccomps);
				//num_connect = title;
				//cv::imshow("tt", img_color);
				//cv::waitKey(0);

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

#ifdef DEBUG_
					printf("ratio_ = %f;S_ = %f\n", ratio_, S_);
#endif
					R_rect tempR;
					tempR.rect = aRect_;
					tempR.label = i2;
					if (ratio_ > 1.1&&ratio_ < 4.5&&S_>1100 && S_ < 10000)//设定参数
					{
						char_rect.push_back(tempR);
						num_contour++;
					}
				}

#ifdef DEBUG_
				printf("contour_num=%d\n", num_contour);
#endif
				if (num_contour >= 8)
				{
					printf("start recognition\n");

					rect_rank_x(char_rect);
					//*************************识别第一个字符****************************//
					Mat ROI_binary_char_;
					img_gray.copyTo(ROI_binary_char_);
					Mat mat_char = ROI_binary_char_(Rect(char_rect[0].rect.x, char_rect[0].rect.y, char_rect[0].rect.width, char_rect[0].rect.height));
					Mat imgresize, dst;
					resize(mat_char, imgresize, Size(resize_COL, resize_ROW), 0, 0, INTER_LINEAR);
					IplImage* pword_ = &IplImage(imgresize);
					for (int ti = 0; ti < imgresize.rows; ti++)
					{
						for (int tj = 0; tj < imgresize.cols; tj++)
						{
							Char_m[ti + tj*resize_ROW] = (unsigned char)pword_->imageData[ti*pword_->widthStep + tj];
							//printf("%d ", Char_m[ti + tj*resize_ROW]);
						}
						//printf("\n");
					}
					char out_word = WordsRecog_lu(Char_m, resize_COL, resize_ROW, 1);


#ifdef DEBUG_
					printf("flag_word = %c\n", out_word);
#endif
					plate.push_back(out_word);
					//*****************************************************************//
					float char_ratio;
					for (int i3 = 1; i3 < char_rect.size(); i3++)
					{
						char_ratio = char_rect[i3].rect.height*1.0 / char_rect[i3].rect.width;
						//printf("char_ratio = %f\n", char_ratio);
						//*****************识别*****************//

						ROI_binary_char_;
						img_gray.copyTo(ROI_binary_char_);

#ifdef SAVE_CHAR

						sprintf_s(savefile, "%s\\%s_%d_char_cour%d_c%d_%d.jpg", mkdirfile.c_str(), filepathlist[i + 1].c_str(), numframe, contour, contour_temp, i3);
						imwrite(savefile, ROI_binary_char_);
#endif

						mat_char = ROI_binary_char_(Rect(char_rect[i3].rect.x, char_rect[i3].rect.y, char_rect[i3].rect.width, char_rect[i3].rect.height));
						imgresize, dst;
						resize(mat_char, imgresize, Size(resize_COL, resize_ROW), 0, 0, INTER_LINEAR);
						//cv::imshow("mat_char", mat_char);
						//cv::waitKey(0);

						pword_ = &IplImage(imgresize);

						//cvShowImage("CHAR", pword_);
						//cvWaitKey(0);

						for (int ti = 0; ti < imgresize.rows; ti++)
						{
							for (int tj = 0; tj < imgresize.cols; tj++)
							{


								Char_m[ti + tj*resize_ROW] = (unsigned char)pword_->imageData[ti*pword_->widthStep + tj];

								if ((unsigned char)pword_->imageData[ti*pword_->widthStep + tj] == 1)
								{
									pword_->imageData[ti*pword_->widthStep + tj] = 255;
								}
								else
								{
									pword_->imageData[ti*pword_->widthStep + tj] = 0;
								}
							}

						}

#ifdef DEBUG_
						cvShowImage("CHAR", pword_);
						cvWaitKey(0);
#endif

						int flag_char = 0;
						if (out_word == 'D')
						{
							if (i3 == 1 || i3 == 3)
							{
								flag_char = 1;
							}
							else
							{
								flag_char = 3;
							}
							port = '0';
						}
						else // H开头
						{
							if (i3 == 1 || i3 == 2 || i3 == 4)
							{
								flag_char = 1;
							}
							else
							{
								flag_char = 3;
							}
						}


						char out_word2 = WordsRecog_lu(Char_m, resize_COL, resize_ROW, flag_char);
						if (flag_char == 3 && char_ratio > 2.2)//此处的char_ratio阈值设定受安装角度影响
						{
							out_word2 = '1';
						}

#ifdef DEBUG_
						printf("flag_word = %d %c\n", flag_char, out_word2);
#endif
						plate.push_back(out_word2);

						//*************************************//
					}
					if (plate.size() == 11)
					{
						port = '2';
					}
					if (plate.size() == 10)
					{

						{
							port = '1';
						}
					}
					//printf("port = %c\n", port);
					//printf("plate.size() = %d\n", plate.size());


#ifdef DEBUG_
					printf("\n");
#endif
				}
				//****************************************************************//	
#ifdef DEBUG_
				cvShowImage("ROI", ROI_image_temp);
				cvWaitKey(0);
#endif
				cvReleaseImage(&ROI_image_temp);
				cvReleaseImage(&ROI_binary_temp);
				//********************************************************************//
			}
		}

		
		//cvShowImage("inputimage", inputimage);
		//cvShowImage("maskimage", maskimage);
		//cvWaitKey(0);
#endif
	}


}


void portreco(IplImage *srcimage, char &port)
{
//#define GAMMA_
#ifdef GAMMA_

	Mat image = cvarrToMat(srcimage);
	Mat X;
	image.convertTo(X, CV_32FC1);
	//image.convertTo(X, CV_32F);
	// Start preprocessing:
	Mat I;
	float gamma = 4;
	pow(X, gamma, I);
	I = norm_0_255(I);
	//// Draw it on screen:
	//imshow("Original Image", image);
	//imshow("Gamma correction image", I);
	//// Show the images:
	//waitKey(10);

	srcimage = &IplImage(I);
	//cvShowImage("inputimage", inputimage);
	//cvWaitKey(0);
#endif

	if (srcimage->nChannels == 3)
	{

		for (int iy = 0; iy < srcimage->height; iy++)
		{
			for (int ix = 0; ix < srcimage->width; ix++)
			{
				inputimage->imageData[iy*inputimage->width + ix] = srcimage->imageData[iy*srcimage->widthStep + ix * 3 + 2];
			}
		}

		//cvShowImage("inputimage", inputimage);
		//cvWaitKey(10);

		//cvResize(inputimage, curframet);
	}
	else
	{
		cvCopy(srcimage, inputimage);
		//cvResize(inputimage, curframet);
	}

	cvResize(inputimage, curframet_port);
	cvSmooth(curframet_port, image_save_port, 2, 5);
	cvSetImageROI(image_save_port, cvRect(ROI_rect_M_port.x, ROI_rect_M_port.y, ROI_rect_M_port.w, ROI_rect_M_port.h));
	cvCopy(image_save_port, ROI_image_port);
	cvResetImageROI(image_save_port);

	if (inputimage->nChannels == 3)
	{
		cvCvtColor(ROI_image_port, ROI_grayimage_port, CV_BGR2GRAY);
		cvThreshold(ROI_grayimage_port, ROI_binaryimage_port, 1, 255, CV_THRESH_OTSU);
	}
	else
	{
		//cvThreshold(ROI_image, ROI_binaryimage, 50, 255, CV_THRESH_BINARY);
		cvThreshold(ROI_image_port, ROI_binaryimage_port, 1, 255, CV_THRESH_OTSU);
	}

	//*****************************************************************//
	//连通域计算  
	Mat ROI_char = cvarrToMat(ROI_binaryimage_port);
	Mat ROI_binary_char;
	ROI_char.copyTo(ROI_binary_char);
	cv::Mat  labels, stats, centroids, img_color, img_gray;
	int nccomps = cv::connectedComponentsWithStats(
		ROI_char, //二值图像  
		labels,     //和原图一样大的标记图  
		stats, //nccomps×5的矩阵 表示每个连通区域的外接矩形和面积（pixel）  
		centroids //nccomps×2的矩阵 表示每个连通区域的质心  
		);
	//显示原图统计结果  
	char title[1024];
	//sprintf(title, "原图中连通区域数：%d\n", nccomps);
	cv::String num_connect(title);
	//cv::imshow(num_connect, ROI_char);

	//去除过小区域，初始化颜色表  
	vector<cv::Vec3b> colors(nccomps);
	colors[0] = cv::Vec3b(0, 0, 0); // background pixels remain black.  
	for (int i1 = 1; i1 < nccomps; i1++)
	{
		colors[i1] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
		//去除面积小于100的连通域  
		if (stats.at<int>(i1, cv::CC_STAT_AREA) < 50 || stats.at<int>(i1, cv::CC_STAT_AREA) > 500)//调整参数,此处可以处理判断端号
			colors[i1] = cv::Vec3b(0, 0, 0); // small regions are painted with black too.  
	}
	//按照label值，对不同的连通域进行着色  
	img_color = cv::Mat::zeros(ROI_char.size(), CV_8UC3);
	for (int y = 0; y < img_color.rows; y++)
		for (int x = 0; x < img_color.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			img_color.at<cv::Vec3b>(y, x) = colors[label];
		}

	//统计降噪后的连通区域  
	cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);
	cv::threshold(img_gray, img_gray, 0, 255, cv::THRESH_BINARY);//注意二值化数值
																 //************************图像预处理*******************************//
																 //Mat element_ = getStructuringElement(MORPH_RECT, Size(20, 15));
																 //morphologyEx(img_gray, img_gray, MORPH_OPEN, element_);
																 //cv::imshow(num_connect, img_gray);
																 //cv::waitKey(0);
																 //****************************************************************//
																 //sprintf_s(savefile, "%s\\%s_%d_char_cour%d_c%d.jpg", mkdirfile.c_str(), filepathlist[i + 1].c_str(), numframe, contour, contour_temp);
																 //imwrite(savefile, img_gray);

	nccomps = cv::connectedComponentsWithStats(img_gray, labels, stats, centroids);
	//sprintf(title, "过滤小目标后的连通区域数量：%d\n", nccomps);
	//num_connect = title;
	//cv::imshow(num_connect, img_color);
	//cv::waitKey(10);

	vector<R_rect>char_rect;
	int num_contour = 0;
	for (int i2 = 1; i2 < nccomps; i2++)
	{
		CvRect aRect_;
		aRect_.x = stats.at<int>(i2, cv::CC_STAT_LEFT);
		aRect_.y = stats.at<int>(i2, cv::CC_STAT_TOP);
		aRect_.width = stats.at<int>(i2, cv::CC_STAT_WIDTH);
		aRect_.height = stats.at<int>(i2, cv::CC_STAT_HEIGHT);

		CvPoint center;
		center.x = centroids.at<double>(i2, 0);
		center.y = centroids.at<double>(i2, 1);
		printf("center.x = %d;center.y = %d\n", center.x, center.y);
		//cvCircle(ROI_image, center, 12, cvScalar(0, 0, 0), CV_FILLED, 8, 0);

		float ratio_ = (float)aRect_.height / (float)aRect_.width;
		float S_ = aRect_.height*aRect_.width;
		printf("ratio_ = %f;S_ = %f\n", ratio_, S_);
		R_rect tempR;
		tempR.rect = aRect_;
		tempR.label = i2;
		tempR.center = center;
		if (ratio_ > 1.2&&ratio_ < 4.5&&S_>200 && S_ < 800 && aRect_.height < 40 && aRect_.height>15)//设定参数判断是I还是II
		{
			char_rect.push_back(tempR);
			num_contour++;
		}
	}
	if (num_contour>2)
	{
		port = 'II';
	} 
	else
	{
		port = 'I';
	}
	//printf("duan hao = %d\n", num_contour);
}
