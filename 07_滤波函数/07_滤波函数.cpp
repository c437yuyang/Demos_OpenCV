// 07_滤波函数.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{

	Mat src = imread("../lena.jpg");
	imshow("src", src);

	Mat resultBoxFilter;
	boxFilter(src, resultBoxFilter, src.depth(), Size(3, 3)); //默认是归一化了的，归一化了就是均值滤波(和blur函数作用相同)，不归一化就是计算模板内的和了
	imshow("resultBoxFilter", resultBoxFilter);

	Mat resultBlur; //就是均值滤波
	blur(src, resultBlur, Size(3, 3));
	imshow("resultBlur", resultBlur);

	Mat resultGaussinBlur; //高斯均值滤波
	GaussianBlur(src, resultGaussinBlur, Size(3, 3),3.0,3.0);
	imshow("resultGaussinBlur", resultGaussinBlur);



	cv::waitKey(0);
	system("pause");
	return 0;
}

