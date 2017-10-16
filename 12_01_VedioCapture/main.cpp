#include <opencv2/opencv.hpp>

#include <string>
using namespace std;
int main() 
{
	
	string filename = "%02d.jpg"; //VideoCaptureֻ��Ҫָ�������ָ�ʽ���ַ�������ʽ�����Զ�Ѱ���ļ�
	int n = 12;
	cv::VideoCapture cap(filename);
	int i = 0;
	cv::Mat frame;
	while (i <n)
	{
		cap >> frame;
		cout << frame.rows << "," << frame.cols << endl;;
		resize(frame, frame, cv::Size(0, 0), 0.3, 0.3);
		cv::imshow("image",frame);
		++i;
		cv::waitKey(0);
	}


}
