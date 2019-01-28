#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main() 
{
	Mat frame1 = imread("../lena.jpg");
	imshow("lena", frame1);
	waitKey();
	

	return 0;
}