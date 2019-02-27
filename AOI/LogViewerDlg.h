#pragma once
#include "afxwin.h"


class CLogViewerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogViewerDlg)

public:
	CLogViewerDlg(CWnd* pParent = NULL);  
	virtual ~CLogViewerDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGVIEWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()
private:
	CBitmap    m_bgImg; // background image
	WindowBackgroundBrushHelper             m_bgBrushHelper;
	CLabelCtrl m_labelLogo;
	CLabelCtrl m_btnClose;
	CFont      m_smallFont;
	CFont      m_txtFont;
	CFont      m_titleFont;
	CFont       m_logFont;
	CPen       m_penFrame;
	CPen       m_penTitle;
	CListCtrl  m_logList;//log list 
	circular_buffer<EVENT_PACKAGE>   m_eventBuffer;
	CString      m_strCopyLog;

private:
	void DisplayBackgroundImage(CPaintDC& dc);
	BOOL OnInitDialog();
	void CreateLogo(CRect& rcClient);
	void CreateCloseButton(CRect& rcClient);
	void CreateFunctionButton(CRect& rcClient);
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void DisplayMessage(CString sMsg);
public:
	int m_nLevel;
	int m_nLogSource;
	CDateTimeCtrl m_LogDate;
	CDateTimeCtrl m_StartTime;
	CDateTimeCtrl m_EndTime;
	CTime m_QueryBeginDate;
	CTime m_QueryBeginTime;
	CTime m_QueryEndTime;
	afx_msg void OnStnClickedClose();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadioBtnLevelClicked();
	afx_msg void OnRadioBtnSourceClicked();
	afx_msg void OnBnClickedButtonConfirm();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	BOOL ShowLogList(wstring wstrLogMsg);
	void Empty();
	afx_msg void OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickListBoxLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCopylog();
	afx_msg void OnDtnDatetimechangeDatetimepickerStarttime(NMHDR *pNMHDR, LRESULT *pResult);
};
