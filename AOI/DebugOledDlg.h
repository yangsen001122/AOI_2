#pragma once


// DebugOledDlg �Ի���

class DebugOledDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DebugOledDlg)

public:
	DebugOledDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DebugOledDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEBUG_OLED };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CRect rcScreenOled;
	CRect rcScreenOledClient;
	void DrawRgbImg(COLORREF picColor);
};
