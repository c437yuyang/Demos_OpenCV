#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

//这个demo就是上一个demo只考虑颜色的情况(3维)，发现并没有什么区别

//这是Kmeans算法的一个缺点，在聚类之前需要指定类别个数
const int nClusters = 12;

int main()
{
	Mat src;
	src = imread("../lena.jpg");        //cvLoadImage
	imshow("original", src);        //cvShowImage

	blur(src, src, Size(11, 11));
	imshow("blurred", src);

	//p是特征矩阵，每行表示一个特征，每个特征对应src中每个像素点的(x,y,r,g,b共5维)
	Mat p = Mat::zeros(src.cols*src.rows, 3, CV_32F);    //初始化全0矩阵
	Mat bestLabels, centers, clustered;
	vector<Mat> bgr;
	cv::split(src, bgr);    //分隔出src的三个通道

	for (int i = 0; i < src.cols*src.rows; i++)
	{
		p.at<float>(i, 0) = bgr[0].data[i] / 255.0;
		p.at<float>(i, 1) = bgr[1].data[i] / 255.0;
		p.at<float>(i, 2) = bgr[2].data[i] / 255.0;
	}

	//计算时间
	double t = (double)cvGetTickCount();

	//kmeans聚类，每个样本的标签保存在bestLabels中
	cv::kmeans(p, nClusters, bestLabels,
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, KMEANS_PP_CENTERS, centers);

	t = (double)cvGetTickCount() - t;
	float timecost = t / (cvGetTickFrequency() * 1000);

	//给每个类别赋颜色，其值等于每个类第一个元素的值
	Vec3b colors[nClusters];
	bool colormask[nClusters];
	memset(colormask, 0, nClusters * sizeof(bool));
	int count = 0;
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		int clusterindex = bestLabels.at<int>(i, 0);
		for (int j = 0; j < nClusters; j++)
		{
			if (j == clusterindex && colormask[j] == 0)
			{
				int y = i / src.cols;
				int x = i % src.cols;
				colors[j] = src.at<Vec3b>(y, x);
				colormask[j] = 1;
				count++;
				break;
			}
		}
		if (nClusters == count)break;
	}

	//显示聚类结果
	clustered = Mat(src.rows, src.cols, CV_8UC3);
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		int y = i / src.cols;
		int x = i % src.cols;
		int clusterindex = bestLabels.at<int>(i, 0);
		clustered.at<Vec3b>(y, x) = colors[clusterindex];
	}

	imshow("clustered", clustered);

	cout << "time cost = %gms\n" << timecost;

	//保存图像
	stringstream s1, s2;
	s1 << timecost;
	s2 << nClusters;
	string name = "n=" + s2.str() + "_timecost=" + s1.str() + ".png";
	//imwrite(name, clustered);
	waitKey();
	return 0;
}