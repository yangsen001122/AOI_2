#include "stdafx.h"
#include "ImgShowWnd.h"
#include <Windef.h>
#include <math.h>
#include "ImgShowWndListener.h"
#include "ROIBase.h"
#include "RectangleROI.h"
#include"SystemManager.h"
#define MAX_BITMAPBUF 10000*7096*10
#define MAX_SCALE  10
#define MIN_SCALE 0.1

IMPLEMENT_DYNAMIC(CImgShowWnd, CWnd)

CImgShowWnd::CImgShowWnd()
	: m_bkColor(RGB(180,180, 180))
	, m_imgZoomPoint(cv::Point2f(-1.0f, -1.0f))
	, m_wndCenterPoint(cv::Point2f(-1.0f, -1.0f))
	, m_wndZoomPoint(CPoint(-1, -1))
	, m_lBtnDownPoint(CPoint(-1, -1))
	, m_checkClickPoint(CPoint(-1, -1))
	, m_dScale(1.0f)
	, m_blBtnDown(false)
	, m_bTrack(false)
	, m_bDrag(false)
	, m_bZoom(false)
	,m_bROIDrag(true)
{

	m_criticalShowData.Lock();
	m_dictDrawColor[L"Red"] = RGB(255, 0, 0);
	m_dictDrawColor[L"Green"] = RGB(0, 255, 0);
	m_dictDrawColor[L"Blue"] = RGB(0, 0, 255);
	m_dictDrawColor[L"White"] = RGB(255, 255, 255);
	m_dictDrawColor[L"Black"] = RGB(0, 0, 0);
	m_dictDrawColor[L"Yellow"] = RGB(250, 250, 0);
	m_dictImgText.clear();
	m_dictImgLine.clear();
	m_dictImgCircle.clear();
	m_dictImgRectangle.clear();
	m_criticalShowData.Unlock();
	m_dictSelectROI.clear();
}


CImgShowWnd::~CImgShowWnd()
{
}

void CImgShowWnd::SetListener(ImageShowWndListener * pListener)
{
	if (NULL == pListener)
	{
		return;
	}
	auto it = m_dictListener.begin();
	for (; it != m_dictListener.end(); it++)
	{
		if ((*it) == pListener)
		{
			break;
		}
	}
	if (it == m_dictListener.end())
	{
		m_dictListener.push_back(pListener);
	}
}

void CImgShowWnd::DeleteListener(ImageShowWndListener * pListener)
{
	if (NULL == pListener)
	{
		return;
	}
	auto it = m_dictListener.begin();
	for (; it != m_dictListener.end(); ++it)
	{
		if ((*it) == pListener)
		{
			m_dictListener.erase(it);
			break;
		}
	}
}

void CImgShowWnd::OnROIChange(eROI_TYPE eType, cv::Rect2f rect)
{
	IMG_ROI_INFO imgROIInfo;
	switch (eType)
	{
		case ROI_LINE:
		{
			//TODO：
			break;
		}
		case ROI_RECTANGLE:
		{
			GetROIRectangle(imgROIInfo, m_dictSelectROI[0]);
			break;
		}	
		case ROI_RECT_ROTATE:
		{
			//TODO：
			break;
		}
		case ROI_CIRCLE:
		{
			//TODO：
			break;
		}
		case ROI_ELLIPSE:
		{
			//TODO：
			break;
		}
		case ROI_POLYGON:
		{
			//TODO：
			break;
		}
		default:
			break;
	}
	std::vector<ImageShowWndListener*>::iterator it = m_dictListener.begin();
	for (; it != m_dictListener.begin(); it++)
	{
		(*it)->OnSelectROIChange(eType,imgROIInfo);
	}
}

BEGIN_MESSAGE_MAP(CImgShowWnd, CWnd)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


void CImgShowWnd::GetImage(cv::Mat & mat)
{
	//TODO
}

void CImgShowWnd::DeleteAllText()
{
	m_criticalShowData.Lock();
	m_dictImgText.clear();
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::DeleteAllLine()
{
	m_criticalShowData.Lock();
	m_dictImgLine.clear();
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::DeleteAllCross()
{
	//TODO:
}

void CImgShowWnd::DeleteAllCircle()
{
	m_criticalShowData.Lock();
	m_dictImgCircle.clear();
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::DeleteAllEllipse()
{
	//TODO:
}

void CImgShowWnd::DeleteAllRectangle()
{
	m_criticalShowData.Lock();
	m_dictImgRectangle.clear();
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::DeleteAllPolygions()
{
	//TODO:
}

void CImgShowWnd::DeleteAllPoint()
{
	//TODO:
}

void CImgShowWnd::DeleteSelectROI()
{
	//TODO:
}

void CImgShowWnd::EnableZoom(bool bZoom)
{
	m_bZoom = bZoom;
}

void CImgShowWnd::EnableDrag(bool bDrag)
{
	m_bDrag = bDrag;
}

void CImgShowWnd::EnableROISelect(bool bEnableROISel)
{
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		(*it)->SetROIEnableSelected(bEnableROISel);
	}
}

void CImgShowWnd::EnableROIDrag(bool bROIDrag)
{
	m_bROIDrag = bROIDrag;
}

void CImgShowWnd::SetROIMouseOnColor(COLORREF color)
{
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		(*it)->SetROIMouseOnColor(color);
	}
}

bool CImgShowWnd::LoadImageData(IplImage * pImage)
{
	if (NULL != m_drawImage.GetImage())
	{
		m_drawImage.Destroy();
	}
	m_drawImage.CopyOf(pImage);
	m_criticalShowData.Lock();
	m_dictImgText.clear();
	m_dictImgLine.clear();
	m_dictImgCircle.clear();
	m_dictImgRectangle.clear();
	m_criticalShowData.Unlock();
	Invalidate(TRUE);

	return 0;
}

bool CImgShowWnd::LoadImageDataReset(IplImage * pImage,eCAMERA_IMAGE_TYPE eType)
{
	if (NULL != m_drawImage.GetImage())
	{
		m_drawImage.Destroy();
	}
	switch (eType)
	{
	    case IMAGE_INVALID:
		{
			//TODO:log this event
			return 0;
		}
	    case IMAGE_GRAY8:
		case IMAGE_RGB8:
		{
			m_drawImage.CopyOf(pImage);
			break;
		}	
		case IMAGE_GRAY16:
		{
			double dMinGray, dMaxGray;
			cvMinMaxLoc(pImage, &dMinGray, &dMaxGray, NULL, NULL, NULL);
			cvNormalize(pImage, pImage, 0, BIT_RANGE_16, CV_MINMAX);
			m_drawImage.CopyOf(pImage);
			break;
		}	 
		default:
			 break;
	}
	ResetData();
	CalculateScale(m_dScale);
	m_criticalShowData.Lock();
	m_dictImgText.clear();
	m_dictImgLine.clear();
	m_dictImgCircle.clear();
	m_dictImgRectangle.clear();
	m_criticalShowData.Unlock();
	Invalidate(TRUE);
	return 0;
}

bool CImgShowWnd::LoadImage(CString strFile)
{
	if (NULL != m_drawImage.GetImage())
	{
		m_drawImage.Destroy();
	}
	m_dScale = 1.0f;
	string sFile = localUtils::utils::Unicode2Utf8(strFile.GetString());
	const char* cFile = sFile.c_str();
	bool bRet = m_drawImage.Load(cFile, CV_LOAD_IMAGE_ANYDEPTH);
	ResetData();
	CalculateScale(m_dScale);
	m_criticalShowData.Lock();
	m_dictImgText.clear();
	m_dictImgLine.clear();
	m_dictImgCircle.clear();
	m_dictImgRectangle.clear();
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
	return bRet;
}

bool CImgShowWnd::SaveImage(CString strFile)
{
	if (!m_drawImage.GetImage())
	{
		return false;
	}
	string sFile = localUtils::utils::Unicode2Utf8(strFile.GetString());
	const char* cFile = sFile.c_str();
	return m_drawImage.Save(cFile);
}

bool CImgShowWnd::ClearImage()
{
	if (NULL != m_drawImage.GetImage())
	{
		m_drawImage.Destroy();
	}
	DeleteAllROI();
	m_criticalShowData.Lock();
	m_dictImgText.clear();
	m_dictImgLine.clear();
	m_dictImgCircle.clear();
	m_dictImgRectangle.clear();
	Invalidate(FALSE);
	return true;
}

bool CImgShowWnd::CheckImageLoad()
{
	return (m_drawImage.GetImage()!=NULL?true:false);
}

void CImgShowWnd::SetBkColor(COLORREF color)
{
	m_bkColor = color;
}

COLORREF CImgShowWnd::GetBkColor()
{
	return m_bkColor;
}

cv::Point2f CImgShowWnd::GetWndCenterImgPos()
{
	return m_wndCenterPoint;
}

double CImgShowWnd::GetScale()
{
	return m_dScale;
}

void CImgShowWnd::UnLockCriticalShow()
{
	if (m_criticalShowData.m_sect.RecursionCount <= 0)
	{
		return;
	}
	m_criticalShowData.Unlock();
}

bool CImgShowWnd::SaveBitmapToFile(const CBitmap & bitmap, CString fileName)
{
	//TODO
	return false;
}

void CImgShowWnd::GetSaveBitmap(CBitmap & bitmap)
{
	//TODO
}

void CImgShowWnd::ShowText(CString strText, float x, float y,wstring txtColorName)
{
	IMG_TEXT  ImgText(strText, txtColorName, cv::Point2f(x, y));
	m_criticalShowData.Lock();
	m_dictImgText.push_back(ImgText);
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::ShowLine(float fStartX, float fStartY, float fEndX, float fEndY, wstring lineColorName, int lineWide)
{
	IMG_LINE ImgLine(cv::Point2f(fStartX, fStartY), cv::Point2f(fEndX, fEndY), lineColorName, lineWide);
	m_criticalShowData.Lock();
	m_dictImgLine.push_back(ImgLine);
	m_criticalShowData.Unlock();
	Invalidate(FALSE);
}

void CImgShowWnd::ShowCross(CString strText, float x, float y, float dRadius, wstring lineColorName, int lineWide)
{
	IMG_LINE ImgLine1(cv::Point2f(x - dRadius, y - dRadius), cv::Point2f(x + dRadius, y + dRadius), lineColorName, lineWide);
	IMG_LINE ImgLine2(cv::Point2f(x - dRadius, y + dRadius), cv::Point2f(x + dRadius, y - dRadius), lineColorName, lineWide);
	m_criticalShowData.Lock();
	m_dictImgLine.push_back(ImgLine1);
	m_dictImgLine.push_back(ImgLine2);
	m_criticalShowData.Unlock();
	ShowText(strText, x+30, y-20, lineColorName);
	Invalidate(FALSE);
}

void CImgShowWnd::ShowCircle(float fCenterX, float fCenterY, float dRadius, wstring lineColorName, int lineWide)
{
	IMG_CIRCLE ImgCircle(cv::Point2f(fCenterX, fCenterY), dRadius, lineColorName, lineWide);
	m_criticalShowData.Lock();
	m_dictImgCircle.push_back(ImgCircle);
	m_criticalShowData.Unlock();
	ShowCross(L"", fCenterX, fCenterY, 10, lineColorName, lineWide);
	Invalidate(FALSE);
}

void CImgShowWnd::ShowEllipse(float fCenterX, float fCenterY, double dAngle, double dRadius1, double dRadius2, wstring lineColorName, int lineWide)
{
	//TODO:
}

void CImgShowWnd::ShowRectangle(cv::Point2f point1, cv::Point2f point2, wstring lineColorName, int lineWide)
{
	//TODO:
}

void CImgShowWnd::ShowRectangle(float x,float y, float length1, float length2, wstring lineColorName, int lineWide)
{
	IMG_RECTANGLE ImgRectangle(cv::Point2f(x,y), length1, length2, lineColorName, lineWide);
	m_criticalShowData.Lock();
	m_dictImgRectangle.push_back(ImgRectangle);
	m_criticalShowData.Unlock();
	//ShowCross(L"", x, y, 10, lineColorName, lineWide);
	Invalidate(FALSE);
}

void CImgShowWnd::ShowPolygons(vector<cv::Point2f> dictPos, wstring lineColorName, int lineWide)
{
	//TODO
}

void CImgShowWnd::ShowPoint(cv::Point2f point, wstring lineColorName, int lineWide)
{
	//TODO:
}

void CImgShowWnd::GetROIAll(std::vector<IMG_ROI_INFO>& vROIAllInfo)
{
	//TODO:
}

void CImgShowWnd::GetROIRectangle(IMG_ROI_INFO & imgROIInfo, CROIBase * pROI)
{
	imgROIInfo.m_eType = pROI->GetROIType();
	CRectangleROI* pRectangleROI = (CRectangleROI*)pROI;
	CRect rectWnd;
	cv::Rect2f cvRect;
	GetClientRect(&rectWnd);
	pRectangleROI->GetRectangleSize(cvRect);
	double dCenterX = (cvRect.x + cvRect.width / 2.0f - rectWnd.Width()*1.0 / 2) / m_dScale + m_wndCenterPoint.x;//计算选中ROI中心点相对于窗口中心点位置
	double dCenterY = (cvRect.y + cvRect.height / 2.0f - rectWnd.Height()*1.0 / 2) / m_dScale + m_wndCenterPoint.y;
	double dL1 = cvRect.width*1.0 / 2.0f / m_dScale;
	double dL2 = cvRect.height*1.0 / 2.0f / m_dScale;
	imgROIInfo.m_arrROIInfo[0] = dCenterX;
	imgROIInfo.m_arrROIInfo[1] = dCenterY;
	imgROIInfo.m_arrROIInfo[2] = dL1;
	imgROIInfo.m_arrROIInfo[3] = dL2;
}

void CImgShowWnd::SnapShot(wstring strSnapName)
{
	//TODO:
}

void CImgShowWnd::DrawROI()
{
	//TODO:
}

void CImgShowWnd::CropROI()
{
	//TODO:
}

void CImgShowWnd::SelectROI()
{
	//TODO:
}

int CImgShowWnd::GetROINumber()
{
	//TODO:
	return int(m_dictSelectROI.size());
}

void CImgShowWnd::AddSelectROI(eROI_TYPE eType)
{
	CRect rect;
	GetClientRect(&rect);
	if (eType == ROI_RECTANGLE)//创建初始矩形并居中显示
	{
		int nWidth = 200;
		int nHeight = 150;
		int nLeft = rect.left + rect.Width() / 2 - nWidth / 2;
		int nTop = rect.top + rect.Height() / 2 - nHeight / 2;
		CRectangleROI* pRectangle = new CRectangleROI(this, eType);
		cv::Rect2d cvRect(nLeft, nTop, nWidth, nHeight);
		pRectangle->SetRectangleSize(cvRect);
		m_dictSelectROI.push_back(pRectangle);

		OnROIChange(eType, cvRect);
		cv::Rect2d ImgRect;
		ImgRect.x = (nLeft - rect.Width() / 2) / m_dScale + m_wndCenterPoint.x;
		ImgRect.y = (nTop - rect.Height() / 2) / m_dScale + m_wndCenterPoint.y;
		ImgRect.width = nWidth / m_dScale;
		ImgRect.height = nHeight / m_dScale;
		pRectangle->SetRectImgZoom(ImgRect);
	}
	else if (eType == ROI_RECT_ROTATE)//创建初始旋转矩形并居中显示
	{
		//TODO
	}
	else if (eType == ROI_CIRCLE)//创建初始旋转矩形并居中显示
	{
		//TODO
	}
	Invalidate(FALSE);
}

void CImgShowWnd::DeleteAllROI()
{
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		delete(*it);
	}
	m_dictSelectROI.clear();
	Invalidate(FALSE);
}

void CImgShowWnd::CheckSelectROI(const CPoint & point)
{
	if (m_checkClickPoint.x != -1&& m_checkClickPoint.y != -1)
	{
		double dVal = (point.x - m_checkClickPoint.x)*(point.x - m_checkClickPoint.x) + (point.y - m_checkClickPoint.y)*(point.y - m_checkClickPoint.y);
		dVal = sqrt(dVal);
		if (dVal < 3.0f)
		{
			int nIndex = -1;
			for (int i = 0; i < m_dictSelectROI.size(); i++)
			{
				if (m_dictSelectROI[i]->IsPointIner(point))
				{
					nIndex = i;
					m_dictSelectROI[i]->SetROISelected(true);
					break;
				}
			}
			if (-1 != nIndex)
			{
				for (int i = 0; i < m_dictSelectROI.size(); i++)
				{
					if (i != nIndex)
					{
						m_dictSelectROI[i]->SetROISelected(false);
					}
				}
			}
		}
	}
	m_checkClickPoint.x = -1;
	m_checkClickPoint.y = -1;
}

void CImgShowWnd::OnDraw(CDC * pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.right = rcClient.left + rcClient.Width();
	rcClient.bottom = rcClient.top + rcClient.Height();
	
	if (m_drawImage.GetImage())
	{
		CDC dcMemBK;
		dcMemBK.CreateCompatibleDC(pDC);
		CBitmap memBitmapBK;
		memBitmapBK.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
		CBitmap* m_oldBitmapBK = dcMemBK.SelectObject(&memBitmapBK);
		CBrush brush;
		brush.CreateSolidBrush(m_bkColor);
		dcMemBK.FillRect(&rcClient, &brush);
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dcMemBK);
		if (m_compatibleBitMap.m_hObject)
		{
			m_compatibleBitMap.DeleteObject();
		}
		m_compatibleBitMap.CreateCompatibleBitmap(&dcMemBK, (int)(m_drawImage.Width()*m_dScale), (int)(m_drawImage.Height()*m_dScale));
		CBitmap* m_oldBitmap = dcMem.SelectObject(&m_compatibleBitMap);

		CRect rcImg;
		rcImg.left = 0;
		rcImg.top = 0;
		rcImg.right = rcImg.left + (LONG)(m_drawImage.Width()*m_dScale);
		rcImg.bottom = rcImg.top + (LONG)(m_drawImage.Height()*m_dScale);
		m_drawImage.DrawToHDC(dcMem.m_hDC, &rcImg);
		//绘制文字
		int nScalX, nScalY;
		m_criticalShowData.Lock();
		std::vector<IMG_TEXT> dictImgText(m_dictImgText);
		m_criticalShowData.Unlock();
		std::vector<IMG_TEXT>::iterator itText = dictImgText.begin();
		int nOldMode = dcMem.SetBkMode(TRANSPARENT);
		for (; itText != dictImgText.end(); ++itText)
		{
			dcMem.SetTextColor(m_dictDrawColor[(*itText).m_txtColorName]);
			GetLocalXYScale((*itText).m_point, nScalX, nScalY);
			dcMem.TextOut(nScalX, nScalY, (*itText).m_strText);
		}
        //绘制直线
		int fStartX_Local, fStartY_Local, fEndX_Local, fEndY_Local;
		m_criticalShowData.Lock();
		std::vector<IMG_LINE> LineInfoVec(m_dictImgLine);
		m_criticalShowData.Unlock();
		std::vector<IMG_LINE>::iterator itLine = LineInfoVec.begin();
		for (; itLine != LineInfoVec.end(); ++itLine)
		{
			CPen penLine(PS_SOLID, (*itLine).m_nLineWide, m_dictDrawColor[(*itLine).m_lineColorName]);
			CPen* pOldePen = dcMem.SelectObject(&penLine);
			GetLocalXYScale((*itLine).m_pointStart, fStartX_Local, fStartY_Local);
			GetLocalXYScale((*itLine).m_pointEnd, fEndX_Local, fEndY_Local);
			dcMem.MoveTo(fStartX_Local, fStartY_Local);
			dcMem.LineTo(fEndX_Local, fEndY_Local);
		}
		//绘制圆
		cv::Point2f point1, point2;
		m_criticalShowData.Lock();
		std::vector<IMG_CIRCLE>dictCircle(m_dictImgCircle);
		m_criticalShowData.Unlock();
		std::vector<IMG_CIRCLE>::iterator itCircle = dictCircle.begin();
		for (; itCircle != dictCircle.end(); ++itCircle)
		{
			point1.x = itCircle->m_pointCenter.x - itCircle->m_fRadius;
			point1.y= itCircle->m_pointCenter.y - itCircle->m_fRadius;
			point2.x = itCircle->m_pointCenter.x + itCircle->m_fRadius;
			point2.y = itCircle->m_pointCenter.y + itCircle->m_fRadius;
			GetLocalXYScale(point1, fStartX_Local, fStartY_Local);
			GetLocalXYScale(point2, fEndX_Local, fEndY_Local);
			dcMem.SelectStockObject(NULL_BRUSH);
			CPen penLine(PS_SOLID, (*itCircle).m_nLineWide, m_dictDrawColor[(*itCircle).m_lineColorName]);
			CPen* pOldePen = dcMem.SelectObject(&penLine);
			dcMem.Ellipse(fStartX_Local, fStartY_Local, fEndX_Local, fEndY_Local);
		}
		//绘制矩形
		m_criticalShowData.Lock();
		std::vector<IMG_RECTANGLE>dictRectangle(m_dictImgRectangle);
		m_criticalShowData.Unlock();
		std::vector<IMG_RECTANGLE>::iterator itRectangle = dictRectangle.begin();
		for (; itRectangle != dictRectangle.end(); ++itRectangle)
		{
			point1.x = itRectangle->m_pointCenter.x - itRectangle->m_length1;
			point1.y = itRectangle->m_pointCenter.y - itRectangle->m_length2;
			point2.x = itRectangle->m_pointCenter.x + itRectangle->m_length1;
			point2.y = itRectangle->m_pointCenter.y + itRectangle->m_length2;
			GetLocalXYScale(point1, fStartX_Local, fStartY_Local);
			GetLocalXYScale(point2, fEndX_Local, fEndY_Local);
			dcMem.SelectStockObject(NULL_BRUSH);
			CPen penLine(PS_SOLID, (*itRectangle).m_nLineWide, m_dictDrawColor[(*itRectangle).m_lineColorName]);
			CPen* pOldePen = dcMem.SelectObject(&penLine);
			dcMem.Rectangle(fStartX_Local, fStartY_Local, fEndX_Local, fEndY_Local);
		}

		//根据当前窗口大小判断图像的原始大小
		int rectWith = rcClient.Width();
		int rectHeight = rcClient.Height();
		int rectImgWith = rcImg.Width();
		int rectImgHeight = rcImg.Height();

		if (rectImgWith <= rectWith
			&& rectImgHeight <= rectHeight)
		{
			//说明显示窗口的宽高都比图像的宽高大,图像居中显示,显示点在图像中心位置(可根据需要修改)
			dcMemBK.BitBlt(rcClient.left + rectWith / 2 - rectImgWith / 2,rcClient.top + rectHeight / 2 - rectImgHeight / 2,rectImgWith,rectImgHeight, &dcMem,rcImg.left, rcImg.top, SRCCOPY);
			m_wndCenterPoint.x = (float)(m_drawImage.Width()*1.0) / 2;
			m_wndCenterPoint.y = (float)(m_drawImage.Height()*1.0) / 2;
		}
		else if ((rectImgWith >rectWith)  &&  (rectImgHeight <= rectHeight))
		{
			//说明窗口的宽比图像小，但高比图像大 ，图像居中显示
			//计算放大点x方向在放大后图像中的位置
			double zoomx = m_imgZoomPoint.x*m_dScale;
			//在这种情况下y方向上的图像位置为图像的中心位置
			double zoomy = m_drawImage.Height()*1.0 / 2 * m_dScale;
			//当前点在显示窗口中的位置保存在m_WndZoomPoint中，需要显示在窗口中的起始点计算
			int picleft = (int)(zoomx - m_wndZoomPoint.x);
			dcMemBK.BitBlt(rcClient.left,rcClient.top + rectHeight / 2 - rectImgHeight / 2,rectWith, rectImgHeight,&dcMem,picleft, rcImg.top, SRCCOPY);
			//重新计算显示窗口中心点对应的图像位置
			float zoomCenterX = (float)zoomx + (float)(rcClient.Width()*1.0 / 2) - m_wndZoomPoint.x;
			m_wndCenterPoint.x = (float)(zoomCenterX / m_dScale);
			m_wndCenterPoint.y = (float)(m_drawImage.Height()*1.0) / 2;
		}
		else if (rectImgWith <= rectWith&& rectImgHeight > rectHeight)
		{
			//说明窗口的宽比图像大 但高比图像小  图像居中显示
			//计算放大点x方向的位置在图像的中心位置
			double zoomx = m_drawImage.Width()*1.0 / 2 * m_dScale;
			//在这种情况下y方向上的图像位置为图像的中心位置
			double zoomy = m_imgZoomPoint.y*m_dScale;
			//当前点在显示窗口中的位置保存在m_WndZoomPoint中 需要显示在窗口中的起始点计算
			int pictop = (int)(zoomy - m_wndZoomPoint.y);
			dcMemBK.BitBlt(rcClient.left + rectWith / 2 - rectImgWith / 2,rcClient.top,rectImgWith, rectHeight,&dcMem,rcImg.left, pictop, SRCCOPY);
			//重新计算显示窗口中心点对应的图像位置
			float zoomCenterY = (float)zoomy + (float)(rcClient.Height()*1.0) / 2 - m_wndZoomPoint.y;
			m_wndCenterPoint.y = (float)(zoomCenterY / m_dScale);
			m_wndCenterPoint.x = (float)(m_drawImage.Width()*1.0) / 2;
		}
		else if ((rectImgWith > rectWith)&& (rectImgHeight > rectHeight))
		{
			//说明窗口的宽高都比图像小
			//计算放大点x方向的位置在图像的中心位置
			double zoomx = m_imgZoomPoint.x*m_dScale;
			//在这种情况下y方向上的图像位置为图像的中心位置
			double zoomy = m_imgZoomPoint.y*m_dScale;
			//当前点在显示窗口中的位置保存在m_WndZoomPoint中 需要显示在窗口中的起始点计算
			int pictop = (int)(zoomy - m_wndZoomPoint.y);
			int picleft = (int)(zoomx - m_wndZoomPoint.x);
			dcMemBK.BitBlt(rcClient.left,rcClient.top,rectWith, rectHeight,&dcMem,picleft, pictop, SRCCOPY);
			//重新计算显示窗口中心点对应的图像位置
			float zoomCenterX = (float)zoomx + (float)(rcClient.Width()*1.0 / 2) - m_wndZoomPoint.x;
			m_wndCenterPoint.x = (float)(zoomCenterX / m_dScale);
			float zoomCenterY = (float)zoomy + (float)(rcClient.Height()*1.0 / 2) - m_wndZoomPoint.y;
			m_wndCenterPoint.y = (float)(zoomCenterY / m_dScale);
		}
		OnDrawROI(&dcMemBK);//利用dcMemBK绘制选中的区域
		pDC->StretchBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMemBK,rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SRCCOPY);
		dcMem.SelectObject(m_oldBitmap);
		dcMemBK.SelectObject(m_oldBitmapBK);
		memBitmapBK.DeleteObject();
		dcMem.DeleteDC();
		dcMemBK.DeleteDC();
		brush.DeleteObject();
	}
	else//draw background
	{
		CDC dcMemBK;
		dcMemBK.CreateCompatibleDC(pDC);
		CBitmap memBitmapBk;
		memBitmapBk.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
		CBitmap* m_oldBitmapBK = dcMemBK.SelectObject(&memBitmapBk);
		CBrush brush;
		brush.CreateSolidBrush(m_bkColor);
		dcMemBK.FillRect(&rcClient, &brush);
		pDC->StretchBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMemBK,rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SRCCOPY);
		dcMemBK.SelectObject(m_oldBitmapBK);
		dcMemBK.DeleteDC();
		memBitmapBk.DeleteObject();
	}
}

void CImgShowWnd::OnDrawROI(CDC * pDC)
{
	std::vector<CROIBase*>::iterator it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		(*it)->OnUpdateRectShow();
		(*it)->DrawROI(pDC);
	}
}

BOOL CImgShowWnd::PreCreateWindow(CREATESTRUCT & cs)
{
	if (!CWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1), NULL);
	return TRUE;
}

void CImgShowWnd::OnDestroy()
{
	if (m_compatibleBitMap.m_hObject)
	{
		m_compatibleBitMap.DeleteObject();
	}
	if (NULL != m_drawImage.GetImage())
	{
		m_drawImage.Destroy();
	}
	DeleteAllROI();
	CWnd::OnDestroy();
}

BOOL CImgShowWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

void CImgShowWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (cx != 0 && cy != 0)
	{
		ResetData();
		CalculateScale(m_dScale);
		auto it = m_dictSelectROI.begin();
		for (; it != m_dictSelectROI.end(); it++)
		{
			(*it)->OnSize(nType, cx, cy);
		}
		Invalidate(FALSE);
	}
}

BOOL CImgShowWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (NULL == m_drawImage.GetImage()|| !m_bZoom)
	{
		return  CWnd::OnMouseWheel(nFlags, zDelta, pt);
	}
	this->ScreenToClient(&pt);
	CalculateZoomPointInImg(pt);
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); it++)
	{
		(*it)->OnPreMouseWheel(nFlags, zDelta, pt);
	}
	if (zDelta > 0)
	{
		if (!m_drawImage.GetImage())
		{
			return TRUE;
		}
		int nSize = int(m_drawImage.Width()*m_dScale*m_drawImage.Height()*m_dScale);
		if (nSize > MAX_BITMAPBUF)
		{
			return TRUE;
		}

		if (m_dScale - MAX_SCALE > 0.001)    //上限,太大就不放大了
		{
			return TRUE;
		}
		m_dScale *= 1.2;
	}
	else if (zDelta < 0)
	{
		if (MIN_SCALE - m_dScale > 0.001)   //下限，太小就不缩小了
		{
			return TRUE;
		}
		m_dScale /= 1.1;
	}
	for (auto it : m_dictSelectROI)
	{
		(*it).OnPreMouseWheel(nFlags, zDelta, pt);
	}
	Invalidate(FALSE);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CImgShowWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool bInner = false;
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); it++)
	{
		if ((*it)->IsPointIner(point))
		{
			bInner = true;
			(*it)->OnLButtonDown(point);
			break;
		}
	}
	if (!bInner)
	{
		if (m_bDrag)
		{
			m_blBtnDown = true;
			m_bTrack = TRUE;
			m_lBtnDownPoint = point;
		}
	}
	m_checkClickPoint = point;
	CWnd::OnLButtonDown(nFlags, point);
}

void CImgShowWnd::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CImgShowWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	//计算图像中光标的位置
	CRect rect;
	GetClientRect(&rect);
	double dAddX = (point.x - rect.Width() / 2) / m_dScale;
	double dAddY = (point.y - rect.Height() / 2) / m_dScale;
	cv::Point2d pt;
	pt.x = (float)dAddX + m_wndCenterPoint.x;
	pt.y = (float)dAddY + m_wndCenterPoint.y;
	std::vector<ImageShowWndListener*>::iterator it = m_dictListener.begin();
	for (; it != m_dictListener.end(); ++it)
	{
		if (NULL != (*it))
		{
			(*it)->OnMouseMove(pt);
		}
	}
	//计算图像中光标的位置 ,拖拽图像
	if (m_bTrack)
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE | TME_HOVER;
		csTME.hwndTrack = m_hWnd;
		csTME.dwHoverTime = 10;
		::_TrackMouseEvent(&csTME);
		m_bTrack = false;
	}

	if (m_bDrag)
	{
		//图像拖拽
		if (m_blBtnDown)
		{
			if ((m_lBtnDownPoint.x != -1)&& (m_lBtnDownPoint.x != -1))
			{
				int nAddX1 = point.x - m_lBtnDownPoint.x;
				int nAddY1 = point.y - m_lBtnDownPoint.y;
				if (abs(nAddX1) > 1|| abs(nAddY1) > 1)
				{
					m_lBtnDownPoint.x = point.x;
					m_lBtnDownPoint.y = point.y;
					m_wndZoomPoint.x += nAddX1;
					m_wndZoomPoint.y += nAddY1;
				}
				auto it = m_dictSelectROI.begin();//通知选中的ROI跟着图跑
				for (; it != m_dictSelectROI.end(); it++)
				{
					(*it)->OnMouseMoveAttachImage();
				}
			}
		}
	}

	if (m_bROIDrag)//向当前ROI透传鼠标移动的消息
	{
		auto it = m_dictSelectROI.begin();
		for (; it != m_dictSelectROI.end(); ++it)
		{
			(*it)->OnMouseMove(point);
		}
	}
	Invalidate(FALSE);
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CImgShowWnd::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	BOOL bHandle = FALSE;
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		if (TRUE == (*it)->OnSetCursor())
		{
			bHandle = TRUE;
		}

	}
	if (TRUE == bHandle)
	{
		return TRUE;
	}
	else
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}
	//return CWnd::OnSetCursor( pWnd, nHitTest,  message);
}

void CImgShowWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDrag)
	{
		m_blBtnDown = false;
		if (m_lBtnDownPoint.x != -1&& m_lBtnDownPoint.x != -1)
		{
			int nAddX = point.x - m_lBtnDownPoint.x;
			int nAddY = point.y - m_lBtnDownPoint.y;
			m_wndZoomPoint.x += nAddX;
			m_wndZoomPoint.y += nAddY;
			auto it = m_dictSelectROI.begin();
			for (; it != m_dictSelectROI.end(); ++it)
			{
				(*it)->OnMouseMoveAttachImage();//通知ROI跟着移动消息跑起来
			}
			m_lBtnDownPoint.x = -1;
			m_lBtnDownPoint.y = -1;
		}
	}
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		(*it)->OnLButtonUp(point);	//向ROI透传光标移动消息
	}
	CheckSelectROI(point);// 判断用户是否单击了绘制窗口 如果单击做相应处理
	Invalidate(FALSE);
	CWnd::OnLButtonUp(nFlags, point);
}

void CImgShowWnd::OnMouseLeave()
{
	m_bTrack = FALSE;
	if (m_bDrag)
	{
		if (m_blBtnDown == true)
		{
			m_lBtnDownPoint.x = -1;
			m_lBtnDownPoint.y = -1;
			m_blBtnDown = false;
		}
	}
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); it++)
	{
		(*it)->OnMouseLeave();
	}
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	CWnd::OnMouseLeave();
}

void CImgShowWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	auto it = m_dictSelectROI.begin();
	for (; it != m_dictSelectROI.end(); ++it)
	{
		(*it)->SetROISelected(false);
	}
	CWnd::OnRButtonUp(nFlags, point);
}

void CImgShowWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	ResetData();
	CalculateScale(m_dScale);
	Invalidate(FALSE);
	CWnd::OnRButtonDown(nFlags, point);
}

void CImgShowWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//TODO
}

bool CImgShowWnd::CalculateZoomPointInImg(const CPoint & point)
{
	CRect rect;
	GetClientRect(&rect);
	double dAddX = (point.x - rect.Width() / 2) / m_dScale;
	double dAddY = (point.y - rect.Height() / 2) / m_dScale;
	m_imgZoomPoint.x = m_wndCenterPoint.x + dAddX;
	m_imgZoomPoint.y = m_wndCenterPoint.y + dAddY;
	m_wndZoomPoint = point;
	return true;
}

void CImgShowWnd::ResetData()
{
	m_imgZoomPoint = cv::Point2f(-1.0f, -1.0f);
	m_wndCenterPoint = cv::Point2f(-1.0f, -1.0f);
	m_wndZoomPoint = CPoint(-1, -1);
	m_lBtnDownPoint = CPoint(-1, -1);
	m_dScale = 1.0f;
}

void CImgShowWnd::CalculateScale(double & dScale)
{
	if (NULL == m_drawImage.GetImage())
	{
		return;
	}
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcImg(0, 0, 0, 0);
	rcImg.right = rcImg.left + m_drawImage.Width();
	rcImg.bottom = rcImg.top + m_drawImage.Height();

	int rcWndWidth = rcClient.Width();
	int rcWndHeight = rcClient.Height();
	int rcImgWidth = rcImg.Width();
	int rcImgHeight = rcImg.Height();

	if ((((double)rcWndWidth) / ((double)rcWndHeight)) > (((double)rcImgWidth) / ((double)rcImgHeight)))
	{
		if (rcImgHeight <= rcWndHeight)
		{
			dScale = 1.0f;
		}
		else
		{
			dScale = (double)(((double)rcWndHeight) / ((double)rcImgHeight));
		}
	}
	else
	{
		if (rcImgWidth <= rcWndWidth)
		{
			dScale = 1.0f;
		}
		else
		{
			dScale = (double)(((double)rcWndWidth) / ((double)rcImgWidth));
		}
	}
}

void CImgShowWnd::GetLocalXYScale(const cv::Point2f & point, int & x, int & y)
{
	x = (int)(point.x* m_dScale);
	y = (int)(point.y* m_dScale);
}

