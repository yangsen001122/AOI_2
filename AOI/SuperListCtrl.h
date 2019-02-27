#pragma once
#include "TypeDef.h"
#define EXTEND_IDC_INPLACE_EDIT   (101)
#define EXTEND_IDC_INPLACE_LIST   (102)
#define EXTEND_IDC_INPLACE_BUTTON (103)

struct _EXTEND_EDIT_NOTIFY
{
	NMHDR    m_hDr;
	LPCTSTR   m_lPcstrText;
	int      m_nItem;
	int      m_nSubItem;
	UINT     m_nLastChar;
};

typedef _EXTEND_EDIT_NOTIFY EXTEND_EDIT_NOTIFY;
typedef _EXTEND_EDIT_NOTIFY *EXTEND_PDIT_NOTIFY;

//Super function-Edit
class CItemEdit:public CEdit
{
	DECLARE_DYNAMIC(CItemEdit)
public:
	CItemEdit();
public:
	CString  m_strInitText;
	int      m_nItem;
	int      m_nSubItem;
	UINT     m_nLastChar;
public:
	~CItemEdit();
	void EditBegin(LPCTSTR lpszInitText, int nItem, int nSubItem, const RECT& rect, const POINT& point = POINT());
	void EditEnd();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
private:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
};

//Super function-ListBox
//TODO

//Super function-Button
struct _EXTEND_BUTTON_NOTIFY
{
	NMHDR   hdr;
	int     iItem;
	int     iSubItem;
	UINT    nLastChar;
};

typedef _EXTEND_BUTTON_NOTIFY  EXTEND_BUTTON_NOTIFY;
typedef _EXTEND_BUTTON_NOTIFY* EXTEND_PBUTTON_NOTIFY;

class CItemButton : public CButton
{
	DECLARE_DYNAMIC(CItemButton)
public:
	CItemButton();
    ~CItemButton();
public:
	CString m_strInitText;
	int     m_iItem;
	int     m_iSubItem;
	UINT    m_nLastChar;
public:
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void EditBegin(LPCTSTR lpszInitText, int iItem, int iSubItem, const RECT& rect, const POINT& point = POINT());
	void EditEnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

//CListCtrlEx 

class ICListCtrlExListener;
class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	~CListCtrlEx();
public:
	void HandleItemClick(int nItem, int nSubItem, eSUPER_LIST_CTRL_TYPE eShowType, CStringArray& strArray, CString strValue);	
																													
	void SetListener(ICListCtrlExListener* pListener);
	void DeleteListener();
	void GetSelItem(int& nItem, int& nSubItem);
	void SetDisableDivider(bool bDisable);
protected:
	//virtual void PreSubClassWindow();
protected:
	afx_msg BOOL OnHeaderDividerDblClick(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHeaderBeginResize(UINT, NMHDR* pNmhdr, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnEndInplaceEdit(NMHDR *pNotifyStruct, LRESULT * result);
	afx_msg void OnEndInplaceList(NMHDR *pNotifyStruct, LRESULT * result);
	afx_msg void OnEndInplaceButton(NMHDR *pNotifyStruct, LRESULT * result);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	DECLARE_MESSAGE_MAP()

	void InitChildCtrl();						
	int GetListWidth(CListBox *pWnd, const CStringArray& ar);
	int GetListHeight(CListBox *pWnd, const CStringArray& ar);
	BOOL GetListRect(RECT& rcList, const RECT& rcItem, int iWidth, int iHeight);
private:
	bool m_bDisableDivider;		
	int  m_nSubItem;				
	int  m_nItem;			
	BOOL m_bHighLight;		
	BOOL m_bFocus;				

	eSUPER_LIST_CTRL_TYPE m_ShowType;		
	CItemEdit m_edtItemEdit;
	//CItemListBox m_ListItem;
	CItemButton m_ButtonItem;
	ICListCtrlExListener* m_pListerner;
public:

};