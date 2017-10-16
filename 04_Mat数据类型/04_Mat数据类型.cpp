// 04_Mat数据类型.cpp : 定义控制台应用程序的入口点。
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

	int i = CV_8U; //0x00
	i = CV_8UC1;//0x00
	i = CV_8UC2;//0x08
	i = CV_8UC3;//0x10
	i = CV_32F;//0x05
	i = CV_32FC1;//0x05
	i = CV_32FC2;//0x0d
	i = CV_32FC3;//0x15
	i = CV_64F;//0x06
	i = CV_64FC1;//0x06 //向下一次左移一位
	i = CV_64FC2;//0x0e
	i = CV_64FC3;//0x16

	//初始化方式1
	Mat a = imread("../lena.jpg");
	Mat b = a;
	imshow("a orgin", a);
	imshow("b operator=from a", a);

	blur(a, a, Size(11, 11));
	imshow("blured", a);
	imshow("b operator=from a after a be blured", b); //可以看到operator=是浅拷贝

	//初始化方式2
	Mat m2 = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	std::cout << m2 << std::endl; //VS15下直接输出会报错不知道为什么，VS10正常，换了VC14版本的就没问题了
	printMat(m2, CV_64FC1);
	double m[3][3] = { { 1.0, 2.0, 3.0 },{ 4, 5,6 },{ 7, 8, 9 } };
	//初始化方式3
	Mat m3 = Mat(3, 3, CV_64FC1, m); //这里指定什么类型就要用什么类型数组去实现，比如float和double就不能互换
	//内部应该是通过指针移位去实现的，所以如果类型出错就会出错
	printMat(m3, CV_64FC1);

	float m1[3][3] = { { 1.1f, 2.0f, 3.0f },{ 4.0f, 5.0f,6.0f },{ 7.2f, 8.0f, 9.0f } };
	Mat m4 = Mat(3, 3, CV_32FC1, m1);
	printMat(m4, CV_32FC1, '\t');
	cv::transpose(m4, m4);
	printMat(m4, CV_32FC1);

	//初始化方式4
	Mat m5 = Mat::zeros(3, 3, CV_8UC3);
	Mat m6 = Mat::ones(3, 3, CV_8UC1); //8UC1的乘以标量3才是全部为3
	printMat(m6 * 3);
	Mat m7 = Mat::ones(3, 3, CV_8UC3); //8UC3的乘以标量3只有第一个通道是3，全部乘以只能遍历???
	//Mat m8 = Mat(1, 1, CV_8UC3); //错误的操作
	//m8.at<Vec3b>(0, 0) = { 3,3,3 };
	//printMat(m7 * m8);

	//初始化方式5（从scalar初始化，比较好用）
	Mat m8(3, 3, CV_8UC3, cv::Scalar(3, 2, 1));
	//Mat m8(3, 3, CV_8UC3, { 1,2,3 }); 相同
	printMat(m8);

	//初始化方式6
	Mat m9;
	m9.create(3, 3, CV_8UC3);
	m9.setTo(0); //全部置为指定值
	printMat(m9);
	m9.setTo(10);
	printMat(m9);
	m9.setTo(Vec3b(1, 2, 3)); //常量，矢量，颜色都可以
	printMat(m9);
	m9.setTo(Scalar(4, 5, 6));
	printMat(m9);
	//初始化方式7
	Mat m10;
	m10.create(5, 5, CV_8UC1);
	m10 = 255; //直接赋值，所有元素都是这个值
	printMat(m10);
	//设置ROI区域整体赋值
	m10(Rect(1, 1, 3, 3)) = 0;
	printMat(m10);


	//拷贝来自opencv帮助文档，Mat支持的操作:
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

