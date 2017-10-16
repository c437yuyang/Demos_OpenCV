// 04_Mat��������.cpp : �������̨Ӧ�ó������ڵ㡣
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

	int i = CV_8U; //0x00
	i = CV_8UC1;//0x00
	i = CV_8UC2;//0x08
	i = CV_8UC3;//0x10
	i = CV_32F;//0x05
	i = CV_32FC1;//0x05
	i = CV_32FC2;//0x0d
	i = CV_32FC3;//0x15
	i = CV_64F;//0x06
	i = CV_64FC1;//0x06 //����һ������һλ
	i = CV_64FC2;//0x0e
	i = CV_64FC3;//0x16

	//��ʼ����ʽ1
	Mat a = imread("../lena.jpg");
	Mat b = a;
	imshow("a orgin", a);
	imshow("b operator=from a", a);

	blur(a, a, Size(11, 11));
	imshow("blured", a);
	imshow("b operator=from a after a be blured", b); //���Կ���operator=��ǳ����

	//��ʼ����ʽ2
	Mat m2 = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	std::cout << m2 << std::endl; //VS15��ֱ������ᱨ��֪��Ϊʲô��VS10����������VC14�汾�ľ�û������
	printMat(m2, CV_64FC1);
	double m[3][3] = { { 1.0, 2.0, 3.0 },{ 4, 5,6 },{ 7, 8, 9 } };
	//��ʼ����ʽ3
	Mat m3 = Mat(3, 3, CV_64FC1, m); //����ָ��ʲô���;�Ҫ��ʲô��������ȥʵ�֣�����float��double�Ͳ��ܻ���
	//�ڲ�Ӧ����ͨ��ָ����λȥʵ�ֵģ�����������ͳ���ͻ����
	printMat(m3, CV_64FC1);

	float m1[3][3] = { { 1.1f, 2.0f, 3.0f },{ 4.0f, 5.0f,6.0f },{ 7.2f, 8.0f, 9.0f } };
	Mat m4 = Mat(3, 3, CV_32FC1, m1);
	printMat(m4, CV_32FC1, '\t');
	cv::transpose(m4, m4);
	printMat(m4, CV_32FC1);

	//��ʼ����ʽ4
	Mat m5 = Mat::zeros(3, 3, CV_8UC3);
	Mat m6 = Mat::ones(3, 3, CV_8UC1); //8UC1�ĳ��Ա���3����ȫ��Ϊ3
	printMat(m6 * 3);
	Mat m7 = Mat::ones(3, 3, CV_8UC3); //8UC3�ĳ��Ա���3ֻ�е�һ��ͨ����3��ȫ������ֻ�ܱ���???
	//Mat m8 = Mat(1, 1, CV_8UC3); //����Ĳ���
	//m8.at<Vec3b>(0, 0) = { 3,3,3 };
	//printMat(m7 * m8);

	//��ʼ����ʽ5����scalar��ʼ�����ȽϺ��ã�
	Mat m8(3, 3, CV_8UC3, cv::Scalar(3, 2, 1));
	//Mat m8(3, 3, CV_8UC3, { 1,2,3 }); ��ͬ
	printMat(m8);

	//��ʼ����ʽ6
	Mat m9;
	m9.create(3, 3, CV_8UC3);
	m9.setTo(0); //ȫ����Ϊָ��ֵ
	printMat(m9);
	m9.setTo(10);
	printMat(m9);
	m9.setTo(Vec3b(1, 2, 3)); //������ʸ������ɫ������
	printMat(m9);
	m9.setTo(Scalar(4, 5, 6));
	printMat(m9);
	//��ʼ����ʽ7
	Mat m10;
	m10.create(5, 5, CV_8UC1);
	m10 = 255; //ֱ�Ӹ�ֵ������Ԫ�ض������ֵ
	printMat(m10);
	//����ROI�������帳ֵ
	m10(Rect(1, 1, 3, 3)) = 0;
	printMat(m10);


	//��������opencv�����ĵ���Mat֧�ֵĲ���:
	//Addition, subtraction, negation: A + B, A - B, A + s, A - s, s + A, s - A, -A

	//	Scaling : A*alpha

	//	Per - element multiplication and division : A.mul(B), A / B, alpha / A

	//	Matrix multiplication : A*B

	//	Transposition : A.t() (means AT)

	//	Matrix inversion and pseudo - inversion, solving linear systems and least - squares problems :

	//A.inv([method]) (~A - 1), A.inv([method])*B(~X: AX = B)

	//	Comparison : A cmpop B, A cmpop alpha, alpha cmpop A, where cmpop is one of : > , >= , == , != , <= , < .The result of comparison is an 8 - bit single channel mask whose elements are set to 255 (if the particular element or pair of elements satisfy the condition) or 0.

	//	Bitwise logical operations : A logicop B, A logicop s, s logicop A, ~A, where logicop is one of : &, | , ^.

	//	Element - wise minimum and maximum : min(A, B), min(A, alpha), max(A, B), max(A, alpha)

	//	Element - wise absolute value : abs(A)

	//	Cross - product, dot - product : A.cross(B) A.dot(B)

	//	Any function of matrix or matrices and scalars that returns a matrix or a scalar, such as norm, mean, sum, countNonZero, trace, determinant, repeat, and others.

	//	Matrix initializers(Mat::eye(), Mat::zeros(), Mat::ones()), matrix comma - separated initializers, matrix constructors and operators that extract sub - matrices(see Mat description).

	//	Mat_<destination_type>() constructors to cast the result to the proper type.

	cv::waitKey(0);
	return 0;
}

