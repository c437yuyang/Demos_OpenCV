// 07_�˲�����.cpp : �������̨Ӧ�ó������ڵ㡣
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
	boxFilter(src, resultBoxFilter, src.depth(), Size(3, 3)); //Ĭ���ǹ�һ���˵ģ���һ���˾��Ǿ�ֵ�˲�(��blur����������ͬ)������һ�����Ǽ���ģ���ڵĺ���
	imshow("resultBoxFilter", resultBoxFilter);

	Mat resultBlur; //���Ǿ�ֵ�˲�
	blur(src, resultBlur, Size(3, 3));
	imshow("resultBlur", resultBlur);

	Mat resultGaussinBlur; //��˹��ֵ�˲�
	GaussianBlur(src, resultGaussinBlur, Size(3, 3),3.0,3.0);
	imshow("resultGaussinBlur", resultGaussinBlur);



	cv::waitKey(0);
	system("pause");
	return 0;
}

