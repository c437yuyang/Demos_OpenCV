#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {

	//其他窗口->image watch //vs13,15都装了

	cout << "a" << endl;

	Mat lena = imread("lena.jpg");
	Mat lena_gray,lena_gray1;
	cvtColor(lena, lena_gray, CV_BGR2GRAY);
	cvtColor(lena, lena_gray1, CV_RGB2GRAY);
	Mat lena_hsv,lena_hsv_full;
	cvtColor(lena, lena_hsv, CV_BGR2HSV);
	cvtColor(lena, lena_hsv_full, CV_BGR2HSV_FULL);

	Mat mat1 = Mat::ones(Size(100, 150), CV_8UC3);

	/*imshow("lena", lena);*/

	cv::waitKey(0);

	return 0;
}