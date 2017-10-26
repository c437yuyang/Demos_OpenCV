#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

//���demo������һ��demoֻ������ɫ�����(3ά)�����ֲ�û��ʲô����

//����Kmeans�㷨��һ��ȱ�㣬�ھ���֮ǰ��Ҫָ��������
const int nClusters = 12;

int main()
{
	Mat src;
	src = imread("../lena.jpg");        //cvLoadImage
	imshow("original", src);        //cvShowImage

	blur(src, src, Size(11, 11));
	imshow("blurred", src);

	//p����������ÿ�б�ʾһ��������ÿ��������Ӧsrc��ÿ�����ص��(x,y,r,g,b��5ά)
	Mat p = Mat::zeros(src.cols*src.rows, 3, CV_32F);    //��ʼ��ȫ0����
	Mat bestLabels, centers, clustered;
	vector<Mat> bgr;
	cv::split(src, bgr);    //�ָ���src������ͨ��

	for (int i = 0; i < src.cols*src.rows; i++)
	{
		p.at<float>(i, 0) = bgr[0].data[i] / 255.0;
		p.at<float>(i, 1) = bgr[1].data[i] / 255.0;
		p.at<float>(i, 2) = bgr[2].data[i] / 255.0;
	}

	//����ʱ��
	double t = (double)cvGetTickCount();

	//kmeans���࣬ÿ�������ı�ǩ������bestLabels��
	cv::kmeans(p, nClusters, bestLabels,
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, KMEANS_PP_CENTERS, centers);

	t = (double)cvGetTickCount() - t;
	float timecost = t / (cvGetTickFrequency() * 1000);

	//��ÿ�������ɫ����ֵ����ÿ�����һ��Ԫ�ص�ֵ
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

	//��ʾ������
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

	//����ͼ��
	stringstream s1, s2;
	s1 << timecost;
	s2 << nClusters;
	string name = "n=" + s2.str() + "_timecost=" + s1.str() + ".png";
	//imwrite(name, clustered);
	waitKey();
	return 0;
}