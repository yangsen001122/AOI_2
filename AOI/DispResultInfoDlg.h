#pragma once
#include "afxwin.h"

class CDispResultInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDispResultInfoDlg)

public:
	CDispResultInfoDlg(CWnd* pParent = NULL);   
	virtual ~CDispResultInfoDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECTS_DISPLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	CListBox m_listBox;
	afx_msg void OnLbnDblclkListBoxDefectDetail();
};
