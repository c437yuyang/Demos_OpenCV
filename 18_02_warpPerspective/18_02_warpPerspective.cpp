#include <opencv2\opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void onMouse(int event, int x, int y, int flags, void *utsc);
Point2f srcTri[4], dstTri[4];
int clickTimes = 0;  //在图像上单击次数  
Mat image;

int main(int argc, char *argv[]) {
	//image = imread("test1.jpg");
	//image = imread("8.jpg");
	image = imread("9.png");
	imshow("Source Image", image);
	setMouseCallback("Source Image", onMouse);
	waitKey();
	return 0;
}

Mat perspective_transformation(const Point2f* final_points, Mat& src) {
	
	Point2f _srcTriangle[4];
	Point2f _dstTriangle[4];
	vector<Point2f>srcTriangle(_srcTriangle, _srcTriangle + 4);
	vector<Point2f>dstTriangle(_dstTriangle, _dstTriangle + 4);
	Mat after_transform;

	const int leftTopX = final_points[0].x;
	const int leftTopY = final_points[0].y;
	const int rightTopX = final_points[1].x;
	const int rightTopY = final_points[1].y;
	const int leftDownX = final_points[2].x;
	const int leftDownY = final_points[2].y;
	const int rightDownX = final_points[3].x;
	const int rightDownY = final_points[3].y;

	int newWidth = 0;
	int newHeight = 0;

	newWidth = sqrt((leftTopX - rightTopX) * (leftTopX - rightTopX) + 
					(leftTopY - rightTopY) * (leftTopY - rightTopY));
	newHeight = sqrt((leftTopX - leftDownX) * (leftTopX - leftDownX) + 
					(leftTopY - leftDownY) * (leftTopY - leftDownY));
	cout << newWidth << " " << newHeight << endl;
	after_transform = Mat::zeros(newHeight, newWidth, src.type());

	srcTriangle[0] = Point2f(leftTopX, leftTopY);
	srcTriangle[1] = Point2f(rightTopX, rightTopY);
	srcTriangle[2] = Point2f(leftDownX, leftDownY);
	srcTriangle[3] = Point2f(rightDownX, rightDownY);

	dstTriangle[0] = Point2f(0, 0);
	dstTriangle[1] = Point2f(newWidth, 0);
	dstTriangle[2] = Point2f(0, newHeight);
	dstTriangle[3] = Point2f(newWidth, newHeight);


	Mat m1 = Mat(srcTriangle);
	Mat m2 = Mat(dstTriangle);
	Mat status;
	Mat h = findHomography(m1, m2, status, 0, 3);
	perspectiveTransform(srcTriangle, dstTriangle, h);
	warpPerspective(src, after_transform, h, after_transform.size());
	//debug->show_img("after_transform", after_transform);
	return after_transform;
}

Mat image_enhance(Mat& input) {
	Mat output;
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0); //锐化一下
	filter2D(input, output, input.depth(), kernel);
	return output;
}


void onMouse(int event, int x, int y, int flags, void *utsc) {
	if (event == CV_EVENT_LBUTTONUP)   //响应鼠标左键抬起事件  
	{
		circle(image, Point(x, y), 2.5, Scalar(0, 0, 255), 2.5);  //标记选中点  
		imshow("Source Image", image);
		srcTri[clickTimes].x = x;
		srcTri[clickTimes].y = y;
		clickTimes++;
	}
	if (clickTimes == 4) {

		sort(srcTri, srcTri + 4, [](Point2f p1, Point2f p2) {return p1.y < p2.y; });
		sort(srcTri, srcTri + 2, [](Point2f p1, Point2f p2) {return p1.x < p2.x; });
		sort(srcTri + 2, srcTri + 4, [](Point2f p1, Point2f p2) {return p1.x < p2.x; });
		Mat imageWarp =  perspective_transformation(srcTri, image);
		Mat enhanced = image_enhance(imageWarp);
		imshow("After WarpPerspecttive", imageWarp);
		imshow("After Enhanced", enhanced);
	}
}