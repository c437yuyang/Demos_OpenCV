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
	start = clock();//开始计时
	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(frame);//创建一个对象

	slic->iterate();//迭代次数，默认为10
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask);//获取超像素的边界
	slic->getLabels(labels);//获取labels
	int number = slic->getNumberOfSuperpixels();//获取超像素的数量
	cout << "超像素数目:" << number << endl;
	frame.setTo(Scalar(0, 0, 255), mask);
	end = clock();//结束计时
	cout << "时间:" << end - start << endl;

	imshow("test", frame);

	int key = waitKey(0);

	return 0;
}