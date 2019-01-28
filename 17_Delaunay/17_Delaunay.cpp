#include <opencv2/imgproc/imgproc_c.h>  
#include <opencv2/legacy/legacy.hpp>  
#include "opencv2/highgui/highgui.hpp"  
#include<opencv2\opencv.hpp>  
#include<iostream>  
#include <stdio.h>  
using namespace std;
using namespace cv;
static void help(void) {
	printf("\nThis program demostrates iterative construction of\n"//������������delaunay�ʷֺ�voronoiϸ�ֵĵ�������  
		"delaunay triangulation and voronoi tesselation.\n"
		"It draws a random set of points in an image and then delaunay triangulates them.\n"//��ͼ���ϻ���һЩ����㣬Ȼ�����delaunay�����ʷ�  
		"Usage: \n"
		"./delaunay \n"
		"\nThis program builds the traingulation interactively, you may stop this process by\n"
		"hitting any key.\n");//�������������ʷ֣������ֹͣ���������  
}

static CvSubdiv2D* init_delaunay(CvMemStorage* storage,//��ʼ�������ʷֽṹ��Ϊ����䵥Ԫ  
	CvRect rect) {
	CvSubdiv2D* subdiv;//�����ʷֵ����ݵ�Ԫ  

	subdiv = cvCreateSubdiv2D(CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
		sizeof(CvSubdiv2DPoint),
		sizeof(CvQuadEdge2D),
		storage);
	cvInitSubdivDelaunay2D(subdiv, rect);

	return subdiv;
}


static void draw_subdiv_point(IplImage* img, CvPoint2D32f fp, CvScalar color)//���������ʷֵĶ���  
{
	cvCircle(img, cvPoint(cvRound(fp.x), cvRound(fp.y)), 5, color, CV_FILLED, 8, 0);
}


static void draw_subdiv_edge(IplImage* img, CvSubdiv2DEdge edge, CvScalar color)//���������ʷֵı�  
{
	CvSubdiv2DPoint* org_pt;//Դ����  
	CvSubdiv2DPoint* dst_pt;//Ŀ�ض���  
	CvPoint2D32f org;
	CvPoint2D32f dst;
	CvPoint iorg, idst;

	org_pt = cvSubdiv2DEdgeOrg(edge);//ͨ���߻�ȡ����  
	dst_pt = cvSubdiv2DEdgeDst(edge);

	if (org_pt && dst_pt)//��������˵㲻Ϊ��  
	{
		org = org_pt->pt;
		dst = dst_pt->pt;

		iorg = cvPoint(cvRound(org.x), cvRound(org.y));
		idst = cvPoint(cvRound(dst.x), cvRound(dst.y));

		cvLine(img, iorg, idst, color, 1, CV_AA, 0);
	}
}


static void draw_subdiv(IplImage* img, CvSubdiv2D* subdiv,
	CvScalar delaunay_color, CvScalar voronoi_color)//�����ʷֺ�ϸ��  
{
	CvSeqReader  reader;
	int i, total = subdiv->edges->total;//�ߵ�����  
	int elem_size = subdiv->edges->elem_size;//�ߵĴ�С  
	cout << typeid(subdiv->edges).name() << endl;

	cvStartReadSeq((CvSeq*)(subdiv->edges), &reader, 0);//ʹ��CvSeqReader����Delaunay����Voronoi��  

	for (i = 0; i < total; i++) {
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

		if (CV_IS_SET_ELEM(edge)) {
			// draw_subdiv_edge( img, (CvSubdiv2DEdge)edge + 1, voronoi_color );  
			draw_subdiv_edge(img, (CvSubdiv2DEdge)edge, delaunay_color);
		}

		CV_NEXT_SEQ_ELEM(elem_size, reader);
	}

}


static void locate_point(CvSubdiv2D* subdiv, CvPoint2D32f fp, IplImage* img,//���������ʷֵı�  
	CvScalar active_color) {
	CvSubdiv2DEdge e;
	CvSubdiv2DEdge e0 = 0;
	CvSubdiv2DPoint* p = 0;

	cvSubdiv2DLocate(subdiv, fp, &e0, &p);

	if (e0) {
		e = e0;
		do {
			draw_subdiv_edge(img, e, active_color);
			e = cvSubdiv2DGetEdge(e, CV_NEXT_AROUND_LEFT);
		} while (e != e0);
	}

	draw_subdiv_point(img, fp, active_color);
}

//@author andme-��Ŀ�Ӿ�  
static void dashLine(Mat &img, Point2d& pt1, Point2d& pt2, int n)//nΪ���߶���  
{
	Point sub = pt2 - pt1;
	for (int i = 0; i < 2 * n; i += 2) {
		line(img, Point(pt1.x + sub.x * i / (2 * n - 1), pt1.y + sub.y * i / (2 * n - 1)), Point(pt1.x + sub.x * (i + 1) / (2 * n - 1), pt1.y + sub.y * (i + 1) / (2 * n - 1)), Scalar(0, 255, 0), 2);
	}
}



//������ʽdraw_subdiv_facet( img, cvSubdiv2DRotateEdge( e, 1 ));  
static void draw_subdiv_facet(IplImage* img, CvSubdiv2DEdge edge)//����voronoi��   
{
	//cout<<edge<<endl;//edge����λ��ʾ��ʾ��������λ��ʾ�ķ���Եָ�롣  
	//cout<<(edge&3)<<endl;  
	CvSubdiv2DEdge t = edge;//�����ǰ�����ĵ�����ʽʱ��edgeΪeRot��  
	int i, count = 0;
	CvPoint* buf = 0;
	Point2d *buf1 = 0;

	// count number of edges in facet //���ڱߵļ���  
	do {
		count++;
		t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);
	} while (t != edge);//��������һ��voronoi��Ԫһ�ܣ�������vornonoi��Ե��ӵ�еı�Ե����  

	buf = (CvPoint*)malloc(count * sizeof(buf[0]));
	buf1 = (Point2d*)malloc(count * sizeof(buf1[0]));

	// gather points  
	t = edge;
	for (i = 0; i < count; i++) {
		CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg(t);//��һ�λ�ȡeRot��Ե����ʼ��  
		if (!pt) break;//����ò�����Դ�㣬���˳�ѭ��  
		buf[i] = cvPoint(cvRound(pt->pt.x), cvRound(pt->pt.y));//���õ�ת��ΪcvPoint���͵㣬�洢��buf��  
		t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);//Ȼ������vornonoi��Ԫ������ת��  
	}

	if (i == count)//������еĵ㶼�洢�����ˡ�  
	{
		CvSubdiv2DPoint* pt = cvSubdiv2DEdgeDst(cvSubdiv2DRotateEdge(edge, 1));//����eRot����ת��ԵӦ����reversed e,��ôĿ�ĵ㣬����e��Դ�㡣  
																			   // cvFillConvexPoly( img, buf, count, CV_RGB(rand()&255,rand()&255,rand()&255), CV_AA, 0 );//���͹�����  
		for (i = 0; i<count; i++) {
			buf1[i].x = buf[i].x;
			buf1[i].y = buf[i].y;
		}
		Mat mat_img(img);

		cvPolyLine(img, &buf, &count, 1, 1, CV_RGB(0, 200, 0), 1, CV_AA, 0);//�����ߡ�  
																			//for(int i=0;i<count-1;i++)  
																			//{  
																			//dashLine(mat_img,buf1[i],buf1[i+1],100);  
																			//}  
																			//dashLine(mat_img,buf1[i],buf1[0],100);  
		draw_subdiv_point(img, pt->pt, CV_RGB(255, 0, 0));//�ú�ɫ���������ʷֶ��㡣  
	}
	free(buf);
}

static void paint_voronoi(CvSubdiv2D* subdiv, IplImage* img)//����voronoi��  
{
	CvSeqReader  reader;
	int i, total = subdiv->edges->total;//��Ե����  
	int elem_size = subdiv->edges->elem_size;//��Ե�Ĵ�С  

	cvCalcSubdivVoronoi2D(subdiv);

	cvStartReadSeq((CvSeq*)(subdiv->edges), &reader, 0);

	for (i = 0; i < total; i++) {
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);//��ȡ�ķ���Ե  

		if (CV_IS_SET_ELEM(edge))//�жϱ�Ե�Ƿ��ڱ�Ե����  
		{
			CvSubdiv2DEdge e = (CvSubdiv2DEdge)edge;//edge���ķ���Ե��ָ�룬��CvSubdiv2DEdge��λ��ʾ�ķ���Ե��ָ�롣  
													//cout<<(e&3)<<endl;//ͨ������e��2λ������ֵӦ������Ϊ0�ˣ��������Ե  
													// left  
			draw_subdiv_facet(img, cvSubdiv2DRotateEdge(e, 1));//eΪDelaunay�ߣ����Delaunay�߶�Ӧ��voronoi�ߣ���e����ת��Ե  

															   // right  
			draw_subdiv_facet(img, cvSubdiv2DRotateEdge(e, 3));//�������ת��Ե  
		}

		CV_NEXT_SEQ_ELEM(elem_size, reader);//�ƶ�����һ��λ��  
	}
}


static void run(void) {
	char win[] = "source";
	int i;
	CvRect rect = { 0, 0, 600, 600 };
	CvMemStorage* storage;
	CvSubdiv2D* subdiv;
	IplImage* img;
	CvScalar active_facet_color, delaunay_color, voronoi_color, bkgnd_color;

	active_facet_color = CV_RGB(255, 0, 0);//��ɫ  
	delaunay_color = CV_RGB(0, 0, 0);//��ɫ  
	voronoi_color = CV_RGB(0, 180, 0);//��ɫ  
	bkgnd_color = CV_RGB(255, 255, 255);//��ɫ  

	img = cvCreateImage(cvSize(rect.width, rect.height), 8, 3);
	cvSet(img, bkgnd_color, 0);

	cvNamedWindow(win, 1);

	storage = cvCreateMemStorage(0);
	subdiv = init_delaunay(storage, rect);

	printf("Delaunay triangulation will be build now interactively.\n"
		"To stop the process, press any key\n\n");

	vector<CvPoint2D32f> points;
	for (i = 0; i < 10; i++) {
		CvPoint2D32f fp = cvPoint2D32f((float)(rand() % (rect.width - 10)),//ʹ��Լ���ھ���߿�10����֮�ڡ�  
			(float)(rand() % (rect.height - 10)));
		points.push_back(fp);

		locate_point(subdiv, fp, img, active_facet_color);//��λ���λ�ã�������������voronoi��ıߡ�  
		cvShowImage(win, img);//ˢ����ʾ  

		if (cvWaitKey(100) >= 0)
			break;

		cvSubdivDelaunay2DInsert(subdiv, fp);//�������ʷ��в���õ㣬���Ըõ���������ʷ�  
		cvCalcSubdivVoronoi2D(subdiv);//����Voronoiϸ�֣���ʱ�����ǲ���Ҫ  
		cvSet(img, bkgnd_color, 0);//����ͼ��ı�����ɫΪ��ɫ  
		draw_subdiv(img, subdiv, delaunay_color, voronoi_color);
		cvShowImage(win, img);

		//cvWaitKey();  
		if (cvWaitKey(100) >= 0)
			break;
	}
	for (int i = 0; i<points.size(); i++)
		draw_subdiv_point(img, points[i], active_facet_color);
	cvShowImage(win, img);
	cvWaitKey();

	//  cvSet( img, bkgnd_color, 0 );//����ˢ�»����������ñ�����ɫΪ��ɫ  
	paint_voronoi(subdiv, img);//����ϸ��  
	cvShowImage(win, img);//  

	cvWaitKey(0);

	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img);
	cvDestroyWindow(win);
}

int main1(int argc, char** argv) {
	(void)argc; (void)argv;
	help();
	run();
	return 0;
}

#ifdef _EiC  
main(1, "delaunay.c");
#endif  