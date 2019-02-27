#pragma once
#include "afxwin.h"

class CSystemLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemLoginDlg)

public:
	CSystemLoginDlg(CWnd* pParent = NULL); 
	virtual ~CSystemLoginDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SYS_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
private:
	CButton    m_btnLogin;
	CLabelCtrl m_btnClose;
	CLabelCtrl m_labelLogo;
	CFont      m_smallFont;
	CFont      m_txtFont;
	CFont      m_titleFont;
	CEdit      m_editPassword;
	CEdit      m_editUser;
	CPen       m_penFrame;
	CPen       m_penTitle;

private:
	BOOL PreTranslateMessage(MSG * pMsg);
	BOOL OnInitDialog();
	void CreateLogo(CRect& rcClient);
	void CreateCloseButton(CRect& rcClient);
	void CreateFunctionButton(CRect& rcClient);
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void DisplayMessage(CString sMsg);
public:
	afx_msg void OnStnClickedClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeEditUser();
};
