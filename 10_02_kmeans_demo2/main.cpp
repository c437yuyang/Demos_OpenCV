#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include "KMeans.h"
using namespace cv;
using namespace std;

//����Kmeans�㷨��һ��ȱ�㣬�ھ���֮ǰ��Ҫָ��������
const int nClusters = 12;

int main()
{
	Mat src;
	src = imread("../lena.jpg");        //cvLoadImage
	imshow("original", src);        //cvShowImage

	cv::Mat res = Kmeans::DoKMeansAlgorithm(src, nClusters);

	imshow("clustered", res);
	cv::waitKey(0);

	return 0;
}