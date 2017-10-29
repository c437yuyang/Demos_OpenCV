#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
//#include "SLIC.hpp"
#include <ctime>
using namespace cv;
using namespace std;

int main()
{
	clock_t start;
	clock_t end;
	Mat frame = imread("../lena.jpg"), labels;
	Mat mask;
	start = clock();//��ʼ��ʱ
	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(frame);//����һ������

	slic->iterate();//����������Ĭ��Ϊ10
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask);//��ȡ�����صı߽�
	slic->getLabels(labels);//��ȡlabels
	int number = slic->getNumberOfSuperpixels();//��ȡ�����ص�����
	cout << "��������Ŀ:" << number << endl;
	frame.setTo(Scalar(0, 0, 255), mask);
	end = clock();//������ʱ
	cout << "ʱ��:" << end - start << endl;

	imshow("test", frame);

	int key = waitKey(0);

	return 0;
}