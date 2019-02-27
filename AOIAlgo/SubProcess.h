#pragma once
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
using namespace std;

#define ERROR_CALC_PERSPECTIVE      0XE0F80000
#define ERROR_PERSPECTIVE_TRANSFORM 0XE0F80001
#define ERROR_FIND_LINE             0XE0F80002

typedef struct _tagLine
{
	cv::Point2f lineStart;
	cv::Point2f lineEnd;
	cv::Point2f lineCenter;
	double      lineLength;
	double      Phi;

	_tagLine()
	{
		lineLength = 0;
		Phi = 0;
	}
}SegLine;
class SubProcess
{
public:
	SubProcess();
	~SubProcess();
public:
	// 计算小相机透视变换矩阵
	HRESULT CalcPerspectiveMat(const cv::Mat& srcImage, int maskSize, double lineMinLen, int newWidth, int newHeight);
	// 进行透视变换
	HRESULT PerspectiveTransform(cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight);
	//
	HRESULT RotationImg(cv::Mat& SrcImg, cv::Mat& dstImg,int angle);
private:
	void FillUp(cv::Mat& srcImage, cv::Mat& dstImage);
	int  SelectMaxRegion(const vector<vector<cv::Point>> Contours);
	cv::Point2f ProjPoint(cv::Point2f pt, cv::Point2f lineStart, cv::Point2f lineEnd);
	double DistancePoint(cv::Point2f pt1, cv::Point2f pt2);
	void GetRegionFeature(const cv::Mat& srcImage, vector<cv::Point> Contour, double& Phi, double& Row, double& Col, double& Length1, double& Length2);
	double LineAngle(cv::Point2f line1Start, cv::Point2f line1End, cv::Point2f line2Start, cv::Point2f line2End);
	void UnionCollinearLine(vector<SegLine>& lines, vector<SegLine>& unionLine, double MaxDistAbs, double MaxShift, double MaxAngle);
	void IntersectionPoint(cv::Point2f line1Start, cv::Point2f line1End, cv::Point2f line2Start, cv::Point2f line2End, cv::Point2f& crossPoint);
	void FindCorner(cv::Mat srcImage, vector<cv::Point> contour, vector<cv::Point2f>& corner);
	float GetDistance(cv::Point2f f1, cv::Point2f f2);
	void FindCorner2(cv::Mat srcImage, vector<vector<cv::Point>> contours, int Idx, double linMinLen, vector<cv::Point2f>& corner);
private:
	cv::Mat m_PerspMat;  // 保存变换矩阵
};

