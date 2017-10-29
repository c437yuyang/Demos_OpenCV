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
	VideoCapture capture("E://image/skating2.avi");//打开文件
	Mat mask;
	if (!capture.isOpened())
	{
		cout << "文件打开失败！" << endl;
	}

	while (1)
	{
		capture >> frame;//获取一帧图像
		start = clock();//开始计时

		Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(frame);//创建一个对象
		
		slic->iterate();//迭代次数，默认为10
		slic->enforceLabelConnectivity();
		slic->getLabelContourMask(mask);//获取超像素的边界
		slic->getLabels(labels);//获取labels
		int number = slic->getNumberOfSuperpixels();//获取超像素的数量

		frame.setTo(Scalar(255, 255, 255), mask);
		end = clock();//结束计时
		cout << "时间:" << end - start << endl;

		imshow("test", frame);

		int key = waitKey(1);
		if (key == 27)
			break;
	}
	return 0;
}