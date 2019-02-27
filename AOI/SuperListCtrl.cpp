#include "stdafx.h"
#include "SuperListCtrl.h"


#ifdef DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

#define ITEM_HEIGHT  (18)

//CItemEdit
IMPLEMENT_DYNAMIC(CItemEdit,CEdit);

CItemEdit::CItemEdit()
{

}

CItemEdit::~CItemEdit()
{

}

BEGIN_MESSAGE_MAP(CItemEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()
void CItemEdit::EditBegin(LPCTSTR lpszInitText, int nItem, int nSubItem, const RECT & rect, const POINT & point)
{
	m_strInitText.SetString(lpszInitText);
	m_nItem = nItem;
	m_nSubItem = nSubItem;
	m_nLastChar = VK_ESCAPE;

	SetWindowText(lpszInitText);
	MoveWindow(&rect, FALSE);
	ShowWindow(SW_SHOW);
	SetFocus();
	SetSel(0, -1);
}

void CItemEdit::EditEnd()
{
	CString strText;
	GetWindowText(strText);

	EXTEND_EDIT_NOTIFY ipe;
	ipe.m_hDr.hwndFrom = GetSafeHwnd();
	ipe.m_hDr.idFrom = GetDlgCtrlID();
	ipe.m_hDr.code = NM_RETURN;
	ipe.m_lPcstrText = (LPCTSTR)strText;
	ipe.m_nItem = m_nItem;
	ipe.m_nSubItem = m_nSubItem;
	ipe.m_nLastChar = m_nLastChar;

	CWnd *pOwner = GetOwner();
	if (pOwner && !pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&ipe))
	{
		m_strInitText.Empty();
		m_nItem = 0;
		m_nSubItem = 0;
		m_nLastChar = VK_ESCAPE;

		ShowWindow(SW_HIDE);
		SetWindowText(_T(""));
		MoveWindow(CRect(), FALSE);
	}

}

BOOL CItemEdit::Create(DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	if (!CEdit::Create(dwStyle | WS_BORDER | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, rect, pParentWnd, nID))
	{
		return FALSE;
	}
	return TRUE;
}

void CItemEdit::OnKillFocus(CWnd * pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	m_nLastChar = VK_RETURN;
	EditEnd();
}

int CItemEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	LOGFONT lFont;
	lFont.lfOutPrecision = OUT_STROKE_PRECIS;
	lFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	lFont.lfQuality = DRAFT_QUALITY;
	lFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	lFont.lfHeight = 12;
	lFont.lfWidth = 0;
	lFont.lfEscapement = 0;
	lFont.lfOrientation = 0;
	lFont.lfWeight = FW_NORMAL;
	lFont.lfItalic = FALSE;
	lFont.lfUnderline = FALSE;
	lFont.lfStrikeOut = FALSE;
	lFont.lfCharSet = GB2312_CHARSET;
	_tcscpy_s(lFont.lfFaceName, _T("Ms Shell Dlg"));
	m_font.CreateFontIndirect(&lFont);
	this->SetFont(&m_font);
	return 0;
}

void CItemEdit::OnDestroy()
{
	if (m_font.m_hObject)
	{
		m_font.DeleteObject();

	}
	CEdit::OnDestroy();
}

BOOL CItemEdit::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}

//CItemButton

IMPLEMENT_DYNAMIC(CItemButton, CButton)
CItemButton::CItemButton()
{

}

CItemButton::~CItemButton()
{

}

BOOL  CItemButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CButton::Create(lpszCaption, dwStyle | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, pParentWnd, nID))
	{
		return FALSE;
	}
	return TRUE;
}

void CItemButton::EditBegin(LPCTSTR lpszInitText, int iItem, int iSubItem, const RECT& rect, const POINT& point)
{
	m_strInitText.SetString(lpszInitText);
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_nLastChar = VK_ESCAPE;

	SetWindowText(lpszInitText);
	MoveWindow(&rect, FALSE);

	ShowWindow(SW_SHOW);
	SetFocus();

}

void CItemButton::EditEnd()
{
	EXTEND_BUTTON_NOTIFY ipl;
	ipl.hdr.hwndFrom = GetSafeHwnd();
	ipl.hdr.idFrom = GetDlgCtrlID();
	ipl.hdr.code = NM_RETURN;
	ipl.iItem = m_iItem;
	ipl.iSubItem = m_iSubItem;
	ipl.nLastChar = m_nLastChar;

	ShowWindow(SW_HIDE);

	CWnd *pOwner = GetOwner();
	if (pOwner && !pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&ipl))
	{

		m_iItem = 0;
		m_iSubItem = 0;
		m_nLastChar = VK_ESCAPE;


		MoveWindow(CRect(), FALSE);
	}


}

BEGIN_MESSAGE_MAP(CItemButton, CButton)

	ON_CONTROL_REFLECT(BN_CLICKED, &CItemButton::OnBnClicked)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CItemButton::OnBnClicked()
{
	ShowWindow(SW_HIDE);
	m_nLastChar = VK_RETURN;
	EditEnd();
}

void CItemButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);

}

//CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
	: m_nItem(-1)
	, m_nSubItem(-1)
	, m_bHighLight(FALSE)
	, m_bFocus(FALSE)
	, m_ShowType(SUPER_LIST_CTRL_VALID)
	, m_pListerner(NULL)
	, m_bDisableDivider(true)
{

}

CListCtrlEx::~CListCtrlEx()
{

}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_EX(HDN_DIVIDERDBLCLICKA, 0, OnHeaderDividerDblClick)
	ON_NOTIFY_EX(HDN_DIVIDERDBLCLICKW, 0, OnHeaderDividerDblClick)
	ON_NOTIFY_EX(HDN_BEGINTRACKA, 0, OnHeaderBeginResize)
	ON_NOTIFY_EX(HDN_BEGINTRACKW, 0, OnHeaderBeginResize)
	ON_NOTIFY(NM_RETURN, EXTEND_IDC_INPLACE_EDIT, OnEndInplaceEdit)
	ON_NOTIFY(NM_RETURN, EXTEND_IDC_INPLACE_LIST, OnEndInplaceList)
	ON_NOTIFY(NM_RETURN, EXTEND_IDC_INPLACE_BUTTON, OnEndInplaceButton)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

void CListCtrlEx::GetSelItem(int& nItem, int& nSubItem)
{
	nItem = m_nItem;
	nSubItem = m_nSubItem;
}

void CListCtrlEx::SetDisableDivider(bool bDisable)
{
	m_bDisableDivider = bDisable;
}

void CListCtrlEx::HandleItemClick(int nItem, int nSubItem, eSUPER_LIST_CTRL_TYPE eShowType, CStringArray& strArray, CString strVal)
{
	m_ShowType = eShowType;
	CRect rect;
	GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rect);
	if (m_ShowType == SUPER_LIST_CTRL_EDIT)
	{
		if (m_edtItemEdit.GetSafeHwnd())
		{
			SetItemText(nItem, nSubItem, strVal);
			m_edtItemEdit.EditBegin(strVal, nItem, nSubItem, rect);
		}
	}
	else if (m_ShowType == SUPER_LIST_CTRL_BUTTON)
	{
		if (m_ButtonItem.GetSafeHwnd())
		{
			SetItemText(nItem, nSubItem, strVal);
			CRect rectButton = rect;
			rectButton.left = rectButton.right - rectButton.Height() - 5;
			m_ButtonItem.EditBegin(_T("..."), nItem, nSubItem, rectButton);
		}
	}
	else if (m_ShowType == SUPER_LIST_CTRL_LIST_BOX)
	{
		/*if (m_ListItem.GetSafeHwnd())
		{
			CRect rcItem;
			GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rcItem);
			CRect rcList;
			if (GetListRect(
				rcList, rcItem,
				GetListWidth(&m_ListItem, strArray),
				GetListHeight(&m_ListItem, strArray)
			))
			{
				rcList.OffsetRect(3, 0);
				int nCurs = 0;
				for (int i = 0; i < strArray.GetSize(); i++)
				{
					if (strArray.GetAt(i) == strVal)
					{
						nCurs = i;
						break;
					}
				}
				m_ListItem.EditBegin(
					strArray, nCurs, nItem, nSubItem, rcList
				);
			}
		}*/
	}

}


void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bFocus = TRUE;

	LVHITTESTINFO  lvhit;
	lvhit.pt = point;
	int nTtem = SubItemHitTest(&lvhit);

	CRect rect;

	if (nTtem != -1 && (lvhit.flags & LVHT_ONITEM))
	{
		CListCtrl::OnLButtonDown(nFlags, point);
		if (m_nItem != -1 && m_nSubItem != -1)
		{
			GetSubItemRect(m_nItem, m_nSubItem, LVIR_LABEL, rect);
			rect.InflateRect(2, 2, 2, 2);
			InvalidateRect(&rect);
		}
		if (m_bHighLight && m_nItem == lvhit.iItem && m_nSubItem == lvhit.iSubItem && m_nSubItem != 0)
		{
			if (m_nSubItem != 0 && m_nSubItem != -1 && m_nItem != -1)
			{
				if (m_pListerner)
				{
					//m_pListerner->OnLButtonDown(m_nItem, m_nSubItem);
				}
			}

			return;
		}
		else
		{
			m_nItem = lvhit.iItem;
			m_nSubItem = lvhit.iSubItem;
			if (m_nSubItem == 0)
			{
				//m_pListerner->OnEndInplace(m_nItem, m_nSubItem, "", -1);
			}
			m_bHighLight = TRUE;
		}
		if (m_nItem != -1 && m_nSubItem != -1)
		{
			GetSubItemRect(m_nItem, m_nSubItem, LVIR_LABEL, rect);
			rect.InflateRect(2, 2, 2, 2);
			InvalidateRect(&rect);
		}
	}
	else
	{
		if (m_edtItemEdit.m_hWnd == NULL)
		{
			m_bHighLight = FALSE;
		}
		if (m_nItem != -1 && m_nSubItem != -1)
		{

			GetSubItemRect(m_nItem, m_nSubItem, LVIR_LABEL, rect);
			rect.InflateRect(2, 2, 2, 2);
			InvalidateRect(&rect);
		}
	}
}


BOOL  CListCtrlEx::OnHeaderDividerDblClick(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = TRUE; // disable tracking 
	BOOL bRet = m_bDisableDivider ? TRUE : FALSE;
	return bRet;
}

BOOL CListCtrlEx::OnHeaderBeginResize(UINT, NMHDR* pNmhdr, LRESULT* pResult)
{
	*pResult = TRUE; // disable tracking 
	BOOL bRet = m_bDisableDivider ? TRUE : FALSE;
	return bRet;
}

void CListCtrlEx::InitChildCtrl()
{
	if (!m_edtItemEdit.Create(0, CRect(0, 0, 0, 0), this, EXTEND_IDC_INPLACE_EDIT))
		return;

	//if (!m_ListItem.Create(0, CRect(0, 0, 0, 0), this, EXTEND_IDC_INPLACE_LIST))
		//return;


	if (!m_ButtonItem.Create(_T("设置"), 0, CRect(0, 0, 0, 0), this, EXTEND_IDC_INPLACE_BUTTON))
		return;


}


int CListCtrlEx::GetListWidth(CListBox *pWnd, const CStringArray& ar)
{
	CClientDC dc(pWnd);
	int nSave = dc.SaveDC();
	dc.SelectObject(pWnd->GetFont());
	int nWidth = 0;
	for (int i = 0; i < ar.GetCount(); i++)
		nWidth = max(nWidth, (int)(dc.GetTextExtent(ar[i]).cx));
	nWidth += dc.GetTextExtent(_T("0")).cx;
	nWidth += ::GetSystemMetrics(SM_CXVSCROLL);
	dc.RestoreDC(nSave);
	return nWidth;
}

int CListCtrlEx::GetListHeight(CListBox *pWnd, const CStringArray& ar)
{
	CClientDC dc(pWnd);
	int nSave = dc.SaveDC();
	dc.SelectObject(pWnd->GetFont());
	int nHeight = dc.GetTextExtent(_T("0")).cy;
	dc.RestoreDC(nSave);
	return nHeight * (int)ar.GetSize() + 2;
}

BOOL CListCtrlEx::GetListRect(RECT& rcList, const RECT& rcItem, int iWidth, int iHeight)
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	rcClient.top += ITEM_HEIGHT;
	::InflateRect(&rcClient, -3, -3);

	if (rcClient.right - rcClient.left <= 8 || rcClient.bottom - rcClient.top <= 8)
	{
		return FALSE;
	}
	if ((rcItem.right - rcItem.left) > iWidth)
	{
		iWidth = (rcItem.right - rcItem.left);
	}
	::SetRect(&rcList,rcItem.left, rcItem.bottom,rcItem.left + iWidth, rcItem.bottom + iHeight);
	if (rcList.bottom > rcClient.bottom)
	{
		::OffsetRect(&rcList, 0, rcClient.bottom - rcList.bottom);
	}
	if (rcList.top < rcClient.top)
	{
		rcList.top = rcClient.top;
	}
	if (rcList.right > rcClient.right)
	{
		::OffsetRect(&rcList, rcClient.right - rcList.right, 0);
	}
	if (rcList.left < rcClient.left)
	{
		rcList.left = rcClient.left;
	}
	return TRUE;
}


void CListCtrlEx::OnPaint()
{
	CListCtrl::OnPaint();
}


void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{

	NMLVCUSTOMDRAW* pNMLVCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;

	*pResult = CDRF_DODEFAULT;

	if (pNMLVCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pNMLVCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pNMLVCustomDraw->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		int iItem = (int)pNMLVCustomDraw->nmcd.dwItemSpec;
		int iSubItem = pNMLVCustomDraw->iSubItem;
		if (iSubItem == 0)
		{
			return;
		}

		CDC* pDC = CDC::FromHandle(pNMLVCustomDraw->nmcd.hdc);

		CString strItemText = GetItemText(iItem, iSubItem);
		CRect rcItem, rcText;
		GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rcItem);
		rcText = rcItem;
		COLORREF colbk = this->GetBkColor();
		pDC->FillSolidRect(&rcItem, colbk);
		CSize size = pDC->GetTextExtent(strItemText);
		if (strItemText == _T(""))
		{
			size.cx = 41;
		}

		COLORREF crOldTextColor = pDC->GetTextColor();

		if (m_bFocus)
		{

			if ((m_nItem == iItem) && (m_nSubItem == iSubItem)
				&& m_nSubItem != 0)
			{
				if (m_bHighLight)
				{
					pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
					pDC->FillSolidRect(&rcText, ::GetSysColor(COLOR_HIGHLIGHT));
				}
				pDC->DrawFocusRect(&rcText);
			}
		}

		rcItem.left += 6;
		pDC->DrawText(strItemText, &rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOCLIP);

		pDC->SetTextColor(crOldTextColor);
		*pResult = CDRF_SKIPDEFAULT;// We've painted everything.
	}
}


void CListCtrlEx::OnEndInplaceEdit(NMHDR *pNotifyStruct, LRESULT * result)
{
	*result = FALSE;
	//EXTEND_PEDIT_NOTIFY en = (EXTEND_PEDIT_NOTIFY)pNotifyStruct;

	//if (::IsWindow(GetSafeHwnd()) && en->nLastChar == VK_RETURN)
	{
		CString strTmp = _T("");
		m_edtItemEdit.GetWindowText(strTmp);
		int nItem = m_nItem;
		int nSubItem = m_nSubItem;
		this->SetItemText(nItem, nSubItem, strTmp);
		if (m_pListerner)
		{
			//m_pListerner->OnEndInplace(nItem, nSubItem, strTmp, m_ShowType);
		}
	}
}

void  CListCtrlEx::OnEndInplaceButton(NMHDR *pNotifyStruct, LRESULT * result)
{
	*result = FALSE;
	//EXTEND_PEDIT_NOTIFY en = (EXTEND_PEDIT_NOTIFY)pNotifyStruct;

	//if (::IsWindow(GetSafeHwnd()) && en->nLastChar == VK_RETURN)
	{
		CString strTmp = _T("");
		int nItem = m_nItem;
		int nSubItem = m_nSubItem;
		if (m_pListerner)
		{
			//m_pListerner->OnEndInplace(nItem, nSubItem, strTmp, m_ShowType);
		}
	}
}

void CListCtrlEx::OnEndInplaceList(NMHDR *pNotifyStruct, LRESULT * result)
{
	*result = FALSE;
	//EXTEND_PLIST_NOTIFY ln = (EXTEND_PLIST_NOTIFY)pNotifyStruct;

	//if (::IsWindow(GetSafeHwnd()) && ln->nLastChar == VK_RETURN)
	{
		//EXTEND_PLIST_NOTIFY ln = (EXTEND_PLIST_NOTIFY)pNotifyStruct;
		//if (ln->nLastChar == VK_RETURN)
		{
			CString strTmp = _T("");
			//int nCurItem = m_ListItem.GetCurSel();
			//if (nCurItem == -1)
			//{
			//	return;
			//}
			//m_ListItem.GetText(nCurItem, strTmp);
			//int nItem = m_nItem;
			//int nSubItem = m_nSubItem;
			//this->SetItemText(nItem, nSubItem, strTmp);
			//if (m_pListerner)
			//{
			//	m_pListerner->OnEndInplace(nItem, nSubItem, strTmp, m_ShowType);
			//}
		}
	}
}

//void CListCtrlEx::PreSubclassWindow()
//{
//	CListCtrl::PreSubclassWindow();
//	InitChildCtrl();
//}


void CListCtrlEx::SetListener(ICListCtrlExListener* pListener)
{
	if (NULL != pListener)
	{
		m_pListerner = pListener;
	}
}

void CListCtrlEx::DeleteListener()
{
	m_pListerner = NULL;
}

void CListCtrlEx::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);
	//CRect rect;
	//GetWindowRect(&rect);
	//CPoint point;
	//::GetCursorPos(&point);
	//if(!rect.PtInRect(point) && GetParent()->GetFocus() != NULL)
	//{
	//	m_iItem = -1;
	//	m_iSubItem = -1;
	//	m_bFocus = FALSE;
	//	m_bHighLight = FALSE;
	//	Invalidate();
	//}
}

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd)
{
   // TODO:
}

int CListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}	
	return 0;
}

void CListCtrlEx::OnDestroy()
{
	if (m_edtItemEdit.GetSafeHwnd())
	{
		m_edtItemEdit.DestroyWindow();
	}

	/*if (m_ListItem.GetSafeHwnd())
	{
		m_ListItem.DestroyWindow();
	}*/
	CListCtrl::OnDestroy();
}
void CListCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 20;
}

void CListCtrlEx::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
