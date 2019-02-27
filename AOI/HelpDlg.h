#pragma once
#include "afxwin.h"

// CHelpDlg 对话框

class CHelpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHelpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HELP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CLabelCtrl m_btnClose;
	CLabelCtrl m_labelLogo;
	CFont      m_smallFont;
	CFont      m_txtFont;
	CPen       m_penFrame;
	CPen       m_penTitle;
	HCURSOR	   m_hCursor;
	RECT	   m_StaticRect;
	bool	   m_focusFlag;

	void CreateLogo(CRect& rcClient);
	void CreateCloseButton(CRect& rcClient);
	void CreateFunctionButton(CRect& rcClient);
	void InitializeCtrl(int nCtrlID, CRect& rcPos, CString nodeName, int nStrID = -1, bool bSmallFont = false);

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnStnClickedClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
