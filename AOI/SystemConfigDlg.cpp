#include "stdafx.h"
#include "AOI.h"
#include "SystemConfigDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"


IMPLEMENT_DYNAMIC(CSystemConfigDlg, CDialogEx)

CSystemConfigDlg::CSystemConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SYS_CONFIG, pParent),
	m_nBtnSize(72),
	m_crtSettingType(T_CAMERA)
{
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	float fRatio = rcScreen.Width() / 1920.0f;

	m_smallFont.CreateFont(int(24 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
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
	m_nBtnSize = int(gSystemMgr.m_fRatioH * BTN_SIZE);
	m_penFrame.CreatePen(PS_SOLID, 6, RGB(200, 220, 250));
	m_penTitle.CreatePen(PS_SOLID, 3, RGB(4,40,94));
}

CSystemConfigDlg::~CSystemConfigDlg()
{
}

void CSystemConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CSystemConfigDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CSystemConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CDialogEx::SetBackgroundImage(IDB_BITMAP_SYS_CONFIG_DLG_BK);

	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_CONFIG_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_CONFIG_UI_HEIGHT * fRatioV);

	CPoint lt;
	lt.x = int((rcScreen.Width() - nWidth) / 2.0f) + int(fRatioH * 105);
	lt.y = int((rcScreen.Height() - nHeight) / 2.0f) + int(fRatioV * 45);
	if (lt.y < int(100 * fRatioV))
	{
		lt.y = int(100 * fRatioV);
	}
	::SetWindowPos(m_hWnd, HWND_TOP, lt.x,lt.y, nWidth, nHeight, SWP_SHOWWINDOW);

	CRect rcClient;
	GetClientRect(&rcClient);
	CreateTabButton(rcClient);
	CreateLogo(rcClient);
	CreateCloseButton(rcClient);

	m_rcChild.left   = rcClient.left + int(10* fRatioH);
	m_rcChild.top    = int((10 + 10 + BTN_SIZE)*gSystemMgr.m_fRatioH) + 3;
	m_rcChild.right  = rcClient.right - int(5 * fRatioH);
	m_rcChild.bottom = rcClient.bottom - int(37 * fRatioV);
	m_rcCurrentBtn = m_rcCameraBtn;
    SetTimer(SYS_DLG_TIMER_ID,200,NULL);
	return TRUE;
}


BEGIN_MESSAGE_MAP(CSystemConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSystemConfigDlg::OnBnClickedOk)
	ON_STN_CLICKED(BTN_CLOSE, &CSystemConfigDlg::OnStnClickedClose)
	ON_STN_CLICKED(BTN_CAMERA, &CSystemConfigDlg::OnStnClickedCamera)
	ON_STN_CLICKED(BTN_LIGHT, &CSystemConfigDlg::OnStnClickedLight)
	ON_STN_CLICKED(BTN_PG_DEVICE, &CSystemConfigDlg::OnStnClickedPgDevice)
	ON_STN_CLICKED(BTN_PROJECT, &CSystemConfigDlg::OnStnClickedProject)
	ON_STN_CLICKED(BTN_MOTION_CARD, &CSystemConfigDlg::OnStnClickedMotion)
	ON_STN_CLICKED(BTN_CONFIG_FILE_SET, &CSystemConfigDlg::OnStnClickedConfigFileSet)
	ON_WM_PAINT()
    ON_WM_TIMER()
END_MESSAGE_MAP()


void CSystemConfigDlg::CreateTabButton(CRect & rcClient)
{
	int nGap = int(10 * gSystemMgr.m_fRatioH);
	CRect rcBtn;
	rcBtn.left = rcClient.left + 2*nGap;
	rcBtn.top = rcClient.top + nGap;
	rcBtn.right = rcBtn.left +m_nBtnSize;
	rcBtn.bottom = rcBtn.top + m_nBtnSize;
	m_btnCamera.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_CAMERA);
	m_btnCamera.SetLabelImage(IMG_PNG_CAMERA_SETTING);
	m_btnCamera.BringWindowToTop();
	m_btnCamera.EnableWindow(TRUE);
	m_btnCamera.Invalidate();
	m_rcCameraBtn = rcBtn;

	rcBtn.left = rcBtn.right + 2*nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnProject.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_PROJECT);
	m_btnProject.SetLabelImage(IMG_PNG_PROJECT_SETTING);
	m_btnProject.BringWindowToTop();
	m_btnProject.EnableWindow(TRUE);
	m_btnProject.Invalidate();
	m_rcProjectBtn = rcBtn;
	//Add Btn Motion Card
	//-----------------------------
	rcBtn.left = rcBtn.right + 2 * nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnMotion.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_MOTION_CARD);
	m_btnMotion.SetLabelImage(IMG_PNG_MOTION_CARD);
	m_btnMotion.BringWindowToTop();
	m_btnMotion.EnableWindow(TRUE);
	m_btnMotion.Invalidate();
	m_rcMotionBtn = rcBtn;

	//Add config file Set Button
	rcBtn.left = rcBtn.right + 2 * nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnConfigFile.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_CONFIG_FILE_SET);
	m_btnConfigFile.SetLabelImage(IMG_PNG_SYS_CONFIG);
	m_btnConfigFile.BringWindowToTop();
	m_btnConfigFile.EnableWindow(TRUE);
	m_btnConfigFile.Invalidate();
	m_rcConfigFile = rcBtn;

}

void CSystemConfigDlg::CreateLogo(CRect & rcClient)
{
	CRect rcBtn;
	rcBtn.right = rcClient.right - 5;
	rcBtn.left  = rcClient.right - int(158*gSystemMgr.m_fRatioH);
	rcBtn.top   = rcClient.top + 5;
	rcBtn.bottom = rcBtn.top + int(64 * gSystemMgr.m_fRatioH);
	m_labelLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, NULL);
	m_labelLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_labelLogo.BringWindowToTop();
	m_labelLogo.EnableWindow(TRUE);
	m_labelLogo.Invalidate();
}

void CSystemConfigDlg::CreateCloseButton(CRect & rcClient)
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

void CSystemConfigDlg::SetDlgVisibility(SETTING_TYPE oldType, SETTING_TYPE newType)
{
	if (oldType == T_CAMERA)
	{
		m_cameraSettingDlg.ShowWindow(SW_HIDE);
	}
	if (oldType == T_PROJECT)
	{
		m_projectSettingDlg.ShowWindow(SW_HIDE);
	}
	if (oldType == T_CONFIGFILE)
	{
		m_configFileCpyDlg.ShowWindow(SW_HIDE);
	}
	//New type;
	if (newType == T_CAMERA)
	{
		m_cameraSettingDlg.MoveWindow(&m_rcChild);
		m_cameraSettingDlg.ShowWindow(SW_SHOW);
	}
	if (newType == T_PROJECT)
	{
		m_projectSettingDlg.MoveWindow(&m_rcChild);
		m_projectSettingDlg.ShowWindow(SW_SHOW);
	}
	if (newType == T_CONFIGFILE)
	{
		m_configFileCpyDlg.MoveWindow(&m_rcChild);
		m_configFileCpyDlg.ShowWindow(SW_SHOW);
	}
	m_crtSettingType = newType;
}

void CSystemConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

void CSystemConfigDlg::OnStnClickedClose()
{
	if (IsWindow(m_projectSettingDlg.GetSafeHwnd()))
	{
		m_projectSettingDlg.OnClose();
	}
	CDialogEx::OnOK();
}

void CSystemConfigDlg::OnStnClickedCamera()
{
	Invalidate(TRUE);
	m_rcCurrentBtn = m_rcCameraBtn;
    if (!IsWindow(m_cameraSettingDlg.GetSafeHwnd()))
    {
        m_cameraSettingDlg.Create(IDD_DIALOG_CAMERA_SETTING, this);
        m_cameraSettingDlg.ShowWindow(FALSE);
    }
	SetDlgVisibility(m_crtSettingType, T_CAMERA);
	Invalidate(FALSE);
}

void CSystemConfigDlg::OnStnClickedLight()
{
	Invalidate(TRUE);
	Invalidate(FALSE);
}

void CSystemConfigDlg::OnStnClickedPgDevice()
{
}

void CSystemConfigDlg::OnStnClickedProject()
{
	Invalidate(TRUE);
	m_rcCurrentBtn = m_rcProjectBtn;
	if (!IsWindow(m_projectSettingDlg.GetSafeHwnd()))
	{
		m_projectSettingDlg.Create(IDD_DIALOG_PROJECT_SETTING, this);
		m_projectSettingDlg.ShowWindow(FALSE);
	}
	SetDlgVisibility(m_crtSettingType,T_PROJECT);
	Invalidate(FALSE);
}

void CSystemConfigDlg::OnStnClickedMotion()
{
	Invalidate(TRUE);

	Invalidate(FALSE);
}

void CSystemConfigDlg::OnStnClickedConfigFileSet()
{
	Invalidate(TRUE);
	m_rcCurrentBtn = m_rcConfigFile;
	if (!IsWindow(m_configFileCpyDlg.GetSafeHwnd()))
	{
		m_configFileCpyDlg.Create(IDD_DIALOG_CONFIGFILE, this);
		m_configFileCpyDlg.ShowWindow(FALSE);
	}
	SetDlgVisibility(m_crtSettingType, T_CONFIGFILE);
	Invalidate(FALSE);
}

void CSystemConfigDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rcClient;
	GetClientRect(&rcClient);
	CPen* pOldPen = dc.SelectObject(&m_penFrame);
	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.right, rcClient.top);

	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.left, rcClient.bottom);
	dc.SelectObject(pOldPen);

	pOldPen = dc.SelectObject(&m_penTitle);
	int nCenterX = m_rcCurrentBtn.left + int(m_rcCurrentBtn.Width() / 2.0f);
	int nX1 = nCenterX - int(8 * gSystemMgr.m_fRatioH);
	int nX2 = nCenterX + int(8 * gSystemMgr.m_fRatioH);
	int nPosY = int((10 + 10 + BTN_SIZE)*gSystemMgr.m_fRatioH);
	int nPosX = int(10 * gSystemMgr.m_fRatioH);
	dc.MoveTo(nPosX, nPosY);
	dc.LineTo(nX1, nPosY);

	dc.MoveTo(nX1, nPosY);
	dc.LineTo(nCenterX, m_rcCurrentBtn.bottom);

	dc.MoveTo(nCenterX, m_rcCurrentBtn.bottom);
	dc.LineTo(nX2, nPosY);

	dc.MoveTo(nX2, nPosY);
	dc.LineTo(rcClient.right - nPosX, nPosY);
	dc.SelectObject(pOldPen);
}

void CSystemConfigDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == SYS_DLG_TIMER_ID)
    {
        KillTimer(SYS_DLG_TIMER_ID);
        OnStnClickedCamera();
    }
    CDialogEx::OnTimer(nIDEvent);
}
