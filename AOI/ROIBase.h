#pragma once
#include "ImgShowWndListener.h"
#include "TypeDef.h"
class CImgShowWnd;
class CROIBase
{
public:
	CROIBase(CImgShowWnd* ptrParent, eROI_TYPE nROIType = ROI_RECTANGLE);
    ~CROIBase();
	virtual  void DrawROI(CDC* ptrDC);
	virtual  bool IsPointIner(CPoint point);
	virtual  void OnLButtonDown(CPoint point);
	virtual  void OnLButtonUp(CPoint point);
	virtual  void OnMouseMove(CPoint point);
	virtual  void OnMouseLeave();
	virtual  BOOL OnSetCursor();
	virtual  BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual  BOOL OnPreMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual  void OnMouseMoveAttachImage();
	virtual  void OnUpdateRectShow();
	virtual  void OnSize(UINT nType, int cx, int cy);
	virtual  void SetROIColor(COLORREF color);
	virtual  void SetROIMouseOnColor(COLORREF color);
	virtual  void SetROISelected(bool bSelected);
	virtual  void SetRectImgZoom(cv::Rect rect);
	void  SetROILineWidth(int nLineWidth);		
	void  SetROIPixInterval(int nInterval);	
	COLORREF GetROIColor();						
	int  GetROILineWidth();					
	eROI_TYPE  GetROIType();
	bool IsROISelected();														
	void SetROIEnableSelected(bool bEnableSel);				
protected:
	static COLORREF	m_selectROIColor;				
	static int      m_selectROILineWidth;				
	CImgShowWnd*    m_pWndParent;				
	eROI_TYPE       m_nROIType;
	COLORREF        m_nROIColor;							 
	COLORREF        m_nColorCopy;						 
	COLORREF        m_mouseOnColor;					 
	int             m_nLineWidth;								 
	int             m_nSelInterval;								 
	bool            m_bROISelect;									 
	bool            m_bEnableSel;								 
	bool            m_bLbtnDown;							 
	CPoint          m_LbtnDownPoint;							 
};

