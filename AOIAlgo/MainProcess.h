#pragma once
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
using namespace std;

/* Error Code*/
#define ERROR_CALC_PERSPECTIVE      0XE0F80000
#define ERROR_PERSPECTIVE_TRANSFORM 0XE0F80001
#define ERROR_FIND_LINE             0XE0F80002
/* Error Code*/

class MainProcess
{
public:
	MainProcess();
	~MainProcess();
public:
	// 计算大相机透视变换矩阵
	HRESULT CalcPerspectiveMat(const cv::Mat& srcImage, int maskSize, float MeasureThresh, int newWidth, int newHeight);
	// 进行透视变换
	HRESULT PerspectiveTransform(cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight);
	// 估计背景平均亮点
	double EstimationBg(cv::Mat& srcImage);
	//
	HRESULT GetMainMatrix2D(cv::Mat& Matrix2D);
private:
	void FillUp(cv::Mat& srcImage, cv::Mat& dstImage);
	int  SelectMaxRegion(const vector<vector<cv::Point>> Contours);
	void GetRegionFeature(const cv::Mat& srcImage, vector<cv::Point> Contour, float& Phi, float& Row, float& Col, float& Length1, float& Length2);
	bool FindStraightEdge(const cv::Mat& srcImage, float& Row, float& Col, float& Phi, float& Length1, float& Length2,
		int numPoints, float MeasureThresh, cv::Point2d& lineStart, cv::Point2d& lineEnd);
	void IntersectionPoint(cv::Point2d line1Start, cv::Point2d line1End, cv::Point2d line2Start, cv::Point2d line2End, cv::Point2d& crossPoint);
	void GenMeasureRectangle(const cv::Mat& srcImage, float Phi, double Length1, double Length2, int Width, int Height, cv::InterpolationFlags InterFlag);
	void MeasurePos(const cv::Mat& srcImage, float Row, float Col, float Length1, float Length2, double sigma, float threshval, cv::Point2f& EdgePoint);
	cv::Point2f AffineTransPoint(cv::Point2f srcPoint, cv::Mat& M);
	float Rad(float deg);
	float Deg(float rad);
private:
	cv::Mat m_PerspMat;  // 保存变换矩阵
private:
	cv::Mat m_RotMat;
	cv::Mat m_InvRotMat;
	cv::Mat m_ROIImage;
};

