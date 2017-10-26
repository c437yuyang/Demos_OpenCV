#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class Kmeans
{
public:
	static cv::Mat DoKMeansAlgorithm(const cv::Mat & src, int clusters);
};


