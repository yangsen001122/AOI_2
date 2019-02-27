#include "stdafx.h"
#include "SubProcess.h"


SubProcess::SubProcess()
{
}


SubProcess::~SubProcess()
{
}


void Cross(cv::Mat& img, cv::Point2f point, cv::Scalar color, int size, int thickness)
{
	// 绘制横线
	cv::line(img, cv::Point2f(point.x - size / 2, point.y), cv::Point2f(point.x + size / 2, point.y), color, thickness, 8, 0);
	// 绘制竖线
	cv::line(img, cv::Point2f(point.x, point.y - size / 2), cv::Point2f(point.x, point.y + size / 2), color, thickness, 8, 0);
}

float SubProcess::GetDistance(cv::Point2f f1, cv::Point2f f2)
{
	return sqrt((float)(f1.x - f2.x)*(f1.x - f2.x) + (f1.y - f2.y)*(f1.y - f2.y));
}

void SubProcess::FindCorner(cv::Mat srcImage, vector<cv::Point> contour, vector<cv::Point2f>& corner)
{
	size_t icount = contour.size();
	float fmax = -1;
	int   imax = -1;
	bool  bstart = false;

	int k = 5;
	double shapeFactor = 0.08;
	for (int i = 0; i < contour.size(); i++)
	{
		cv::Point2f pa = (cv::Point2f)contour[(i + icount - k) % icount];
		cv::Point2f pb = (cv::Point2f)contour[(i + icount + k) % icount];
		cv::Point2f pc = (cv::Point2f)contour[i];

		// 两支撑点距离
		float fa = GetDistance(pa, pb);
		float fb = GetDistance(pa, pc) + GetDistance(pb, pc);
		float fang = fa / fb;
		float fsharp = 1 - fang;
		if (fsharp > shapeFactor)
		{
			bstart = true;
			if (fsharp > fmax)
			{
				fmax = fsharp;
				imax = i;
			}
		}
		else
		{
			if (bstart)
			{
				corner.push_back((cv::Point2f)contour[imax]);
				Cross(srcImage, contour[imax], cv::Scalar(255), 10, 1);
				imax = -1;
				fmax = -1;
				bstart = false;
			}
		}
	}
}

void SubProcess::FindCorner2(cv::Mat srcImage, vector<vector<cv::Point>> contours, int Idx, double linMinLen, vector<cv::Point2f>& corner)
{
	// 检测轮廓直线(直线检测方式)
	cv::Mat ContourImage = cv::Mat::zeros(srcImage.size(), CV_8UC1);
	cv::drawContours(ContourImage, contours, Idx, cv::Scalar(255), cv::FILLED, cv::LINE_AA);
	cv::Ptr<cv::LineSegmentDetector> ls = cv::createLineSegmentDetector(cv::LSD_REFINE_STD);
	vector<cv::Vec4f> lines_std;
	ls->detect(ContourImage, lines_std);

	// 计算每条线段的特征,并保留一定长度以上的线段
	vector<SegLine> lineSeg;
	for (int i = 0; i < lines_std.size(); i++)
	{
		double len = sqrtf(cv::pow(lines_std[i][2] - lines_std[i][0], 2) + cv::pow(lines_std[i][3] - lines_std[i][1], 2));
		if (len > linMinLen)
		{
			SegLine sl;
			sl.lineStart = cv::Point2f(lines_std[i][0], lines_std[i][1]);
			sl.lineEnd = cv::Point2f(lines_std[i][2], lines_std[i][3]);
			sl.lineCenter = cv::Point2f((lines_std[i][0] + lines_std[i][2]) / 2, (lines_std[i][1] + lines_std[i][3]) / 2);
			sl.lineLength = len;
			double k = (lines_std[i][3] - lines_std[i][1]) / (lines_std[i][2] - lines_std[i][0]);
			sl.Phi = atan(k);
			lineSeg.push_back(sl);
		}
	}
	// 将共线的线段集合并拟合为一整条直线
	double MaxAngle = 0.5;
	double MaxDistAbs = 1000;
	double MaxShift = 50;
	if (lineSeg.size() > 4)
	{
		UnionCollinearLine(lineSeg, lineSeg, MaxDistAbs, MaxShift, MaxAngle);
		if (lineSeg.size() != 4)
			return;
	}
	// 求四个交点(左上,右上,右下,左下) //
	cv::Point2f TLPoint, TRPoint, BLPoint, BRPoint;
	IntersectionPoint(lineSeg[0].lineStart, lineSeg[0].lineEnd, lineSeg[3].lineStart, lineSeg[3].lineEnd, TLPoint);
	IntersectionPoint(lineSeg[0].lineStart, lineSeg[0].lineEnd, lineSeg[1].lineStart, lineSeg[1].lineEnd, TRPoint);
	IntersectionPoint(lineSeg[1].lineStart, lineSeg[1].lineEnd, lineSeg[2].lineStart, lineSeg[2].lineEnd, BRPoint);
	IntersectionPoint(lineSeg[2].lineStart, lineSeg[2].lineEnd, lineSeg[3].lineStart, lineSeg[3].lineEnd, BLPoint);
	corner.push_back(TLPoint);
	corner.push_back(TRPoint);
	corner.push_back(BRPoint);
	corner.push_back(BLPoint);
}

// 计算小相机透视变换矩阵
HRESULT SubProcess::CalcPerspectiveMat(const cv::Mat& srcImage, int maskSize, double lineMinLen, int newWidth, int newHeight)
{
	try
	{
		cv::Mat grayImage;
		cv::cvtColor(srcImage, grayImage, cv::COLOR_BGR2GRAY);

		/* Step1 : 有效区域粗定位 */
		cv::Mat Region;

		cv::Mat GrayMean, GrayDev;
		cv::meanStdDev(srcImage, GrayMean, GrayDev);
		double CurrentThresh = GrayMean.at<double>(0, 0) - 0.1*GrayDev.at<double>(0, 0);
		cv::threshold(grayImage, Region, CurrentThresh, 255, cv::THRESH_BINARY);
		cv::Mat SE = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(maskSize, maskSize));
		cv::morphologyEx(Region, Region, cv::MORPH_CLOSE, SE);
		FillUp(Region, Region);

		// 获取最大轮廓
		cv::Mat tmpImage = Region.clone();
		vector<vector<cv::Point>> Contours;
		cv::findContours(tmpImage, Contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_TC89_KCOS);
		int Idx = SelectMaxRegion(Contours);
		if (Idx == -1)
			return ERROR_EMPTY;

		// 获取该区域特征值
		double Phi, centRow, centCol, Length1, Length2;
		GetRegionFeature(grayImage, Contours[Idx], Phi, centRow, centCol, Length1, Length2);

		// 找角点的方式获取矩形角点
		vector<cv::Point2f> corner;
		FindCorner(grayImage, Contours[Idx], corner);
		cv::Point2d TLPoint, TRPoint, BLPoint, BRPoint;
		if (corner.size() == 4)
		{
			for (int i = 0; i<corner.size(); i++)
			{
				if (corner[i].x < centCol && corner[i].y < centRow)
					TLPoint = corner[i];
				else if (corner[i].x > centCol && corner[i].y < centRow)
					TRPoint = corner[i];
				else if (corner[i].x > centCol && corner[i].y > centRow)
					BRPoint = corner[i];
				else if (corner[i].x < centCol && corner[i].y > centRow)
					BLPoint = corner[i];
			}
		}
		// 获取放射变换矩阵
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
			cv::Point2d(0,0),
			cv::Point2d(newWidth - 1,0),
			cv::Point2d(newWidth - 1,newHeight - 1),
			cv::Point2d(0,newHeight - 1)
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

// 进行透视变换
HRESULT SubProcess::PerspectiveTransform(cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight)
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

void SubProcess::FillUp(cv::Mat& srcImage, cv::Mat& dstImage)
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

int SubProcess::SelectMaxRegion(const vector<vector<cv::Point>> Contours)
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

cv::Point2f SubProcess::ProjPoint(cv::Point2f pt, cv::Point2f lineStart, cv::Point2f lineEnd)
{
	float x1 = lineStart.x;
	float y1 = lineStart.y;
	float x2 = lineEnd.x;
	float y2 = lineEnd.y;
	float x3 = pt.x;
	float y3 = pt.y;

	float yk = ((x3 - x2)*(x1 - x2)*(y1 - y2) + y3*(y1 - y2)*(y1 - y2) + y2*(x1 - x2)*(x1 - x2)) /
		static_cast<float>(pow(cv::norm(cv::Point2f(x1 - x2, y1 - y2)), 2));
	float xk = ((x1 - x2)*x2*(y1 - y2) + (x1 - x2)*(x1 - x2)*(yk - y2)) / ((x1 - x2)*(y1 - y2));


	if (x1 == x2)
		xk = x1;
	if (y1 == y2)
		xk = x3;

	return cv::Point2f(xk, yk);
}

void SubProcess::UnionCollinearLine(vector<SegLine>& lines, vector<SegLine>& unionLine, double MaxDistAbs, double MaxShift, double MaxAngle)
{
	vector<SegLine> tmpLine(lines);
	vector<SegLine> HoriLine;
	vector<SegLine> VertLine;

	while (tmpLine.size() > 0)
	{
		vector<cv::Point2f> pts;

		// 取出第一条线段
		SegLine sl = tmpLine[0];
		pts.push_back(sl.lineStart);
		pts.push_back(sl.lineCenter);
		pts.push_back(sl.lineEnd);

		tmpLine.erase(tmpLine.begin());

		// 计算其他线段是否与其共线
		vector<SegLine>::iterator Iter = tmpLine.begin();
		for (Iter; Iter != tmpLine.end();)
		{
			// 计算两直线夹角 < MaxAngle
			double angle = LineAngle(sl.lineStart, sl.lineEnd, Iter->lineStart, Iter->lineEnd);
			if (abs(angle) > MaxAngle)
			{
				Iter++;
				continue;
			}

			// 计算线段在直线上的投影点
			cv::Point2f projStart = ProjPoint(Iter->lineStart, sl.lineStart, sl.lineEnd);
			cv::Point2f projEnd = ProjPoint(Iter->lineEnd, sl.lineStart, sl.lineEnd);

			// 计算偏移距离
			double shift1 = DistancePoint(projStart, Iter->lineStart);
			double shift2 = DistancePoint(projEnd, Iter->lineEnd);
			if (shift1 > MaxShift && shift2 > MaxShift)
			{
				Iter++;
				continue;
			}

			// 计算投影点直线距离
			double dist1 = DistancePoint(sl.lineStart, projStart);
			double dist2 = DistancePoint(sl.lineStart, projEnd);
			double dist3 = DistancePoint(sl.lineEnd, projStart);
			double dist4 = DistancePoint(sl.lineEnd, projEnd);
			if (dist1 > MaxDistAbs && dist2 > MaxDistAbs && dist3 > MaxDistAbs && dist4 > MaxDistAbs)
			{
				Iter++;
				continue;
			}

			// 符合的直线将其放入点集
			pts.push_back(Iter->lineStart);
			pts.push_back(Iter->lineCenter);
			pts.push_back(Iter->lineEnd);

			Iter = tmpLine.erase(Iter);
		}

		// 根据这些点重新拟合直线
		if (pts.size() > 3)
		{
			cv::Vec4f line;
			cv::fitLine(pts, line, cv::DIST_HUBER, 0, 1e-2, 1e-2);

			SegLine seg;
			seg.lineStart.x = line[2] - 400 * line[0];
			seg.lineStart.y = line[3] - 400 * line[1];
			seg.lineEnd.x = line[2] + 400 * line[0];
			seg.lineEnd.y = line[3] + 400 * line[1];
			double k = (seg.lineEnd.y - seg.lineStart.y) / (seg.lineEnd.x - seg.lineStart.x);
			seg.Phi = atan(k);

			// 水平方向
			if (-CV_PI / 4.0 < seg.Phi && seg.Phi < CV_PI / 4.0)
			{
				HoriLine.push_back(seg);
			}
			else
			{
				VertLine.push_back(seg);
			}
		}
		else if (pts.size() == 3)
		{
			// 水平方向
			if (-CV_PI / 4.0 < sl.Phi && sl.Phi < CV_PI / 4.0)
			{
				HoriLine.push_back(sl);
			}
			else
			{
				VertLine.push_back(sl);
			}
		}
	}

	// 对拟合线进行排序(上,右,下,左)
	SegLine arySeg[4];
	if (HoriLine.size() == 2 && VertLine.size() == 2)
	{
		if (HoriLine[0].lineStart.y < HoriLine[1].lineStart.y)
		{
			arySeg[0] = HoriLine[0];
			arySeg[2] = HoriLine[1];
		}
		else
		{
			arySeg[0] = HoriLine[1];
			arySeg[2] = HoriLine[0];
		}

		if (VertLine[0].lineStart.x < VertLine[1].lineStart.x)
		{
			arySeg[1] = VertLine[1];
			arySeg[3] = VertLine[0];
		}
		else
		{
			arySeg[1] = VertLine[0];
			arySeg[3] = VertLine[1];
		}

		unionLine.clear();
		for (int i = 0; i < 4; i++)
		{
			unionLine.push_back(arySeg[i]);
		}
	}
}

void SubProcess::IntersectionPoint(cv::Point2f line1Start, cv::Point2f line1End, cv::Point2f line2Start, cv::Point2f line2End, cv::Point2f& crossPoint)
{
	float ka, kb;
	ka = (line1End.y - line1Start.y) / (line1End.x - line1Start.x);
	kb = (line2End.y - line2Start.y) / (line2End.x - line2Start.x);

	crossPoint.x = (ka*line1Start.x - line1Start.y - kb*line2Start.x + line2Start.y) / (ka - kb);
	crossPoint.y = (ka*kb*(line1Start.x - line2Start.x) + ka*line2Start.y - kb*line1Start.y) / (ka - kb);
}

double SubProcess::LineAngle(cv::Point2f line1Start, cv::Point2f line1End, cv::Point2f line2Start, cv::Point2f line2End)
{
	double K1 = (line1End.y - line1Start.y) / (line1End.x - line1Start.x);
	double K2 = (line2End.y - line2Start.y) / (line2End.x - line2Start.x);

	double angle = atan(abs((K2 - K1) / (1 + K1 * K2)));
	if (angle > CV_PI)
		angle -= 2 * CV_PI;
	else if (angle < -CV_PI)
		angle += 2 * CV_PI;

	return angle;
}

double SubProcess::DistancePoint(cv::Point2f pt1, cv::Point2f pt2)
{
	return sqrt(pow(pt1.y - pt2.y, 2) + pow(pt1.x - pt2.x, 2));
}

void SubProcess::GetRegionFeature(const cv::Mat& srcImage, vector<cv::Point> Contour, double& Phi, double& Row, double& Col, double& Length1, double& Length2)
{
	cv::Moments M = cv::moments(Contour);

	// 质心
	Col = M.m10 / M.m00;
	Row = M.m01 / M.m00;

	// 旋转角
	Phi = -0.5 * atan2(2 * M.mu11, M.mu20 - M.mu02);

	// 长短轴
	cv::RotatedRect rc = cv::minAreaRect(Contour);
	Length1 = rc.size.width > rc.size.height ? rc.size.width / 2 : rc.size.height / 2;
	Length2 = rc.size.width > rc.size.height ? rc.size.height / 2 : rc.size.width / 2;
}

HRESULT SubProcess::RotationImg(cv::Mat& SrcImg, cv::Mat& dstImg, int angle)
{
	try
	{
		if (!SrcImg.empty())
		{
			switch (angle)
			{
				case 0:
					dstImg = SrcImg.clone();
				case 90:
					cv::transpose(SrcImg, SrcImg);
					cv::flip(SrcImg, dstImg, 1);
				case 180:
					cv::flip(SrcImg, SrcImg, 0);
					cv::flip(SrcImg, dstImg, 1);
				case 270:
					cv::transpose(SrcImg, SrcImg);
					cv::flip(SrcImg, dstImg, 0);
			default:
				break;
			}
		}
		return ERROR_SUCCESS;
	}
	catch (cv::Exception& e)
	{
		OutputDebugStringA(e.what());
		return ERROR_PERSPECTIVE_TRANSFORM;
	}
}
