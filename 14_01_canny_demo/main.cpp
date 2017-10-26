

//---------------------------------��ͷ�ļ��������ռ�������֡�----------------------------
//		����������������ʹ�õ�ͷ�ļ��������ռ�
//------------------------------------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

#define WINDOW_NAME "���ڳ���"

void on_Cany(int, void*);

cv::Mat	g_srcImage, g_srcImage1;
int g_nThreshold1 = 150;
int g_nThreshold2 = 50;


//-----------------------------------��main( )������-------------------------------------------
//            ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-----------------------------------------------------------------------------------------------
int main()
{
	system("color 1F");

	//����ԭʼͼ  
	g_srcImage = imread("../lena.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ
	g_srcImage1 = g_srcImage.clone();
	imshow("��ԭʼͼ��Canny��Ե���", g_srcImage);
	namedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
	createTrackbar("��ֵ1", WINDOW_NAME, &g_nThreshold1, 300, on_Cany);
	createTrackbar("��ֵ2", WINDOW_NAME, &g_nThreshold2, 100, on_Cany);
	//��ʾԭʼͼ 
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
	//	һ����򵥵�canny�÷����õ�ԭͼ��ֱ���á�
	//---------------------------------------------------------------------------------
	return 0;
}

void on_Cany(int, void*)
{
	Canny(g_srcImage, g_srcImage1, g_nThreshold1, g_nThreshold2, 3);
	imshow(WINDOW_NAME, g_srcImage1);
}