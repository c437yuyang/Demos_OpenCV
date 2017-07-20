// 05_normalize.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;

using namespace cv;

void printMat(const Mat& m, int MAT_TYPE = CV_8UC3, char colDelimiter = ' ', char rowDelimiter = '\n')
{
	int w = m.cols;
	int h = m.rows;
	int ch = m.channels();

	if (MAT_TYPE == CV_8UC1 || MAT_TYPE == CV_8UC2 || MAT_TYPE == CV_8UC3) //8U和8UC1相同，其他的类似
		for (int i = 0; i != h; ++i)
		{
			const uchar* ptr = m.ptr<uchar>(i);
			for (int j = 0; j != w * ch; ++j)
			{
				cout << (int)ptr[j] << colDelimiter;
			}
			cout << rowDelimiter;
		}
	else if (MAT_TYPE == CV_64FC1 || MAT_TYPE == CV_64FC3 || MAT_TYPE == CV_64FC2)
	{
		for (int i = 0; i != h; ++i)
		{
			const double* ptr = m.ptr<double>(i);
			for (int j = 0; j != w * ch; ++j)
			{
				cout << ptr[j] << colDelimiter;
			}
			cout << rowDelimiter;
		}
	}
	else if (MAT_TYPE == CV_32FC1 || MAT_TYPE == CV_32FC3 || MAT_TYPE == CV_32FC2)
	{
		for (int i = 0; i != h; ++i)
		{
			for (int j = 0; j != w; ++j)
			{
				cout << m.at<float>(i, j) << colDelimiter;
			}
			cout << rowDelimiter;
		}
	}
}

int main()
{

	double m[3][3] = { { 1.0, 2.0, 3.0 },{ 4, 5,6 },{ 7, 8, 9 } };
	Mat m3 = Mat(3, 3, CV_64FC1, m);
	Mat m2 = (Mat_<double>(3, 3) << 2, 3, 4, 5, 6, 7, 8, 9, 10);


	printMat(m3, CV_64FC1);
	Mat m4, m5; //用ImageWatch查看

	cv::normalize(m3, m4, 0, 1, NORM_MINMAX, CV_64FC1); //就是缩放到0-1
	m3.convertTo(m5, CV_64FC1, 1 / 255.0, 0); //convertTo是直接乘以alpha

	Mat m6 = (Mat_<double>(1, 9) << 2, 2, 4, 4, 4, 5, 1, 4, 2),m7;
	normalize(m6, m7, 1, 0, NORM_L1, CV_64FC1); //计算1范数，然后最大值为限制为1(归一化到1的范围就是)

	double sum = 0.0;
	for (int i=0;i!=m7.cols;++i)
	{
		sum += m7.at<double>(0, i);
	}

	//when normType=NORM_MINMAX (for dense arrays only). 
	//The optional mask specifies a sub-array to be normalized.
	//This means that the norm or min-n-max are calculated over the sub-array, 
	//and then this sub-array is modified to be normalized. 
	//If you want to only use the mask to calculate the norm or min-max but modify the whole array, 
	//you can use norm and Mat::convertTo.

	system("PAUSE");
	return 0;
}

