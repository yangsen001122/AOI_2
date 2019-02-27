#pragma once

#include <string>

using namespace std;

class CCopyProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCopyProjectDlg)

public:
	CCopyProjectDlg(CString sSrcName,wstring& wstrRet,CWnd* pParent = NULL);  
	virtual ~CCopyProjectDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COPY_PROJECT};
#endif

	void InitializeCtrl(int nCtrlID, int nStrID = -1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_bgImg; // background image
	CFont m_txtFont;
	CString  m_sSourceName;
	wstring& m_wstrTargetName;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
