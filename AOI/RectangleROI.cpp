#include "stdafx.h"
#include "RectangleROI.h"
#include "ImgShowWnd.h"

CRectangleROI::CRectangleROI(CImgShowWnd* ptrParent, eROI_TYPE nROIType):
	CROIBase(ptrParent, nROIType),
	m_bImgZoom(false),
	m_nDragType(ROI_RECT_DRAG_NULL),
	m_nColorDiagonal(m_nROIColor),
	m_rect(cv::Rect2f(0, 0, 0, 0)),
	m_rectLeft(CRect(0, 0, 0, 0)),
	m_rectTop(CRect(0, 0, 0, 0)),
	m_rectRight(CRect(0, 0, 0, 0)),
	m_rectBottom(CRect(0, 0, 0, 0)),
	m_rectIner(CRect(0, 0, 0, 0))
{
}


CRectangleROI::~CRectangleROI()
{
}

void CRectangleROI::GetRectangleSize(cv::Rect2f & rect)
{
	rect = m_rect;
}

void CRectangleROI::SetRectangleSize(cv::Rect2f rect)
{
	m_rect = rect;
	CRect rt;
	rt.left = (LONG)m_rect.x;
	rt.right = (LONG)(m_rect.x + m_rect.width);
	rt.top = (LONG)m_rect.y;
	rt.bottom = (LONG)(m_rect.y + m_rect.height);
	//左
	m_rectLeft.left = rt.left - m_nSelInterval;
	m_rectLeft.right = rt.left + m_nSelInterval;
	m_rectLeft.top = rt.top + m_nSelInterval;
	m_rectLeft.bottom = rt.bottom - m_nSelInterval;

	//右
	m_rectRight.left = rt.right - m_nSelInterval;
	m_rectRight.right = rt.right + m_nSelInterval;
	m_rectRight.top = rt.top + m_nSelInterval;
	m_rectRight.bottom = rt.bottom - m_nSelInterval;

	//上
	m_rectTop.left = rt.left - m_nSelInterval;
	m_rectTop.right = rt.right + m_nSelInterval;
	m_rectTop.top = rt.top - m_nSelInterval;
	m_rectTop.bottom = rt.top + m_nSelInterval;

	//下
	m_rectBottom.left = rt.left - m_nSelInterval;
	m_rectBottom.right = rt.right + m_nSelInterval;
	m_rectBottom.top = rt.bottom - m_nSelInterval;
	m_rectBottom.bottom = rt.bottom + m_nSelInterval;

	//中间
	m_rectIner.left = rt.left + m_nSelInterval;
	m_rectIner.right = rt.right - m_nSelInterval;
	m_rectIner.top = rt.top + m_nSelInterval;
	m_rectIner.bottom = rt.bottom - m_nSelInterval;
}

void CRectangleROI::OnMouseMove(CPoint point)
{
	if (m_bLbtnDown)
	{
		if (m_LbtnDownPoint.x != -1&& m_LbtnDownPoint.y != -1)
		{
			int nAddX = point.x - m_LbtnDownPoint.x;
			int nAddY = point.y - m_LbtnDownPoint.y;
			if (abs(nAddX) > 1 || abs(nAddY) > 1)
			{
				if (m_nDragType == ROI_RECT_DRAG_LEFT)
				{
					if (m_rect.x + m_rect.width - point.x < ROI_MINWIDTH)
					{
						return;
					}
					double dx = m_rect.x;
					m_rect.x = point.x;
					m_rect.width += dx - m_rect.x;
					SetRectangleSize(m_rect);
					UpdateRectImgZoom(m_rect);
					if (NULL != m_pWndParent)
					{
						m_pWndParent->OnROIChange(m_nROIType, m_rect);
					}

				}
				else if (m_nDragType == ROI_RECT_DRAG_TOP)
				{
					if (m_rect.y + m_rect.height - point.y < ROI_MINHEIGHT)
					{
						return;
					}
					double dy = m_rect.y;
					m_rect.y = point.y;
					m_rect.height += dy - m_rect.y;
					SetRectangleSize(m_rect);
					UpdateRectImgZoom(m_rect);
					if (NULL != m_pWndParent)
					{
						m_pWndParent->OnROIChange(m_nROIType, m_rect);
					}
				}
				else if (m_nDragType == ROI_RECT_DRAG_RIGHT)
				{
					if (point.x - m_rect.x < ROI_MINWIDTH)
					{
						return;
					}
					double dx = m_rect.x + m_rect.width;
					m_rect.width += point.x - dx;
					SetRectangleSize(m_rect);
					UpdateRectImgZoom(m_rect);
					if (NULL != m_pWndParent)
					{
						m_pWndParent->OnROIChange(m_nROIType, m_rect);
					}
				}
				else if (m_nDragType == ROI_RECT_DRAG_BOTTOM)
				{
					if (point.y - m_rect.y < ROI_MINHEIGHT)
					{
						return;
					}
					double dy = m_rect.y + m_rect.height;
					m_rect.height += point.y - dy;
					SetRectangleSize(m_rect);
					UpdateRectImgZoom(m_rect);
					if (NULL != m_pWndParent)
					{
						m_pWndParent->OnROIChange(m_nROIType, m_rect);
					}
				}
				else if (m_nDragType == ROI_RECT_DRAG_INNER)
				{
					m_rect.x += nAddX;
					m_rect.y += nAddY;
					SetRectangleSize(m_rect);
					UpdateRectImgZoom(m_rect);
					if (NULL != m_pWndParent)
					{
						m_pWndParent->OnROIChange(m_nROIType, m_rect);
					}
				}
				m_LbtnDownPoint.x = point.x;
				m_LbtnDownPoint.y = point.y;
			}
		}
	}
	else
	{
		if (m_bROISelect)
		{
			//说明是选中状态
			m_nColorDiagonal = CROIBase::m_selectROIColor;
			m_nROIColor = CROIBase::m_selectROIColor;
		}
		else
		{
			if (m_rectLeft.PtInRect(point)|| m_rectTop.PtInRect(point)|| m_rectRight.PtInRect(point)|| m_rectBottom.PtInRect(point))
			{
				m_nROIColor = m_mouseOnColor;
				m_nColorDiagonal = m_nColorCopy;
			}
			else if (m_rectIner.PtInRect(point))
			{
				m_nColorDiagonal = m_mouseOnColor;
				m_nROIColor = m_mouseOnColor;
			}
			else
			{
				m_nColorDiagonal = m_nColorCopy;
				m_nROIColor = m_nColorCopy;
			}
		}

	}
}

BOOL CRectangleROI::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return TRUE;
}

BOOL CRectangleROI::OnPreMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pWndParent && m_pWndParent->GetSafeHwnd())
	{
		m_bImgZoom = true;
	}
	return TRUE;
}

void CRectangleROI::OnUpdateRectShow()
{
	if (m_bImgZoom)
	{
		m_bImgZoom = false;
		if (m_pWndParent && m_pWndParent->GetSafeHwnd())
		{
			CRect rect;
			cv::Rect2d cvRect;
			m_pWndParent->GetClientRect(&rect);
			double dScale = m_pWndParent->GetScale();
			cv::Point2d centerImage = m_pWndParent->GetWndCenterImgPos();
			cvRect.x = (m_rectImgZoom.x - centerImage.x)*dScale + rect.Width()*1.0 / 2;
			cvRect.y = (m_rectImgZoom.y - centerImage.y)*dScale + rect.Height()*1.0 / 2;
			cvRect.width = m_rectImgZoom.width*dScale;
			cvRect.height = m_rectImgZoom.height*dScale;
			SetRectangleSize(cvRect);
			m_pWndParent->OnROIChange(m_nROIType,m_rect);
		}
	}
}

void CRectangleROI::OnMouseLeave()
{
	m_bLbtnDown = false;
	m_LbtnDownPoint.x = -1;
	m_LbtnDownPoint.y = -1;
}

void CRectangleROI::OnLButtonDown(CPoint point)
{
	if (m_rectLeft.PtInRect(point))
	{
		m_bLbtnDown = true;
		m_nDragType = ROI_RECT_DRAG_LEFT;
	}
	else if (m_rectTop.PtInRect(point))
	{
		m_bLbtnDown = true;
		m_nDragType = ROI_RECT_DRAG_TOP;
	}
	else if (m_rectRight.PtInRect(point))
	{
		m_bLbtnDown = true;
		m_nDragType = ROI_RECT_DRAG_RIGHT;
	}
	else if (m_rectBottom.PtInRect(point))
	{
		m_bLbtnDown = true;
		m_nDragType = ROI_RECT_DRAG_BOTTOM;
	}
	else if (m_rectIner.PtInRect(point))
	{
		m_bLbtnDown = true;
		m_nDragType = ROI_RECT_DRAG_INNER;
	}
	else
	{
		m_bLbtnDown = false;
		m_nDragType = ROI_RECT_DRAG_NULL;
	}
	if (m_bLbtnDown)
	{
		m_LbtnDownPoint = point;
	}
}

void CRectangleROI::OnLButtonUp(CPoint point)
{
	m_bLbtnDown = false;
	m_LbtnDownPoint.x = -1;
	m_LbtnDownPoint.y = -1;
}

void CRectangleROI::OnMouseMoveAttachImage()
{
	if (m_pWndParent&& m_pWndParent->GetSafeHwnd())
	{
		m_bImgZoom = true;//将该标志置为true就可以实现ROI跟着图像跑功能
	}
}

void CRectangleROI::DrawROI(CDC * pDC)
{
	CRect rect;
	rect.left = (LONG)m_rect.x;
	rect.right = (LONG)(m_rect.x + m_rect.width);
	rect.top = (LONG)m_rect.y;
	rect.bottom = (LONG)(m_rect.y + m_rect.height);

	CPen pen(PS_SOLID, m_nLineWidth, m_nROIColor);
	CPen* pOldpen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.left, rect.top);
	pDC->SelectObject(pOldpen);
	pen.DeleteObject();
}

bool CRectangleROI::IsPointIner(CPoint point)
{
	if (m_rectLeft.PtInRect(point))
	{
		return true;
	}
	if (m_rectTop.PtInRect(point))
	{
		return true;
	}
	if (m_rectRight.PtInRect(point))
	{
		return true;
	}
	if (m_rectBottom.PtInRect(point))
	{
		return true;
	}
	if (m_rectIner.PtInRect(point))
	{
		return true;
	}
	return false;
}

BOOL CRectangleROI::OnSetCursor()
{
	if (!m_bLbtnDown)
	{
		CPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_pWndParent->m_hWnd, &point);

		if (m_rectIner.PtInRect(point))
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
		}
		else if(m_rectTop.PtInRect(point)|| m_rectBottom.PtInRect(point))
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
		}
		else if (m_rectLeft.PtInRect(point) || m_rectRight.PtInRect(point))
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CRectangleROI::OnSize(UINT nType, int cx, int cy)
{
	if (m_pWndParent&& m_pWndParent->GetSafeHwnd())
	{
		m_bImgZoom = true;
	}
}

void CRectangleROI::SetROIColor(COLORREF color)
{
	CROIBase::SetROIColor(color);
	m_nColorDiagonal = color;
}

void CRectangleROI::SetROISelected(bool bSelected)
{
	CROIBase::SetROISelected(bSelected);
	if (m_bROISelect)
	{
		m_nColorDiagonal = CROIBase::m_selectROIColor;
	}
	else
	{
		m_nColorDiagonal = m_nColorCopy;
	}
}

void CRectangleROI::SetRectImgZoom(cv::Rect rect)
{
	m_rectImgZoom = rect;
}

void CRectangleROI::UpdateRectImgZoom(cv::Rect2f cvRect)
{
	if (NULL != m_pWndParent)
	{
		CRect rect;
		m_pWndParent->GetClientRect(&rect);
		double dScale = m_pWndParent->GetScale();
		cv::Point2f centImage = m_pWndParent->GetWndCenterImgPos();

		cv::Rect2d ImgRect;
		ImgRect.x = (cvRect.x - rect.Width()*1.0 / 2) / dScale + centImage.x;
		ImgRect.y = (cvRect.y - rect.Height()*1.0 / 2) / dScale + centImage.y;
		ImgRect.width = cvRect.width / dScale;
		ImgRect.height = cvRect.height / dScale;

		m_rectImgZoom = ImgRect;
	}
}
