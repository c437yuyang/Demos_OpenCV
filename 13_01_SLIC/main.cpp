#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
//#include "SLIC.hpp"
#include <ctime>
using namespace cv;
using namespace std;

int main()
{
	//cv::Mat lena = imread("../lena.jpg");
	//imshow("lena", lena);
	//cv::waitKey(0);
	clock_t start;
	clock_t end;
	Mat frame, labels;
	VideoCapture capture("E://image/skating2.avi");//���ļ�
	Mat mask;
	if (!capture.isOpened())
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
	}

	while (1)
	{
		capture >> frame;//��ȡһ֡ͼ��
		start = clock();//��ʼ��ʱ

		Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(frame);//����һ������
		
		slic->iterate();//����������Ĭ��Ϊ10
		slic->enforceLabelConnectivity();
		slic->getLabelContourMask(mask);//��ȡ�����صı߽�
		slic->getLabels(labels);//��ȡlabels
		int number = slic->getNumberOfSuperpixels();//��ȡ�����ص�����

		frame.setTo(Scalar(255, 255, 255), mask);
		end = clock();//������ʱ
		cout << "ʱ��:" << end - start << endl;

		imshow("test", frame);

		int key = waitKey(1);
		if (key == 27)
			break;
	}
	return 0;
}