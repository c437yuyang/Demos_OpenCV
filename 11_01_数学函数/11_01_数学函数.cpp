// 11_01_数学函数.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main()
{
	Mat m1 = Mat::ones(Size(3, 3), CV_32FC1);
	m1.at<float>(1, 2) = 2.1;
	cout << m1;

	cout << sum(m1) << endl; //返回都是scalar类型，分别对应不同通道，这里就取[0]就是所需结果
	cout << mean(m1) << endl;
	system("pause");
	return 0;
}

