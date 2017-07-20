// 03_TraverseImage.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main()
{
	Mat lena = imread("../lena.jpg");
	imshow("lena", lena);

	Mat lena_withNoise = Mat::zeros(lena.rows, lena.cols, CV_8UC3);

	//指针遍历
	auto time_start = cv::getTickCount();
	for (int i = 0; i != lena.rows; ++i)//每一行
	{
		uchar* src = lena.ptr<uchar>(i);
		uchar* dst = lena_withNoise.ptr<uchar>(i);
		for (int j = 0; j != lena.cols*lena.channels(); ++j)//行上的每一个像素
		{
			*(dst + j) = *(src + j);
		}
	}
	auto time_end = cv::getTickCount() - time_start;
	cout << "time consumed:" << time_end*1.0 / cv::getTickFrequency() << endl;

	//测试两种遍历方式的时间（发现也就差一倍,debug release都是）
	//at遍历
	time_start = cv::getTickCount();
	for (int i = 0; i != lena.rows; ++i)//每一行
	{

		for (int j = 0; j != lena.cols; ++j)//行上的每一个像素
		{
			lena_withNoise.at<Vec3b>(j, i) = lena.at<Vec3b>(j, i);
		}
	}
	time_end = cv::getTickCount() - time_start;
	cout << "time consumed:" << time_end*1.0 / cv::getTickFrequency() << endl;

	imshow("lena_withNoise", lena_withNoise);

	waitKey(0);
	return 0;
}

