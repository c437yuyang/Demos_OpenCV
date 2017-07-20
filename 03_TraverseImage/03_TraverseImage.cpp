// 03_TraverseImage.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//ָ�����
	auto time_start = cv::getTickCount();
	for (int i = 0; i != lena.rows; ++i)//ÿһ��
	{
		uchar* src = lena.ptr<uchar>(i);
		uchar* dst = lena_withNoise.ptr<uchar>(i);
		for (int j = 0; j != lena.cols*lena.channels(); ++j)//���ϵ�ÿһ������
		{
			*(dst + j) = *(src + j);
		}
	}
	auto time_end = cv::getTickCount() - time_start;
	cout << "time consumed:" << time_end*1.0 / cv::getTickFrequency() << endl;

	//�������ֱ�����ʽ��ʱ�䣨����Ҳ�Ͳ�һ��,debug release���ǣ�
	//at����
	time_start = cv::getTickCount();
	for (int i = 0; i != lena.rows; ++i)//ÿһ��
	{

		for (int j = 0; j != lena.cols; ++j)//���ϵ�ÿһ������
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

