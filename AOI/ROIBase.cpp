#include "stdafx.h"
#include "ROIBase.h"

COLORREF	CROIBase::m_selectROIColor = RGB(255,255,255);
int         CROIBase::m_selectROILineWidth = 2;
CROIBase::CROIBase(CImgShowWnd* ptrParent, eROI_TYPE nROIType) :
	m_nROIType(nROIType),
	m_pWndParent(ptrParent),
	m_nROIColor(RGB(0,255,0)),
	m_nColorCopy(RGB(0, 255,0)),
	m_mouseOnColor(RGB(255, 0, 0)),
	m_nLineWidth(2),
	m_nSelInterval(5),
	m_bROISelect(false),
	m_bLbtnDown(false),
	m_LbtnDownPoint(CPoint(-1, -1)),
	m_bEnableSel(true)
{

}


CROIBase::~CROIBase()
{
}

void CROIBase::DrawROI(CDC * ptrDC)
{
	//TODO
}

bool CROIBase::IsPointIner(CPoint point)
{
	//TODO
	return 0;
}

void CROIBase::OnLButtonDown(CPoint point)
{
	//TODO
}

void CROIBase::OnLButtonUp(CPoint point)
{
	//TODO
}

void CROIBase::OnMouseMove(CPoint point)
{
	//TODO
}

void CROIBase::OnMouseLeave()
{
	//TODO
}

BOOL CROIBase::OnSetCursor()
{
	//TODO
	return TRUE;
}

BOOL CROIBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//TODO
	return TRUE;
}

BOOL CROIBase::OnPreMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//TODO
	return TRUE;
}

void CROIBase::OnMouseMoveAttachImage()
{
	//TODO
}

void CROIBase::OnUpdateRectShow()
{
	//TODO
}

void CROIBase::OnSize(UINT nType, int cx, int cy)
{
	//TODO
}

void CROIBase::SetROIColor(COLORREF color)
{
	m_nROIColor = color;
	m_nColorCopy = m_nROIColor;
}

void CROIBase::SetROIMouseOnColor(COLORREF color)
{
	m_mouseOnColor = color;
}

void CROIBase::SetROISelected(bool bSelected)
{
	if (!m_bEnableSel)
	{
		return;
	}
	m_bROISelect = bSelected;
	if (m_bROISelect)
	{
		m_nLineWidth = CROIBase::m_selectROILineWidth;
		m_nROIColor = CROIBase::m_selectROIColor;
	}
	else
	{
		m_nLineWidth = 1;
		m_nROIColor = m_nColorCopy;
	}
}

void CROIBase::SetRectImgZoom(cv::Rect rect)
{
	//TODO
}

void CROIBase::SetROILineWidth(int nLineWidth)
{
	m_nLineWidth = nLineWidth;
}

void CROIBase::SetROIPixInterval(int nInterval)
{
	m_nSelInterval = nInterval;
}

COLORREF CROIBase::GetROIColor()
{
	return m_nROIColor;
}

int CROIBase::GetROILineWidth()
{
	return m_nLineWidth;
}

eROI_TYPE CROIBase::GetROIType()
{
	return m_nROIType;
}

bool CROIBase::IsROISelected()
{
	return m_bROISelect;
}

void CROIBase::SetROIEnableSelected(bool bEnableSel)
{
	m_bEnableSel = bEnableSel;
}
