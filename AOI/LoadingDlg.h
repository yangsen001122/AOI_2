#pragma once
#include "afxcmn.h"


// CLoadingDlg �Ի���

class CLoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadingDlg)

public:
	CLoadingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoadingDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOADING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	void SetLoadingTime(int nloadingTime);
	CProgressCtrl m_CProg;
	int           m_nProgressPos;
	int           m_nProgressMax;
	int           m_nProgressMin;
	int           m_nProgressStep;
	int 	      m_nLoadingTime;
};
