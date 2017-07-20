#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(int argv, char *args[])
{

	Mat img = imread("../lena.jpg");
	imshow("ԭͼ", img);

	vector<Mat> RgbChannels;
	split(img, RgbChannels); //分解为三个不同通道
	Mat r = RgbChannels[2];
	Mat g = RgbChannels[1];
	Mat b = RgbChannels[0];

	Mat imgGray;
	cvtColor(img, imgGray, CV_BGR2GRAY);
	
	imshow("imgGray", imgGray);

	Mat imgLab;
	cvtColor(img, imgLab, CV_BGR2Lab); //本身是lab的l在0-100，但是之后因为目标图像是8bit,故转换到了0-255，参见官方帮助文档
	vector<Mat> LabChannels;
	split(imgLab, LabChannels);
	Mat lab_l = LabChannels[0];
	Mat lab_a = LabChannels[1];
	Mat lab_b = LabChannels[2];


	cv::waitKey(0);
	return 0;

}