// 11_01_��ѧ����.cpp : �������̨Ӧ�ó������ڵ㡣
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

	cout << sum(m1) << endl; //���ض���scalar���ͣ��ֱ��Ӧ��ͬͨ���������ȡ[0]����������
	cout << mean(m1) << endl;
	system("pause");
	return 0;
}

