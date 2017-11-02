

//---------------------------------【头文件、命名空间包含部分】----------------------------
//		描述：包含程序所使用的头文件和命名空间
//------------------------------------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

#define WINDOW_NAME "窗口程序"

void on_Cany(int, void*);

cv::Mat	g_srcImage, g_dstImage;
int g_nThreshold1 = 120;
int g_nThreshold2 = 60;


//-----------------------------------【main( )函数】-------------------------------------------
//            描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{
	system("color 1F");

	//载入原始图  
	g_srcImage = imread("../lena_slic_2000.bmp");  //工程目录下应该有一张名为1.jpg的素材图
	g_dstImage = g_srcImage.clone();
	imshow("【原始图】Canny边缘检测", g_srcImage);
	namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
	createTrackbar("阈值1", WINDOW_NAME, &g_nThreshold1, 300, on_Cany);
	createTrackbar("阈值2", WINDOW_NAME, &g_nThreshold2, 100, on_Cany);
	//显示原始图 
	on_Cany(0, 0);
	while (1)
	{
		int key = waitKey(10);
		if (key == 27)
		{
			break;
		}
	}
	//----------------------------------------------------------------------------------
	//	一、最简单的canny用法，拿到原图后直接用。
	//---------------------------------------------------------------------------------
	return 0;
}

void on_Cany(int, void*)
{
	Mat tmp = g_srcImage.clone();
	//blur(tmp, tmp, Size(11, 11));
	Canny(tmp, g_dstImage, g_nThreshold1, g_nThreshold2, 3);
	imshow(WINDOW_NAME, g_dstImage);
}