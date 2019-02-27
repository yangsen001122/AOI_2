#pragma once
#include "ROIBase.h"
#include"TypeDef.h"
class CImgShowWnd;
class CRectangleROI :public CROIBase
{
public:
	CRectangleROI(CImgShowWnd* ptrParent, eROI_TYPE = ROI_RECTANGLE);
	~CRectangleROI();

	void GetRectangleSize(cv::Rect2f& rect);
	void SetRectangleSize(cv::Rect2f rect);
	void OnMouseMove(CPoint point);			
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL OnPreMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnUpdateRectShow();
	void OnMouseLeave();
	void OnLButtonDown(CPoint point);
	void OnLButtonUp(CPoint point);
	void OnMouseMoveAttachImage();
	void DrawROI(CDC* pDC);
	bool IsPointIner(CPoint point);
	BOOL OnSetCursor();
	void OnSize(UINT nType, int cx, int cy);
	void SetROIColor(COLORREF color);
	void SetROISelected(bool bSelected);
	void SetRectImgZoom(cv::Rect rect);  //设置选中的矩形框相对于图像坐标系的位置

protected:

	//其他
	virtual void UpdateRectImgZoom(cv::Rect2f cvRect);		

protected:
	bool		         m_bImgZoom;
	cv::Rect2d           m_rectImgZoom;						
	eROI_RECT_DRAG_TYPE	 m_nDragType;						
	COLORREF             m_nColorDiagonal;
	cv::Rect2d	         m_rect;
	CRect		         m_rectLeft;
	CRect		         m_rectTop;
	CRect		         m_rectRight;
	CRect		         m_rectBottom;
	CRect		         m_rectIner;
};

