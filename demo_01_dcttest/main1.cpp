#include<iostream>  
#include<fstream>  
#include<opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp> //图形用户界面 
#include<opencv2/imgproc/imgproc.hpp> //调用copyMakeBorder()函数需要

using namespace std;
using namespace cv;
const double pi = 3.141592;
void Normalize(cv::Mat & mat);//归一化到0-255的函数
void PaddingImage(cv::Mat & srcImage, cv::Mat & dstImage);

int main()
{
	Mat image = imread("width.bmp", CV_LOAD_IMAGE_GRAYSCALE); //读为灰度图 
	/*int nWidth=image.cols;
	int nHeight=image.rows;*/

	//Mat imageTemp(Size(nWidth+8-image.cols%8, nHeight+8-image.rows%8), CV_32FC1);
	//copyMakeBorder(image,imageTemp,0,8-image.rows%8,0,8-image.cols%8,BORDER_CONSTANT,Scalar::all(0));
	//Mat imageAfter(Size(nWidth+8-image.cols%8, nHeight+8-image.rows%8), CV_32FC1);
	//imageTemp.convertTo(imageAfter, CV_32FC1);

	Mat imageAfter;
	PaddingImage(image, imageAfter);

	Mat G(Size(8, 8), CV_32FC1);//8*8系数矩阵,,32位float型,单通道  
	if (!image.empty())
		imshow("原图", image);////显示原图   

	//计算G系数 
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			float a;
			if (i == 0)
				a = sqrt(1.0 / 8.0);
			else
				a = sqrt(2.0 / 8.0);
			G.at<float>(i, j) = a*cos((j + 0.5)*pi*i / 8);
		}
	}
	cout << "系数矩阵G= " << endl << G << endl << endl;//打印系数矩阵 
	//imageAfter.convertTo(imageAfter, CV_32FC1);

	//imageAfter -= 128;//减了128的话对比度更明显
	for (int i = 0; i < imageAfter.rows; i = i + 8)
	{
		for (int j = 0; j < imageAfter.cols; j = j + 8)
		{
			imageAfter(Range(i, i + 8), Range(j, j + 8)) = G * imageAfter(Range(i, i + 8), Range(j, j + 8)) *G.t(); //分块，每8*8提取出来，A.t()是矩阵的转置
			//Normalize(imageAfter(Range(i, i + 8), Range(j, j + 8)));
			cv::normalize(imageAfter(Range(i, i + 8), Range(j, j + 8)), imageAfter(Range(i, i + 8), Range(j, j + 8)), 0, 255, cv::NORM_MINMAX);
		}
	}

	for (int i = 0; i < 8; i++)//打印第一个8*8DCT数据
	{
		for (int j = 0; j < 8; j++)
		{
			cout << imageAfter.at<float>(i, j) << ' ' << ' ';
		}
		cout << endl;
	}

	imshow("DCT", imageAfter);
	//cv::Mat dst;
	//cv::normalize(imageAfter, dst, 0, 255, cv::NORM_MINMAX);
	cv::Mat dst_8UC1;
	imageAfter.convertTo(dst_8UC1, CV_8UC1, 1.0);
	imshow("0to255", dst_8UC1);

	waitKey(0);
	return 0;
}

void Normalize(cv::Mat & mat)
{
	float max = -65536.0, min = 65536.0;

	for (int i = 0; i != mat.rows; ++i)
	{
		for (int j = 0; j != mat.cols; ++j)
		{
			if (mat.at<float>(i, j) < min)
			{
				min = mat.at<float>(i, j);
			}
			if (mat.at<float>(i, j) > max)
			{
				max = mat.at<float>(i, j);
			}
		}
	}


	for (int i = 0; i != mat.rows; ++i)
	{
		for (int j = 0; j != mat.cols; ++j)
		{
			mat.at<float>(i, j) = (mat.at<float>(i, j) - min) / (max - min)*(255);
		}
	}
}

void PaddingImage(cv::Mat & srcImage, cv::Mat & dstImage)
{

	/*int nWidth=srcImage.rows;
	int nHeight=srcImage.cols;*/
	int nWidth = srcImage.cols;
	int nHeight = srcImage.rows;
	Mat imageTemp;

	if (!(nWidth % 8) && !(nHeight % 8))//宽高都不需要填充
	{
		imageTemp.create(nWidth, nHeight, 0);//申请
		srcImage.copyTo(imageTemp);

	}
	if (nWidth % 8 && nHeight % 8)//宽高都需要填充
	{
		imageTemp.create(nHeight + 8 - (nHeight % 8), nWidth + 8 - (nWidth % 8), 0);//imageTemp.create(nWidth+8-(nWidth%8), nHeight+8-(nHeight%8),0);//申请
		copyMakeBorder(srcImage, imageTemp, 0, 8 - nHeight % 8, 0, 8 - nWidth % 8, BORDER_CONSTANT, Scalar::all(0));//扩充
		//Mat dstImage(Size(nWidth+8-srcImage.cols%8, nHeight+8-srcImage.rows%8), CV_32FC1);
		//dstImage=imageTemp.clone();
		//imageTemp.copyTo(dstImage, CV_32FC1);
	}
	if (nWidth % 8 && !(nHeight % 8))//宽需要填充
	{
		imageTemp.create(nHeight, nWidth + 8 - nWidth % 8, 0);//imageTemp.create(nWidth+8-nWidth%8, nHeight,0);//申请
		copyMakeBorder(srcImage, imageTemp, 0, 0, 0, 8 - (nWidth % 8), BORDER_CONSTANT, Scalar::all(0));//扩充
		//Mat dstImage(Size(nWidth+8-srcImage.cols%8, nHeight), CV_32FC1);
		//imageTemp.copyTo(dstImage, CV_32FC1);
		//dstImage=imageTemp.clone();
	}
	if (!(nWidth % 8) && nHeight % 8)//高需要填充
	{
		imageTemp.create(nHeight + 8 - (nHeight % 8), nWidth, 0);//imageTemp.create(nWidth, nHeight+8-(nHeight%8),0);//申请
		copyMakeBorder(srcImage, imageTemp, 0, 8 - nHeight % 8, 0, 0, BORDER_CONSTANT, Scalar::all(0));//扩充
		//Mat dstImage(Size(nWidth, nHeight+8-srcImage.rows%8), CV_32FC1);
		//imageTemp.copyTo(dstImage, CV_32FC1);
		//dstImage=imageTemp.clone();
	}
	imageTemp.copyTo(dstImage);
	return;
}