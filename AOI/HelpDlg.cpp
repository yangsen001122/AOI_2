// HelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AOI.h"
#include "HelpDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"
#include <shellapi.h>

extern ConfigFileManager gConfigFileMgr;
extern CSystemManager    gSystemMgr;

IMPLEMENT_DYNAMIC(CHelpDlg, CDialogEx)

CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_HELP, pParent)
{
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	float fRatio = rcScreen.Width() / 1920.0f;

	m_smallFont.CreateFont(int(26 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_txtFont.CreateFont(int(30 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_penFrame.CreatePen(PS_SOLID, 6, RGB(200, 220, 250));
	m_penTitle.CreatePen(PS_SOLID, 3, RGB(4, 40, 94));
	m_focusFlag = false;
}

void CHelpDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, CString nodeName,int nStrID, bool bSmallFont)
{
	CWnd* pCtrl = (CWnd*)GetDlgItem(nCtrlID);
	if (pCtrl != nullptr)
	{
		if (bSmallFont)
		{
			pCtrl->SetFont(&m_smallFont);
		}
		else
		{
			pCtrl->SetFont(&m_txtFont);
		}
		pCtrl->MoveWindow(&rcPos);
		CString str;
		if (nStrID != -1)
		{
			str.LoadStringW(nStrID);
		}
		else
		{
			str = (nCtrlID == IDC_STATIC_HELP_PRODUCT_VALUE) ? gConfigFileMgr.GetCurrentProjectName().c_str()
				: gConfigFileMgr.GetHelpInfo(nodeName.GetString()).c_str();
		}
		pCtrl->SetWindowTextW(str);
		pCtrl->Invalidate();
	}
}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_STN_CLICKED(BTN_CLOSE, &CHelpDlg::OnStnClickedClose)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CHelpDlg::CreateLogo(CRect & rcClient)
{
	CRect rcBtn, rcTitle;
	rcBtn.right = rcClient.right - 15;
	rcBtn.left = rcClient.right - int(158 * gSystemMgr.m_fRatioH);
	rcBtn.top = rcClient.top + 5;
	rcBtn.bottom = rcBtn.top + int(64 * gSystemMgr.m_fRatioH);
	m_labelLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, NULL);
	m_labelLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_labelLogo.BringWindowToTop();
	m_labelLogo.EnableWindow(TRUE);
	m_labelLogo.Invalidate();
}

void CHelpDlg::CreateCloseButton(CRect & rcClient)
{
	CRect rcBtn;
	rcBtn.right = rcClient.right - 5;
	rcBtn.left = rcClient.right - int(32 * gSystemMgr.m_fRatioH);
	rcBtn.bottom = rcClient.bottom - 5;
	rcBtn.top = rcBtn.bottom - int(32 * gSystemMgr.m_fRatioV);
	m_btnClose.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_CLOSE);
	m_btnClose.SetLabelImage(IMG_PNG_CLOSE);
	m_btnClose.BringWindowToTop();
	m_btnClose.EnableWindow(TRUE);
	m_btnClose.Invalidate();
}

void CHelpDlg::OnStnClickedClose()
{
	CDialogEx::OnCancel();
}

void CHelpDlg::CreateFunctionButton(CRect& rcClient)
{
	CRect rcStatic, rcEdit;
	int nGapH = int(60 * gSystemMgr.m_fRatioH);
	int nGapV = int(18 * gSystemMgr.m_fRatioV);
	int nLen = int(100 * gSystemMgr.m_fRatioH);
	int nHeight = int(30 * gSystemMgr.m_fRatioV);

	rcStatic.left = rcClient.left + 2*nGapH;
	rcStatic.top = rcClient.top + int(7 * nGapV);
	rcStatic.right = rcStatic.left + int(1.5*nLen);
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_PRODUCT, rcStatic, L"",IDS_STRING_HELPINFO_PRODUCT);

	m_StaticRect.left = rcEdit.left = rcStatic.right+10;
	rcEdit.top = rcStatic.top;
	m_StaticRect.right = rcEdit.right = rcClient.right -  int(3*nGapH);
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_PRODUCT_VALUE, rcEdit,L"");

	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_VERSION, rcStatic, L"", IDS_STRING_HELPINFO_VERSION);
	rcEdit.top = rcEdit.bottom + nGapV;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_VERSION_VALUE, rcEdit,L"HELP_VERSION");

	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_FACDATA, rcStatic, L"", IDS_STRING_HELPINFO_FACDATA);
	rcEdit.top = rcEdit.bottom + nGapV;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_FACDATA_VALUE, rcEdit, L"HELP_FACDATA");

	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_PHONE, rcStatic, L"", IDS_STRING_HELPINFO_PHONE);
	rcEdit.top = rcEdit.bottom + nGapV;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_PHONE_VALUE, rcEdit, L"HELP_PHONE");

	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_SRVTIME, rcStatic, L"", IDS_STRING_HELPINFO_SRVTIME);
	rcEdit.top = rcEdit.bottom + nGapV;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_SRVTIME_VALUE, rcEdit, L"HELP_SRVTIME");

	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_HELP_WEB, rcStatic, L"", IDS_STRING_HELPINFO_WEB);
	m_StaticRect.top = rcEdit.top = rcEdit.bottom + nGapV;
	m_StaticRect.bottom = rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_STATIC_HELP_WEB_VALUE, rcEdit, L"HELP_WEB");
}

// CHelpDlg 消息处理程序


BOOL CHelpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CDialogEx::SetBackgroundImage(IDB_BITMAP_SYS_CONFIG_DLG_BK);

	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_LOGIN_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_LOGIN_UI_HEIGHT * fRatioV);

	CPoint lt;
	lt.x = int((rcScreen.Width() - nWidth) / 2.0f) + int(fRatioH * 115);
	lt.y = int((rcScreen.Height() - nHeight) / 2.0f) + int(fRatioV * 45);
	if (lt.y < int(100 * fRatioV))
	{
		lt.y = int(100 * fRatioV);
	}
	::SetWindowPos(m_hWnd, HWND_TOP, lt.x, lt.y, nWidth, nHeight, SWP_SHOWWINDOW);

	CRect rcClient;
	GetClientRect(&rcClient);
	CreateLogo(rcClient);
	CreateCloseButton(rcClient);
	CreateFunctionButton(rcClient);
	return TRUE;
}


BOOL CHelpDlg::PreTranslateMessage(MSG* pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


HBRUSH CHelpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_HELP_PRODUCT || ctrlID == IDC_STATIC_HELP_VERSION
		|| ctrlID == IDC_STATIC_HELP_FACDATA || ctrlID == IDC_STATIC_HELP_PHONE
		|| ctrlID == IDC_STATIC_HELP_SRVTIME || ctrlID == IDC_STATIC_HELP_WEB
		|| ctrlID == IDC_STATIC_HELP_PRODUCT_VALUE || ctrlID == IDC_STATIC_HELP_VERSION_VALUE
		|| ctrlID == IDC_STATIC_HELP_FACDATA_VALUE || ctrlID == IDC_STATIC_HELP_PHONE_VALUE
		|| ctrlID == IDC_STATIC_HELP_SRVTIME_VALUE || ctrlID == IDC_STATIC_HELP_WEB_VALUE)
	{
		pDC->SetTextColor(m_focusFlag&&ctrlID == IDC_STATIC_HELP_WEB_VALUE? RGB(0, 0, 250):RGB(250, 250, 250));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}


void CHelpDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rcClient, rcTitle;
	GetClientRect(&rcClient);

	CPen* pOldPen = dc.SelectObject(&m_penFrame);
	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.right, rcClient.top);

	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.left, rcClient.bottom);
	dc.SelectObject(pOldPen);

	pOldPen = dc.SelectObject(&m_penTitle);
	rcTitle.left = rcClient.left + int(150 * gSystemMgr.m_fRatioH);
	rcTitle.top = rcClient.top + int(20 * gSystemMgr.m_fRatioV);
	rcTitle.right = rcTitle.left + int(700 * gSystemMgr.m_fRatioH);
	rcTitle.bottom = rcTitle.top + int(64 * gSystemMgr.m_fRatioV);
	dc.MoveTo(rcClient.left + 10, rcTitle.bottom);
	dc.LineTo(rcClient.right - 10, rcTitle.bottom);
}


void CHelpDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	bool curStatue = (point.x > m_StaticRect.left)&&(point.x<m_StaticRect.right)&&
		(point.y>m_StaticRect.top)&&(point.y < m_StaticRect.bottom);
	if (m_focusFlag != curStatue)
	{
		m_focusFlag = curStatue;
		GetDlgItem(IDC_STATIC_HELP_WEB_VALUE)->Invalidate();
	}
	if (m_focusFlag)
	{
		SetCursor(m_hCursor);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CHelpDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x>m_StaticRect.left&&point.x<m_StaticRect.right&&point.y>m_StaticRect.top&&point.y<m_StaticRect.bottom)
	{
		SetCursor(m_hCursor);
		ShellExecute(NULL,L"open",L"iexplore.exe", gConfigFileMgr.GetHelpInfo(L"HELP_WEB").c_str(), NULL, SW_SHOWNORMAL);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}
