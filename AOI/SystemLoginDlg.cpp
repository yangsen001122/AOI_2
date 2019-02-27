#include "stdafx.h"
#include "AOI.h"
#include "SystemLoginDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"

extern ConfigFileManager gConfigFileMgr;
extern CSystemManager    gSystemMgr;


IMPLEMENT_DYNAMIC(CSystemLoginDlg, CDialogEx)

CSystemLoginDlg::CSystemLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SYS_LOGIN, pParent)
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

	m_txtFont.CreateFont(int(36 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_titleFont.CreateFont(int(46 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_penFrame.CreatePen(PS_SOLID, 6, RGB(200, 220, 250));
	m_penTitle.CreatePen(PS_SOLID, 3, RGB(4, 40, 94));
}

CSystemLoginDlg::~CSystemLoginDlg()
{
}

void CSystemLoginDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
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
		if (nStrID != -1)
		{
			CString str;
			str.LoadStringW(nStrID);
			pCtrl->SetWindowTextW(str);
		}
		pCtrl->Invalidate();
	}
}

void CSystemLoginDlg::DisplayMessage(CString sMsg)
{
	CStatic* pMsgPanel = (CStatic*)GetDlgItem(IDC_STATIC_LOGIN_MESSAGE);
	pMsgPanel->ShowWindow(SW_HIDE);
	pMsgPanel->SetWindowText(sMsg);
	pMsgPanel->ShowWindow(SW_SHOW);
}

void CSystemLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_btnLogin);
}

BOOL CSystemLoginDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_RETURN)
	{
		CWnd* ctrlID = GetFocus();
		if (ctrlID != NULL)
		{
			if (IDC_EDIT_PASSWORD == ctrlID->GetDlgCtrlID())
			{
				OnBnClickedButtonLogin();
			}
		}
		return TRUE;
	}
	if (param == VK_ESCAPE ||param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CSystemLoginDlg::OnInitDialog()
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
	m_editUser.SetWindowTextW(gSystemMgr.m_wstrDefaultUserName.c_str());
	m_editPassword.SetPasswordChar('*');
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSystemLoginDlg, CDialogEx)
	ON_STN_CLICKED(BTN_CLOSE, &CSystemLoginDlg::OnStnClickedClose)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CSystemLoginDlg::OnBnClickedButtonLogin)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_USER, &CSystemLoginDlg::OnEnChangeEditUser)
END_MESSAGE_MAP()

void CSystemLoginDlg::CreateLogo(CRect & rcClient)
{
	CRect rcBtn,rcTitle;
	rcBtn.right = rcClient.right - 15;
	rcBtn.left = rcClient.right - int(158 * gSystemMgr.m_fRatioH);
	rcBtn.top = rcClient.top + 5;
	rcBtn.bottom = rcBtn.top + int(64 * gSystemMgr.m_fRatioH);
	m_labelLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, NULL);
	m_labelLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_labelLogo.BringWindowToTop();
	m_labelLogo.EnableWindow(TRUE);
	m_labelLogo.Invalidate();

	rcTitle.left   = rcClient.left+ int(80 * gSystemMgr.m_fRatioH);
	rcTitle.top    = rcClient.top+ int(20 * gSystemMgr.m_fRatioV);
	rcTitle.right  = rcTitle.left+ int(620 * gSystemMgr.m_fRatioH);
	rcTitle.bottom = rcTitle.top+ int(64 * gSystemMgr.m_fRatioV);
	wstring wstrTitle = gConfigFileMgr.GetCommonSetting(L"LOGIN_DLG_TITLE");
	CWnd* pCtrl = (CWnd*)GetDlgItem(IDC_STATIC_TITLE);
	pCtrl->SetFont(&m_titleFont);
	pCtrl->MoveWindow(&rcTitle);
	pCtrl->SetWindowTextW(wstrTitle.c_str());
}

void CSystemLoginDlg::CreateCloseButton(CRect & rcClient)
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

void CSystemLoginDlg::OnStnClickedClose()
{
	CDialogEx::OnCancel();
}

void CSystemLoginDlg::CreateFunctionButton(CRect& rcClient)
{
	CRect rcStatic, rcEdit,rcBtn;
	int nGapH = int(60 * gSystemMgr.m_fRatioH);
	int nGapV = int(25 * gSystemMgr.m_fRatioV);
	int nLen = int(100 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);

	rcStatic.left = rcClient.left + 2 * nGapH;
	rcStatic.top = rcClient.top + int(5 * nGapV);
	rcStatic.right = rcStatic.left + nLen;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_USER, rcStatic, IDS_STRING_USER);

	rcEdit.left = rcStatic.right + 30;
	rcEdit.top = rcStatic.top;
	rcEdit.right = rcClient.right - 2 * nGapH;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_EDIT_USER, rcEdit);
	
	rcStatic.top   = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_PASSWORD, rcStatic, IDS_STRING_PASSWORD);

	rcEdit.top   = rcEdit.bottom + nGapV;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_EDIT_PASSWORD, rcEdit);

	rcBtn.right = rcEdit.right;
	rcBtn.left = rcEdit.right - int(1.5*nLen);
	rcBtn.top = rcEdit.bottom + nGapV;
	
	rcBtn.bottom = rcBtn.top + nHeight;
	InitializeCtrl(IDC_BUTTON_LOGIN, rcBtn,IDS_STRING_LOGIN);

	CRect rcMsg;
	rcMsg.left = rcEdit.left;
	rcMsg.top = rcBtn.top;
	rcMsg.right = rcBtn.left - 5;
	rcMsg.bottom = rcBtn.bottom;
	InitializeCtrl(IDC_STATIC_LOGIN_MESSAGE, rcMsg,-1,true);
}

HBRUSH CSystemLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_USER || ctrlID == IDC_STATIC_PASSWORD)
	{
		pDC->SetTextColor(RGB(250, 250, 250));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID == IDC_STATIC_TITLE)
	{
		pDC->SetTextColor(RGB(10, 10, 90));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID == IDC_STATIC_LOGIN_MESSAGE)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 10, 10));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CSystemLoginDlg::OnBnClickedButtonLogin()
{
	CString strUser, strPassword;
	GetDlgItemText(IDC_EDIT_USER, strUser);
	GetDlgItemText(IDC_EDIT_PASSWORD, strPassword);
	gSystemMgr.m_strOperatorID = strUser;
	string strInputUser = localUtils::utils::Unicode2Utf8(strUser.GetString());
	CString sMsg;
	UAC crtUser;
	if (gSystemMgr.m_sysDatabase.Connect())
	{
		if (strUser != L"")
		{
			if (gSystemMgr.m_sysDatabase.QueryRow(strInputUser.c_str()) > 0)
			{
				QUERY_USER_RESULT queryResult = gSystemMgr.m_sysDatabase.GetPassWord(strInputUser.c_str());
				if (strPassword == queryResult.key)
				{
					CString UserLog;
					crtUser.m_strUsername = strUser.GetString();
					crtUser.m_strPassword = strPassword.GetString();
					if (!strcmp(queryResult.level, "engineer"))
					{
						crtUser.m_uacLevel = SADMIN;
						UserLog.Format(_T("Engineer %s is logging on"), crtUser.m_strUsername.c_str());
					}
					if (!strcmp(queryResult.level, "operator"))
					{
						crtUser.m_uacLevel = ADMIN;
						UserLog.Format(_T("Operator %s is logging on"), crtUser.m_strUsername.c_str());
					}
					if (!strcmp(queryResult.level, "producer"))
					{
						crtUser.m_uacLevel = OPERATOR;
						UserLog.Format(_T("Producer %s is logging on"), crtUser.m_strUsername.c_str());
					}
					gSystemMgr.SetCurrentUser(crtUser);
					wstring UserLogOn = UserLog;
					gSystemLogger.QueueUpLog(UserLogOn);
					CDialog::OnOK();
				}
				else
				{
					sMsg.LoadStringW(IDS_STRING_USERLOGIN_ERROR);
					DisplayMessage(sMsg);
				}
			}
			else
			{
				sMsg.LoadStringW(IDS_STRING_USERLOGIN_NOTEXIST);
				DisplayMessage(sMsg);
			}
		}
	}
}

void CSystemLoginDlg::OnPaint()
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

void CSystemLoginDlg::OnEnChangeEditUser()
{
   m_editUser.GetWindowText(gSystemMgr.m_strOperatorID);
}
