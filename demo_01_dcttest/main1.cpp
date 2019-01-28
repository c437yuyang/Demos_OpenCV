#include<iostream>  
#include<fstream>  
#include<opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp> //ͼ���û����� 
#include<opencv2/imgproc/imgproc.hpp> //����copyMakeBorder()������Ҫ

using namespace std;
using namespace cv;
const double pi = 3.141592;
void Normalize(cv::Mat & mat);//��һ����0-255�ĺ���
void PaddingImage(cv::Mat & srcImage, cv::Mat & dstImage);

int main()
{
	Mat image = imread("width.bmp", CV_LOAD_IMAGE_GRAYSCALE); //��Ϊ�Ҷ�ͼ 
	/*int nWidth=image.cols;
	int nHeight=image.rows;*/

	//Mat imageTemp(Size(nWidth+8-image.cols%8, nHeight+8-image.rows%8), CV_32FC1);
	//copyMakeBorder(image,imageTemp,0,8-image.rows%8,0,8-image.cols%8,BORDER_CONSTANT,Scalar::all(0));
	//Mat imageAfter(Size(nWidth+8-image.cols%8, nHeight+8-image.rows%8), CV_32FC1);
	//imageTemp.convertTo(imageAfter, CV_32FC1);

	Mat imageAfter;
	PaddingImage(image, imageAfter);

	Mat G(Size(8, 8), CV_32FC1);//8*8ϵ������,,32λfloat��,��ͨ��  
	if (!image.empty())
		imshow("ԭͼ", image);////��ʾԭͼ   

	//����Gϵ�� 
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
	cout << "ϵ������G= " << endl << G << endl << endl;//��ӡϵ������ 
	//imageAfter.convertTo(imageAfter, CV_32FC1);

	//imageAfter -= 128;//����128�Ļ��Աȶȸ�����
	for (int i = 0; i < imageAfter.rows; i = i + 8)
	{
		for (int j = 0; j < imageAfter.cols; j = j + 8)
		{
			imageAfter(Range(i, i + 8), Range(j, j + 8)) = G * imageAfter(Range(i, i + 8), Range(j, j + 8)) *G.t(); //�ֿ飬ÿ8*8��ȡ������A.t()�Ǿ����ת��
			//Normalize(imageAfter(Range(i, i + 8), Range(j, j + 8)));
			cv::normalize(imageAfter(Range(i, i + 8), Range(j, j + 8)), imageAfter(Range(i, i + 8), Range(j, j + 8)), 0, 255, cv::NORM_MINMAX);
		}
	}

	for (int i = 0; i < 8; i++)//��ӡ��һ��8*8DCT����
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

	if (!(nWidth % 8) && !(nHeight % 8))//��߶�����Ҫ���
	{
		imageTemp.create(nWidth, nHeight, 0);//����
		srcImage.copyTo(imageTemp);

	}
	if (nWidth % 8 && nHeight % 8)//��߶���Ҫ���
	{
		imageTemp.create(nHeight + 8 - (nHeight % 8), nWidth + 8 - (nWidth % 8), 0);//imageTemp.create(nWidth+8-(nWidth%8), nHeight+8-(nHeight%8),0);//����
		copyMakeBorder(srcImage, imageTemp, 0, 8 - nHeight % 8, 0, 8 - nWidth % 8, BORDER_CONSTANT, Scalar::all(0));//����
		//Mat dstImage(Size(nWidth+8-srcImage.cols%8, nHeight+8-srcImage.rows%8), CV_32FC1);
		//dstImage=imageTemp.clone();
		//imageTemp.copyTo(dstImage, CV_32FC1);
	}
	if (nWidth % 8 && !(nHeight % 8))//����Ҫ���
	{
		imageTemp.create(nHeight, nWidth + 8 - nWidth % 8, 0);//imageTemp.create(nWidth+8-nWidth%8, nHeight,0);//����
		copyMakeBorder(srcImage, imageTemp, 0, 0, 0, 8 - (nWidth % 8), BORDER_CONSTANT, Scalar::all(0));//����
		//Mat dstImage(Size(nWidth+8-srcImage.cols%8, nHeight), CV_32FC1);
		//imageTemp.copyTo(dstImage, CV_32FC1);
		//dstImage=imageTemp.clone();
	}
	if (!(nWidth % 8) && nHeight % 8)//����Ҫ���
	{
		imageTemp.create(nHeight + 8 - (nHeight % 8), nWidth, 0);//imageTemp.create(nWidth, nHeight+8-(nHeight%8),0);//����
		copyMakeBorder(srcImage, imageTemp, 0, 8 - nHeight % 8, 0, 0, BORDER_CONSTANT, Scalar::all(0));//����
		//Mat dstImage(Size(nWidth, nHeight+8-srcImage.rows%8), CV_32FC1);
		//imageTemp.copyTo(dstImage, CV_32FC1);
		//dstImage=imageTemp.clone();
	}
	imageTemp.copyTo(dstImage);
	return;
}