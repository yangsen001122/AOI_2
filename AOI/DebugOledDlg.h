#pragma once


// DebugOledDlg 对话框

class DebugOledDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DebugOledDlg)

public:
	DebugOledDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DebugOledDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEBUG_OLED };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CRect rcScreenOled;
	CRect rcScreenOledClient;
	void DrawRgbImg(COLORREF picColor);
};
