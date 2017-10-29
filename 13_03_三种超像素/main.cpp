#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include <ctime>
using namespace cv;
using namespace std;

int main()
{
	clock_t start;
	clock_t end;
	Mat src = imread("../1.jpg"), labels;
	imshow("src", src);

	Mat mask_slic,mask_seeds,mask_lsc;
	start = clock();//��ʼ��ʱ

	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(src);//����һ������

	slic->iterate();//����������Ĭ��Ϊ10
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask_slic);//��ȡ�����صı߽�
	slic->getLabels(labels);//��ȡlabels
	int number_slic = slic->getNumberOfSuperpixels();//��ȡ�����ص�����
	cout << "number_slic:" << number_slic << endl;

	//������Էŵ�ѭ������
	Ptr<cv::ximgproc::SuperpixelSEEDS> seeds = cv::ximgproc::createSuperpixelSEEDS(src.cols, src.rows, src.channels(), 1000, 15, 2, 5, true);
	seeds->iterate(src);//����������Ĭ��Ϊ4
	seeds->getLabels(labels);//��ȡlabels
	seeds->getLabelContourMask(mask_seeds);;//��ȡ�����صı߽�
	int number_seeds = seeds->getNumberOfSuperpixels();//��ȡ�����ص�����
	cout << "number_seeds:" << number_seeds << endl;

	Ptr<cv::ximgproc::SuperpixelLSC> lsc = cv::ximgproc::createSuperpixelLSC(src);

	lsc->iterate();//����������Ĭ��Ϊ4
	lsc->enforceLabelConnectivity();
	lsc->getLabels(labels);//��ȡlabels
	lsc->getLabelContourMask(mask_lsc);;//��ȡ�����صı߽�
	int number_lsc = lsc->getNumberOfSuperpixels();//��ȡ�����ص�����
	cout << "number_lsc:" << number_lsc << endl;

	//��ʾ���
	Mat dst_slic, dst_seeds, dst_lsc;
	src.copyTo(dst_slic);
	src.copyTo(dst_seeds);
	src.copyTo(dst_lsc);
	dst_slic.setTo(Scalar(255, 255, 255), mask_slic);
	dst_seeds.setTo(Scalar(255, 255, 255), mask_seeds);
	dst_lsc.setTo(Scalar(255, 255, 255), mask_lsc);
	imshow("dst_slic", dst_slic);
	imshow("dst_seeds", dst_seeds);
	imshow("dst_lsc", dst_lsc);


	end = clock();//������ʱ
	cout << "ʱ��:" << end - start << endl;


	 waitKey(0);

	return 0;
}