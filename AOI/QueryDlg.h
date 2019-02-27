#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"



class CQueryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CQueryDlg)

public:
	CQueryDlg(CWnd* pParent = NULL);   
	virtual ~CQueryDlg();

	
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_QUERY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()
private:
	CLabelCtrl m_btnClose;
	CLabelCtrl m_labelLogo;
	CLabelCtrl m_labelTotalTxt;
	CLabelCtrl m_labelOkRateTxt;
	CLabelCtrl m_labelAvgTimeTxt;
	CLabelCtrl m_labelTotal;
	CLabelCtrl m_labelOkRate;
	CLabelCtrl m_labelAvgTime;
	CFont      m_smallFont;
	CFont      m_txtFont;
	CFont      m_titleFont;
	CPen       m_penFrame;
	CPen       m_penTitle;
	COLORREF  m_fgColor;       // foreground color;
	COLORREF  m_bgColor;       // Background color;
private:
	BOOL PreTranslateMessage(MSG * pMsg);
	BOOL OnInitDialog();
	void CreateLogo(CRect& rcClient);
	void CreateCloseButton(CRect& rcClient);
	void CreateFunctionButton(CRect& rcClient);
	void CreateQueryReportDisplayingWidget(CRect& rcClient);
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void DisplayMessage(CString sMsg);
public:
	afx_msg void OnStnClickedClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	CDateTimeCtrl m_dateCtrl2;
	CTime m_QueryBeginDate;
	CTime m_QueryBeginTime;
	CTime m_QueryEndDate;
	CTime m_QueryEndTime;
	CppSQLite3Query  QueryData;
	QUERY_COUNT_DATA  QueryCountData[1024];
private:
	CButton m_btnConfirm;
public:
	afx_msg void OnBnClickedButtonQuery();
	void Empty();
private:
	CListCtrl m_listQueryData;
	CButton m_btnCSV;
public:
	afx_msg void OnBnClickedButtonCsv();
	void ListAllProjectName();
	CDateTimeCtrl m_StartTime;
	CDateTimeCtrl m_EndDate;
	CDateTimeCtrl m_EndTime;
private:
	CEdit m_ReportTotal;
	CEdit m_ReportOkRate;
	CEdit m_AvgTime;
public:
	CComboBox m_cboxProj;
};