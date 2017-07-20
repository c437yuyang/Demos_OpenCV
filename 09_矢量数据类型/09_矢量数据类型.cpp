// 09_ʸ����������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void printMat(const Mat& m, int MAT_TYPE = CV_8UC3, char colDelimiter = ' ', char rowDelimiter = '\n')
{
	int w = m.cols;
	int h = m.rows;
	int ch = m.channels();

	if (MAT_TYPE == CV_8UC1 || MAT_TYPE == CV_8UC2 || MAT_TYPE == CV_8UC3) //8U��8UC1��ͬ������������
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

	Vec3b vb1(1, 2, 3);
	
	cout << vb1 << endl;
	cout << vb1[0] << "," << vb1[1] << "," << vb1[2] << endl;

	Vec3d vd1 = (Vec3d)vb1; //����ֱ��ת��
	cout << vd1 << endl;

	Vec3b vb2 = { 3,4,5 }; //ֻ�����������˻�assert����
	cout << vb2 << endl;
	cout << vb2 - vb1 << endl; //֧��ֱ�����

	Scalar s1(1, 2, 3); //Scalar ��ʾԪ����ʵ��һ��4��Ԫ�ص�
	cout << s1 << endl;

	Scalar s2 = Scalar::all(0); //��һ�ֳ�ʼ����ʽ
	cout << s2 << endl;



	system("pause");
    return 0;
}

