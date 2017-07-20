// 06_grabCut.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
	medianBlur(binMask, binMask, 6 * 2 + 1); //ȥ������
}

int main()
{

	//Rect rect(170, 150, 700 - 170, 500 - 150);
	//Mat src = imread("3dIMG_ 2_resize.jpg");
	Rect rect(480, 300, 1350 - 480, 900 - 300);
	Mat src = imread("64_���ӱ���.jpg");
	Mat src_lab;
	cvtColor(src, src_lab, CV_BGR2Lab); //ʵ��ת����LAB�ռ�
	
	Mat dst_rgb_it3,bgModel,fgdModel,mask,binMask,dst_rgb_it3_it3;
	//�õ���mask��ֻ�������λ��ֵ�����һλΪ1������ǰ���������ڶ�λΪ1���������ǰ���������Լ����ľ���
	grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT); //�������release�£�debug̫��
	getBinMask(mask, binMask);
	//binMask = mask.clone(); //����ֻȡ���λ��mask���������ֱ��ָ���ľ���ȡ��
	binMask.convertTo(binMask, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it3,binMask); 

	//������ʹ�õ�����ͽ�����

	//�Ա�lab�ռ����3�ε�Ч�������Կ���lab�ռ��������ͨ��Ҳ�в�С��������
	Mat dst_lab_it3, bgModel_lab, fgdModel_lab, mask_lab, binMask_lab, dst_lab_it3_it3;
	grabCut(src_lab, mask_lab, rect, bgModel_lab, fgdModel_lab, 3, GC_INIT_WITH_RECT);
	getBinMask(mask_lab, binMask_lab);
	binMask_lab.convertTo(binMask_lab, CV_8UC1, 255.0);
	src.copyTo(dst_lab_it3, binMask_lab); 


	//��֮ǰ�Ļ����ϼ�������(���Կ���Ч�����˺ܶ�)
	grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT);
	getBinMask(mask, binMask);
	//binMask = mask.clone(); //����ֻȡ���λ��mask���������ֱ��ָ���ľ���ȡ��
	binMask.convertTo(binMask, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it3_it3, binMask);

	//�Ա�ֱ�ӵ���6�Σ�����Ч�������в�С����ġ�����
	Mat bgModel_1, fgdModel_1, mask_1, binMask_1, dst_rgb_it6; 
	grabCut(src, mask_1, rect, bgModel_1, fgdModel_1, 6, GC_INIT_WITH_RECT);
	getBinMask(mask_1, binMask_1);
	//binMask = mask.clone(); //����ֻȡ���λ��mask���������ֱ��ָ���ľ���ȡ��
	binMask_1.convertTo(binMask_1, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it6, binMask_1);


	//����:���������У�bgmodel��fbgmodel�Լ�mask�ֱ������?




	system("pause");
    return 0;
}

