#include "stdafx.h"
#include "ComboBoxCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComboBoxCtrl::CComboBoxCtrl()
{
	m_bIsColor=FALSE;
	m_iTxtHeight = 14;
	m_iTxtWeight = FW_NORMAL;

}

CComboBoxCtrl::~CComboBoxCtrl()
{
}


BEGIN_MESSAGE_MAP(CComboBoxCtrl, CComboBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTILabel message handlers

void CComboBoxCtrl::OnPaint() 
{
	CComboBox::OnPaint();

	CPaintDC dc(this); // device context for painting
	dc.SetTextColor(m_fgColor);
	CRect rect;
	GetClientRect(&rect);		//获得控件客户区矩形大小
    dc.SetBkColor(m_bgColor);
	dc.FillSolidRect(0,0,rect.Width(),rect.Height(),m_bgColor);
}

VOID CComboBoxCtrl::SetColor(COLORREF fgColor,COLORREF bgColor)
{
	m_bIsColor = TRUE;
	m_fgColor = fgColor;
	m_bgColor = bgColor;	
}

VOID CComboBoxCtrl::SetTxtFont(int height,int weight)
{
	m_iTxtHeight=height;
	m_iTxtWeight=weight;
}

BOOL CComboBoxCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return CComboBox::OnEraseBkgnd(pDC);
}