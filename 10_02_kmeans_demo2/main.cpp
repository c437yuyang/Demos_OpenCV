#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include "KMeans.h"
using namespace cv;
using namespace std;

//����Kmeans�㷨��һ��ȱ�㣬�ھ���֮ǰ��Ҫָ��������
const int nClusters = 16;

int main()
{
	Mat src;
	src = imread("../lena_slic_2000.bmp");        //cvLoadImage
	imshow("original", src);        //cvShowImage

	cv::Mat res = Kmeans::DoKMeansAlgorithm(src, nClusters,false,Kmeans::Feature::BGRXY);
	imshow("clustered_BGRXY", res);

	cv::Mat res1 = Kmeans::DoKMeansAlgorithm(src, nClusters, false, Kmeans::Feature::BGR);
	imshow("clustered_BGR", res1);

	cv::waitKey(0);

	return 0;
}