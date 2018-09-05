#include <math.h>
#include "lpr.h"

FILE* fp;

static  int g_doublePlateSupport = 0;

static  int g_enableDefaultProvince = 0;//enable default province setting
static int g_bInited = 0;  //initialized flag
static int g_iRecogArea = 0;  //recog area count
							  //static int g_iRecogFlag = 0;  //
static int g_iDetectFlag = 0;   //detect flag
static int g_iComplexBGFlag = 0;  //complex background flag
static int g_iBackLight = 1;     //back light setting

static int g_iBackLightFlag = 0;   //back light flag


static int g_iPlateNum = 0;        //plate count
static int g_pPlatePos[6][4] = { 0 };  //plate position


extern int extHeap, intHeap;

static int     g_MaxCol;   //max col width
						   //#else	//这部分数组定义同上

typedef unsigned char Uint8;
typedef unsigned int  Uint32;

extern const short Specialchinese_net[15254];

//	static Uint8 ** bwMtr;

int **    Csum;
static Uint8 **  BGgrayMtr[3];
Uint8 ** grayMtr;       // 用来存放灰度图的矩阵

static Uint8 ** plateMtr_temp;
//	static Uint8 ** grayMtr_bak;
static Uint8 ** houghMtr;
static Uint8 ** bigMtr_1;
static Uint8 ** bigMtr_2;
static Uint8 ** bigMtr_3;
static Uint8 ** bigMtr_4;
Uint8 ** plateMtr;
static Uint8 ** plateMtr_bak;
static Uint8 ** plateMtr_bak2;
static int * pRow_1;
static int * pRow_2;
static int * pCol_1;
static int * pCol_2;
static int * pCol_3;
//	static int * pCol_4;
static int * plateHistGram;
static int * pInt256_1;
static int * pInt256_2;
static int * pInt256_3;
static int * pInt256_4;
static int * pInt300;
static int * rgb_r;
static int * rgb_g;
static int * rgb_b;
static int * hsv_h;
static int * hsv_s;
static int * hsv_v;

static float * pFlt60;
static float * pFlt256;
static short * pWord;

int _max2(int v1, int v2)  //求较大值
{
	return v1 > v2 ? v1 : v2;
}

int _min2(int v1, int v2)  //求较小值
{
	return v1 < v2 ? v1 : v2;
}

int _abs(int v)  //求绝对值
{
	return v >= 0 ? v : -v;
}

unsigned int _mpyu(int v1, int v2)  //无符号乘法
{
	unsigned short m, n;

	m = (unsigned short)v1;
	n = (unsigned short)v2;

	return m*n;
}

int _mpy(int v1, int v2)//乘法
{
	short m, n;
	m = (short)v1;
	n = (short)v2;

	return m*n;
}
//#endif

static char ReleaseDate[] = "libLPR 2.0.1 BUILD 2010.06.23"; //version info //版本信息

static char Default_Province[3] = "沪";  //default province //默认省份
static char Default_EngCharacter = 'A';  //default character // 用于电警时黄色双行牌照的默认当地分配字符
static float OCR_minP = 10.0f;  // ocr min value // OCR 准则，卡口的参考值为 9.0, 3G电警为 5.5, 高清为 11.0或更高 ;
static float Big_minP = 14.0f;  //double plate ocr min value // 两行式牌照的 OCR 值，默认为 7.0, 卡口中一般可以设置到12.0
static float SensitiveK = 0.3f; //Sensitive rate
static int AdaptiveParameter = 0; //adaptive param flag  // 跟自适应参数有关
static int MaxPlateWidth = 160;  //max plate widht // 牌照-++++++的最大宽度，关系到纵向定位的准确性！
static int MaxPlateHeight = 40;   //max plate height //号牌最大高度

static int HighResolution = 1;    //default 1 // 取值为 1 时，用于高清；为 0 时，用于普通3G电警和卡口
static int ColorFilter = 0;       //color filter // ColorFilter=1 则进行颜色过滤
static int DEBUG_LPR = 0;  //debug flag //调试标识
static int ChangeLaneFlag = 0; //lane flag //车道标识
static int UpRowCut = 0;		//up cut lines	 // 行上预割量
static int DownRowCut = 0;		//down cut lines  // 行下预割量
static int LeftColCut = 0;		//left cut cols // 列左预割量
static int RightColCut = 0;		//right cut cols // 列右预割量
static int ShearTransformState = 0; //transform state
static int RefShearAngle = 0; //transfor angle
static int UpRowCut_x[2] = { 0,0 }; //up cut lines array
static int DownRowCut_x[2] = { 0,0 }; //down cut lines array
static int ShearTransformState_x[2] = { 0,0 }; //transform states array
static int RefShearAngle_x[2] = { -5,-5 }; //transfor angles array

static float AVG_BackGroundGrayLevel = 0.0f;//avg background gray value//背景灰度平均值
static float AVG_S = 0.0f;//avg s value //S平均值
static float AVG_V = 0.0f;//avg v value //V平均值
static float ForeGroundAverageGrayLevel = 0.0f;//fore ground gray value //前景灰度平均值
static float TOTAL_P = 0.0f;//total rate //整体置信度
static float SumPlateWidth = 0.0f;//sum of plate width //统计的总号牌宽度
static float SumPlateHeight = 0.0f;//sum of plate height //统计的总号牌高度
static float CurrentRotateAngle = 0.0f;//current rotate angle //当前旋转角度
static float SumRotateAngle = 0.0f;//sum of rotate angle //统计的总旋转角度

static int AVG_HSV[3];//avg hsv value //HSV分量平均值
static int ReadCofingParaState = 1;//read config paras flag //读取配置文件参数
static int MaxImBufRow = 3000;//allow max img height  //最大图片高度
static int MaxImBufCol = 3000;//allow max img width //最大图片宽度
static int ImBufRow = 0;//img height //图片高度
static int ImBufCol = 0;//img width //图片宽度
//static int resize_ROW = 20;	//resize height	 // 归一化后字符切片的高 转移到头文件
//static int resize_COL = 15;	//resize width	 // 归一化后字符切片的宽
static int CntPics = 0;		//pictures counter     // 跟自适应参数有关

static int IsWJPlate = 0;//wjplate flag //是否武警号牌
static int RealPlateWidth;//real plate width //实际得到的号牌宽度
static int RealPlateHeight;//real plate height //实际得到的号牌高度
static int SafeModeState = 0;//safe mode flag //安全模式标识
static int PlateStructure = 1;  //plate structure // 号牌结构变量，0=其他，1=单排，2=武警，3=警用，4=双排	
static int IsLocalPlate = 1;   //local plate flag // 当本地号牌占总数达到 80% 以上时，取值为1;

void InitNetData()
{
}

void imresize(Uint8 ** BW, int * matrix_info, short * out, int ROW, int COL)  // 特别注意：本程序只将图像归一化至 30 个像素以内！
{


	int row, col;
	int zr, zc, cnt;   // float zr=float(row)/ROW, zc=float(col)/COL;
	int a[20] = { 0 }, b[15] = { 0 };
	int i = 0, j = 0;
	row = matrix_info[1] - matrix_info[0] + 1;
	col = matrix_info[3] - matrix_info[2] + 1;
	zr = row * 16384 / ROW;
	zc = col * 16384 / COL;
	for (i = 0; i<ROW - 7; i += 8)
	{
		a[i] = ((zr >> 1) + i*zr + 2) >> 14;
		a[i + 1] = ((zr >> 1) + (i + 1)*zr + 2) >> 14;
		a[i + 2] = ((zr >> 1) + (i + 2)*zr + 2) >> 14;
		a[i + 3] = ((zr >> 1) + (i + 3)*zr + 2) >> 14;

		a[i + 4] = ((zr >> 1) + (i + 4)*zr + 2) >> 14;
		a[i + 5] = ((zr >> 1) + (i + 5)*zr + 2) >> 14;
		a[i + 6] = ((zr >> 1) + (i + 6)*zr + 2) >> 14;
		a[i + 7] = ((zr >> 1) + (i + 7)*zr + 2) >> 14;
	}
	for (; i<ROW; i++)
	{
		a[i] = ((zr >> 1) + i*zr + 2) >> 14;
	}

	for (i = 0; i<COL - 7; i += 8)
	{
		b[i] = ((zc >> 1) + i*zc + 2) >> 14;
		b[i + 1] = ((zc >> 1) + (i + 1)*zc + 2) >> 14;
		b[i + 2] = ((zc >> 1) + (i + 2)*zc + 2) >> 14;
		b[i + 3] = ((zc >> 1) + (i + 3)*zc + 2) >> 14;

		b[i + 4] = ((zc >> 1) + (i + 4)*zc + 2) >> 14;
		b[i + 5] = ((zc >> 1) + (i + 5)*zc + 2) >> 14;
		b[i + 6] = ((zc >> 1) + (i + 6)*zc + 2) >> 14;
		b[i + 7] = ((zc >> 1) + (i + 7)*zc + 2) >> 14;
	}
	for (; i<COL; i++)
	{
		b[i] = ((zc >> 1) + i*zc + 2) >> 14;
	}


	cnt = -1;
	for (j = 0; j<COL; ++j)  // 类似于 MM(:,1)=word(1:end)';
	{/*
	 for (i=0;i<ROW;++i)
	 {
	 ++cnt;
	 out[cnt]=BW[a[i]+matrix_info[0]][b[j]+matrix_info[2]];
	 }*/
		for (i = 0; i<ROW - 7; i += 8)
		{
			out[cnt + 1] = BW[a[i] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 2] = BW[a[i + 1] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 3] = BW[a[i + 2] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 4] = BW[a[i + 3] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 5] = BW[a[i + 4] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 6] = BW[a[i + 5] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 7] = BW[a[i + 6] + matrix_info[0]][b[j] + matrix_info[2]];
			out[cnt + 8] = BW[a[i + 7] + matrix_info[0]][b[j] + matrix_info[2]];

			cnt += 8;
		}
		for (; i<ROW; i++)
		{
			cnt++;
			out[cnt] = BW[a[i] + matrix_info[0]][b[j] + matrix_info[2]];
		}
	}
}
//init int array
void InitiateIntArray(int *array_in, int LengthOfArray)  // 对一个数列初始化
{
	/*	int i=0;

	for (i=0;i<LengthOfArray;++i)
	array_in[i]=0;*/
	memset(array_in, 0, LengthOfArray*sizeof(int));
}
int SumIntMatrix(Uint8 ** Matrix, int a1, int a2, int a3, int a4, int * E, int direction)//calculate the sum of int array
{
	int i = 0, j = 0, k = 0;
	int total;
	int s1 = 0, s2 = 0, s3 = 0, s4 = 0;

	if (E == NULL)  // 此榭霆对指定区域求和，返回一个值
	{
		total = 0;
		if ((a3 || a4) == 0)  // 在这种情况下就是对整个矩阵求和 a1=row, a2=col
		{
			for (i = 0; i<a1; ++i)
			{
				for (j = 0; j<a2 - 3; j += 4)
				{
					s1 += Matrix[i][j];
					s2 += Matrix[i][j + 1];
					s3 += Matrix[i][j + 2];
					s4 += Matrix[i][j + 3];

					//total+=(s1+s2)+(s3+s4);
				}
				for (k = j; k<a2; k++)
				{
					s1 += Matrix[i][k];
				}
			}
			total = (s1 + s2) + (s3 + s4);
			/*for (i=0;i<a1;++i)
			for (j=0;j<a2;++j)
			total+=Matrix[i][j];*/
		}
		else  // 在这种情况下实现对指定范围内的行、列求和 a1=start_row, a2=end_row, a3=start_col, a4=end_col
		{
			s1 = s2 = s3 = s4 = 0;
			for (i = a1; i <= a2; ++i)
			{
				for (j = a3; j <= a4 - 3; j += 4)
				{
					s1 += Matrix[i][j];
					s2 += Matrix[i][j + 1];
					s3 += Matrix[i][j + 2];
					s4 += Matrix[i][j + 3];

					//total+=(s1+s2)+(s3+s4);
				}
				for (k = j; k <= a4; k++)
				{
					s1 += Matrix[i][k];
				}
			}
			total = (s1 + s2) + (s3 + s4);
			/*for (i=a1;i<=a2;++i)
			for (j=a3;j<=a4;++j)
			total+=Matrix[i][j];*/
		}
		return total;
	}
	else  // 此情况为对指定区域按行（或列）的方向求和，返回一个数组
	{
		switch (direction)
		{
		case 0: // default method 对数据按列进行求和
		{
			InitiateIntArray(E, a4 - a3 + 1);  // 为了防止不经意间忘记对存放的数据组清零
			for (j = a3; j <= a4; ++j)
			{
				s1 = s2 = s3 = s4 = 0;
				for (i = a1; i <= a2 - 3; i += 4)
				{
					s1 += Matrix[i][j];
					s2 += Matrix[i + 1][j];
					s3 += Matrix[i + 2][j];
					s4 += Matrix[i + 3][j];

					//E[j-a3]+=(s1+s2)+(s3+s4);
				}
				for (k = i; k <= a2; k++)
				{
					s1 += Matrix[k][j];
				}
				E[j - a3] += (s1 + s2) + (s3 + s4);
			}/*
			 for (j=a3;j<=a4;++j)
			 for (i=a1;i<=a2;++i)
			 {
			 E[j-a3]+=Matrix[i][j];
			 }*/
		}
		break;
		case 1:  // 对矩阵按行方向求和
		{
			InitiateIntArray(E, a2 - a1 + 1);   // 为死不经意间忘记对存放的数据组清零
			for (i = a1; i <= a2; ++i)
			{
				s1 = s2 = s3 = s4 = 0;
				for (j = a3; j <= a4 - 3; j += 4)
				{
					s1 += Matrix[i][j];
					s2 += Matrix[i][j + 1];
					s3 += Matrix[i][j + 2];
					s4 += Matrix[i][j + 3];

					//E[i-a1]+=(s1+s2)+(s3+s4);
				}
				for (k = j; k <= a4; k++)
				{
					s1 += Matrix[i][k];
				}
				E[i - a1] += (s1 + s2) + (s3 + s4);
			}/*
			 for (i=a1;i<=a2;++i)
			 for (j=a3;j<=a4;++j)
			 E[i-a1]+=Matrix[i][j];*/
		}
		break;
		}
	}
	return 0;
}
//find the average value of a int array
float IntMEAN(int *array_in, int LengthOfArray)
{
	int sum_array = 0;
	int i = 0;
	int s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0, s8 = 0;
	/*
	for (i=0;i<LengthOfArray;++i)
	{
	sum_array+=array_in[i];
	}*/

	for (i = 0; i<LengthOfArray - 7; i += 8)
	{
		s1 = array_in[i];
		s2 = array_in[i + 1];
		s3 = array_in[i + 2];
		s4 = array_in[i + 3];

		s5 = array_in[i + 4];
		s6 = array_in[i + 5];
		s7 = array_in[i + 6];
		s8 = array_in[i + 7];

		sum_array += s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8;
	}
	for (; i<LengthOfArray; i++)
	{
		sum_array += array_in[i];
	}

	return (float)sum_array / (float)(LengthOfArray);
}

void SlimWord(Uint8 ** BW, int * matrix_info)//slim each word
{
	int row, col;
#ifdef DSP
	int * restrict E;
#else
	int * E = NULL;
#endif
	int i = 0, j = 0;
	int meanE;

	row = matrix_info[1] - matrix_info[0] + 1;
	col = matrix_info[3] - matrix_info[2] + 1;
	E = pCol_2;		//	E=(int *) MEM_calloc(extHeap, col* sizeof(int), 128);
	//E = (int *)malloc(col* sizeof(int));
	memset(E, 0, col* sizeof(int));
	for (j = matrix_info[2]; j <= matrix_info[3]; ++j)
	{
		for (i = matrix_info[0]; i <= matrix_info[1]; ++i)
			E[j - matrix_info[2]] += BW[i][j];
	}

	meanE = (int)(112.64f*IntMEAN(E, col)); //meanE=0.11f*IntMEAN(E,col); 以下为对切片的左右边界进行处理
	for (i = 0; i<col; ++i)
	{
		if ((E[i] << 10)<meanE)
			++matrix_info[2];
		else
			break;
	}

	for (i = col - 1; i>-1; --i)
	{
		if ((E[i] << 10)<meanE)
			--matrix_info[3];
		else
			break;
	}
	//	MEM_free(extHeap, E, col* sizeof(int));

	memset(E, 0, row* sizeof(int));		//	E=(int *) MEM_calloc(extHeap, row* sizeof(int), 128);  // 下面是对切片的上下边界进行休整，去掉没有字符的区域
	SumIntMatrix(BW, 0, row - 1, matrix_info[2], matrix_info[3], E, 1);
	for (i = 0; i<row; ++i)
	{
		if (E[i]<1)
			++matrix_info[0];
		else
			break;
	}
	for (i = row - 1; i>-1; --i)
	{
		if (E[i]<1)
			--matrix_info[1];
		else
			break;
	}
	//	MEM_free(extHeap, E, row* sizeof(int));
}
void GetNNetInfo(int net_type, int *net_IW, int *net_LW)//get ANNs's info
{
	switch (net_type)
	{
	case 0:  // 中文网络 参数
	{
		*net_IW = 300;
		*net_LW = 32;
		break;
}
	case 1:  // 英文网络 参数
	{
		*net_IW = 240;
		*net_LW = 26;
		break;
	}
	case 2:  // 分类网络参数
	{
		*net_IW = 240;
		*net_LW = 2;
		break;
	}
	case 3: // 数字网络参数
	{
		*net_IW = 140;
		*net_LW = 10;
		break;
	}
	case 4: // 特殊网络参数
	{
		*net_IW = 160;
		*net_LW = 21;
		break;
	}
	case 5: // 新军牌第一位字母
	{
		*net_IW = 140;
		*net_LW = 13;
		break;
	}
	case 6: // 特殊中文网络参数，4个汉字输出
	{
		*net_IW = 50;
		*net_LW = 4;
		break;
	}
	}
}
//init float array
void InitiateFloatArray(float *array_in, int LengthOfArray)  // 对一个数列初始化
{
	/*	int i=0;

	for (i=0;i<LengthOfArray;++i)
	array_in[i]=0;*/
	memset(array_in, 0, LengthOfArray*sizeof(int));
}

//find the max value of a float array
float MAXFloatArray(float *array, int LengthOfArray, int *location) // 寻找并返回最大值,及其所在位置
{
	int LOCATION = 0;
	int i = 1;
	float temp;

	temp = *array;
	if (LengthOfArray > 1)
	{
		for (i = 1; i < LengthOfArray; ++i)
		{
			if (temp < *(array + i))
			{
				temp = *(array + i);
				LOCATION = i;  //  最大值所处位置
			}
		}
	}

	if (location != NULL)
	{
		*location = LOCATION;
	}
	return temp; //  最大值
}

int CalculateNetData(const short *net, int net_IW, int net_LW, const short * word, int word_length, float *nP)
{
	int i = 0, j = 0, cnt = 0, temp = 0, location = 0, m1 = 0, m2 = 0;
	int Ib_index, Lb_index, LW_index;
	int *E1 = pInt300;
	float *E2 = pFlt60;

	InitiateIntArray(E1, net_IW);
	Ib_index = net_IW*word_length;  // 第一层网络的 b 数据起始地址

#pragma MUST_ITERATE (100);
	for (i = 0; i < net_IW; ++i)
	{
#pragma MUST_ITERATE (100);
		for (j = 0; j < word_length - 1; j += 2)
		{
			m1 += word[j] * net[cnt];  // 网络输入与网络权值相乘，得到输入和
			m2 += word[j + 1] * net[cnt + 1];
			cnt += 2;
			//			++cnt;
		}
		//		E1[i]+=m1;	
		E1[i] += (m1 + m2);	//特别注意！这里没有考虑word_length为奇数时的情况，代码不具有通用性！
		m1 = m2 = 0;
		E1[i] += net[Ib_index + i] + 20480; // 按照 2^11 的10倍
		if (E1[i] > 40960)	E1[i] = 40960;
		if (E1[i] < 0)	E1[i] = 0;
		E1[i] = fst_net[E1[i]];
	}

	LW_index = Ib_index + net_IW;  // 第二层神经网络的 LW 数据起始地址
	InitiateFloatArray(E2, net_LW);
	Lb_index = LW_index + net_IW*net_LW;  // 第二层网络的 b 数据起始地址
	cnt = -1;
	for (i = 0; i < net_LW; ++i)
	{
		temp = 0;
#pragma MUST_ITERATE (100);
		for (j = 0; j < net_IW; ++j)
		{
			++cnt;
			temp += E1[j] * net[cnt + LW_index];
		}
		temp = (temp / 2048) + net[Lb_index + i] + 20480;
		if (temp > 40960)	temp = 40960;
		if (temp < 0)	temp = 0;
		E2[i] = scd_net[temp];
	}

	if (nP != NULL)
		*nP = MAXFloatArray(E2, net_LW, &location);  // 求取最大值的位置处，可能和 compet 函数有些出入，以后修改
	else
		MAXFloatArray(E2, net_LW, &location);

	return location;
}
char GetJunFisrtCharacter(int index)//get army plate character
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = 'B';
		break;
	}
	case 1:
	{
		result = 'C';
		break;
	}
	case 2:
	{
		result = 'G';
		break;
	}
	case 3:
	{
		result = 'H';
		break;
	}
	case 4:
	{
		result = 'J';
		break;
	}
	case 5:
	{
		result = 'K';
		break;
	}
	case 6:
	{
		result = 'L';
		break;
	}
	case 7:
	{
		result = 'N';
		break;
	}
	case 8:
	{
		result = 'S';
		break;
	}
	case 9:
	{
		result = 'V';
		break;
	}
	case 10:
	{
		result = 'W';
		break;
	}
	case 11:
	{
		result = 'Z';
		break;
	}
	case 12:
	{
		result = '#';
		break;
	}
	default:
	{
		result = 'H';
		break;
	}
}
	return result;
}

char GetSpecialChineseCharacter(int index)//针对“新、川、豫、藏”特别训练的样本
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = 'W';//'新';
		break;
	}
	case 1:
	{
		result = '#';//'川';
		break;
	}
	case 2:
	{
		result = 'u';//'藏';
		break;
	}
	case 3:
	{
		result = 'M';//'豫';
		break;
	}
	}
	return result;
}
//************************************************************************************************************************************************
//************************************************************************************************************************************************
char GetSpecialCharacter(int index)//get special character,such as "gua"
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = '1';
		break;
	}
	case 1:
	{
		result = '2';
		break;
	}
	case 2:
	{
		result = '3';
		break;
	}
	case 3:
	{
		result = '4';
		break;
	}
	case 4:
	{
		result = '5';
		break;
	}
	case 5:
	{
		result = '6';
		break;
	}
	case 6:
	{
		result = '7';
		break;
	}
	case 7:
	{
		result = '8';
		break;
	}
	case 8:
	{
		result = '9';
		break;
	}
	case 9:
	{
		result = '0';
		break;
	}
	case 10:
	{
		result = 'a';  // 警
		break;
	}
	case 11:
	{
		result = 'b';  // 学
		break;
	}
	case 12:
	{
		result = 'c';  // 试
		break;
	}
	case 13:
	{
		result = 'c';  // 式---试
		break;
	}
	case 14:
	{
		result = 'd';  // 挂
		break;
	}
	case 15:
	{
		result = 'e';  // 消
		break;
	}
	case 16:
	{
		result = 'f';  // 边
		break;
	}
	case 17:
	{
		result = 'g';  // 林
		break;
	}
	case 18:
	{
		result = 'h';  // 电
		break;
	}
	case 19:
	{
		result = 'i';  // 港
		break;
	}
	case 20:
	{
		result = 'j';  // 澳
		break;
	}
	}
	return result;
}
//************************************************************************************************************************************************
//************************************************************************************************************************************************
char GetChineseCharacter(int index)//get chinese character
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = 'A';//'沪';
		break;
	}
	case 1:
	{
		result = 'B';//'苏';
		break;
	}
	case 2:
	{
		result = 'C';//'黑';
		break;
	}
	case 3:
	{
		result = 'D';//'浙';
		break;
	}
	case 4:
	{
		result = 'E';//'京';
		break;
	}
	case 5:
	{
		result = 'F';//'津';
		break;
	}
	case 6:
	{
		result = 'G';//'渝';
		break;
	}
	case 7:
	{
		result = 'H';//'吉';
		break;
	}
	case 8:
	{
		result = 'I';//'辽';
		break;
	}
	case 9:
	{
		result = 'J';//'晋';
		break;
	}
	case 10:
	{
		result = 'K';//'鲁';
		break;
	}
	case 11:
	{
		result = 'L';//'冀';
		break;
	}
	case 12:
	{
		result = 'M';//'豫';
		break;
	}
	case 13:
	{
		result = 'N';//'湘';
		break;
	}
	case 14:
	{
		result = 'O';//'鄂';
		break;
	}
	case 15:
	{
		result = 'P';//'蒙';
		break;
	}
	case 16:
	{
		result = 'Q';//'粤';
		break;
	}
	case 17:
	{
		result = 'R';//'桂';
		break;
	}
	case 18:
	{
		result = 'S';//'皖';
		break;
	}
	case 19:
	{
		result = 'T';//'闽';
		break;
	}
	case 20:
	{
		result = 'W';//'新';
		break;
	}
	case 21:
	{
		result = 'X';//'赣';
		break;
	}
	case 22:
	{
		result = 'Y';//'陕';
		break;
	}
	case 23:
	{
		result = 'Z';//'甘';
		break;
	}
	case 24:
	{
		result = '!';//'宁';
		break;
	}
	case 25:
	{
		result = '@';//'青';
		break;
	}
	case 26:
	{
		result = '#';//'川';
		break;
	}
	case 27:
	{
		result = '$';//'云';
		break;
	}
	case 28:
	{
		result = 'r';//'贵';
		break;
	}
	case 29:
	{
		result = 'u';//'藏';
		break;
	}
	case 30:
	{
		result = 'v';//'琼';
		break;
	}
	case 31:
	{
		result = '~';//'wrong';
		break;
	}
	}
	return result;
}
//************************************************************************************************************************************************
//************************************************************************************************************************************************
char GetEnglishCharacter(int index)//get english character
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = 'A';
		break;
	}
	case 1:
	{
		result = 'B';
		break;
	}
	case 2:
	{
		result = 'C';
		break;
	}
	case 3:
	{
		result = 'D';
		break;
	}
	case 4:
	{
		result = 'E';
		break;
	}
	case 5:
	{
		result = 'F';
		break;
	}
	case 6:
	{
		result = 'G';
		break;
	}
	case 7:
	{
		result = 'H';
		break;
	}
	case 8:
	{
		result = 'I';
		break;
	}
	case 9:
	{
		result = 'J';
		break;
	}
	case 10:
	{
		result = 'K';
		break;
	}
	case 11:
	{
		result = 'L';
		break;
	}
	case 12:
	{
		result = 'M';
		break;
	}
	case 13:
	{
		result = 'N';
		break;
	}
	case 14:
	{
		result = 'O';
		break;
	}
	case 15:
	{
		result = 'P';
		break;
	}
	case 16:
	{
		result = 'Q';
		break;
	}
	case 17:
	{
		result = 'R';
		break;
	}
	case 18:
	{
		result = 'S';
		break;
	}
	case 19:
	{
		result = 'T';
		break;
	}
	case 20:
	{
		result = 'U';
		break;
	}
	case 21:
	{
		result = 'V';
		break;
	}
	case 22:
	{
		result = 'W';
		break;
	}
	case 23:
	{
		result = 'X';
		break;
	}
	case 24:
	{
		result = 'Y';
		break;
	}
	case 25:
	{
		result = 'Z';
		break;
	}
	}
	return result;
}
//************************************************************************************************************************************************
//************************************************************************************************************************************************
char GetNumericCharacter(int index)//get digit character
{
	char result = '*';
	switch (index)
	{
	case 0:
	{
		result = '1';
		break;
	}
	case 1:
	{
		result = '2';
		break;
	}
	case 2:
	{
		result = '3';
		break;
	}
	case 3:
	{
		result = '4';
		break;
	}
	case 4:
	{
		result = '5';
		break;
	}
	case 5:
	{
		result = '6';
		break;
	}
	case 6:
	{
		result = '7';
		break;
	}
	case 7:
	{
		result = '8';
		break;
	}
	case 8:
	{
		result = '9';
		break;
	}
	case 9:
	{
		result = '0';
		break;
	}
	}
	return result;
}

char LookUpTable(int location, int net_type)//lookup table,recog the word
{
	char result = '*';
	switch (net_type)
	{
	case 0:   // 中文网络
	{
		result = GetChineseCharacter(location);
	}
	break;
	case 1:  // 英文网络
	{
		result = GetEnglishCharacter(location);
	}
	break;
	case 3:  // 数字网络
	{
		result = GetNumericCharacter(location);
	}
	break;
	case 4:  // 特殊网络
	{
		result = GetSpecialCharacter(location);
	}
	break;
	case 5:  // 新军牌第一个字母
	{
		result = GetJunFisrtCharacter(location);
	}
	break;
	case 6:  // 特殊中文网络，只输出4个汉字
	{
		result = GetSpecialChineseCharacter(location);
	}
	break;
	}
	return result;
}

char WordsRecog(Uint8 **BW, int up, int down, int left, int right, int net_type, float standard_P, int Height, float *nP, float *pWidth)
{
#ifdef DSP
	const short * restrict net = NULL;
	short * restrict word = pWord;
#else
	const short *net = NULL;
	short *word = pWord;
#endif
	int matrix_info[4] = { 0 };
	int net_IW = 0, net_LW = 0;
	int words_width, location, net_type_old;
	float mm, np_temp;
	char resultbuf, result_temp;

	net_type_old = net_type;

	matrix_info[0] = up;
	matrix_info[1] = down;
	matrix_info[2] = left;
	matrix_info[3] = right;
	SlimWord(BW, matrix_info);
	words_width = matrix_info[3] - matrix_info[2] + 1;
	//mm = (float)(words_width) / (standard_P*(float)Height);
	//if (pWidth != NULL)
	//{
	//	*pWidth = mm;
	//}

	//if ((mm<0.57f) && (net_type>1)) // 汉字,英文字符不参与宽度判断(即第一位第二位字符不参与)
	//{
	//	if (nP != NULL)
	//	{
	//		if (mm<0.3501f)
	//		{
	//			*nP = 0.9f;
	//		}
	//		else
	//			if (mm<0.4001f)
	//			{
	//				*nP = 0.85f;
	//			}
	//			else
	//			{
	//				*nP = 0.8f;
	//			}
	//	}
	//	return '1'; // 牌照忻挥蠭字和O字
	//}


	imresize(BW, matrix_info, word, resize_ROW, resize_COL);  // 对切片归一化大小，并按matlab的规则将其向量化			

	switch (net_type)
	{
	case 0:
	{
		net = chinese_net;
		break;
	}
	case 1:
	{
		net = english_net;
		break;
	}
	case 2:
	{
		net = class_net;
		break;
	}
	case 3:
	{
		net = numeric_net;
		break;
	}
	case 4:
	{
		net = special_net;
		break;
	}
	//case 5:
	//{
	//	net = junF_net;
	//	break;
	//}
	}
	GetNNetInfo(net_type, &net_IW, &net_LW);
	location = CalculateNetData(net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//if (net_type == 2)  // 分类的情况
	//{
	//	if (location == 0) // 英文,  
	//	{
	//		net_type = 1;
	//		GetNNetInfo(1, &net_IW, &net_LW);
	//		location = CalculateNetData(english_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);

	//	}
	//	else  // 数字, 
	//	{
	//		net_type = 3;
	//		GetNNetInfo(3, &net_IW, &net_LW);
	//		location = CalculateNetData(numeric_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//	}
	//}
	resultbuf = LookUpTable(location, net_type);
	//if (((resultbuf == 'Q') || (resultbuf == 'Z') || (resultbuf == 'T') || (resultbuf == 'D') || (resultbuf == 'B')) && (*nP<0.8) && (net_type_old == 2) && (net_type = 1))
	//{
	//	net_type = 3;
	//	GetNNetInfo(3, &net_IW, &net_LW);
	//	location = CalculateNetData(numeric_net, net_IW, net_LW, word, resize_ROW*resize_COL, &np_temp);
	//	result_temp = LookUpTable(location, net_type);
	//	if ((np_temp>0.85) && (np_temp>*nP))
	//	{
	//		resultbuf = result_temp;
	//		*nP = np_temp;
	//	}
	//}

	//if ((resultbuf == 'W') || (resultbuf == '#') || (resultbuf == 'u') || (resultbuf == 'M'))
	//{
	//	GetNNetInfo(6, &net_IW, &net_LW);
	//	location = CalculateNetData(Specialchinese_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//	result_temp = LookUpTable(location, 6);
	//	if (result_temp != resultbuf)
	//	{
	//		*nP = 0;
	//		resultbuf = '?';
	//	}
	//}

	return resultbuf;
}

char WordsRecog_lu(unsigned char *char_input, int char_w, int char_h, int net_type)
{
	float *nP = NULL;
	const short *net = NULL;
	short *word = pWord;
	int net_IW = 0, net_LW = 0;
	int words_width, location, net_type_old;
	float mm, np_temp;
	char resultbuf, result_temp;

	//fopen_s(&fp, "out.txt", "w");
	for (int i = 0; i < char_w; i++)
	{
		for (int j = 0; j < char_h; j++)
		{
			word[i*char_h + j] = char_input[i*char_h + j];
			//fprintf(fp, "%d ", word[i*char_h + j]);
		}
		//fprintf(fp, "\n");
	}
	//fclose(fp);

	net_type_old = net_type;

	switch (net_type)
	{
	case 0:
	{
		net = chinese_net;
		break;
	}
	case 1:
	{
		net = english_net;
		break;
	}
	case 2:
	{
		net = class_net;
		break;
	}
	case 3:
	{
		net = numeric_net;
		break;
	}
	case 4:
	{
		net = special_net;
		break;
	}
	}
	GetNNetInfo(net_type, &net_IW, &net_LW);
	location = CalculateNetData(net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//if (net_type == 2)  // 分类的情况
	//{
	//	if (location == 0) // 英文,  
	//	{
	//		net_type = 1;
	//		GetNNetInfo(1, &net_IW, &net_LW);
	//		location = CalculateNetData(english_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);

	//	}
	//	else  // 数字, 
	//	{
	//		net_type = 3;
	//		GetNNetInfo(3, &net_IW, &net_LW);
	//		location = CalculateNetData(numeric_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//	}
	//}
	resultbuf = LookUpTable(location, net_type);
	//if (((resultbuf == 'Q') || (resultbuf == 'Z') || (resultbuf == 'T') || (resultbuf == 'D') || (resultbuf == 'B')) && (*nP<0.8) && (net_type_old == 2) && (net_type = 1))
	//{
	//	net_type = 3;
	//	GetNNetInfo(3, &net_IW, &net_LW);
	//	location = CalculateNetData(numeric_net, net_IW, net_LW, word, resize_ROW*resize_COL, &np_temp);
	//	result_temp = LookUpTable(location, net_type);
	//	if ((np_temp>0.85) && (np_temp>*nP))
	//	{
	//		resultbuf = result_temp;
	//		*nP = np_temp;
	//	}
	//}

	//if ((resultbuf == 'W') || (resultbuf == '#') || (resultbuf == 'u') || (resultbuf == 'M'))
	//{
	//	GetNNetInfo(6, &net_IW, &net_LW);
	//	location = CalculateNetData(Specialchinese_net, net_IW, net_LW, word, resize_ROW*resize_COL, nP);
	//	result_temp = LookUpTable(location, 6);
	//	if (result_temp != resultbuf)
	//	{
	//		*nP = 0;
	//		resultbuf = '?';
	//	}
	//}

	return resultbuf;
}

//init int array
int InitiateIntMatrix(int **matrix, int row, int col)
{
	int i = 0;

#ifdef DSP
	matrix[0] = (int *)MEM_calloc(extHeap, row*col*sizeof(int), 4);
#else
	matrix[0] = (int *)malloc(row*col*sizeof(int));
#endif

	if (matrix[0] == NULL)
		return 0;

	for (i = 0; i < row; ++i)
	{
		matrix[i] = (int *)(matrix[0] + col * i);

		if (matrix[i] == NULL)
			return 0;
	}

	return 1;
}

//init uin8 array
int InitiateUInt8Matrix(Uint8 **matrix, int row, int col)
{
	int i = 0;

#ifdef DSP
	matrix[0] = (Uint8 *)MEM_calloc(extHeap, row*col*sizeof(Uint8), 4);
#else
	matrix[0] = (Uint8 *)malloc(row*col*sizeof(Uint8));
#endif

	if (matrix[0] == NULL)
		return 0;

	for (i = 0; i < row; ++i)
	{
		matrix[i] = (Uint8 *)(matrix[0] + col * i *sizeof(Uint8));

		if (matrix[i] == NULL)
			return 0;
	}

	//SetIntMatrix(matrix, row, col);
	return 1;
}

void SetIntMatrix(Uint8 **mtr, int row, int col)//init array
{
	int i;
#ifdef DSP
	for (i = 0; i < row; ++i)
	{
		memset(mtr[i], 0, sizeof(Uint8)*col);
	}
	/*	memset(mtr[0],0,row*col*sizeof(Uint8));	*/
#else
	for (i = 0; i < row; ++i)
	{
		memset(mtr[i], 0, sizeof(Uint8)*col);
	}
#endif

}

int InitiateMEM(int row, int col)//alloc memeory
{
	int state, colvu = (int)(col*0.5 + 0.0001);
	long long memCost = 0;


#ifdef DSP	
	Csum = (int **)MEM_calloc(extHeap, (300)*sizeof(int *), 4);
#else
	Csum = (int **)malloc((300)*sizeof(int *));
#endif
	if (Csum == NULL) return 0;
#ifdef DSP
	state = InitiateIntMatrix(Csum, 300, col);
#else
	state = InitiateIntMatrix(Csum, 300, col);
#endif

	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(300, sizeof(Uint8 *));
#else
	memCost += _mpy(300, sizeof(Uint8 *));
#endif
	memCost += _mpy(300, col);


#ifdef DSP	
	bigMtr_4 = (Uint8 **)MEM_calloc(extHeap, (300)*sizeof(Uint8 *), 4);
#else
	bigMtr_4 = (Uint8 **)malloc((300)*sizeof(Uint8 *));
#endif
	if (bigMtr_4 == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(bigMtr_4, 300, col);
#else
	state = InitiateUInt8Matrix(bigMtr_4, 300, col);
#endif

	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(300, sizeof(Uint8 *));
#else
	memCost += _mpy(300, sizeof(Uint8 *));
#endif
	memCost += _mpy(300, col);

#ifdef DSP
	BGgrayMtr[0] = (Uint8 **)MEM_calloc(extHeap, row*sizeof(Uint8 *), 4);
#else
	BGgrayMtr[0] = (Uint8 **)malloc(row*sizeof(Uint8 *));
#endif
	if (BGgrayMtr[0] == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(BGgrayMtr[0], row, col);
#else
	state = InitiateUInt8Matrix(BGgrayMtr[0], row, col);
#endif
	if (state == 0) return 0;

	SetIntMatrix(BGgrayMtr[0], row, col);

#ifdef DSP
	memCost = _mpy(row, sizeof(Uint8 *));
#else
	memCost = _mpy(row, sizeof(Uint8 *));
#endif
	memCost += _mpy(row, col);

#ifdef DSP
	BGgrayMtr[1] = (Uint8 **)MEM_calloc(extHeap, row*sizeof(Uint8 *), 4);
#else
	BGgrayMtr[1] = (Uint8 **)malloc(row*sizeof(Uint8 *));
#endif
	if (BGgrayMtr[1] == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(BGgrayMtr[1], row, col);
#else
	state = InitiateUInt8Matrix(BGgrayMtr[1], row, col);
#endif
	if (state == 0) return 0;

	SetIntMatrix(BGgrayMtr[1], row, col);

#ifdef DSP
	memCost = _mpy(row, sizeof(Uint8 *));
#else
	memCost = _mpy(row, sizeof(int *));
#endif
	memCost += _mpy(row, col);

#ifdef DSP
	BGgrayMtr[2] = (Uint8 **)MEM_calloc(extHeap, row*sizeof(Uint8 *), 4);
#else
	BGgrayMtr[2] = (Uint8 **)malloc(row*sizeof(Uint8 *));
#endif
	if (BGgrayMtr == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(BGgrayMtr[2], row, col);
#else
	state = InitiateUInt8Matrix(BGgrayMtr[2], row, col);
#endif
	if (state == 0) return 0;

	SetIntMatrix(BGgrayMtr[2], row, col);

#ifdef DSP
	memCost = _mpy(row, sizeof(Uint8 *));
#else
	memCost = _mpy(row, sizeof(Uint8 *));
#endif
	memCost += _mpy(row, col);



#ifdef DSP
	grayMtr = (Uint8 **)MEM_calloc(extHeap, row*sizeof(Uint8 *), 4);
#else
	grayMtr = (Uint8 **)malloc(row*sizeof(Uint8 *));
#endif
	if (grayMtr == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(grayMtr, row, col);
#else
	state = InitiateUInt8Matrix(grayMtr, row, col);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost = _mpy(row, sizeof(Uint8 *));
#else
	memCost = _mpy(row, sizeof(Uint8 *));
#endif
	memCost += _mpy(row, col);
	/*
	#ifdef DSP
	bwMtr=(Uint8 **)MEM_calloc(extHeap, row*sizeof(Uint8 *), 4);
	#else
	bwMtr = (Uint8 **) malloc(row*sizeof(int *));
	#endif
	if ( bwMtr==NULL ) return 0;
	#ifdef DSP
	state=InitiateUInt8Matrix(bwMtr,row,col);
	#else
	state=InitiateIntMatrix(bwMtr,row,col);
	#endif
	if (state==0) return 0;
	*/
#ifdef DSP
	plateMtr_temp = (Uint8 **)MEM_calloc(extHeap, (300)*sizeof(Uint8 *), 4);
#else
	plateMtr_temp = (Uint8 **)malloc((300)*sizeof(Uint8 *));
#endif

	if (plateMtr_temp == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(plateMtr_temp, 300, 300);
#else
	state = InitiateUInt8Matrix(plateMtr_temp, 300, 300);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(row / 3, sizeof(Uint8 *));
#else
	memCost += _mpy(row / 3, sizeof(Uint8 *));
#endif
	memCost += _mpy(row / 3, col / 3);


#ifdef DSP
	plateMtr_bak2 = (Uint8 **)MEM_calloc(extHeap, (300)*sizeof(Uint8 *), 4);
#else
	plateMtr_bak2 = (Uint8 **)malloc((300)*sizeof(Uint8 *));
#endif

	if (plateMtr_bak2 == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(plateMtr_bak2, 300, 300);
#else
	state = InitiateUInt8Matrix(plateMtr_bak2, 300, 300);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(row / 3, sizeof(Uint8 *));
#else
	memCost += _mpy(row / 3, sizeof(Uint8 *));
#endif
	memCost += _mpy(row / 3, col / 3);

	// 	if ( DEBUG_LPR==1 )
	// 	{
	// #ifdef DSP
	// 		grayMtr_bak=(Uint8 **)MEM_calloc(extHeap, row*col*sizeof(int *), 128);
	// #else
	// 		grayMtr_bak = (Uint8 **) malloc(row*col*sizeof(int *));
	// #endif
	// 		if ( grayMtr_bak==NULL ) return 0;
	// 		state=InitiateIntMatrix(grayMtr_bak,row,col);
	// 		if (state==0) return 0;		
	// 	}		

#ifdef DSP
	houghMtr = (Uint8 **)MEM_calloc(extHeap, 181 * sizeof(Uint8 *), 4);
#else
	houghMtr = (Uint8 **)malloc(181 * sizeof(Uint8 *));
#endif

	if (houghMtr == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(houghMtr, 181, col / 2);
#else
	state = InitiateUInt8Matrix(houghMtr, 181, col / 2);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(181, sizeof(Uint8 *));
#else
	memCost += _mpy(181, sizeof(Uint8 *));
#endif
	memCost += _mpy(181, col / 2);

#ifdef DSP
	bigMtr_1 = (Uint8 **)MEM_calloc(extHeap, (row)*sizeof(Uint8 *), 4);
#else
	bigMtr_1 = (Uint8 **)malloc((row)*sizeof(Uint8 *));
#endif
	if (bigMtr_1 == NULL) return 0;

#ifdef DSP
	state = InitiateUInt8Matrix(bigMtr_1, row, col);
#else
	state = InitiateUInt8Matrix(bigMtr_1, row, col);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy((row / 2), sizeof(Uint8 *));
#else
	memCost += _mpy((row / 2), sizeof(Uint8 *));
#endif
	memCost += _mpy((row / 2), col);

#ifdef DSP	
	bigMtr_2 = (Uint8 **)MEM_calloc(extHeap, ((row))*sizeof(Uint8 *), 4);
#else
	bigMtr_2 = (Uint8 **)malloc(((row))*sizeof(Uint8 *));
#endif
	if (bigMtr_2 == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(bigMtr_2, (row), col);
#else
	state = InitiateUInt8Matrix(bigMtr_2, (row), col);
#endif

	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy((row), sizeof(Uint8 *));
#else
	memCost += _mpy((row), sizeof(Uint8 *));
#endif
	memCost += _mpy((row), col);

#ifdef DSP	
	bigMtr_3 = (Uint8 **)MEM_calloc(extHeap, (row)*sizeof(Uint8 *), 4);
#else
	bigMtr_3 = (Uint8 **)malloc((row)*sizeof(Uint8 *));
#endif
	if (bigMtr_3 == NULL) return 0;

#ifdef DSP
	state = InitiateUInt8Matrix(bigMtr_3, row, col);
#else
	state = InitiateUInt8Matrix(bigMtr_3, row, col);
#endif
	if (state == 0) return 0;

	memCost += _mpy(row, col);

#ifdef DSP
	plateMtr = (Uint8 **)MEM_calloc(extHeap, (300)*sizeof(Uint8 *), 4);
#else
	plateMtr = (Uint8 **)malloc((300)*sizeof(Uint8 *));
#endif
	if (plateMtr == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(plateMtr, 300, 300);
#else
	state = InitiateUInt8Matrix(plateMtr, 300, 300);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(300, sizeof(Uint8 *));
#else
	memCost += _mpy(300, sizeof(Uint8 *));
#endif
	memCost += _mpy(300, 300);

#ifdef DSP	
	plateMtr_bak = (Uint8 **)MEM_calloc(extHeap, (300)*sizeof(Uint8 *), 4);
#else
	plateMtr_bak = (Uint8 **)malloc((300)*sizeof(Uint8 *));
#endif

	if (plateMtr_bak == NULL) return 0;
#ifdef DSP
	state = InitiateUInt8Matrix(plateMtr_bak, 300, 300);
#else
	state = InitiateUInt8Matrix(plateMtr_bak, 300, 300);
#endif
	if (state == 0) return 0;

#ifdef DSP
	memCost += _mpy(300, sizeof(Uint8 *));
#else
	memCost += _mpy(300, sizeof(Uint8 *));
#endif
	memCost += _mpy(300, 300);


#ifdef DSP
	pRow_1 = (int *)MEM_calloc(extHeap, row*sizeof(int), 4);
#else
	pRow_1 = (int *)malloc(row*sizeof(int));
#endif

	if (pRow_1 == NULL) return 0;

	memCost += _mpy(row, sizeof(int));

#ifdef DSP
	pRow_2 = (int *)MEM_calloc(extHeap, row*sizeof(int), 4);
#else
	pRow_2 = (int *)malloc(row*sizeof(int));
#endif

	if (pRow_2 == NULL) return 0;

	memCost += _mpy(row, sizeof(int));

#ifdef DSP
	pCol_1 = (int *)MEM_calloc(extHeap, col*sizeof(int), 4);
#else
	pCol_1 = (int *)malloc(col*sizeof(int));
#endif

	if (pCol_1 == NULL) return 0;

	memCost += _mpy(col, sizeof(int));

#ifdef DSP
	pCol_2 = (int *)MEM_calloc(extHeap, col*sizeof(int), 4);
#else
	pCol_2 = (int *)malloc(col*sizeof(int));
#endif

	if (pCol_2 == NULL) return 0;

	memCost += _mpy(col, sizeof(int));

#ifdef DSP
	pCol_3 = (int *)MEM_calloc(extHeap, col*sizeof(int), 4);
#else
	pCol_3 = (int *)malloc(col*sizeof(int));
#endif
	if (pCol_3 == NULL) return 0;

	memCost += _mpy(col, sizeof(int));

	/*
	#ifdef DSP
	pCol_4=(int *)MEM_calloc(extHeap, col*sizeof(int), 4);
	#else
	pCol_4 = (int *) malloc(col*sizeof(int));
	#endif
	if ( pCol_4==NULL ) return 0;

	memCost += _mpy(col, sizeof(int));
	*/
#ifdef DSP
	plateHistGram = (int *)MEM_calloc(extHeap, 256 * sizeof(int), 4);
#else
	plateHistGram = (int *)malloc(256 * sizeof(int));
#endif
	if (plateHistGram == NULL) return 0;

	memCost += _mpy(256, sizeof(int));

#ifdef DSP
	pInt256_1 = (int *)MEM_calloc(extHeap, 256 * sizeof(int), 4);
#else
	pInt256_1 = (int *)malloc(256 * sizeof(int));
#endif
	if (pInt256_1 == NULL) return 0;

	memCost += _mpy(256, sizeof(int));

#ifdef DSP
	pInt256_2 = (int *)MEM_calloc(extHeap, 256 * sizeof(int), 4);
#else
	pInt256_2 = (int *)malloc(256 * sizeof(int));
#endif
	if (pInt256_2 == NULL) return 0;

	memCost += _mpy(256, sizeof(int));

#ifdef DSP
	pInt256_3 = (int *)MEM_calloc(extHeap, 256 * sizeof(int), 4);
#else
	pInt256_3 = (int *)malloc(256 * sizeof(int));
#endif
	if (pInt256_3 == NULL) return 0;

	memCost += _mpy(256, sizeof(int));

#ifdef DSP
	pInt256_4 = (int *)MEM_calloc(extHeap, 256 * sizeof(int), 4);
#else
	pInt256_4 = (int *)malloc(256 * sizeof(int));
#endif
	if (pInt256_4 == NULL) return 0;

	memCost += _mpy(256, sizeof(int));

#ifdef DSP
	rgb_r = (int *)MEM_calloc(extHeap, ((row / 3)*(col))*sizeof(int), 4);
#else
	rgb_r = (int *)malloc(((row / 3)*(col))*sizeof(int));
#endif
	if (rgb_r == NULL) return 0;

	memCost += _mpy(row / 3, (col)*sizeof(int));


#ifdef DSP
	rgb_g = (int *)MEM_calloc(extHeap, ((row / 3)*(col))*sizeof(int), 4);
#else
	rgb_g = (int *)malloc(((row / 3)*(col))*sizeof(int));
#endif
	if (rgb_g == NULL) return 0;

	memCost += _mpy(row / 3, (col)*sizeof(int));

#ifdef DSP
	rgb_b = (int *)MEM_calloc(extHeap, ((row / 3)*(col))*sizeof(int), 4);
#else
	rgb_b = (int *)malloc(((row / 3)*(col))*sizeof(int));
#endif
	if (rgb_b == NULL) return 0;

	memCost += _mpy(row / 3, (col)*sizeof(int));

#ifdef DSP
	hsv_h = (int *)MEM_calloc(extHeap, ((row / 3)*(col))*sizeof(int), 4);
#else
	hsv_h = (int *)malloc(((row / 3)*(col))*sizeof(int));
#endif
	if (hsv_h == NULL) return 0;

	memCost += _mpy(row / 3, (col)*sizeof(int));

#ifdef DSP
	hsv_s = (int *)MEM_calloc(extHeap, ((row / 3)*(col / 3))*sizeof(int), 4);
#else
	hsv_s = (int *)malloc(((row / 3)*(col / 3))*sizeof(int));
#endif
	if (hsv_s == NULL) return 0;

	memCost += _mpy(row / 3, (col / 3)*sizeof(int));

#ifdef DSP
	hsv_v = (int *)MEM_calloc(extHeap, ((row / 3)*(col / 3))*sizeof(int), 4);
#else
	hsv_v = (int *)malloc(((row / 3)*(col / 3))*sizeof(int));
#endif
	if (hsv_v == NULL) return 0;

	memCost += _mpy(row / 3, (col / 3)*sizeof(int));

#ifdef DSP
	pInt300 = (int *)MEM_calloc(extHeap, 300 * sizeof(int), 4);
#else
	pInt300 = (int *)malloc(300 * sizeof(int));
#endif
	if (pInt300 == NULL) return 0;

	memCost += _mpy(300, sizeof(int));

#ifdef DSP
	pFlt60 = (float *)MEM_calloc(extHeap, 60 * sizeof(float), 4);
#else
	pFlt60 = (float *)malloc(60 * sizeof(float));
#endif
	if (pFlt60 == NULL) return 0;

	memCost += _mpy(60, (col / 3)*sizeof(float));

#ifdef DSP
	pFlt256 = (float *)MEM_calloc(extHeap, 256 * sizeof(float), 4);
#else
	pFlt256 = (float *)malloc(256 * sizeof(float));
#endif
	if (pFlt256 == NULL) return 0;

	memCost += _mpy(256, sizeof(float));

#ifdef DSP
	pWord = (short *)MEM_calloc(extHeap, resize_ROW*resize_COL*sizeof(short), 4);
#else
	pWord = (short *)malloc(resize_ROW*resize_COL*sizeof(short));
#endif
	if (pWord == NULL) return 0;

	memCost += _mpy(resize_ROW, resize_COL*sizeof(short));

	return (int)(memCost / 1024);
}
