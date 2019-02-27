#pragma once
#include "ui_decorator.h"

enum TEXT_ALIGH
{
	CENTER = 0,
	LEFT,
	RIGHT
};

class CLabelCtrl : public CStatic
{
public:
	CLabelCtrl();
	virtual ~CLabelCtrl();
	void SetTxtFont(int height,int weight, TEXT_ALIGH alignType = CENTER);
	void SetColor(COLORREF Color,COLORREF BgColor);
	void SetLabelImage(eIMAGE_TYPE imgType = IMG_INVALID);
	COLORREF m_Color;
	COLORREF m_BgColor;
	int      m_nID;
	CWnd*    m_ptrMainUi;
	void SetMainUiPtr(CWnd * ptrMainUi);
protected:
	CString  m_strText;
	BOOL	 m_bOver;
	BOOL	 m_bIsColor;	
	
	CFont m_font;
	int m_iTxtHeight;
	int m_iTxtWeight;
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
private:
	ATL::CImage* m_ptrImage;
	TEXT_ALIGH m_txtAlignType;
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};