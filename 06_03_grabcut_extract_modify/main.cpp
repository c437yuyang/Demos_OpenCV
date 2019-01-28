#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
#include "grabcut_sc.h"
typedef unsigned int UINT;



static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
	medianBlur(binMask, binMask, 6 * 2 + 1); //去除噪声
}
int main()
{
	//Rect rect(170, 150, 700 - 170, 500 - 150);
	//Mat src = imread("3dIMG_ 2_resize.jpg");

	Rect rect(78, 47, 348 ,471);
	Mat src = imread("lena.jpg");

	//cvtColor(src, src, CV_BGR2Lab);
	//Rect rect(480, 300, 1350 - 480, 900 - 300);
	//Mat src = imread("64_复杂背景.jpg");

	Mat dst_rgb_it3, bgModel, fgdModel, mask, binMask, dst_rgb_it3_it3;
	//得到的mask里只有最后两位有值，最后一位为1代表是前景，倒数第二位为1代表可能是前景，就是自己画的矩形
	yxp_utility::grabCut(src, mask, rect, bgModel, fgdModel, 4, GC_INIT_WITH_RECT); //最好是在release下，debug太慢
	getBinMask(mask, binMask);
	//binMask = mask.clone(); //必须只取最低位的mask，否则就是直接指定的矩形取到
	binMask.convertTo(binMask, CV_8UC1, 255.0);
	src.copyTo(dst_rgb_it3, binMask);
	imshow("res", dst_rgb_it3);
	waitKey(0);
	system("pause");
	return 0;
}




