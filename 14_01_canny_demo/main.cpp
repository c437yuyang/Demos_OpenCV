

//---------------------------------��ͷ�ļ��������ռ�������֡�----------------------------
//		����������������ʹ�õ�ͷ�ļ��������ռ�
//------------------------------------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;

#define WINDOW_NAME "���ڳ���"

void on_Cany(int, void*);

cv::Mat	g_srcImage, g_dstImage;
int g_nThreshold1 = 120;
int g_nThreshold2 = 60;


//-----------------------------------��main( )������-------------------------------------------
//            ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-----------------------------------------------------------------------------------------------
int main()
{
	system("color 1F");

	//����ԭʼͼ  
	g_srcImage = imread("../lena_slic_2000.bmp");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ
	g_dstImage = g_srcImage.clone();
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
	Mat tmp = g_srcImage.clone();
	//blur(tmp, tmp, Size(11, 11));
	Canny(tmp, g_dstImage, g_nThreshold1, g_nThreshold2, 3);
	imshow(WINDOW_NAME, g_dstImage);
}