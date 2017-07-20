#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
static void ajustWeight(int, void*);
int g_srcWeight = 50;
int g_addPlainWeight = 50;

Mat g_src = imread("3dIMG_ 1.jpg");
Mat g_addPlain = imread("3dIMG_ 1_SLICO.bmp");
Mat g_dst;

int main()
{
	CV_Assert(g_addPlain.size() == g_src.size());
	namedWindow("dst", WINDOW_NORMAL);
	createTrackbar("原始图像比例：", "dst", &g_srcWeight, 100, ajustWeight);
	createTrackbar("叠加图层比例：", "dst", &g_addPlainWeight, 100, ajustWeight);
	ajustWeight(g_srcWeight, 0);
	imshow("dst", g_dst);
	while (char(waitKey(1)) != 'q') {}
	return 0;

}

static void ajustWeight(int, void*) 
{
	//g_dst = Mat::zeros(g_src.size(), CV_8UC3);
	addWeighted(g_src, g_srcWeight / 100.0, g_addPlain, g_addPlainWeight/100.0, 0.0, g_dst);
	namedWindow("dst", WINDOW_NORMAL);
	imshow("dst", g_dst);

}