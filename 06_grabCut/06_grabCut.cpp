// 06_grabCut.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
	medianBlur(binMask, binMask, 6 * 2 + 1); //去除噪声
}

int main()
{

	//Rect rect(170, 150, 700 - 170, 500 - 150);
	//Mat src = imread("3dIMG_ 2_resize.jpg");
	Rect rect(480, 300, 1350 - 480, 900 - 300);
	Mat src = imread("64_复杂背景.jpg");
	Mat src_lab;
	cvtColor(src, src_lab, CV_BGR2Lab); //实验转换到LAB空间
	
	Mat dst_rgb_it3,bgModel,fgdModel,mask,binMask,dst_rgb_it3_it3;
	//得到的mask里只有最后两位有值，最后一位为1代表是前景，倒数第二位为1代表可能是前景，就是自己画的矩形
	grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT); //最好是在release下，debug太慢
	getBinMask(mask, binMask);
	//binMask = mask.clone(); //必须只取最低位的mask，否则就是直接指定的矩形取到
	binMask.convertTo(binMask, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it3,binMask); 

	//基础的使用到这里就结束了

	//对比lab空间迭代3次的效果（可以看到lab空间进行运算通常也有不小的提升）
	Mat dst_lab_it3, bgModel_lab, fgdModel_lab, mask_lab, binMask_lab, dst_lab_it3_it3;
	grabCut(src_lab, mask_lab, rect, bgModel_lab, fgdModel_lab, 3, GC_INIT_WITH_RECT);
	getBinMask(mask_lab, binMask_lab);
	binMask_lab.convertTo(binMask_lab, CV_8UC1, 255.0);
	src.copyTo(dst_lab_it3, binMask_lab); 


	//在之前的基础上继续迭代(可以看到效果好了很多)
	grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT);
	getBinMask(mask, binMask);
	//binMask = mask.clone(); //必须只取最低位的mask，否则就是直接指定的矩形取到
	binMask.convertTo(binMask, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it3_it3, binMask);

	//对比直接迭代6次（发现效果还是有不小区别的。。）
	Mat bgModel_1, fgdModel_1, mask_1, binMask_1, dst_rgb_it6; 
	grabCut(src, mask_1, rect, bgModel_1, fgdModel_1, 6, GC_INIT_WITH_RECT);
	getBinMask(mask_1, binMask_1);
	//binMask = mask.clone(); //必须只取最低位的mask，否则就是直接指定的矩形取到
	binMask_1.convertTo(binMask_1, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it6, binMask_1);


	//问题:继续迭代中，bgmodel和fbgmodel以及mask分别的作用?




	system("pause");
    return 0;
}

