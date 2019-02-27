#pragma once
#include "LabelCtrl.h"

// CMessageDlg 对话框

class CMessageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(int nStrID,WPARAM wParam, CWnd* pParent = NULL);//
	virtual ~CMessageDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MESSAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:

	CFont      m_smallFont;
	CFont      m_txtFont;
	CFont      m_titleFont;
	CPen       m_penFrame;
	CPen       m_penTitle;
	bool       m_MsgConfirm;
	bool       m_MsgChoose;
	int        m_StrId;
	CLabelCtrl m_labelMsg;
private:
	BOOL PreTranslateMessage(MSG * pMsg);
	BOOL OnInitDialog();
	void CreateFunctionButton(CRect& rcClient);
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void DisplayMessage(int nCtrlID);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
