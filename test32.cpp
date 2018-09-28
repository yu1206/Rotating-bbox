
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
#include <io.h>
#include <Windows.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <direct.h>
using namespace cv;
using namespace std;

extern int readDir(char *dirName, vector<string> &filesName);
 #define M_PI 3.14159265358979323846
void coordinates(Point2d src,float angle,Point2d & dst)
{
	dst.x = src.x*cos(angle) - src.y*sin(angle);
	dst.y = src.x*sin(angle) + src.y*cos(angle);
}
void J_Rotate1(Mat src,int x1,int y1,int x2, int y2,float angle,Mat dst)
{
	Point center;
	center.x = (x1+x2) / 2; center.y = (y1+y2) / 2;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(x1-center.x, -y1+center.y); //(x1,y1)
	Point2d rightTop(x2 - center.x,-y1+center.y); // (x2,y1)
	Point2d leftBottom(x1-center.x, -y2 + center.y); //(x1,y2)
	Point2d rightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	 coordinates(leftTop, angle, transLeftTop);
	 coordinates(rightTop, angle, transRightTop);
	 coordinates(leftBottom, angle, transLeftBottom);
	  coordinates(rightBottom, angle, transRightBottom);


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 旋转中心为原点的坐标
	Point pt1, pt2;
	pt1.x = left, pt1.y = top;
	pt2.x = right, pt2.y = down;

	// 左上角为原点的坐标
	Point pt3, pt4;
	pt3.x = pt1.x + center.x, pt3.y = -pt1.y + center.y;
	pt4.x = pt2.x + center.x, pt4.y = pt2.y + center.x;


	//const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	//const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p;
	for (int i = 0; i <height; i++)
	{
		int h = i + pt3.y;// +center.y - height / 2;
		p = dst.ptr<Vec3b>(h);
		for (int j = 0; j < width; j++)
		{
			int w = j + pt3.x;// +center.x - width / 2;
			//坐标变换
			/*int x = static_cast<int>(w * cosAngle + h * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-w * sinAngle + h * cosAngle + num2 + 0.5);*/

			int x = static_cast<int>(w * cosAngle + h * sinAngle + 0.5);
			int y = static_cast<int>(-w * sinAngle + h * cosAngle  + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[w] = src.ptr<Vec3b>(y)[x];
		}
	}
}
void J_Rotate2(Mat src, int x1, int y1, int x2, int y2, float angle, Mat dst)
{
	Point center;
	center.x = (x1 + x2) / 2; center.y = (y1 + y2) / 2;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(x1 - center.x, -y1 + center.y); //(x1,y1)
	Point2d rightTop(x2 - center.x, -y1 + center.y); // (x2,y1)
	Point2d leftBottom(x1 - center.x, -y2 + center.y); //(x1,y2)
	Point2d rightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	coordinates(leftTop, angle, transLeftTop);
	coordinates(rightTop, angle, transRightTop);
	coordinates(leftBottom, angle, transLeftBottom);
	coordinates(rightBottom, angle, transRightBottom);


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的坐标
	Point pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;

	double left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x });
	double right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x });
	double top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y });
	double down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y });

	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p;
	for (int i = 0; i <height; i++)
	{
		int h = i + top1;// +center.y - height / 2;
		p = dst.ptr<Vec3b>(h);
		for (int j = 0; j < width; j++)
		{
			int w = j + left1;// +center.x - width / 2;
			//坐标变换
			/*int x = static_cast<int>(w * cosAngle + h * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-w * sinAngle + h * cosAngle + num2 + 0.5);*/

			int x = static_cast<int>(w * cosAngle + h * sinAngle + 0.5);
			int y = static_cast<int>(-w * sinAngle + h * cosAngle + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[w] = src.ptr<Vec3b>(y)[x];
		}
	}
}
// 旋转中心，坐标为车牌中心，旋转区域车牌区域
void J_Rotate3(Mat src, int x1, int y1, int x2, int y2, float angle, Mat dst,
	int & l1,int & t1,int & r1,int & b1)
{
	Point center;
	center.x = (x1 + x2) / 2; center.y = (y1 + y2) / 2;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(x1 - center.x, -y1 + center.y); //(x1,y1)
	Point2d rightTop(x2 - center.x, -y1 + center.y); // (x2,y1)
	Point2d leftBottom(x1 - center.x, -y2 + center.y); //(x1,y2)
	Point2d rightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	coordinates(leftTop, angle, transLeftTop);
	coordinates(rightTop, angle, transRightTop);
	coordinates(leftBottom, angle, transLeftBottom);
	coordinates(rightBottom, angle, transRightBottom);


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的坐标
	Point pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;

	int left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x });
	int right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x });
	int top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y });
	int down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y });

	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p;
	for (int i = 0; i < height; i++)
	{
		p = dst.ptr<Vec3b>(i+top1);
		for (int j = 0; j < width; j++)
		{
			//坐标变换
			int x = static_cast<int>(j  * cosAngle + i * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-j * sinAngle + i * cosAngle + num2 + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[j+left1] = src.ptr<Vec3b>(y)[x];
		}
	}

	l1 = left1; t1 = top1; r1 = right1; b1 = down1;
}
// 旋转中心，坐标原点，为车牌中心，旋转区域图像区域
void J_Rotate1_1(Mat src, int x1, int y1, int x2, int y2, float angle, Mat & dst1,
	int & l1, int & t1, int & r1, int & b1)
{
	Point center;
	center.x = (x1 + x2) / 2; center.y = (y1 + y2) / 2;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop( - center.x,  + center.y);  //(0,0)
	Point2d rightTop(src.cols- center.x,  + center.y);// (width,0)
	Point2d leftBottom( - center.x, -src.rows + center.y); //(0,height)
	Point2d rightBottom(src.cols - center.x, -src.rows + center.y); // (width,height)

	//原图像车牌四个角的坐标变为以旋转中心的坐标系
	Point2d plateleftTop(x1-center.x, -y1+center.y);  //(x1,y1)
	Point2d platerightTop(x2 - center.x,-y1 +center.y);// (x2,y1)
	Point2d plateleftBottom(x1-center.x, -y2 + center.y); //(x1,y2)
	Point2d platerightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	coordinates(leftTop, angle, transLeftTop);
	coordinates(rightTop, angle, transRightTop);
	coordinates(leftBottom, angle, transLeftBottom);
	coordinates(rightBottom, angle, transRightBottom);

	//以center为中心旋转后车牌四个角的坐标
	Point2d platetransLeftTop, platetransRightTop, platetransLeftBottom, platetransRightBottom;
	coordinates(plateleftTop, angle, platetransLeftTop);
	coordinates(platerightTop, angle, platetransRightTop);
	coordinates(plateleftBottom, angle, platetransLeftBottom);
	coordinates(platerightBottom, angle, platetransRightBottom);


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的图像坐标
	Point pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;

	int left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x });
	int right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x });
	int top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y });
	int down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y });

	// 左上角为原点的车牌坐标
	Point platept1, platept2;
	platept1.x = platetransLeftTop.x + center.x, platept1.y = -platetransLeftTop.y + center.y;
	platept2.x = platetransRightTop.x + center.x, platept2.y = -platetransRightTop.y + center.y;
	Point platept3, platept4;
	platept3.x = platetransLeftBottom.x + center.x, platept3.y = -platetransLeftBottom.y + center.y;
	platept4.x = platetransRightBottom.x + center.x, platept4.y = -platetransRightBottom.y + center.y;

	int plateleft1 = min({ platept1.x, platept2.x, platept3.x, platept4.x });
	int plateright1 = max({ platept1.x, platept2.x, platept3.x, platept4.x });
	int platetop1 = min({ platept1.y, platept2.y, platept3.y, platept4.y });
	int platedown1 = max({ platept1.y, platept2.y, platept3.y, platept4.y });



	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p; Mat dst(height, width,CV_8UC3);
	for (int i = 0; i < height; i++)
	{
		
		if (i + top1 >= height || i + top1<0)
		{
			continue;
		}

		p = dst.ptr<Vec3b>(i+top1);

		

		for (int j = 0; j < width; j++)
		{
			//cout <<"i="<<i<< ",j=" << j << endl;
			if (j + left1 >= width || j + left1<0)
			{
				continue;
			}
			//坐标变换
			int x = static_cast<int>(j  * cosAngle + i * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-j * sinAngle + i * cosAngle + num2 + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[j+left1] = src.ptr<Vec3b>(y)[x];
		}
	}

	l1 = plateleft1; t1 = platetop1; r1 = plateright1; b1 = platedown1;
	dst1 = dst.clone();
}
// 旋转中心，坐标原点，为图像中心，旋转区域图像区域
void J_Rotate1_2(Mat src, int x1, int y1, int x2, int y2, float angle, Mat & dst1,
	int & l1, int & t1, int & r1, int & b1)
{
	Point center;

	//center.x = (x1 + x2) / 2; center.y = (y1 + y2) / 2;
	center.x=(src.cols) / 2; center.y = (src.rows) / 2;

	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(-center.x, +center.y);  //(0,0)
	Point2d rightTop(src.cols - center.x, +center.y);// (width,0)
	Point2d leftBottom(-center.x, -src.rows + center.y); //(0,height)
	Point2d rightBottom(src.cols - center.x, -src.rows + center.y); // (width,height)

	//原图像车牌四个角的坐标变为以旋转中心的坐标系
	Point2d plateleftTop(x1 - center.x, -y1 + center.y);  //(x1,y1)
	Point2d platerightTop(x2 - center.x, -y1 + center.y);// (x2,y1)
	Point2d plateleftBottom(x1 - center.x, -y2 + center.y); //(x1,y2)
	Point2d platerightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	coordinates(leftTop, angle, transLeftTop);
	coordinates(rightTop, angle, transRightTop);
	coordinates(leftBottom, angle, transLeftBottom);
	coordinates(rightBottom, angle, transRightBottom);

	//以center为中心旋转后车牌四个角的坐标
	Point2d platetransLeftTop, platetransRightTop, platetransLeftBottom, platetransRightBottom;
	coordinates(plateleftTop, angle, platetransLeftTop);
	coordinates(platerightTop, angle, platetransRightTop);
	coordinates(plateleftBottom, angle, platetransLeftBottom);
	coordinates(platerightBottom, angle, platetransRightBottom);

	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的坐标
	Point pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;

	int left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x });
	int right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x });
	int top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y });
	int down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y });


	// 左上角为原点的车牌坐标
	Point platept1, platept2;
	platept1.x = platetransLeftTop.x + center.x, platept1.y = -platetransLeftTop.y + center.y;
	platept2.x = platetransRightTop.x + center.x, platept2.y = -platetransRightTop.y + center.y;
	Point platept3, platept4;
	platept3.x = platetransLeftBottom.x + center.x, platept3.y = -platetransLeftBottom.y + center.y;
	platept4.x = platetransRightBottom.x + center.x, platept4.y = -platetransRightBottom.y + center.y;

	int plateleft1 = min({ platept1.x, platept2.x, platept3.x, platept4.x });
	int plateright1 = max({ platept1.x, platept2.x, platept3.x, platept4.x });
	int platetop1 = min({ platept1.y, platept2.y, platept3.y, platept4.y });
	int platedown1 = max({ platept1.y, platept2.y, platept3.y, platept4.y });


	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p; Mat dst(height, width, CV_8UC3);
	for (int i = 0; i < height; i++)
	{
		p = dst.ptr<Vec3b>(i);
		for (int j = 0; j < width; j++)
		{
			//坐标变换
			int x = static_cast<int>(j  * cosAngle + i * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-j * sinAngle + i * cosAngle + num2 + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[j] = src.ptr<Vec3b>(y)[x];
		}
	}

	l1 = plateleft1; t1 = platetop1; r1 = plateright1; b1 = platedown1;

	//l1 = plateleft1+left1; t1 = platetop1+top1; r1 = plateright1+left1; b1 = platedown1+top1;

	//l1 = plateleft1 - left1; t1 = platetop1 - top1; r1 = plateright1 - left1; b1 = platedown1 - top1;

	//l1 = plateleft1 + left1; t1 = platetop1 - top1; r1 = plateright1 + left1; b1 = platedown1 - top1;

	//l1 = plateleft1 + width/2-src.cols/2; t1 = platetop1 + height/2-src.rows/2; 
	//r1 = plateright1 + width / 2 - src.cols / 2; b1 = platedown1 + height / 2 - src.rows / 2;


	//l1 = plateleft1 + width/2-src.cols/2; t1 = platetop1 + height/2-src.rows/2; 
	//r1 = plateright1 + width / 2 - src.cols / 2; b1 = platedown1 + height / 2 - src.rows / 2;

	
	int x12 = static_cast<int>(plateleft1  * cosAngle + platetop1 * sinAngle + num1 + 0.5);
	int y12 = static_cast<int>(-plateleft1 * sinAngle + platetop1 * cosAngle + num2 + 0.5);


	dst1 = dst.clone();
}

Point2d rotationPoint(Point2d srcPoint, const double cosAngle, const double sinAngle)
{
	Point2d dstPoint;
	dstPoint.x = srcPoint.x * cosAngle + srcPoint.y * sinAngle;
	dstPoint.y = -srcPoint.x * sinAngle + srcPoint.y * cosAngle;
	return dstPoint;
}
void J_Rotate1_3(Mat srcImage, int x1, int y1, int x2, int y2, float angle, Mat& dstImage,
	int & l1, int & t1, int & r1, int & b1 )
{
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);
	// 计算标注中心
	double center_x = 0;
	double center_y = 0;
	
	center_x = srcImage.cols/2;
	center_y = srcImage.rows/2;

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(-center_x, center_y); //(0,0)
	Point2d rightTop(srcImage.cols - center_x, center_y); // (width,0)
	Point2d leftBottom(-center_x, -srcImage.rows + center_y); //(0,height)
	Point2d rightBottom(srcImage.cols - center_x, -srcImage.rows + center_y); //(width,height)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
	transLeftTop = rotationPoint(leftTop, cosAngle, sinAngle);
	transRightTop = rotationPoint(rightTop, cosAngle, sinAngle);
	transLeftBottom = rotationPoint(leftBottom, cosAngle, sinAngle);
	transRightBottom = rotationPoint(rightBottom, cosAngle, sinAngle);

	//计算旋转后图像的width，height
	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 分配内存空间
	dstImage.create(height, width, srcImage.type());

	const double dx = -abs(left) * cosAngle - abs(down) * sinAngle + center_x;
	const double dy = abs(left) * sinAngle - abs(down) * cosAngle + center_y;

	int x, y;
	for (int i = 0; i < height; i++) // y
	{
		for (int j = 0; j < width; j++) // x
		{
			//坐标变换
			x = float(j)*cosAngle + float(i)*sinAngle + dx;
			y = float(-j)*sinAngle + float(i)*cosAngle + dy;


			if ((x<0) || (x >= srcImage.cols) || (y<0) || (y >= srcImage.rows))
			{
				if (srcImage.channels() == 3)
				{
					dstImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
				}
				else if (srcImage.channels() == 1)
				{
					dstImage.at<uchar>(i, j) = 0;
				}
			}
			else
			{
				if (srcImage.channels() == 3)
				{
					dstImage.at<cv::Vec3b>(i, j) = srcImage.at<cv::Vec3b>(y, x);
				}
				else if (srcImage.channels() == 1)
				{
					dstImage.at<uchar>(i, j) = srcImage.at<uchar>(y, x);
				}
			}
		}
	}
}




// 只旋转车牌区域，以车牌中心为旋转中心，坐标原点
int test32_roi(int argc, char *argv[])
{
	float f1 = sin(30);
	float f2 = sin(45);
	float f3 = sin(90);
	float f4 = sin(180);
	double d1 = sin(M_PI / 6);
	double d2 = sin(M_PI / 4);
	double d3 = sin(M_PI / 3);
	double d4 = sin(M_PI / 2);
	double d5 = sin(M_PI);

	string inputPath = "I:/mtcnn-train/rotate";
	string outputPath = "I:/mtcnn-train/rotateResult";
	mkdir(outputPath.c_str());
	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());
	string outputtxt = "I:/mtcnn-train/rotateResult/src/000_one.txt";
	fstream finWrite;
	finWrite.open(outputtxt, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());
	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		cout << v_img_[i] << endl;
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data==NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str()<<endl;
			system("pause");
		}
		string nametxt = v_img_[i];
		nametxt.replace(nametxt.find_last_of("."),4,".txt");

		fstream fin1;
		fin1.open(nametxt);
		if (!fin1.is_open())
		{
			cout << "fin1 文件数据打开错误！" << endl;
			system("pause");
			continue;
		}

		 int label; int x1, y1, x2, y2;

		fin1 >>  label >> x1 >> y1 >> x2 >> y2;

		fin1.close();
		
		/*int  degarr[] = {-20,-15,-10,-5,0,5,10,15,20};
		for (int j = 0; j < 9;j++)
		{
			int degree = degarr[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();
			Point pt1, pt2, pt3, pt4;
			pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
			int left = 0; int top = 0; int right = 0; int bottom = 0;
			J_Rotate3(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
			pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " << degree << endl;

			Mat drawimg = dst.clone();
			rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));
			Point pt5, pt6;
			pt5.x = right, pt5.y = top; pt6.x = left, pt6.y = bottom;
			line(drawimg, pt5,pt6,Scalar(0,255,0));
			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);
		}*/
		
		int deg1 = rand() % 10 +1+ 20;
		int deg2 = -rand() % 10 - 1 -20;
		int  degarr1[] = {   deg1,deg2};
		for (int j = 0; j < 2; j++)
		{
			
			int degree =degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();
			Point pt1, pt2, pt3, pt4;
			pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
			int left = 0; int top = 0; int right = 0; int bottom = 0;
			J_Rotate3(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
			pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " << degree << endl;
			Mat drawimg = dst.clone();
			rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));

			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);
		}
		
		
		int jjjjjj = 90;

	}

	finWrite.close();
	return 0;
}
//旋转整个图片 以图片中心，车牌中心为旋转中心，坐标原点
int test32_all(int argc, char *argv[])
{
	

	string inputPath = "I:/mtcnn-train/rotate";
	string outputPath = "I:/mtcnn-train/rotateResult";
	mkdir(outputPath.c_str());
	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());
	string outputtxt = "I:/mtcnn-train/rotateResult/src/000_one.txt";
	fstream finWrite;
	finWrite.open(outputtxt, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());
	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		cout << v_img_[i] << endl;
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}
		string nametxt = v_img_[i];
		nametxt.replace(nametxt.find_last_of("."), 4, ".txt");

		fstream fin1;
		fin1.open(nametxt);
		if (!fin1.is_open())
		{
			cout << "fin1 文件数据打开错误！" << endl;
			system("pause");
			continue;
		}

		int label; int x1, y1, x2, y2;

		fin1 >> label >> x1 >> y1 >> x2 >> y2;

		fin1.close();

		int  degarr[] = {-20,-15,-10,-5,0,5,10,15,20};
		for (int j = 0; j < 9;j++)
		{
		int degree = degarr[j];
		double radian = M_PI*degree*1.0 / 180;

		Mat dst; 
		Point pt1, pt2, pt3, pt4;
		pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
		int left = 0; int top = 0; int right = 0; int bottom = 0;
		J_Rotate1_3(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
		pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

		char intstr[128];
		sprintf(intstr, "-%da%d", j, degree);

		string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
		imwrite(str1.c_str(), dst);

		Mat drawimg = dst.clone();
		rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));

		string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
		imwrite(str2.c_str(), drawimg);

		int jjjjjj = 90;

		}

		/*int deg = rand() % 20 - 10;
		int  degarr1[] = { 0, deg };
		for (int j = 0; j < 2; j++)
		{

			int degree = degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();
			Point pt1, pt2, pt3, pt4;
			pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
			int left = 0; int top = 0; int right = 0; int bottom = 0;
			J_Rotate3(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
			pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " << degree << endl;
			Mat drawimg = dst.clone();
			rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));

			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);
		}*/


		int jjjjjj = 90;

	}

	finWrite.close();
	return 0;
}

int test33_cv(void)
{
	string inputPath = "I:/mtcnn-train/rotate";
	string outputPath = "I:/mtcnn-train/rotateResult";
	mkdir(outputPath.c_str());
	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());
	string outputtxt = "I:/mtcnn-train/rotateResult/src/000_one.txt";
	fstream finWrite;
	finWrite.open(outputtxt, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());
	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		cout << v_img_[i] << endl;
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}
		string nametxt = v_img_[i];
		nametxt.replace(nametxt.find_last_of("."), 4, ".txt");

		fstream fin1;
		fin1.open(nametxt);
		if (!fin1.is_open())
		{
			cout << "fin1 文件数据打开错误！" << endl;
			system("pause");
			continue;
		}

		int label; int x1, y1, x2, y2;

		fin1 >> label >> x1 >> y1 >> x2 >> y2;

		fin1.close();

		int  degarr[] = { -20, -15, -10, -5, 0, 5, 10, 15, 20 };
		for (int j = 0; j < 9; j++)
		{
			int degree = degarr[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst;
			/*Point pt1, pt2, pt3, pt4;
			pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
			int left = 0; int top = 0; int right = 0; int bottom = 0;*/
			//J_Rotate1_2(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
			/*pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;*/

			Point2f center(img.cols / 2, img.rows / 2);

			Mat rot = cv::getRotationMatrix2D(center, degree, 1);
			Rect bbox = cv::RotatedRect(center, img.size(), degree).boundingRect();

			rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
			rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;
			warpAffine(img, dst, rot, bbox.size());

			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			Mat drawimg = dst.clone();
			//rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));

			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);

			int jjjjjj = 90;

		}

		/*int deg = rand() % 20 - 10;
		int  degarr1[] = { 0, deg };
		for (int j = 0; j < 2; j++)
		{

		int degree = degarr1[j];
		double radian = M_PI*degree*1.0 / 180;

		Mat dst = img.clone();
		Point pt1, pt2, pt3, pt4;
		pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
		int left = 0; int top = 0; int right = 0; int bottom = 0;
		J_Rotate3(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom);
		pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

		char intstr[128];
		sprintf(intstr, "-%da%d", j, degree);

		string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
		imwrite(str1.c_str(), dst);

		finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " << degree << endl;
		Mat drawimg = dst.clone();
		rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));

		string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
		imwrite(str2.c_str(), drawimg);
		}*/


		int jjjjjj = 90;

	}

	finWrite.close();
	return 0;

}

int test32(int argc, char *argv[])
{
	test32_roi(argc, argv);
	//test32_all(argc, argv);
	//test33_cv(argc, argv);

	return 0;
}


