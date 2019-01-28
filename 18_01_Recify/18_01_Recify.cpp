#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

#define ERROR 1234

//����ת��
double DegreeTrans(double theta) {
	double res = theta / CV_PI * 180;
	return res;
}


//��ʱ����תͼ��degree�Ƕȣ�ԭ�ߴ磩    
void rotateImage(Mat src, Mat& img_rotate, double degree) {
	//��ת����Ϊͼ������    
	Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols*src.cols + src.rows*src.rows);
	//�����ά��ת�ķ���任����  
	Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255, 255, 255));//����任������ɫ���Ϊ��ɫ  
}

//ͨ������任����Ƕ�
double CalcDegree(const Mat &srcImage, Mat &dst) {
	Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	//ͨ������任���ֱ��
	vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//��5������������ֵ����ֵԽ�󣬼�⾫��Խ��
	 //cout << lines.size() << endl;

	 //����ͼ��ͬ����ֵ�����趨����Ϊ��ֵ�趨���ߵ����޷����ֱ�ߣ���ֵ����ֱ��̫�࣬�ٶȺ���
	 //���Ը�����ֵ�ɴ�С������������ֵ�����������������󣬿��Թ̶�һ���ʺϵ���ֵ��

	if (!lines.size()) {
		HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
	}
	//cout << lines.size() << endl;

	if (!lines.size()) {
		HoughLines(midImage, lines, 1, CV_PI / 180, 150, 0, 0);
	}
	//cout << lines.size() << endl;
	if (!lines.size()) {
		cout << "û�м�⵽ֱ�ߣ�" << endl;
		return ERROR;
	}

	float sum = 0;
	//���λ���ÿ���߶�
	for (size_t i = 0; i < lines.size(); i++) {
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		//cout << theta << endl;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//ֻѡ�Ƕ���С����Ϊ��ת�Ƕ�
		sum += theta;

		line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1); //Scalar�������ڵ����߶���ɫ

		imshow("ֱ��̽��Ч��ͼ", dstImage);
	}
	float average = sum / lines.size(); //�����нǶ���ƽ������������תЧ�������

	cout << "average theta:" << average << endl;

	double angle = DegreeTrans(average) - 90;

	rotateImage(dstImage, dst, angle);
	//imshow("ֱ��̽��Ч��ͼ2", dstImage);
	return angle;
}


void ImageRecify(const char* pInFileName, const char* pOutFileName) {
	double degree;
	Mat src = imread(pInFileName);
	imshow("ԭʼͼ", src);
	Mat dst;
	//��б�ǶȽ���
	degree = CalcDegree(src, dst);
	if (degree == ERROR) {
		cout << "����ʧ�ܣ�" << endl;
		return;
	}
	rotateImage(src, dst, degree);
	cout << "angle:" << degree << endl;
	imshow("��ת������", dst);

	Mat resulyImage = dst(Rect(0, 0, dst.cols, 500)); //��������֪ʶ�����ƺ��ı��ĳ����ٲü�����
	imshow("�ü�֮��", resulyImage);
	imwrite("recified.jpg", resulyImage);
}


int main() {
	ImageRecify("correct2.png", "FinalImage.jpg");
	waitKey();
	return 0;
}