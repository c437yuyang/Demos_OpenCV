#include "KMeans.h"

cv::Mat Kmeans::DoKMeansAlgorithm(const cv::Mat & src, int cluster_num, bool pre_blur, Feature feature)
{
	cv::Mat blured;
	if (pre_blur)
		blur(src, blured, cv::Size(11, 11));
	else
		blured = src.clone();
	//p是特征矩阵，每行表示一个特征，每个特征对应src中每个像素点的(x,y,r,g,b共5维)
	cv::Mat p;
	if (feature == Feature::BGR)
		p = cv::Mat::zeros(src.cols*src.rows, 3, CV_32F);    //初始化全0矩阵
	else if (feature == Feature::BGRXY)
		p = cv::Mat::zeros(src.cols*src.rows, 5, CV_32F);    //初始化全0矩阵

	cv::Mat bestLabels, centers, clustered;
	std::vector<cv::Mat> bgr;
	cv::split(blured, bgr);    //分隔出src的三个通道

	if (feature == Feature::BGR)
		for (int i = 0; i < src.cols*src.rows; i++)
		{
			p.at<float>(i, 0) = bgr[0].data[i] / 255.0; //B
			p.at<float>(i, 1) = bgr[1].data[i] / 255.0; //G
			p.at<float>(i, 2) = bgr[2].data[i] / 255.0; //R

		}
	else if (feature == Feature::BGRXY)
		for (int i = 0; i < src.cols*src.rows; i++)
		{
			p.at<float>(i, 0) = bgr[0].data[i] / 255.0;
			p.at<float>(i, 1) = bgr[1].data[i] / 255.0;
			p.at<float>(i, 2) = bgr[2].data[i] / 255.0;
			p.at<float>(i, 3) = i % src.cols * 1.0 / src.cols; //X
			p.at<float>(i, 4) = i / src.cols * 1.0 / src.rows; //Y
		}

	//计算时间
	double t = (double)cvGetTickCount();

	//kmeans聚类，每个样本的标签保存在bestLabels中
	cv::kmeans(p, cluster_num, bestLabels,
		cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, cv::KMEANS_PP_CENTERS, centers);

	t = (double)cvGetTickCount() - t;
	float timecost = t / (cvGetTickFrequency() * 1000);

	//给每个类别赋颜色，其值等于每个类第一个元素的值
	std::vector<cv::Vec3b> colors(cluster_num);
	std::vector<bool> colormask(cluster_num, false);
	int count = 0;
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		int clusterindex = bestLabels.at<int>(i, 0);
		for (int j = 0; j < cluster_num; j++)
		{
			if (j == clusterindex && colormask[j] == 0)
			{
				int y = i / src.cols;
				int x = i % src.cols;
				colors[j] = src.at<cv::Vec3b>(y, x);
				colormask[j] = 1;
				count++;
				break;
			}
		}
		if (cluster_num == count)break;
	}

	//显示聚类结果
	clustered = cv::Mat(src.rows, src.cols, CV_8UC3);
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		int y = i / src.cols;
		int x = i % src.cols;
		int clusterindex = bestLabels.at<int>(i, 0);
		clustered.at<cv::Vec3b>(y, x) = colors[clusterindex];
	}

	return clustered;
}
