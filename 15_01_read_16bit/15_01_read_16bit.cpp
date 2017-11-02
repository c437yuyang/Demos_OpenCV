// 15_01_read_16bit.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include <iostream>
#include <opencv.hpp>
#include <opencv\highgui.h>
using namespace cv;
using namespace std;

Mat img; //ͼ��
const string lpwindowname = "ͼ��09230021";

const string lpImageName = "1.tif";
void on_mousemove(int event, int x, int y, int flags, void* param);
int mid(int i, int a, int b);

int main()
{

	//CV_LOAD_IMAGE_UNCHANGED �� CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH Ч����һ�µ�
	img = imread(lpImageName, CV_LOAD_IMAGE_UNCHANGED);//����ͼ��

	namedWindow(lpwindowname, WINDOW_AUTOSIZE);//��������
	imshow(lpwindowname, img);//���Ѿ������Ĵ�������ʾͼ��
	setMouseCallback(lpwindowname, on_mousemove, 0);//��Ӧ����¼�
	waitKey(0);
	destroyAllWindows();
	return 0;
}

int mid(int i, int a, int b)
{
	return min(max(i, min(a, b)), max(a, b));
}

void on_mousemove(int event, int x, int y, int flags, void* param)
{
	//Font font;
	//cvinitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1, 1, 1, 1, CV_AA);
	int width = img.cols;//ͼƬ���
	int height = img.rows;//ͼƬ�߶�
	Point text_pt;
	Size text_size;
	int baseline;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		x = mid(x, 0, img.cols);
		y = mid(y, 0, img.rows);


		auto tmp = img.type(); //18
		bool b = CV_16UC3 == img.type(); //true
		b = CV_16SC3 == img.type(); //false

		auto tmp2 = img.depth(); //2
		auto tmp3 = img.elemSize(); //6������ͨ����һ��Ĵ�С
		tmp3 = img.elemSize1(); //2,ÿ��ͨ���Ĵ�С
		int  channels = img.channels(); //3

		//���ӵĻ�ȡ��ʽ���鷳������ֻ��ͨ���ģ���Ϊ������6���ֽڣ����Ի�ȡ��ͨ�����������̫�鷳��
		//uchar * data = img.data;
		//data = img.data + pt.y*img.step + pt.x*img.elemSize();
		//uchar data2 = *(data + 1);
		//int intgray = (*data) + (data2 << 8);


		auto a2 = img.at<Vec3w>(y, x); //ֱ�����������


		// ************* �����û������ ******************
		Point pt = Point(x, y);
		char site[100];
		sprintf(site, "(%u,%u,%u)",x, y, a2[0],a2[1],a2[2]);
		circle(img, pt, 2, Scalar(255, 255, 255), 1, CV_AA, 0);
		text_size = getTextSize(site, CV_FONT_HERSHEY_DUPLEX, 1, 1, &baseline);
		text_pt.x = mid(pt.x, 0, width - text_size.width);
		text_pt.y = mid(pt.y, text_size.height + baseline, height);
		putText(img, site, text_pt, CV_FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 1, CV_AA);
		imshow(lpwindowname, img);
		img = imread(lpImageName, CV_LOAD_IMAGE_UNCHANGED);//����ͼ��

	}
}
