#include "stdafx.h"
#include "MainProcess.h"

MainProcess::MainProcess()
{
}


MainProcess::~MainProcess()
{
}
HRESULT MainProcess::GetMainMatrix2D(cv::Mat& Matrix2D)
{
	Matrix2D = m_PerspMat;
    return ERROR_SUCCESS;
}
HRESULT MainProcess::CalcPerspectiveMat(const cv::Mat& srcImage,int maskSize, float MeasureThresh, int newWidth, int newHeight)
{
	try
	{
		/* Step1 : 有效区域粗定位 */
		cv::Mat Region;
		cv::Mat GrayMean, GrayDev;

		cv::meanStdDev(srcImage, GrayMean, GrayDev);
		double CurrentThresh = GrayMean.at<double>(0, 0) - 0.1*GrayDev.at<double>(0, 0);
		cv::threshold(srcImage, Region, CurrentThresh, 255, cv::THRESH_BINARY);
		cv::Mat SE = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(maskSize, maskSize));
		cv::morphologyEx(Region, Region, cv::MORPH_CLOSE, SE);
		FillUp(Region, Region);

		// 获取最大轮廓
		cv::Mat tmpImage = Region.clone();
		vector<vector<cv::Point>> Contours;
		cv::findContours(tmpImage, Contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		int Idx = SelectMaxRegion(Contours);
		if (Idx == -1)
		{
			return ERROR_EMPTY;
		}
		// 获取该区域特征值
		float  centRow, centCol;
		float  Phi;
		float Length1, Length2;
		GetRegionFeature(srcImage, Contours[Idx], Phi, centRow, centCol, Length1, Length2);

		/* Step2: 拟合四边,获取四边形四个角点 */
		// 拟合Left直线
		float LeftRow = centRow - Length1*sin(Phi + Rad(180));
		float LeftCol = centCol + Length1*cos(Phi + Rad(180));
		float LeftLen1 = 30;
		float LeftLen2 = Length2;
		float LeftPhi = Phi;
		cv::Point2d leftLineStart, leftLineEnd;
		DWORD dwStart = timeGetTime();
		bool bFind = FindStraightEdge(srcImage, LeftRow, LeftCol, LeftPhi, LeftLen1, LeftLen2, 60, MeasureThresh,
			leftLineStart, leftLineEnd);

		if (false == bFind)
		{
			return ERROR_FIND_LINE;
		}
		// 拟合Top直线
		float TopRow = centRow - Length2*sin(Phi + Rad(90));
		float TopCol = centCol + Length2*cos(Phi + Rad(90));
		float TopLen1 = 30;
		float TopLen2 = Length1;
		float TopPhi = Phi - Rad(90);
		cv::Point2d TopLineStart, TopLineEnd;
		bFind = FindStraightEdge(srcImage, TopRow, TopCol, TopPhi, TopLen1, TopLen2, 60, MeasureThresh,
			TopLineStart, TopLineEnd);
		if (false == bFind)
		{
			return ERROR_FIND_LINE;
		}
		// 拟合Right直线
		float RightRow = centRow - Length1*sin(Phi);
		float RightCol = centCol + Length1*cos(Phi);
		float RightLen1 = 30;
		float RightLen2 = Length2;
		float RightPhi = Phi + Rad(180);
		cv::Point2d RightLineStart, RightLineEnd;
		bFind = FindStraightEdge(srcImage, RightRow, RightCol, RightPhi, RightLen1, RightLen2, 60, MeasureThresh,
			RightLineStart, RightLineEnd);
		if (false == bFind)
		{
			return ERROR_FIND_LINE;
		}
		// 拟合Bottom直线
		float BottomRow = centRow - Length2*sin(Phi - Rad(90));
		float BottomCol = centCol + Length2*cos(Phi - Rad(90));
		float BottomLen1 = 30;
		float BottomLen2 = Length1;
		float BottomPhi = Phi + Rad(90);
		cv::Point2d BottomLineStart, BottomLineEnd;
		bFind = FindStraightEdge(srcImage, BottomRow, BottomCol, BottomPhi, BottomLen1, BottomLen2, 60, MeasureThresh,
			BottomLineStart, BottomLineEnd);
		if (false == bFind)
		{
			return ERROR_FIND_LINE;
		}
		// 求四个交点(左上,右上,右下,左下)
		cv::Point2d TLPoint, TRPoint, BLPoint, BRPoint;
		IntersectionPoint(TopLineStart, TopLineEnd, leftLineStart, leftLineEnd, TLPoint);
		IntersectionPoint(TopLineStart, TopLineEnd, RightLineStart, RightLineEnd, TRPoint);
		IntersectionPoint(BottomLineStart, BottomLineEnd, RightLineStart, RightLineEnd, BRPoint);
		IntersectionPoint(BottomLineStart, BottomLineEnd, leftLineStart, leftLineEnd, BLPoint);

		//获取放射变换矩阵
		m_PerspMat.zeros(3, 3, CV_32F);
		cv::Point2f srcPoint[] =
		{
			TLPoint,
			TRPoint,
			BRPoint,
			BLPoint
		};
		cv::Point2f dstPoint[] =
		{
			cv::Point2i(0,0),
			cv::Point2i(newWidth - 1,0),
			cv::Point2i(newWidth - 1,newHeight - 1),
			cv::Point2i(0,newHeight - 1)
		};

		m_PerspMat = cv::getPerspectiveTransform(srcPoint, dstPoint);
		return ERROR_SUCCESS;
	}
	catch (cv::Exception& e)
	{
		OutputDebugStringA(e.what());
		return ERROR_CALC_PERSPECTIVE;
	}
}

HRESULT MainProcess::PerspectiveTransform(cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight)
{
	try
	{
		if (!m_PerspMat.empty())
		{
			cv::warpPerspective(srcImage, dstImage, m_PerspMat, cv::Size(newWidth, newHeight), cv::INTER_CUBIC);
		}
		return ERROR_SUCCESS;
	}
	catch (cv::Exception& e)
	{
		OutputDebugStringA(e.what());
		return ERROR_PERSPECTIVE_TRANSFORM;
	}
}

void MainProcess::FillUp(cv::Mat& srcImage, cv::Mat& dstImage)
{
	cv::Size m_Size = srcImage.size();

	// 向外扩展图像一个像素
	cv::Mat Temp = cv::Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcImage.type());
	srcImage.copyTo(Temp(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)));

	// 洪水填充
	cv::floodFill(Temp, cv::Point(0, 0), cv::Scalar(255));

	// 截取原图大小
	cv::Mat cutImg;
	Temp(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)).copyTo(cutImg);

	// 获取最终填充后的图像
	dstImage = srcImage | (~cutImg);
}

int MainProcess::SelectMaxRegion(const vector<vector<cv::Point>> Contours)
{
	double maxArea = 0;
	int maxIdx = -1;
	for (int Idx = 0; Idx < Contours.size(); Idx++)
	{
		double area = fabs(cv::contourArea(Contours[Idx]));
		if (area > maxArea)
		{
			maxArea = area;
			maxIdx = Idx;
		}
	}
	if (maxArea == 0)
		return -1;

	return maxIdx;
}

void MainProcess::GetRegionFeature(const cv::Mat& srcImage, vector<cv::Point> Contour, float& Phi, float& Row, float& Col, float& Length1, float& Length2)
{
	cv::Moments M = cv::moments(Contour);

	// 质心
	Col = static_cast<float>(M.m10 / M.m00);
	Row = static_cast<float>(M.m01 / M.m00);

	// 旋转角
	Phi = static_cast<float>(-0.5 * atan2(2 * M.mu11, M.mu20 - M.mu02));

	// 长短轴
	cv::RotatedRect rc = cv::minAreaRect(Contour);
	Length1 = rc.size.width > rc.size.height ? rc.size.width / 2 : rc.size.height / 2;
	Length2 = rc.size.width > rc.size.height ? rc.size.height / 2 : rc.size.width / 2;
}

bool MainProcess::FindStraightEdge(const cv::Mat& srcImage, float& Row, float& Col, float& Phi, float& Length1, float& Length2,
	int numPoints, float MeasureThresh, cv::Point2d& lineStart, cv::Point2d& lineEnd)
{
	float Step = (2.0f*Length2) / numPoints;
	float SRow = Row - (Length2 + Step / 2.0f)*sin(Phi + Rad(90));
	float SCol = Col + (Length2 + Step / 2.0f)*cos(Phi + Rad(90));
	float L1 = Length1;
	float L2 = Step / 2.0f;

	GenMeasureRectangle(srcImage, Phi, L1, L2, srcImage.cols, srcImage.rows, cv::INTER_LINEAR);

	float mRow = SRow;
	float mCol = SCol;
	vector<cv::Point2f> EdgePoints;
	for (int i = 0; i<numPoints; i++)
	{
		mRow = mRow - Step * sin(Phi - Rad(90));
		mCol = mCol + Step * cos(Phi - Rad(90));
		cv::Point2f transPoint = AffineTransPoint(cv::Point2d(mCol, mRow), m_RotMat);

		cv::Point2f EdgePoint;
		MeasurePos(srcImage, transPoint.y, transPoint.x, L1, L2, 1.0, MeasureThresh, EdgePoint);
		if (EdgePoint.x != 0 && EdgePoint.y != 0)
			EdgePoints.push_back(EdgePoint);
	}
	if (EdgePoints.size() < 3)
		return false;

	cv::Vec4f line;
	cv::fitLine(EdgePoints, line, cv::DIST_HUBER, 0, 1e-2, 1e-2);
	lineStart.x = line[2] - Length2 * line[0];
	lineStart.y = line[3] - Length2 * line[1];
	lineEnd.x = line[2] + Length2 * line[0];
	lineEnd.y = line[3] + Length2 * line[1];
	return true;
}

void MainProcess::IntersectionPoint(cv::Point2d line1Start, cv::Point2d line1End, cv::Point2d line2Start, cv::Point2d line2End, cv::Point2d& crossPoint)
{
	double ka, kb;
	ka = (line1End.y - line1Start.y) / (line1End.x - line1Start.x);
	kb = (line2End.y - line2Start.y) / (line2End.x - line2Start.x);

	crossPoint.x = (ka*line1Start.x - line1Start.y - kb*line2Start.x + line2Start.y) / (ka - kb);
	crossPoint.y = (ka*kb*(line1Start.x - line2Start.x) + ka*line2Start.y - kb*line1Start.y) / (ka - kb);
}

void MainProcess::GenMeasureRectangle(const cv::Mat& srcImage, float Phi, double Length1, double Length2, int Width, int Height, cv::InterpolationFlags InterFlag)
{
	cv::Point2f center(Width / 2.0f, Height / 2.0f);
	m_RotMat = cv::getRotationMatrix2D(center, Deg(Phi), 1);
	cv::Rect bbox = cv::RotatedRect(center, cv::Size(Width, Height), Deg(Phi)).boundingRect();
	m_RotMat.at<float>(0, 2) += bbox.width / 2.0f - center.x;
	m_RotMat.at<float>(1, 2) += bbox.height / 2.0f - center.y;

	cv::warpAffine(srcImage, m_ROIImage, m_RotMat, bbox.size(), cv::INTER_LINEAR);

	m_InvRotMat = cv::getRotationMatrix2D(cv::Point2i(m_ROIImage.cols / 2, m_ROIImage.rows / 2), Deg(-Phi), 1);
	m_InvRotMat.at<double>(0, 2) += center.x - bbox.width / 2.0;
	m_InvRotMat.at<double>(1, 2) += center.y - bbox.height / 2.0;
}

void MainProcess::MeasurePos(const cv::Mat& srcImage, float Row, float Col, float Length1, float Length2, double sigma, float threshval, cv::Point2f& EdgePoint)
{
	// 统计均值
	vector<float> meangray;
	int CStart = static_cast<int>(Col - Length1);
	int CEnd = static_cast<int>(Col + Length1);
	for (int j = CStart; j < CEnd; j++)
	{
		float gray = 0;
		float meanvalue = 0;
		int RStart = static_cast<int>(Row - Length2);
		int REnd = static_cast<int>(Row + Length2);
		for (int i = RStart; i < REnd; i++)
		{
			gray += m_ROIImage.at<uchar>(i, j);
		}
		meanvalue = gray / (2 * Length2);
		meangray.push_back(meanvalue);
	}
	size_t sizemean = meangray.size();

	// 高斯滤波
	float f1, f0, f2;
	float sumoff;
	f0 = static_cast<float>(1.0f / (sqrtf(2.0f * static_cast<float>(CV_PI)) * 1.0f));
	f1 = static_cast<float>(f0 * exp(-1.0f / (1.0f * 1.0f)));
	f2 = f1;
	sumoff = f0 + f1 + f2;

	vector<float> gaussvalue;
	gaussvalue.push_back(meangray[0]);
	for (int i = 1; i < sizemean - 1; i++)
	{
		float tmp;
		tmp = meangray[i - 1] * f1 + meangray[i] * f0 + meangray[i + 1] * f2;
		tmp = tmp / sumoff;
		gaussvalue.push_back(tmp);
	}
	gaussvalue.push_back(meangray[sizemean - 1]);
	// 求导
	for (int i = 1; i < sizemean - 1; i++)
	{
		meangray[i] = (gaussvalue[i + 1] - gaussvalue[i - 1]) / 2.0f * (sqrtf(2.0f * static_cast<float>(CV_PI)) * 1.0f);
	}
	meangray[sizemean - 1] = 0;

	// 阈值分割
	vector<float> threshvalue;
	for (int i = 0; i < sizemean; i++)
	{
		if (fabs(meangray[i]) >= threshval)
		{
			threshvalue.push_back(meangray[i]);
		}
		else
		{
			threshvalue.push_back(0);
		}
			
	}
	// 求倒数最大值
	for (int i = 1; i < sizemean - 1; i++)
	{
		cv::Point2f tmppoint, outpoint;
		if ((threshvalue[i] > threshvalue[i - 1] && threshvalue[i] > threshvalue[i + 1] && threshvalue[i - 1] != 0 && threshvalue[i + 1] != 0) ||
			(threshvalue[i] < threshvalue[i - 1] && threshvalue[i] < threshvalue[i + 1] && threshvalue[i - 1] != 0 && threshvalue[i + 1] != 0))
		{
			if (threshvalue[i] != 0)
			{
				tmppoint.x = Col - Length1 + i - 1;
				tmppoint.y = Row;
				outpoint = AffineTransPoint(tmppoint, m_InvRotMat);
				EdgePoint.x = outpoint.x;
				EdgePoint.y = outpoint.y;
				break;
			}
		}
	}
}

cv::Point2f MainProcess::AffineTransPoint(cv::Point2f srcPoint, cv::Mat& M)
{
	cv::Point2f resultPoint;
	resultPoint.x = srcPoint.x * M.at<float>(0, 0) + srcPoint.y * M.at<float>(0, 1) + M.at<float>(0, 2);
	resultPoint.y = srcPoint.x * M.at<float>(1, 0) + srcPoint.y * M.at<float>(1, 1) + M.at<float>(1, 2);
	return resultPoint;
}

float MainProcess::Rad(float deg)
{
	return deg / 180.0f * 3.141592654f;
}

float MainProcess::Deg(float rad)
{
	return rad / 3.141592654f * 180.0f;
}

double MainProcess::EstimationBg(cv::Mat& srcImage)
{
	unsigned char* row_ptr;
	unsigned char* data_ptr;
	double sum = 0;
	double avg = 0;
	double lumavg = 0;

	if (srcImage.type() != CV_8UC1)
	{
		return 0;
	}
	for (int y = 0; y<srcImage.rows; y++)
	{
		for (int x = 0; x<srcImage.cols; x++)
		{
			row_ptr = srcImage.ptr<unsigned char>(y);
			data_ptr = &row_ptr[x*srcImage.channels()];
			for (int i = 0; i != srcImage.channels(); i++)
			{
				sum += data_ptr[i];
				lumavg += log(0.0001 + data_ptr[i]);
			}
		}
	}
	lumavg = lumavg / (srcImage.rows * srcImage.cols);
	lumavg = exp(avg);
	avg = sum / (srcImage.rows * srcImage.cols);
	return avg;

}