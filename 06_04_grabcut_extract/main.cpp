#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
#include "grabcut_sc.h"
#include "SLIC.h"
typedef unsigned int UINT;

struct FgdBgdCnt
{
	int fgdCnt;
	int bgdCnt;
};

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
	//medianBlur(binMask, binMask, 6 * 2 + 1); //去除噪声
}

template <class T> //函数不负责分配内存,三通道彩色图像转换
void MatConvert(const Mat & src, T* dst)
{
	if (dst == NULL) return;
	for (int i = 0; i != src.rows; ++i)
	{
		for (int j = 0; j != src.cols; ++j)
		{
			Vec3b pt = src.at<Vec3b>(i, j);
			dst[i*src.cols * 3 + j * 3 + 0] = static_cast<T>(pt[0]);
			dst[i*src.cols * 3 + j * 3 + 1] = static_cast<T>(pt[1]);
			dst[i*src.cols * 3 + j * 3 + 2] = static_cast<T>(pt[2]);
		}
	}
}


int main()
{
	Rect rect(170, 150, 700 - 170, 500 - 150);
	Mat src = imread("3dIMG_ 2_resize.jpg");
	//cvtColor(src, src, CV_BGR2Lab);
	//Rect rect(480, 300, 1350 - 480, 900 - 300);
	//Mat src = imread("64_复杂背景.jpg");


	int w = src.cols, h = src.rows;
	//对图像进行超像素分割
	UINT* imgBuff = new UINT[src.cols * src.rows * 3]();
	//需要传入一个uint的类型，先将图像类型进行转换
	MatConvert(src, imgBuff);
	SLIC slic;
	int *kLables = new int[w*h]();
	int numLabels(0), weight = 0 /*暂时没用*/, color = 222, nSpNum = 4000;
	slic.PerformSLICO_ForGivenK(imgBuff, w, h, kLables, numLabels, nSpNum, weight);

#pragma region 可视化SLIC结果1，用边界的方式画出来
	//UINT* SegmentResult = new UINT[w*h];
	//slic.DrawContoursAroundSegments(SegmentResult, kLables, w, h, color);
	//Mat SLICResult = src.clone();
	//Vec3b red(0, 0, 255);
	//for (int i = 0; i != h; ++i)
	//	for (int j = 0; j != w; ++j)
	//		if (SegmentResult[i*w + j] == color)
	//			SLICResult.at<Vec3b>(i, j) = red;
#pragma endregion

#pragma region 可视化方式2
//	uchar g_colors[16][3] = { //预置颜色表
//{ 223,0,36 },{ 103,110,176 },{ 0,159,60 },{ 240,220,0 },
//{ 237,108,158 },{ 94,138,198 },{ 232,100,27 },{ 239,156,0 },
//{ 106,80,155 },{ 124,198,35 },{ 142,186,155 },{ 0,164,157 },
//{ 168,213,157 },{ 61,16,123 },{ 49,67,150 },{ 0,0,0 }
//	};
//
//	Mat SlicRegionDisplay(h, w, CV_8UC3);
//	for (int i = 0; i != h; ++i)
//	{
//		for (int j = 0; j != w; ++j)
//		{
//			int  clrIdx = kLables[i*w + j] % 16;
//			SlicRegionDisplay.at<Vec3b>(i, j) = Vec3b(g_colors[clrIdx][2], g_colors[clrIdx][1], g_colors[clrIdx][0]);
//		}
//	}
#pragma endregion

	//形成一个新的mat,由超像素组成,一行一个像素，3列分别是rgb值
	//更正:不能一行一个，必须保持和原图像一样，有行数和列数才行,但是求出的超像素的数目和之前设定的通常都不一致，要如何设置呢？（复制边界可行吗？）
	//先统计每个超像素的信息，求出rgb值

	double step = sqrt(double(w*h) / double(nSpNum));
	int cols = static_cast<int> (w / step);
	int rows = static_cast<int> (h / step);
	if (cols*rows != nSpNum) return -16;

	Mat src_sp(numLabels, 1, CV_8UC3);
	vector<Vec3d> rgbInfo(numLabels);
	vector<int> spNum(numLabels, 0);
	vector<FgdBgdCnt> fbgdCnt(numLabels);
	//对这张图，确定哪些点是TU，哪些是TB，之后的初始化用掩模初始化，不能再用rect进行初始化了
	//根据设定的矩形，统计超像素是TU还是TB，对于属于边界的，用投票机制解决(更多像素在TB则是TB)
	for (int i = 0; i != h; ++i)
	{
		for (int j = 0; j != w; ++j)
		{
			int spid = kLables[i*w + j];
			rgbInfo[spid] += src.at<Vec3b>(i, j);
			++spNum[spid]; //统计每个超像素拥有的像素数目
			if (Point(j, i).inside(rect)) 
				++fbgdCnt[spid].fgdCnt;
			else ++fbgdCnt[spid].bgdCnt;
		}
	}

	Mat mask(numLabels, 1, CV_8UC1); //从前面统计的信息得到超像素级别的mask
	mask.setTo(GC_BGD);
	for (int i = 0; i != numLabels; ++i)
	{
		rgbInfo[i] /= spNum[i];
		src_sp.at<Vec3b>(i, 0) = static_cast<Vec3b>(rgbInfo[i]); //得到超像素的均值
		if (fbgdCnt[i].fgdCnt >= fbgdCnt[i].bgdCnt) mask.at<uchar>(i, 0) = GC_PR_FGD;
	}

	
#pragma region 可视化一下得到的mask
	//Mat maskDisp = Mat::zeros(h, w, CV_8UC1);
	//for (int i = 0; i != h; ++i)
	//	for (int j = 0; j != w; ++j)
	//		if (mask.at<uchar>(kLables[i*w + j], 0) == GC_PR_FGD) maskDisp.at<uchar>(i, j) = 255;
#pragma endregion

	//执行grabcut
	Mat gc, bgModel, fgdModel, binMask;
	yxp_utility::grabCut(src_sp, mask, rect, bgModel, fgdModel, 5, GC_INIT_WITH_MASK);
	getBinMask(mask, binMask);

	//恢复出原始图像的掩模
	Mat res = Mat::zeros(h, w, CV_8UC1);
	for (int i = 0; i != h; ++i)
		for (int j = 0; j != w; ++j)
		{
			int spid = kLables[i*w + j];
			if (binMask.at<uchar>(spid) == GC_FGD) res.at<uchar>(i, j) = 255;
		}

	//最终得到分割结果
	Mat dst;
	src.copyTo(dst, res);


	//Mat dst_rgb_it3, bgModel, fgdModel, mask, binMask, dst_rgb_it3_it3;
	////得到的mask里只有最后两位有值，最后一位为1代表是前景，倒数第二位为1代表可能是前景，就是自己画的矩形
	//yxp_utility::grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT); //最好是在release下，debug太慢
	//getBinMask(mask, binMask);
	////binMask = mask.clone(); //必须只取最低位的mask，否则就是直接指定的矩形取到
	//binMask.convertTo(binMask, CV_8UC1, 255.0);
	//src.copyTo(dst_rgb_it3, binMask);


	system("pause");
	return 0;
}




