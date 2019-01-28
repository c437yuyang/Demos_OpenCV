#include <highgui/highgui.hpp>    
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp> 
#include <iostream>  

using namespace cv;
using namespace std;

int main() {
	Mat image01 = imread("01.jpg", 1);    //��ͼ
	Mat image02 = imread("02.jpg", 1);    //��ͼ
	
	//Mat image01 = imread("../C001.jpg", 1);    //��ͼ
	//Mat image02 = imread("../C002.jpg", 1);    //��ͼ

	namedWindow("p2", 0);
	namedWindow("p1", 0);
	imshow("p2", image01);
	imshow("p1", image02);

	//�Ҷ�ͼת��  
	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);


	//��ȡ������    
	SiftFeatureDetector siftDetector(2000);  // ����������ֵ��������������ȣ�ֵԽ���Խ�٣�Խ��׼ 
	vector<KeyPoint> keyPoint1, keyPoint2;
	siftDetector.detect(image1, keyPoint1);
	siftDetector.detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
	SiftDescriptorExtractor SiftDescriptor;
	Mat imageDesc1, imageDesc2;
	SiftDescriptor.compute(image1, keyPoint1, imageDesc1);
	SiftDescriptor.compute(image2, keyPoint2, imageDesc2);

	//���ƥ�������㣬����ȡ�������     
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;

	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
	cout << "total match points: " << matchePoints.size() << endl;


	Mat img_match;
	drawMatches(image01, keyPoint1, image02, keyPoint2, matchePoints, img_match);

	imshow("match", img_match);
	imwrite("match.jpg", img_match);

	waitKey();
	return 0;
}