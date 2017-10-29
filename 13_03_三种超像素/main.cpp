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
	start = clock();//开始计时

	Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(src);//创建一个对象

	slic->iterate();//迭代次数，默认为10
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask_slic);//获取超像素的边界
	slic->getLabels(labels);//获取labels
	int number_slic = slic->getNumberOfSuperpixels();//获取超像素的数量
	cout << "number_slic:" << number_slic << endl;

	//这里可以放到循环外面
	Ptr<cv::ximgproc::SuperpixelSEEDS> seeds = cv::ximgproc::createSuperpixelSEEDS(src.cols, src.rows, src.channels(), 1000, 15, 2, 5, true);
	seeds->iterate(src);//迭代次数，默认为4
	seeds->getLabels(labels);//获取labels
	seeds->getLabelContourMask(mask_seeds);;//获取超像素的边界
	int number_seeds = seeds->getNumberOfSuperpixels();//获取超像素的数量
	cout << "number_seeds:" << number_seeds << endl;

	Ptr<cv::ximgproc::SuperpixelLSC> lsc = cv::ximgproc::createSuperpixelLSC(src);

	lsc->iterate();//迭代次数，默认为4
	lsc->enforceLabelConnectivity();
	lsc->getLabels(labels);//获取labels
	lsc->getLabelContourMask(mask_lsc);;//获取超像素的边界
	int number_lsc = lsc->getNumberOfSuperpixels();//获取超像素的数量
	cout << "number_lsc:" << number_lsc << endl;

	//显示结果
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


	end = clock();//结束计时
	cout << "时间:" << end - start << endl;


	 waitKey(0);

	return 0;
}