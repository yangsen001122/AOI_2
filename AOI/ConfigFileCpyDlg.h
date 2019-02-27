#pragma once
#include "LabelCtrl.h"
#include "afxwin.h"
#include "TypeDef.h"

const int TIMER_ID_CONFIG_FILE_DLG = 191919;

class ConfigFileCpyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConfigFileCpyDlg)

public:
	ConfigFileCpyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ConfigFileCpyDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONFIGFILE };
#endif
private:
	bool       m_bOpRslt;
	CFont      m_txtFont;
	CFont      m_smallFont;
	CBitmap    m_bgImg;
	CLabelCtrl m_LoadBtn;
	CLabelCtrl m_SaveBtn;
	WindowBackgroundBrushHelper m_bgBrushHelper;
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

private:
	void DisplayPromptMessage(CString sMsg, bool bOpRslt);

public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	CButton m_BtnloadConfig;
	CButton m_BtnSaveConfig;
	afx_msg void OnBnClickedImportConfigFile();
	afx_msg void OnBnClickedExportConfigFile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
