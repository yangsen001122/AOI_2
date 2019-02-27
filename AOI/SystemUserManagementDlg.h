#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SqliteDB.h"
#include "utils.h"
#define BUFFERSIZE 1024
const int MAXROWNUM = 100;
typedef struct
{
	const char* username;
	const char* key;
	const char* level;
}STORE_DATA;

// CSystemUserManagementDlg 对话框

class CSystemUserManagementDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemUserManagementDlg)

public:
	CSystemUserManagementDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSystemUserManagementDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SYS_USER_MANAGEMENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CLabelCtrl m_btnClose;
	CLabelCtrl m_labelLogo;
	CLabelCtrl m_labelUser;
	CLabelCtrl m_labelKey;
	CLabelCtrl m_labelLevel;
	CLabelCtrl m_btnCreate;
	CLabelCtrl m_btnDelete;
	CLabelCtrl m_btnUpdate;
	CFont      m_smallFont;
	CFont      m_txtFont;
	CFont      m_titleFont;
	CPen       m_penTitle;
	CListCtrl  m_listUserMgmtList;
	CEdit      m_editUser;
	CEdit      m_editPassword;
	CComboBox  m_cboxLevel;
	QUERY_USER_RESULT selectedUser;
	CString    m_strUserLevel;
	unsigned int m_nConsume;
	bool         m_bHitItem;
protected:
	STORE_DATA		m_StoreData[BUFFERSIZE];
	QUERY_USER_RESULT      m_QueryResult[MAXROWNUM];
private:
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void CreateUiWidget();
	void AddData();

public:
	CppSQLite3Query  QueryResult;
public:
	virtual BOOL OnInitDialog();
	void UpdateList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnStnClickedClose();
	afx_msg void OnBnClickedButtonUserMgmtCreate();
	afx_msg void OnBnClickedButtonUserMgmtUpdate();
	afx_msg void OnBnClickedButtonUserMgmtDelete();
	afx_msg void OnNMClickListUserMgmtList(NMHDR *pNMHDR, LRESULT *pResult);
};
