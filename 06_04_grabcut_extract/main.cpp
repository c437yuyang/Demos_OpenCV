#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
#include "grabcut_sc.h"
#include "SLIC.h"
typedef unsigned int UINT;

struct FgdBgdCnt
{
	int fgdCnt;
	int bgdCnt;
};

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
	//medianBlur(binMask, binMask, 6 * 2 + 1); //ȥ������
}

template <class T> //��������������ڴ�,��ͨ����ɫͼ��ת��
void MatConvert(const Mat & src, T* dst)
{
	if (dst == NULL) return;
	for (int i = 0; i != src.rows; ++i)
	{
		for (int j = 0; j != src.cols; ++j)
		{
			Vec3b pt = src.at<Vec3b>(i, j);
			dst[i*src.cols * 3 + j * 3 + 0] = static_cast<T>(pt[0]);
			dst[i*src.cols * 3 + j * 3 + 1] = static_cast<T>(pt[1]);
			dst[i*src.cols * 3 + j * 3 + 2] = static_cast<T>(pt[2]);
		}
	}
}


int main()
{
	Rect rect(170, 150, 700 - 170, 500 - 150);
	Mat src = imread("3dIMG_ 2_resize.jpg");
	//cvtColor(src, src, CV_BGR2Lab);
	//Rect rect(480, 300, 1350 - 480, 900 - 300);
	//Mat src = imread("64_���ӱ���.jpg");


	int w = src.cols, h = src.rows;
	//��ͼ����г����طָ�
	UINT* imgBuff = new UINT[src.cols * src.rows * 3]();
	//��Ҫ����һ��uint�����ͣ��Ƚ�ͼ�����ͽ���ת��
	MatConvert(src, imgBuff);
	SLIC slic;
	int *kLables = new int[w*h]();
	int numLabels(0), weight = 0 /*��ʱû��*/, color = 222, nSpNum = 4000;
	slic.PerformSLICO_ForGivenK(imgBuff, w, h, kLables, numLabels, nSpNum, weight);

#pragma region ���ӻ�SLIC���1���ñ߽�ķ�ʽ������
	//UINT* SegmentResult = new UINT[w*h];
	//slic.DrawContoursAroundSegments(SegmentResult, kLables, w, h, color);
	//Mat SLICResult = src.clone();
	//Vec3b red(0, 0, 255);
	//for (int i = 0; i != h; ++i)
	//	for (int j = 0; j != w; ++j)
	//		if (SegmentResult[i*w + j] == color)
	//			SLICResult.at<Vec3b>(i, j) = red;
#pragma endregion

#pragma region ���ӻ���ʽ2
//	uchar g_colors[16][3] = { //Ԥ����ɫ��
//{ 223,0,36 },{ 103,110,176 },{ 0,159,60 },{ 240,220,0 },
//{ 237,108,158 },{ 94,138,198 },{ 232,100,27 },{ 239,156,0 },
//{ 106,80,155 },{ 124,198,35 },{ 142,186,155 },{ 0,164,157 },
//{ 168,213,157 },{ 61,16,123 },{ 49,67,150 },{ 0,0,0 }
//	};
//
//	Mat SlicRegionDisplay(h, w, CV_8UC3);
//	for (int i = 0; i != h; ++i)
//	{
//		for (int j = 0; j != w; ++j)
//		{
//			int  clrIdx = kLables[i*w + j] % 16;
//			SlicRegionDisplay.at<Vec3b>(i, j) = Vec3b(g_colors[clrIdx][2], g_colors[clrIdx][1], g_colors[clrIdx][0]);
//		}
//	}
#pragma endregion

	//�γ�һ���µ�mat,�ɳ��������,һ��һ�����أ�3�зֱ���rgbֵ
	//����:����һ��һ�������뱣�ֺ�ԭͼ��һ��������������������,��������ĳ����ص���Ŀ��֮ǰ�趨��ͨ������һ�£�Ҫ��������أ������Ʊ߽�����𣿣�
	//��ͳ��ÿ�������ص���Ϣ�����rgbֵ

	double step = sqrt(double(w*h) / double(nSpNum));
	int cols = static_cast<int> (w / step);
	int rows = static_cast<int> (h / step);
	if (cols*rows != nSpNum) return -16;

	Mat src_sp(numLabels, 1, CV_8UC3);
	vector<Vec3d> rgbInfo(numLabels);
	vector<int> spNum(numLabels, 0);
	vector<FgdBgdCnt> fbgdCnt(numLabels);
	//������ͼ��ȷ����Щ����TU����Щ��TB��֮��ĳ�ʼ������ģ��ʼ������������rect���г�ʼ����
	//�����趨�ľ��Σ�ͳ�Ƴ�������TU����TB���������ڱ߽�ģ���ͶƱ���ƽ��(����������TB����TB)
	for (int i = 0; i != h; ++i)
	{
		for (int j = 0; j != w; ++j)
		{
			int spid = kLables[i*w + j];
			rgbInfo[spid] += src.at<Vec3b>(i, j);
			++spNum[spid]; //ͳ��ÿ��������ӵ�е�������Ŀ
			if (Point(j, i).inside(rect)) 
				++fbgdCnt[spid].fgdCnt;
			else ++fbgdCnt[spid].bgdCnt;
		}
	}

	Mat mask(numLabels, 1, CV_8UC1); //��ǰ��ͳ�Ƶ���Ϣ�õ������ؼ����mask
	mask.setTo(GC_BGD);
	for (int i = 0; i != numLabels; ++i)
	{
		rgbInfo[i] /= spNum[i];
		src_sp.at<Vec3b>(i, 0) = static_cast<Vec3b>(rgbInfo[i]); //�õ������صľ�ֵ
		if (fbgdCnt[i].fgdCnt >= fbgdCnt[i].bgdCnt) mask.at<uchar>(i, 0) = GC_PR_FGD;
	}

	
#pragma region ���ӻ�һ�µõ���mask
	//Mat maskDisp = Mat::zeros(h, w, CV_8UC1);
	//for (int i = 0; i != h; ++i)
	//	for (int j = 0; j != w; ++j)
	//		if (mask.at<uchar>(kLables[i*w + j], 0) == GC_PR_FGD) maskDisp.at<uchar>(i, j) = 255;
#pragma endregion

	//ִ��grabcut
	Mat gc, bgModel, fgdModel, binMask;
	yxp_utility::grabCut(src_sp, mask, rect, bgModel, fgdModel, 5, GC_INIT_WITH_MASK);
	getBinMask(mask, binMask);

	//�ָ���ԭʼͼ�����ģ
	Mat res = Mat::zeros(h, w, CV_8UC1);
	for (int i = 0; i != h; ++i)
		for (int j = 0; j != w; ++j)
		{
			int spid = kLables[i*w + j];
			if (binMask.at<uchar>(spid) == GC_FGD) res.at<uchar>(i, j) = 255;
		}

	//���յõ��ָ���
	Mat dst;
	src.copyTo(dst, res);


	//Mat dst_rgb_it3, bgModel, fgdModel, mask, binMask, dst_rgb_it3_it3;
	////�õ���mask��ֻ�������λ��ֵ�����һλΪ1������ǰ���������ڶ�λΪ1���������ǰ���������Լ����ľ���
	//yxp_utility::grabCut(src, mask, rect, bgModel, fgdModel, 3, GC_INIT_WITH_RECT); //�������release�£�debug̫��
	//getBinMask(mask, binMask);
	////binMask = mask.clone(); //����ֻȡ���λ��mask���������ֱ��ָ���ľ���ȡ��
	//binMask.convertTo(binMask, CV_8UC1, 255.0);
	//src.copyTo(dst_rgb_it3, binMask);


	system("pause");
	return 0;
}




