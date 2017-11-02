#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class Kmeans
{
public:
	
	typedef enum Feature
	{
		BGR, BGRXY
	}Feature;
	static cv::Mat DoKMeansAlgorithm(const cv::Mat & src, int clusters, bool pre_blur, Feature feature);
};


