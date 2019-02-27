#include "stdafx.h"
#include "windows.h"
#include "tlhelp32.h"
#include "AOI.h"
#include "AOIDlg.h"
#include "afxdialogex.h"
#include "utils.h"
#include "SystemConfigDlg.h"
#include "DebugDlg.h"
#include "QueryDlg.h"
#include"TypeDef.h"
#include"SystemLoginDlg.h"
#include "SystemUserManagementDlg.h"
#include "HelpDlg.h"
#include "ImgShowWnd.h"
#include "CreateBMP.h"


extern CSystemManager gSystemMgr;
extern ConfigFileManager gConfigFileMgr;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
UINT ShowLoadingDlg(LPVOID lpParameter)
{
	CLoadingDlg  m_LoadingDlg;
	m_LoadingDlg.SetLoadingTime(3);
	m_LoadingDlg.DoModal();
	return 0;
}

CAOIDlg::CAOIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MURASMOOTHER_DIALOG, pParent),
	 m_nTitleHeight(96)
	,m_fRatioH(1.0f)
	,m_fRatioV(1.0f)
	,m_nTxtFontSize(28)
	,m_cImgWndMainLeft(NULL)
	, m_cImgWndMainRight(NULL)
	,m_cImgWndSub1(NULL)
	,m_cImgWndSub2(NULL)
	,m_cImgWndSub3(NULL)
    ,m_cImgWndSub4(NULL)
	,m_sLogString("")
	,m_nSysStatusFlag(STATUS_OK)
	,m_bStarted(false)
	,m_eventBuffer(100)
	,m_resultEventBuffer(100)
	,m_strDataBase("")
	,m_strDemuraTable("")
	,m_strCopyLog("")
	,m_ptrLogDlg(NULL)
	,m_ptrDefectDlg(NULL)
	,m_nCrtPatternIndex(-1)
	
{
#ifdef NDEBUG
	HANDLE h_LoadingThread = AfxBeginThread(ShowLoadingDlg, NULL, THREAD_PRIORITY_TIME_CRITICAL);
#endif
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	m_smallFont.CreateFont(int(18 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_nTxtFontSize = int(28* gSystemMgr.m_fRatioH);
	m_txtFont.CreateFont(m_nTxtFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	int nBigFontSz = int(46 * gSystemMgr.m_fRatioH);
	m_bigFont.CreateFont(nBigFontSz, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	int nTitleFontSz = int(53 * gSystemMgr.m_fRatioH);
	m_titleFont.CreateFont(nTitleFontSz, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_logFont.CreateFont(int(16 * gSystemMgr.m_fRatioH),0,0,0,FW_NORMAL,FALSE,FALSE,0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_fgColor = RGB(4, 40, 94);
	m_bgColor = RGB(200, 220, 250);
}

CAOIDlg::~CAOIDlg()
{
	
}

void CAOIDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
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

void CAOIDlg::DeleteAllImageWindow()
{
	auto it = m_vImgWindow.begin();
	for (; it != m_vImgWindow.end(); it++)
	{
		(*it)->DestroyWindow();
		delete (*it);
		(*it) = NULL;
	}
}

void CAOIDlg::SegmentResultImageRegion(CImgShowWnd* cImgWnd, int nWidth, int nHeight)
{
	cImgWnd->ShowLine(0.0f, nHeight*1.0f / 3, nWidth*1.0f, nHeight *1.0f / 3, L"Green", 1);
	cImgWnd->ShowLine(0.0f, nHeight*2.0f / 3, nWidth*1.0f, nHeight*2.0f / 3, L"Green", 1);
	cImgWnd->ShowLine(nWidth*1.0f / 3, 0.0f, nWidth*1.0f / 3, nHeight*1.0f, L"Green", 1);
	cImgWnd->ShowLine(nWidth *2.0f / 3, 0.0f, nWidth*2.0f / 3, nHeight*1.0f, L"Green", 1);
}

int CAOIDlg::JudgeDefectRegionNumber(map<string, string> dictPos)
{
	if (gSystemMgr.m_finalDispImg.data)
	{
		int nWidth = gSystemMgr.m_finalDispImg.cols;
		int nHeight= gSystemMgr.m_finalDispImg.rows;
		int nDefectX = (stoi(dictPos["X"]) + stoi(dictPos["Width"])/2);
		int nDefectY=  (stoi(dictPos["Y"]) + stoi(dictPos["Height"])/2);
		int nIndexX = nDefectX/(nWidth / 3);
		int nIndexY = nDefectY/(nHeight / 3);
		int nRegionIndex = (nIndexX + 1)*(nIndexY + 1);
		if (0 == nIndexX && 1 == nIndexY)
		{
			nRegionIndex = 4;
		}
		else if (1 == nIndexX && 1 == nIndexY)
		{
			nRegionIndex = 5;
		}
		else if (0== nIndexX && 2 == nIndexY)
		{
			nRegionIndex = 7;
		}
		else if (1 == nIndexX && 2 == nIndexY)
		{
			nRegionIndex = 8;
		}
		return nRegionIndex;
	}
	else
	{
		return -1;
	}
}

void CAOIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAOIDlg::PreTranslateMessage(MSG* pMsg)
{
	WPARAM param = pMsg->wParam;
	if (pMsg->message == WM_MOUSEWHEEL|| pMsg->message == WM_MOUSEMOVE)
	{
		POINT nPos;
		GetCursorPos(&nPos);
		if (m_rcWndMainRight.PtInRect(nPos))
		{
			m_cImgWndMainRight->SetFocus();
		}
		else
		{
			//m_logList.SetFocus();
		}
		
	}
	if (pMsg->message == WM_KEYDOWN && param == VK_F1)
	{
		OnStnClickedHelp();
	}
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CAOIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_STATISTICS, &CAOIDlg::OnUpdateStatistics)
	ON_MESSAGE(WM_SEND_LOG_MSG_TO_MAIN_UI, &CAOIDlg::OnReceiveSystemLog)
	ON_MESSAGE(WM_SEND_STATE_TO_MAIN_UI, &CAOIDlg::OnReceiveSystemState)
	ON_MESSAGE(WM_UPDATE_DISP_MAIN, &CAOIDlg::OnDisplayImageOnMainScreen)
	ON_MESSAGE(WM_UPDATE_RESULT_INFO, &CAOIDlg::OnDisplayResultInfo)
	ON_MESSAGE(WM_COMM_RXCHAR_DETECTED, &CAOIDlg::OnReceiveVieworksCameraData)
	ON_MESSAGE(WM_UPDATE_STATION_CONFIG, &CAOIDlg::OnUpdateStationConfig)
	ON_STN_CLICKED(BTN_ONOFF, &CAOIDlg::OnStnClickedOnOff)
	ON_CBN_SELCHANGE(IDC_COMBO_PROJECT, &CAOIDlg::OnCbnSelchangeProject)
	ON_BN_CLICKED(IDB_DEBUG, &CAOIDlg::OnBnClickedButtonDebug)
	ON_BN_CLICKED(IDB_SPC, &CAOIDlg::OnBnClickedButtonSpc)
	ON_STN_CLICKED(BTN_SHUTDOWN, &CAOIDlg::OnStnClickedShutdown)
	ON_STN_CLICKED(BTN_LOGIN, &CAOIDlg::OnStnClickedLogin)
	ON_STN_CLICKED(BTN_SYS_CONFIG, &CAOIDlg::OnStnClickedSystemConfig)
	ON_STN_CLICKED(BTN_SYS_UAC, &CAOIDlg::OnStnClickedUAC)
	ON_STN_CLICKED(BTN_HELP, &CAOIDlg::OnStnClickedHelp)
	ON_STN_CLICKED(BTN_LANGUAGE, &CAOIDlg::OnStnClickedLanguage)
	ON_STN_CLICKED(IDB_LOG_VIEWER, &CAOIDlg::OnStnClickedLogViewer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LISTBOX_LOG, &CAOIDlg::OnNMCustomdrawListLog)
	ON_NOTIFY(NM_RCLICK, IDC_LISTBOX_LOG, &CAOIDlg::OnRClickListBoxLog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LISTBOX_RESULT_INFO, &CAOIDlg::OnNMCustomdrawListResultInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTBOX_RESULT_INFO, &CAOIDlg::OnDbClickResultListBox)
	ON_NOTIFY(NM_RCLICK, IDC_LISTBOX_RESULT_INFO, &CAOIDlg::OnRClickResultListBox)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_ADD_COPYLOG, &CAOIDlg::OnCopylog)
END_MESSAGE_MAP()

BOOL CAOIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);
	SetWindowText(_T("Lead - MuraDetector"));
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rcScreen.Width(), rcScreen.Height(), SWP_SHOWWINDOW);
	gSystemLogger.m_hViewHandle = this->GetSafeHwnd();
	m_fRatioH = 1.0f * rcScreen.Width() / BK_IMG_WIDTH;
    m_fRatioV = 1.0f * rcScreen.Height() / BK_IMG_HEIGHT;
    m_nTitleHeight = int(BK_IMG_TITLE_HEIGHT * m_fRatioV);
	
	CreateDateTimeWidget(rcScreen);
	CreateTitleWidget(rcScreen);
	CreateSystemControlButtons(rcScreen);
	CreateStatsDataDisplayingWidget(rcScreen);
	CreateControlWidget(rcScreen);
	CreateImageWindows(rcScreen);
	CreateResultWidget(rcScreen);
#ifdef DEBUG
	GrantSuperAdmin();
#endif // DEBUG

	gSystemMgr.SetMainUiPtr(this);
	gSystemMgr.CreateFileMappingServe();
	gSystemMgr.CreateHttpServe();
	gSystemMgr.ConnectVIQServe();
	CreateStaion();
    m_btnOnOff.SetLabelImage(IMG_PNG_STOP);
	gSystemMgr.StartSaveSrcImageThread();
	gSystemMgr.StartSaveExtractImgThread();
	SetSystemStatus();
	gSystemMgr.CreateUACTable();
	gSystemMgr.CreateCountTable();
	gSystemMgr.m_wstrCrtProjectName = gSystemMgr.GetStation(0)->GetStationCrtProjectName();
	WinExec("D:\\控制软件\\50061_Lead.Detect_20190109_V1.019\\Bin\\PGDetect.exe", SW_SHOWNORMAL);
	return TRUE;  
}

void CAOIDlg::OnMouseMove(cv::Point2f pt)
{
	//TODO:
}

void CAOIDlg::OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo)
{
	//TODO:
}

void CAOIDlg::OnPaint()
{
	CPaintDC dc(this); 
	CRect rect;
	GetClientRect(&rect);
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		LoadBackgroundImage(dc);
	}
	dc.SelectStockObject(NULL_BRUSH);
	CDialogEx::OnPaint();
}

void CAOIDlg::LoadBackgroundImage(CPaintDC& dc)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);

	CBitmap bakImg;
	if (bakImg.LoadBitmap(IDB_BITMAP_BACKGROUND))
	{
		BITMAP bitmap;
		bakImg.GetBitmap(&bitmap);
		CBitmap* pPngOld = dcMem.SelectObject(&bakImg);
		long nWidth = (rect.Width() < bitmap.bmWidth ? rect.Width() : bitmap.bmWidth);
		long nHeight = (rect.Height() < bitmap.bmHeight ? rect.Height() : bitmap.bmHeight);
		dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, nWidth, nHeight, SRCCOPY);
	}
}

void CAOIDlg::CreateDateTimeWidget(CRect & rcScreen)
{
	CWnd* pWidget = (CWnd*)GetDlgItem(IDC_STATIC_TIME);
	CRect rc, rcPos;
	int nWidth = 145;
	int nHeight = 32;
	rcPos.left = int((BK_IMG_LOGO_END + 30)*m_fRatioH);
	rcPos.top = int((m_nTitleHeight - 2 * nHeight * m_fRatioV) / 2.0f) + 5;
	rcPos.right = rcPos.left + int(nWidth*m_fRatioH);
	rcPos.bottom = rcPos.top + int(nHeight*m_fRatioV);
	pWidget->MoveWindow(rcPos);
	pWidget->SetFont(&m_txtFont);
	SetTimer(TIMER_ID_DATE_TIME, 1000 * 60, NULL);
	SetTimer(TIMER_ID_COUNT_PER_HOUR, 1000*60*60, NULL);
	rcPos.top = rcPos.bottom + 1;
	rcPos.bottom = rcPos.top + int(nHeight*m_fRatioV);
	pWidget = (CWnd*)GetDlgItem(IDC_STATIC_DATE);
	pWidget->MoveWindow(rcPos);
	pWidget->SetFont(&m_txtFont);

	SetCurrentDatetime();
}

void CAOIDlg::SetCurrentDatetime()
{
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString strTime;
	strTime.Format(_T("%d:%.2d "), dt.wHour, dt.wMinute);
	int nLangID = 1;// gConfigMgr.GetLanguageID();
	wstring wstrDayOfWeek = localUtils::utils::GetDayOfWeekStr(nLangID, dt.wDayOfWeek);
	strTime.Append(wstrDayOfWeek.c_str());
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_TIME);
	pCtrl->ShowWindow(SW_HIDE);
	pCtrl->SetWindowText(strTime);
	CRect rc;
	pCtrl->GetWindowRect(rc);
	InvalidateRect(rc);
	pCtrl->ShowWindow(SW_SHOW);

	CString strDate;
	strDate.Format(_T("%d/%.2d/%.2d"), dt.wYear, dt.wMonth, dt.wDay);
	pCtrl = GetDlgItem(IDC_STATIC_DATE);
	pCtrl->ShowWindow(SW_HIDE);
	pCtrl->SetWindowText(strDate);
	pCtrl->GetWindowRect(rc);
	InvalidateRect(rc);
	pCtrl->ShowWindow(SW_SHOW);
}

void CAOIDlg::CreateTitleWidget(CRect & rcScreen)
{
	int nBtnSz = int(BTN_SZ * m_fRatioV);
	CRect rcLogo;
	rcLogo.left  = int(BK_IMG_LOGO_BGN*gSystemMgr.m_fRatioH);
	rcLogo.top = int((m_nTitleHeight - 64*gSystemMgr.m_fRatioH) / 2.0f);
	rcLogo.right = rcLogo.left + int(158*gSystemMgr.m_fRatioH);
	rcLogo.bottom = rcLogo.top + int(64 * gSystemMgr.m_fRatioH);
	m_labelLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcLogo, this, NULL);
	m_labelLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_labelLogo.BringWindowToTop();
	m_labelLogo.Invalidate();

	CRect rcPos;
	rcPos.left = int((BK_IMG_LOGO_END + 30)*m_fRatioH) + int(20 * m_fRatioH)-2;
	rcPos.top = 18;
	rcPos.right = rcScreen.right - (5 * nBtnSz - 10 - 4 * 6) - 60;
	rcPos.bottom = m_nTitleHeight - 12;
	CWnd* pTitle = (CWnd*)GetDlgItem(IDC_STATIC_TITLE);
	pTitle->ShowWindow(SW_HIDE);
	pTitle->SetFont(&m_titleFont);
	pTitle->MoveWindow(rcPos);
    wstring wstrTitle = gConfigFileMgr.GetCommonSetting(L"CUSTOMISED_TITLE");
	pTitle->SetWindowTextW(wstrTitle.c_str());
	pTitle->ShowWindow(SW_SHOW);
}

void CAOIDlg::CreateSystemControlButtons(CRect & rcScreen)
{
	CRect rcBtn;
	int nBtnSz = int(BTN_SZ * m_fRatioV);
	rcBtn.top    = int((m_nTitleHeight - m_fRatioV*BTN_SZ) / 2.0f);
	rcBtn.right  = rcScreen.right - 10;
	rcBtn.left   = rcBtn.right - nBtnSz;
	rcBtn.bottom = rcBtn.top + nBtnSz;

	m_btnShutdown.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_SHUTDOWN);
	m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN_DENY);
	m_btnShutdown.EnableWindow(FALSE);
	m_btnShutdown.Invalidate(); 

	rcBtn.right = rcBtn.left - 6;
	rcBtn.left = rcBtn.right - nBtnSz;
	m_btnLanguage.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_LANGUAGE);
	m_btnLanguage.ShowWindow(SW_HIDE);
	m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE_DENY);
	m_btnLanguage.BringWindowToTop();
	m_btnLanguage.EnableWindow(FALSE);
	m_btnLanguage.ShowWindow(SW_SHOW);
	m_btnLanguage.Invalidate();

	rcBtn.right = rcBtn.left - 6;
	rcBtn.left = rcBtn.right - nBtnSz;
	m_btnHelp.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_HELP);
	m_btnHelp.ShowWindow(SW_HIDE);
	m_btnHelp.SetLabelImage(IMG_PNG_HELP);
	m_btnHelp.BringWindowToTop();
	m_btnHelp.EnableWindow(TRUE);
	m_btnHelp.ShowWindow(SW_SHOW);
	m_btnHelp.Invalidate();
	
	rcBtn.right = rcBtn.left - 6;
	rcBtn.left = rcBtn.right - nBtnSz;
	m_btnUAC.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_SYS_UAC);
	m_btnUAC.ShowWindow(SW_HIDE);
	m_btnUAC.SetLabelImage(IMG_PNG_UAC_DENY);
	m_btnUAC.BringWindowToTop();
	m_btnUAC.EnableWindow(FALSE);
	m_btnUAC.ShowWindow(SW_SHOW);
	m_btnUAC.Invalidate();

	rcBtn.right = rcBtn.left - 6;
	rcBtn.left = rcBtn.right - nBtnSz;
	m_btnSysConfig.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_SYS_CONFIG);
	m_btnSysConfig.ShowWindow(SW_HIDE);
	m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG_DENY);
	m_btnSysConfig.BringWindowToTop();
	m_btnSysConfig.EnableWindow(FALSE);
	m_btnSysConfig.ShowWindow(SW_SHOW);
	m_btnSysConfig.Invalidate();

	rcBtn.right = rcBtn.left - 6;
	rcBtn.left = rcBtn.right - nBtnSz;
	m_btnLogon.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_LOGIN);
	m_btnLogon.ShowWindow(SW_HIDE);
	m_btnLogon.SetLabelImage(IMG_PNG_LOG_OUT);
	m_btnLogon.BringWindowToTop();
	m_btnLogon.EnableWindow(TRUE);
	m_btnLogon.ShowWindow(SW_SHOW);
	m_btnLogon.Invalidate();
}

void CAOIDlg::CreateStatsDataDisplayingWidget(CRect & rcScreen)
{
	m_rcBackGround1.left = rcScreen.left + int(2*m_fRatioH);
	m_rcBackGround1.top = m_nTitleHeight + int(8*m_fRatioV);
	int nWidth = int(380 * m_fRatioH);
	int nHeight = int(240 * m_fRatioV);
	m_rcBackGround1.right = m_rcBackGround1.left + nWidth;
	m_rcBackGround1.bottom = m_rcBackGround1.top + nHeight;
	m_labelBackground1.Create(_T(""), WS_CHILD | WS_VISIBLE, m_rcBackGround1, this);
	m_labelBackground1.SetColor(m_fgColor, m_bgColor);
	m_labelBackground1.Invalidate();
	
	CRect rcName, rcValue;
	int nStaticHeight = int(40 * m_fRatioV);
	rcName.left = m_rcBackGround1.left + int(10*m_fRatioH);
	rcName.top = m_rcBackGround1.top + int(16 * m_fRatioV);
	rcName.right = rcName.left + int(150 * m_fRatioH);
	rcName.bottom = rcName.top + nStaticHeight;
	CString sName;
	//Total
	sName.LoadStringW(IDS_STRING_TOTAL);
	m_labelTotalTxt.Create(sName, WS_CHILD | WS_VISIBLE, rcName, this, NULL);
	m_labelTotalTxt.SetTxtFont(m_nTxtFontSize + 2, FW_BOLD,LEFT);
	m_labelTotalTxt.SetColor(m_fgColor, m_bgColor);
	m_labelTotalTxt.Invalidate();

	rcValue.left = rcName.right + int(10*m_fRatioH);
	rcValue.top = rcName.top + 3;
	rcValue.right = m_rcBackGround1.right - int(15 * m_fRatioH);
	rcValue.bottom = rcName.bottom + 3;


	COLORREF tmpColor = RGB(250, 250, 250);
	m_labelTotalValue.Create(_T(""), WS_CHILD | WS_VISIBLE, rcValue, this, NULL);
	m_labelTotalValue.ShowWindow(SW_HIDE);
	m_labelTotalValue.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_labelTotalValue.SetColor(RGB(0, 0, 0), tmpColor);
	unsigned long nTotal = 0;
	CString strTotal;
	strTotal.Format(_T("%d"), nTotal);
	m_labelTotalValue.SetWindowTextW(strTotal);
	m_labelTotalValue.Invalidate();
	m_labelTotalValue.ShowWindow(SW_SHOW);


	int nGapV = int(16 * m_fRatioV);
	rcName.top = rcName.bottom + nGapV;
	rcName.bottom = rcName.top + nStaticHeight;
	sName.LoadStringW(IDS_STRING_OK_COUNT_RATE);
	m_labelOkCountTxt.Create(sName, WS_CHILD | WS_VISIBLE, rcName, this, NULL);
	m_labelOkCountTxt.SetTxtFont(m_nTxtFontSize + 2, FW_BOLD, LEFT);
	m_labelOkCountTxt.SetColor(m_fgColor, m_bgColor);
	m_labelOkCountTxt.Invalidate();

	rcValue.top = rcValue.bottom + nGapV;
	rcValue.bottom = rcValue.top + nStaticHeight;
	m_lableOkCountValue.Create(_T(""), WS_CHILD | WS_VISIBLE, rcValue, this, NULL);
	m_lableOkCountValue.ShowWindow(SW_HIDE);
	m_lableOkCountValue.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_lableOkCountValue.SetColor(RGB(0, 0, 0), tmpColor);
	long nPassCount = 0;
	CString sPassRatio;
	if (nTotal != 0)
	{
		sPassRatio.Format(_T("%d / %.1f%%"), nPassCount, 100.0f*nPassCount / nTotal);
	}
	else
	{
		sPassRatio.Format(_T("%d / 0.0%%"), nPassCount);
	}
	m_lableOkCountValue.SetWindowTextW(sPassRatio);
	m_lableOkCountValue.Invalidate();
	m_lableOkCountValue.ShowWindow(SW_SHOW);
	// NG Ratio
	rcName.top = rcName.bottom + nGapV;
	rcName.bottom = rcName.top + nStaticHeight;
	sName.LoadStringW(IDS_STRING_LAST_TIMING);
	m_labelCrtTimingTxt.Create(sName, WS_CHILD | WS_VISIBLE, rcName, this, NULL);
	m_labelCrtTimingTxt.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_labelCrtTimingTxt.SetColor(m_fgColor, m_bgColor);
	m_labelCrtTimingTxt.Invalidate();

	rcValue.top = rcValue.bottom + nGapV;
	rcValue.bottom = rcValue.top + nStaticHeight;
	m_lableCrtTimingValue.Create(_T(""), WS_CHILD | WS_VISIBLE, rcValue, this, IDC_CURRENT_TIME);
	m_lableCrtTimingValue.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_lableCrtTimingValue.SetColor(RGB(0, 0, 0), tmpColor);
	m_lableCrtTimingValue.Invalidate();
	//m_rcDrawProcess = rcValue;
	//Avg. timing
	rcName.top = rcName.bottom + nGapV;
	rcName.bottom = rcName.top + nStaticHeight;
	sName.LoadStringW(IDS_STRING_AVG_TIMING);
	m_lableAvgTimingTxt.Create(sName, WS_CHILD | WS_VISIBLE, rcName, this, NULL);
	m_lableAvgTimingTxt.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_lableAvgTimingTxt.SetColor(m_fgColor, m_bgColor);
	m_lableAvgTimingTxt.Invalidate();

	rcValue.top = rcValue.bottom + nGapV;
	rcValue.bottom = rcValue.top + nStaticHeight;
	m_lableAvgTimingValue.Create(_T(""), WS_CHILD | WS_VISIBLE, rcValue, this, NULL);
	m_lableAvgTimingValue.SetTxtFont(m_nTxtFontSize, FW_BOLD, LEFT);
	m_lableAvgTimingValue.SetColor(RGB(0, 0, 0), tmpColor);
	long nTotalTime = 0;
	CString sAvgTime;
	if (nTotal != 0)
	{
		double dAvg = nTotalTime / nTotal;
		sAvgTime.Format(_T("%.2f"), dAvg);
		wstring wstrAvgTiming = sAvgTime.GetBuffer();
		wstrAvgTiming.append(L"s");
		m_lableAvgTimingValue.SetWindowTextW(wstrAvgTiming.c_str());
	}
	m_lableAvgTimingValue.Invalidate();
}

void CAOIDlg::CreateControlWidget(CRect & rcScreen)
{
	CRect rcPos;
	rcPos.left = m_rcBackGround1.left;
	rcPos.right = m_rcBackGround1.right;
	rcPos.top = m_rcBackGround1.bottom + 5;
	rcPos.bottom = rcPos.top + m_rcBackGround1.Height();
	m_rcBackGround2 = rcPos;
	m_lableBackground2.Create(_T(""), WS_CHILD | WS_VISIBLE, rcPos, this);
	m_lableBackground2.SetColor(m_fgColor, m_bgColor);
	m_lableBackground2.Invalidate();
	
	CRect rcBtn;
	rcBtn.left = m_rcBackGround1.left + int(10*m_fRatioH);
	rcBtn.top = rcPos.top + int(45*m_fRatioV);
	rcBtn.right = rcBtn.left + int(150 * m_fRatioH);
	rcBtn.bottom = rcBtn.top + int(150 * m_fRatioH);
	m_btnOnOff.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_ONOFF);
	m_btnOnOff.SetLabelImage(IMG_PNG_STOP);
	m_btnOnOff.EnableWindow(TRUE);
	m_btnOnOff.Invalidate();

	CRect rcCombo;
	rcCombo.left = rcBtn.right + int(10 * m_fRatioH);
	rcCombo.top = rcPos.top + int(25 * m_fRatioV);
	rcCombo.right = rcPos.right - int(15 * m_fRatioH);
	rcCombo.bottom = rcCombo.top + int(40 * m_fRatioV);
	m_comboProject.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, rcCombo, this, IDC_COMBO_PROJECT);
	m_comboProject.ShowWindow(SW_HIDE);
	m_comboProject.SetFont(&m_smallFont);
	//m_comboProject.SetFont(&m_txtFont); m_smallFont
	m_comboProject.SetCurSel(-1);
	ListAllProject();
	m_comboProject.EnableWindow(FALSE);
	m_comboProject.Invalidate();
	m_comboProject.ShowWindow(SW_SHOW);

	rcBtn.left = rcCombo.left;
	rcBtn.top = rcCombo.bottom + 10;
	rcBtn.right = rcCombo.right;
	rcBtn.bottom = rcBtn.top + int(55* m_fRatioV);
	CString str;
	str.LoadStringW(IDS_STRING_DEBUG);
	m_btnDebug.Create(str, WS_CHILD | WS_VISIBLE | BS_CENTER | SS_NOTIFY, rcBtn, this, IDB_DEBUG);
	m_btnDebug.ShowWindow(SW_HIDE);
	m_btnDebug.SetFont(&m_bigFont);
	m_btnDebug.EnableWindow(FALSE);
	m_btnDebug.Invalidate();
	m_btnDebug.ShowWindow(SW_SHOW);

	rcBtn.top = rcBtn.bottom + 10;
	rcBtn.bottom = rcBtn.top + int(55 * m_fRatioV);
	str.LoadStringW(IDS_STRING_SPC);
	m_btnSpc.Create(str, WS_CHILD | WS_VISIBLE | BS_CENTER | SS_NOTIFY, rcBtn, this, IDB_SPC);
	m_btnSpc.SetFont(&m_bigFont);
	m_btnSpc.EnableWindow(TRUE);
	m_btnSpc.Invalidate();

	CRect rcLogViewer;
	rcLogViewer.right = rcBtn.right;
	rcLogViewer.top = rcBtn.bottom + 10;
	rcLogViewer.left = rcLogViewer.right - int(32 * gSystemMgr.m_fRatioH);
	rcLogViewer.bottom = rcLogViewer.top + int(32 * gSystemMgr.m_fRatioH);
	m_btnLogViewer.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcLogViewer, this, IDB_LOG_VIEWER);
	m_btnLogViewer.SetLabelImage(IMG_PNG_LOG_VIEWER);
	m_btnLogViewer.EnableWindow(TRUE);
	m_btnLogViewer.Invalidate();
	//listLog
	CRect rcListBox;
	rcListBox.left = m_rcBackGround1.left;
	rcListBox.right = m_rcBackGround1.right;
	rcListBox.top = rcPos.bottom + int(6 * m_fRatioV);
	rcListBox.bottom = rcScreen.bottom - int(5 * m_fRatioV);
	m_logList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT, rcListBox, this, IDC_LISTBOX_LOG);  
	LONG lStyle;
	m_logList.SetBkColor(RGB(200, 220, 250));
	lStyle = GetWindowLong(m_logList.m_hWnd, GWL_STYLE);        
	lStyle &= ~LVS_TYPEMASK;                                         
	lStyle |= LVS_REPORT;                                             
	SetWindowLong(m_logList.m_hWnd, GWL_STYLE, lStyle);      
	m_logList.ShowWindow(SW_SHOW); 
	DWORD dwStyle = m_logList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;                                                   														                   
	dwStyle |= LVS_EX_GRIDLINES;   
	dwStyle |= LVS_EX_INFOTIP;
	m_logList.SetExtendedStyle(dwStyle);             
	m_logList.SetFont(&m_logFont);
	m_logList.EnableWindow(TRUE);
	m_logList.InsertColumn(0, L" ", LVCFMT_LEFT, 1);
	CString strLog;
	strLog.LoadStringW(IDC_STRING_LOG_LEVEL_TIME);
	m_logList.InsertColumn(1, strLog, LVCFMT_LEFT, int(90 * gSystemMgr.m_fRatioH));
	strLog.LoadStringW(IDS_STRING_LOG_DESCRIBE);
	m_logList.InsertColumn(2, strLog, LVCFMT_LEFT, int(920* gSystemMgr.m_fRatioH));
	m_logList.ShowWindow(SW_HIDE);
	m_logList.EnableWindow(TRUE);
	m_logList.Invalidate();
	m_logList.ShowWindow(SW_SHOW);
	m_logList.BringWindowToTop();
	m_rcLogList = rcListBox;
}

void CAOIDlg::CreateImageWindows(CRect & rcScreen)
{
	std::map<int, STATION_CONFIG>* pStaitonConfigArray = gConfigFileMgr.GetStaionConfigArray();
	STATION_CONFIG stationConfig = pStaitonConfigArray->begin()->second;
	double  nMainWndRatio = 0;
	double  nSubWndRatio = 0;
	for (auto it = stationConfig.m_dictCamara.begin(); it != stationConfig.m_dictCamara.end(); it++)
	{
		wstring wstrCameraName = it->first;
		CAMERA_SETTING setting = it->second;
		bool bMainCamera = setting.m_bMainCamera;
		long nCameraWidth = setting.m_nWidth;
		long nCameraHeight = setting.m_nHeight;
		if (bMainCamera)
		{
			nMainWndRatio = 1.0f * nCameraHeight / nCameraWidth;
		}
	}
	CRect rcClient;
	rcClient.left = m_rcBackGround1.right + int(5*m_fRatioH);
	rcClient.top = m_rcBackGround1.top + int(3*m_fRatioV);
	rcClient.right = rcScreen.right - int(5 * m_fRatioH);
	rcClient.bottom = rcScreen.bottom - int(6 * m_fRatioV); 
	int nWidth = rcClient.Width();
	int nMainHeight = int(m_rcBackGround2.bottom- rcClient.top);
	CRect rcWnd;
	rcWnd.left = rcClient.left;
	rcWnd.top = rcClient.top;
	rcWnd.bottom = rcWnd.top + nMainHeight;
	rcWnd.right = rcWnd.left + int(nMainHeight / nMainWndRatio);
	CreateSubImageWindow(rcWnd, m_cImgWndMainLeft);
	m_rcWndMainLeft = rcWnd;
	rcWnd.left = rcWnd.right + int(5 * m_fRatioH);
	rcWnd.right = rcWnd.left + int(nMainHeight / nMainWndRatio);
	m_rcWndMainRight = rcWnd;
	CreateSubImageWindow(rcWnd, m_cImgWndMainRight);
	rcWnd.left = m_rcWndMainRight.right+ int(5 * m_fRatioH);
	rcWnd.right = rcScreen.right - int(5 * m_fRatioH);
	rcWnd.top = m_rcWndMainRight.top;
	int nSubHeight = int((m_rcLogList.bottom - 2.5 * 5 * m_fRatioV - rcWnd.top) / 4);
	rcWnd.bottom = rcWnd.top+ nSubHeight;
	m_rcWndSub1 = rcWnd;
	CreateSubImageWindow(rcWnd, m_cImgWndSub1);//(gige cam1)
	rcWnd.top = rcWnd.bottom+ int(5 * m_fRatioV);
	rcWnd.bottom = rcWnd.top + nSubHeight;
	m_rcWndSub2 = rcWnd;
	CreateSubImageWindow(rcWnd, m_cImgWndSub2);//(gige cam2)
	rcWnd.top = rcWnd.bottom + int(5 * m_fRatioV);
	rcWnd.bottom = rcWnd.top + nSubHeight;
	m_rcWndSub3 = rcWnd;
	CreateSubImageWindow(rcWnd, m_cImgWndSub3);//(gige cam3)
	rcWnd.top = rcWnd.bottom + int(5 * m_fRatioV);
	rcWnd.bottom = rcWnd.top + nSubHeight;
	m_rcWndSub4 = rcWnd;
	CreateSubImageWindow(rcWnd, m_cImgWndSub4);//(gige cam4)
}


void CAOIDlg::CreateSubImageWindow(CRect & rcPos, CImgShowWnd* &cImgWnd)
{
	CRect rcWnd = rcPos;
	if (NULL == cImgWnd)
	{
		cImgWnd = new CImgShowWnd;
		if (FALSE == cImgWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcWnd, this, -1))
		{

			//AfxMessageBox(IDS_STRING_CREATE_WINDOW);
			return;
		}
		m_vImgWindow.push_back(cImgWnd);
		cImgWnd->ShowWindow(SW_SHOW);
		if (cImgWnd == m_cImgWndMainRight)
		{
			cImgWnd->EnableDrag(true);
			cImgWnd->EnableZoom(true);
		}
		else
		{
			cImgWnd->EnableDrag(false);
			cImgWnd->EnableZoom(false);
		}
		cImgWnd->SetBkColor(RGB(100,100,100));
		cImgWnd->SetListener(this);
	}
}

void CAOIDlg::CreateResultWidget(CRect & rcScreen)
{
	//list result information
	CRect rcListBox(m_rcLogList);
	rcListBox.left = m_rcLogList.right + int(5 * m_fRatioH);
	rcListBox.right = m_rcWndMainRight.right - int(200 * m_fRatioH);
	m_logDetectResult.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT, rcListBox, this, IDC_LISTBOX_RESULT_INFO);
	LONG lStyleResultInfo;
	m_logDetectResult.SetBkColor(RGB(200, 220, 250));
	lStyleResultInfo = GetWindowLong(m_logDetectResult.m_hWnd, GWL_STYLE);
	lStyleResultInfo &= ~LVS_TYPEMASK;
	lStyleResultInfo |= LVS_REPORT;
	SetWindowLong(m_logDetectResult.m_hWnd, GWL_STYLE, lStyleResultInfo);
	m_logDetectResult.ShowWindow(SW_SHOW);
	DWORD dwStyleResultInfo = m_logDetectResult.GetExtendedStyle();
	dwStyleResultInfo |= LVS_EX_FULLROWSELECT;
	dwStyleResultInfo |= LVS_EX_GRIDLINES;
	dwStyleResultInfo |= LVS_EX_INFOTIP;
	m_logDetectResult.SetExtendedStyle(dwStyleResultInfo);
	m_logDetectResult.SetFont(&m_smallFont);
	m_logDetectResult.EnableWindow(TRUE);
	m_logDetectResult.InsertColumn(0, L" ", LVCFMT_LEFT, 1);
	CString strLogResultInfo;
	strLogResultInfo.LoadStringW(IDS_STRING_RESULT_PATTERN_INDEX);
	m_logDetectResult.InsertColumn(1, strLogResultInfo, LVCFMT_LEFT, int(150 * gSystemMgr.m_fRatioH));
	strLogResultInfo.LoadStringW(IDS_STRING_RESULT_DESCRIBE);
	m_logDetectResult.InsertColumn(2, strLogResultInfo, LVCFMT_LEFT, int(1500 * gSystemMgr.m_fRatioH));
	m_logDetectResult.ShowWindow(SW_HIDE);
	m_logDetectResult.EnableWindow(TRUE);
	m_logDetectResult.ShowWindow(SW_SHOW);
	m_logDetectResult.BringWindowToTop();
	m_rcResultList = rcListBox;
	
	CRect rcBackGround3(m_rcResultList);
	rcBackGround3.left = m_rcResultList.right + int(5 * m_fRatioH);
	rcBackGround3.right = m_rcWndSub1.left - int(5 * m_fRatioH);
	m_labelBackground3.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBackGround3, this);
	m_labelBackground3.SetColor(m_fgColor, m_bgColor);
	m_labelBackground3.Invalidate();

	CRect rcResult;
	rcResult.left = m_rcResultList.right + int(15 * m_fRatioH);
	rcResult.right = m_rcWndSub1.left - int(15 * m_fRatioH);
	rcResult.top = m_rcResultList.top + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top + int(rcResult.Width()*2/3)-int(15 * m_fRatioV);
	m_labelResult.Create(_T(""), WS_CHILD | WS_VISIBLE, rcResult, this, NULL);
	m_labelResult.ShowWindow(SW_HIDE);
	m_labelResult.SetTxtFont(m_nTxtFontSize * 3, FW_BOLD, CENTER);
	m_labelResult.SetColor(RGB(100,100,100), m_bgColor);
	m_labelResult.SetWindowTextW(L"N/A");
	m_labelResult.Invalidate();
	m_labelResult.ShowWindow(SW_SHOW);

	int nHeight = int(40 * m_fRatioV);

	rcResult.top = rcResult.bottom + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top + nHeight;
	m_labelScreenIDName.Create(_T(""), WS_CHILD | WS_VISIBLE, rcResult, this, NULL);
	m_labelScreenIDName.ShowWindow(SW_HIDE);
	m_labelScreenIDName.SetTxtFont(m_nTxtFontSize, FW_NORMAL, LEFT);
	m_labelScreenIDName.SetColor(m_fgColor, m_bgColor);
	m_labelScreenIDName.SetWindowTextW(L"屏ID");
	m_labelScreenIDName.Invalidate();
	m_labelScreenIDName.ShowWindow(SW_SHOW);


	rcResult.top = rcResult.bottom + int(5 * m_fRatioV);
	rcResult.bottom = rcResult.top + nHeight*2;
	m_labelScreenID.Create(_T(""), WS_CHILD | WS_VISIBLE , rcResult, this, NULL);
	m_labelScreenID.SetTxtFont(m_nTxtFontSize/2, FW_NORMAL, CENTER);
	m_labelScreenID.SetColor(m_fgColor, RGB(250, 250, 250));
	m_labelScreenID.SetWindowTextW(L"");
	m_labelScreenID.Invalidate();
	m_edtScreenID.Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY | ES_MULTILINE | ES_READONLY, rcResult, this, NULL);
	m_edtScreenID.EnableWindow(TRUE);
	m_edtScreenID.SetFont(&m_smallFont);

	rcResult.top = rcResult.bottom + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top  + nHeight;
	m_labelScreenNameName.Create(_T(""), WS_CHILD | WS_VISIBLE, rcResult, this, NULL);
	m_labelScreenNameName.ShowWindow(SW_HIDE);
	m_labelScreenNameName.SetTxtFont(m_nTxtFontSize, FW_NORMAL, LEFT);
	m_labelScreenNameName.SetColor(m_fgColor, m_bgColor);
	m_labelScreenNameName.SetWindowTextW(L"画面序号");
	m_labelScreenNameName.Invalidate();
	m_labelScreenNameName.ShowWindow(SW_SHOW);

	rcResult.top = rcResult.bottom + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top + nHeight;
	m_labelScreenName.Create(_T(""), WS_CHILD | WS_VISIBLE | ES_CENTER | SS_CENTERIMAGE, rcResult, this, NULL);
	m_labelScreenName.SetTxtFont(m_nTxtFontSize, FW_NORMAL, CENTER);
	m_labelScreenName.SetColor(m_fgColor, RGB(250,250,250));
	m_labelScreenName.SetWindowTextW(L"");
	m_labelScreenName.Invalidate();

	rcResult.top = rcResult.bottom + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top + nHeight;
	m_labelDefectNumName.Create(_T(""), WS_CHILD | WS_VISIBLE , rcResult, this, NULL);
	m_labelDefectNumName.ShowWindow(SW_HIDE);
	m_labelDefectNumName.SetTxtFont(m_nTxtFontSize, FW_NORMAL, LEFT);
	m_labelDefectNumName.SetColor(m_fgColor, m_bgColor);
	m_labelDefectNumName.SetWindowTextW(L"缺陷数量");
	m_labelDefectNumName.Invalidate();
	m_labelDefectNumName.ShowWindow(SW_SHOW);

	rcResult.top = rcResult.bottom + int(15 * m_fRatioV);
	rcResult.bottom = rcResult.top + nHeight;
	m_labelDefectNum.Create(_T(""), WS_CHILD | WS_VISIBLE | ES_CENTER | SS_CENTERIMAGE, rcResult, this, NULL);
	m_labelDefectNum.SetTxtFont(m_nTxtFontSize, FW_NORMAL, CENTER);
	
	m_labelDefectNum.SetColor(m_fgColor, RGB(250, 250, 250));
	m_labelDefectNum.SetWindowTextW(L"");
	m_labelDefectNum.Invalidate();
}

void CAOIDlg::CreateStaion()
{
	std::map<int, STATION_CONFIG>* pStationConfigArray = gConfigFileMgr.GetStaionConfigArray();
	if (pStationConfigArray != nullptr && !pStationConfigArray->empty())
	{
		for (auto it = pStationConfigArray->begin(); it != pStationConfigArray->end(); it++)
		{
			STATION_CONFIG& stationConfig = it->second;
			CDemuraStation* ptrStation = new CDemuraStation(stationConfig);
			m_dictStationIndex_Ptr[it->first] = ptrStation;
		}
	}
}

void CAOIDlg::ShutDownStation()
{
	for (auto it = m_dictStationIndex_Ptr.begin(); it != m_dictStationIndex_Ptr.end(); it++)
	{
		it->second->OnShutdownEvent();
		delete it->second;
	}
	m_dictStationIndex_Ptr.clear();
}

HBRUSH CAOIDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(10, 10, 115));
		hbr = HBRUSH(GetStockObject(NULL_BRUSH));
		break;
	case CTLCOLOR_LISTBOX:
		pDC->SetBkColor(m_bgColor);
		hbr = CreateSolidBrush(m_bgColor);
		break;
	default:
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
	}
	return hbr;
}

void CAOIDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_DATE_TIME)
	{
		SetCurrentDatetime();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CAOIDlg::OnStnClickedOnOff()
{

	if (gSystemMgr.m_strOperatorID == L"" || !(gSystemMgr.m_sysDatabase.QueryRow(localUtils::utils::Unicode2Utf8(gSystemMgr.m_strOperatorID.GetString()).c_str()) > 0))
	{
		AfxMessageBox(L"请先登录系统！");
		OnStnClickedLogin();
		return;
	}
	//if (m_nSysStatusFlag==0)
	//{
		if (!m_bStarted)
		{
			m_bStarted = true;
			m_btnOnOff.SetLabelImage(IMG_PNG_RUN);
			m_comboProject.EnableWindow(FALSE);
		}
		else
		{
			m_bStarted = false;
			m_btnOnOff.SetLabelImage(IMG_PNG_STOP);
			UAC crtUser = gSystemMgr.GetCurrentUser();
			m_comboProject.EnableWindow(crtUser.m_uacLevel > 0?TRUE:FALSE);
		}
		gSystemMgr.SetStartedFlag(m_bStarted);
		ControlStationCameras(m_bStarted);
		m_btnOnOff.Invalidate();
	//}
	//else
	//{
	//	m_btnOnOff.SetLabelImage(IMG_PNG_ERROR);
	//	m_btnOnOff.Invalidate();
	//}
    
}


void CAOIDlg::OnCbnSelchangeProject()
{
	DWORD ret = WaitForSingleObject(gSystemMgr.m_hCaptureFinishFlag, 3000);
	if (WAIT_TIMEOUT == ret)
	{
		AfxMessageBox(L"当前物料图像采集未完成，不允许切换工程");
		gSystemLogger.QueueUpLogAndReport(L"not all patterns are captured", LV_ERROR);
		return;
	}
	else
	{
		int crtIndex = m_comboProject.GetCurSel();
		CString sProjectName;
		m_comboProject.GetLBText(crtIndex, sProjectName);
		wstring wstrName = sProjectName.GetString();
		for (auto it = m_dictStationIndex_Ptr.begin(); it != m_dictStationIndex_Ptr.end(); it++)
		{
			it->second->Switch2Project(wstrName);
		}
		gConfigFileMgr.UpdataCrtProjectName(wstrName);
		gSystemMgr.m_crtProject = gConfigFileMgr.GetCurrentProjectConfig();
		gSystemMgr.m_wstrCrtProjectName = wstrName;
		ResetCountValue();
		//OnStnClickedShutdown();
	}
	//ResetEvent(gSystemMgr.m_hCaptureFinishFlag);
}

void CAOIDlg::OnBnClickedButtonDebug()
{
	CDebugDlg debugDlg;
	debugDlg.m_bStarted = m_bStarted;
	debugDlg.DoModal();
}

void CAOIDlg::OnBnClickedButtonSpc()
{
	CQueryDlg queryDlg;
	queryDlg.DoModal();
}

void CAOIDlg::OnStnClickedShutdown()
{
	if (m_ptrLogDlg != nullptr)
	{
		delete m_ptrLogDlg;
		m_ptrLogDlg = nullptr;
	}
	int nRet = 0;
	UAC crtUser = gSystemMgr.GetCurrentUser();
	if (crtUser.m_uacLevel > 0)
	{
		ShutDownStation();
		gSystemMgr.StopWorkerThread();
		gSystemLogger.FlushLogCache();
		gConfigFileMgr.SaveCurrentProjectName();
		DeleteAllImageWindow();
		if (NULL != m_ptrDefectDlg)
		{
			delete m_ptrDefectDlg;
			m_ptrDefectDlg = NULL;
		}
		CloseExternalProgram(L"PGDetect.exe");
		CDialogEx::OnOK();
	}
}

void CAOIDlg::OnStnClickedLogin()
{
	CSystemLoginDlg sysLoginDlg;
	UAC crtUser = gSystemMgr.GetCurrentUser();
	if (crtUser.m_uacLevel > 0) // Log out
	{
        m_btnShutdown.EnableWindow(FALSE);
		m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN_DENY);
		m_btnShutdown.Invalidate();

		m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG_DENY);
		m_btnSysConfig.EnableWindow(FALSE);
		m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE_DENY);
		m_btnLanguage.EnableWindow(FALSE);
		m_btnLogon.SetLabelImage(IMG_PNG_LOG_OUT);
		m_btnUAC.SetLabelImage(IMG_PNG_UAC_DENY);
		m_btnUAC.EnableWindow(FALSE);
		m_btnDebug.EnableWindow(FALSE);
		m_comboProject.EnableWindow(FALSE);
		CString UserLog;
		UserLog.Format(_T("User %s is logging out"), crtUser.m_strUsername.c_str());
		wstring UserLogOut = UserLog;
		gSystemLogger.QueueUpLog(UserLogOut);
		UAC opLevel;
		gSystemMgr.SetCurrentUser(opLevel);
	}
	else
	{
		if (IDOK == sysLoginDlg.DoModal())
		{
			UAC crtUser = gSystemMgr.GetCurrentUser();
			if (crtUser.m_uacLevel == ADMIN)
			{
				m_btnUAC.SetLabelImage(IMG_PNG_UAC_DENY);
				m_btnUAC.EnableWindow(FALSE);

				m_btnShutdown.EnableWindow(TRUE);
				m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN);
				m_btnShutdown.Invalidate();
				m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG);
				m_btnSysConfig.EnableWindow(TRUE);
				m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE);
				m_btnLanguage.EnableWindow(TRUE);
				m_btnLogon.SetLabelImage(IMG_PNG_LOG_ON);
				m_btnDebug.EnableWindow(TRUE);
				m_comboProject.EnableWindow(TRUE);
				
			}
			if (crtUser.m_uacLevel == SADMIN)
			{
				m_btnUAC.SetLabelImage(IMG_PNG_UAC);
				m_btnUAC.EnableWindow(TRUE);

				m_btnShutdown.EnableWindow(TRUE);
				m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN);
				m_btnShutdown.Invalidate();
				m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG);
				m_btnSysConfig.EnableWindow(TRUE);
				m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE);
				m_btnLanguage.EnableWindow(TRUE);
				m_btnLogon.SetLabelImage(IMG_PNG_LOG_ON);
				m_btnDebug.EnableWindow(TRUE);
				m_comboProject.EnableWindow(TRUE);
				
			}
			if (crtUser.m_uacLevel == OPERATOR)
			{
				m_btnUAC.SetLabelImage(IMG_PNG_UAC_DENY);
				m_btnUAC.EnableWindow(FALSE);

				m_btnShutdown.EnableWindow(FALSE);
				m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN_DENY);
				m_btnShutdown.Invalidate();
				m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG_DENY);
				m_btnSysConfig.EnableWindow(FALSE);
				m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE_DENY);
				m_btnLanguage.EnableWindow(FALSE);
				m_btnLogon.SetLabelImage(IMG_PNG_LOG_ON);
				m_btnDebug.EnableWindow(FALSE);
				m_comboProject.EnableWindow(FALSE);
				
		   }
		}
	}
	m_btnLogon.Invalidate();
	m_btnUAC.Invalidate();
	m_btnSysConfig.Invalidate();
	m_btnLanguage.Invalidate();
}

void CAOIDlg::OnStnClickedSystemConfig()
{
	gSystemMgr.m_bProjectChanged = false;
	CSystemConfigDlg sysConfigDlg;
	sysConfigDlg.DoModal();
	if (gSystemMgr.m_bProjectChanged)
	{
		ListAllProject();
	}
}

void CAOIDlg::OnStnClickedUAC()
{
	//TODO:
	CSystemUserManagementDlg sysUserManagermentDlg;
	sysUserManagermentDlg.DoModal();
}

void CAOIDlg::OnStnClickedHelp()
{
	CHelpDlg helpDlg;
	helpDlg.DoModal();
}

void CAOIDlg::OnStnClickedLanguage()
{
	//TODO:
}
void CAOIDlg::OnStnClickedLogViewer()
{
	if (m_ptrLogDlg != nullptr)
	{
		delete m_ptrLogDlg;
		m_ptrLogDlg = nullptr;
	}
	if (m_ptrLogDlg == nullptr)
	{
		m_ptrLogDlg = new CLogViewerDlg;
		m_ptrLogDlg->Create(IDD_DIALOG_LOGVIEWER, this);
	}
	m_ptrLogDlg->ShowWindow(SW_SHOW);
}

LRESULT CAOIDlg::OnUpdateStatistics(WPARAM wParam, LPARAM lParam)
{
	CString sValue;
	sValue.Format(_T("%d"), gSystemMgr.m_nTotal);
	m_labelTotalValue.SetWindowTextW(sValue);

	sValue.Format(_T("%d / %.1f"), gSystemMgr.m_nOkCount, 1.0f*gSystemMgr.m_nOkCount / gSystemMgr.m_nTotal*100);
	sValue.Append(_T("%"));
	m_lableOkCountValue.SetWindowTextW(sValue);
	m_lableCrtTimingValue.SetWindowTextW(gSystemMgr.m_wstrCrtTiming.c_str());
	m_lableAvgTimingValue.SetWindowTextW(gSystemMgr.m_wstrAvgTiming.c_str());
	m_labelTotalValue.Invalidate();
	m_lableOkCountValue.Invalidate();
	m_lableCrtTimingValue.Invalidate();
	m_lableAvgTimingValue.Invalidate();
	return 0;
}

void CAOIDlg::SetSystemStatus()
{
	m_nSysStatusFlag = STATUS_OK;
	for (auto it = m_dictStationIndex_Ptr.begin(); it != m_dictStationIndex_Ptr.end(); it++)
	{
		int nRetFlag = it->second->GetStationStatus();
		if (nRetFlag != STATUS_OK)
		{
			m_nSysStatusFlag = nRetFlag;
			return ;
		}
	}
}

void CAOIDlg::ControlStationCameras(bool bRun)
{
	for (auto it = m_dictStationIndex_Ptr.begin(); it != m_dictStationIndex_Ptr.end(); it++)
	{
		it->second->ControlCamera(bRun);
		std::map<wstring, CAMERA_SETTING> dictCameraSet=it->second ->GetStationCameras();
		int i = 0;
		for (auto it= dictCameraSet.begin(); it!= dictCameraSet.end();it++)
		{
			m_dictCameraName[i] = it->first;
			i++;
		}
	}
}

void CAOIDlg::ListAllProject()
{
	m_comboProject.ResetContent();
	std::map<int,STATION_CONFIG>* ptrDictRet = gConfigFileMgr.GetStaionConfigArray();
	if (!ptrDictRet->empty())
	{
		STATION_CONFIG config = ptrDictRet->begin()->second;
		wstring wstrCrtProjectName = config.m_wstrCrtProjectName;
		int nCrtIndex = 0;
		int nIndex = -1;
		for (auto it = config.m_dictProject.begin(); it != config.m_dictProject.end(); it++)
		{
			nIndex++;
			wstring wstrName = it->first;
			m_comboProject.AddString(wstrName.c_str());
			if (wstrName == wstrCrtProjectName)
			{
				nCrtIndex = nIndex;
			}
		}
		m_comboProject.SetCurSel(nCrtIndex);
	}
}

LRESULT CAOIDlg::OnReceiveSystemState(WPARAM wParam, LPARAM lParam)
{
	m_nBtnState = boost::lexical_cast<int>(wParam);

	switch (m_nBtnState)
	{
	   case RUN_STATE:
		   m_bStarted = true;
		   m_btnOnOff.SetLabelImage(IMG_PNG_RUN);
	       break;
	   case WRONG_RESULT:
		   m_bStarted = true;
		   m_btnOnOff.SetLabelImage(IMG_PNG_RUN);
		   break;
	   case STOP_STATE:
		   m_bStarted = false;
		   gSystemMgr.SetStartedFlag(m_bStarted);
		   m_btnOnOff.SetLabelImage(IMG_PNG_ERROR);
	       break;
	   case READY_STATE:
		   m_bStarted = false;
		   gSystemMgr.SetStartedFlag(m_bStarted);
		   m_btnOnOff.SetLabelImage(IMG_PNG_STOP);
	       break;
	   case INITIAL_STATE:
		   m_bStarted = false;
		   gSystemMgr.SetStartedFlag(m_bStarted);
		   m_btnOnOff.SetLabelImage(IMG_PNG_STOP);
	       break;
	   case ALARM_STATE:
		   m_bStarted = false;
		   gSystemMgr.SetStartedFlag(m_bStarted);
		   m_btnOnOff.SetLabelImage(IMG_PNG_ERROR);
		   break;
	   default:
		   break;
	}
	ControlStationCameras(m_bStarted);
	m_btnOnOff.Invalidate();
	return 0;
}

LRESULT CAOIDlg::OnReceiveSystemLog(WPARAM wParam, LPARAM lParam)
{
	CString cstrp = (BSTR)wParam;
	SysFreeString((BSTR)wParam);
	m_eLevel = (eEVENT_LEVEL)lParam;
	wstring wstrLogMsg = cstrp.GetString();
	std::vector<wstring> vLogItem = localUtils::utils::SplitString(wstrLogMsg, L"$");
	if (vLogItem.size() != 3)
	{
		return 0;
	}
	wstring wstrTime = vLogItem[0];
	wstring wstrMsg  = vLogItem[2];
	int indexItem = m_logList.GetItemCount();
    m_logList.InsertItem(0, L"");
	m_logList.SetItemText(0, 1, wstrTime.c_str());
	m_logList.SetItemText(0, 2, wstrMsg.c_str());
	EVENT_PACKAGE eventPackage;
	eventPackage.m_level = m_eLevel;
	eventPackage.m_wstrTimestamp = wstrTime;
	eventPackage.m_wstrDescription = wstrMsg;
	m_eventBuffer.push_back(eventPackage);
	m_logList.Invalidate();
	if (indexItem >= 100)
	{
		m_logList.DeleteItem(100);
	}
	return 0;
}

LRESULT CAOIDlg::OnDisplayResultInfo(WPARAM wParam, LPARAM lParam)
{
	m_logDetectResult.DeleteAllItems();
	m_resultEventBuffer.clear();
	m_dictPatternsDefectForDisp.clear();
	BASEINFO curInfo = gSystemMgr.m_cfResultInfo.back();
	map<string, string> commInfo = curInfo.m_commInfo;
	auto ptrDictSeqImg = gConfigFileMgr.GetPgSeqSetting();
	bool bResultOK = false;
	for(auto it= commInfo.begin();it!= commInfo.end();it++)//commom info of this screen,(commInfo ={"TaskID":"任务编号","MaterielID":"物料编号","Status":"OK/NG"})
	{
		m_logDetectResult.InsertItem(0, L"");
		CString str1 = CA2T((it->first).c_str());
		m_logDetectResult.SetItemText(0, 1, str1);
		CString str2 = CA2T((it->second ).c_str());
		m_logDetectResult.SetItemText(0, 2, str2);
		gSystemMgr.m_detectLogger.QueueUpLog((str1 +L":"+ str2).GetString());
		gSystemMgr.m_detectLogger.FlushLogCache();
		EVENT_PACKAGE eventPackage;
		eventPackage.m_wstrTimestamp = str1;
		eventPackage.m_wstrDescription = str2;
		if (str1 == L"Status")
		{
			
			COLORREF resultColor;
			if (str2 == L"OK")
			{
				eventPackage.m_level = LV_OK;
				resultColor=RGB(0, 255, 0);
				bResultOK = true;
			}
			else
			{
				eventPackage.m_level = LV_ERROR;
				resultColor = RGB(255, 0, 0);
				bResultOK = false;
			}
			m_labelResult.SetColor(resultColor, m_bgColor);
			m_labelResult.SetWindowTextW(str2);
			m_labelResult.Invalidate();
		}
		else if (str1 == L"MaterielID")
		{
			m_edtScreenID.SetWindowTextW(str2);
			m_edtScreenID.Invalidate();
			//m_labelScreenID.SetWindowTextW(str2);
			m_labelScreenID.Invalidate();
			eventPackage.m_level = LV_EVENT;
		}
		else
		{
			eventPackage.m_level = LV_EVENT;
		}
		m_resultEventBuffer.push_back(eventPackage);
		m_logDetectResult.Invalidate();
	}
	//if (!bResultOK)//defect exist
	{
		std::map<int, vector<DEFECTINFO>>    dictDefectsForPattern;
		std::map<string, vector<DEFECTINFO>> dictDefectsInfo = curInfo.m_defectsInfo;
		vector<DEFECTINFO> vDefectsInfo = curInfo.m_defectsInfo["Defects"];//all patterns defects info
		for (auto it = vDefectsInfo.begin(); it != vDefectsInfo.end(); it++)//select one  defects info
		{
			int nCrtPatternIndex = -1;
			DEFECTINFO crtDefectInfo = (*it); //every defect info 
			std::map<string, string> defCommInfo = crtDefectInfo.defCommInfo;
			nCrtPatternIndex = gSystemMgr.PgPatternNameMap(localUtils::utils::Utf8ToUnicode(defCommInfo["Frame"]));
			dictDefectsForPattern[nCrtPatternIndex].push_back(crtDefectInfo);//devide every defect into every pattern
		}
		for (int i = 0; i < int(gSystemMgr.m_crtProject->m_vPattern.size()); i++)
		{
			if (dictDefectsForPattern.find(i) != dictDefectsForPattern.end())//this pattern has defect
			{
				int nIndex = 0;
					int nCrePatternIndex = -1;
					CString strCrtPatternDefects = L"";
					for (auto it2 = dictDefectsForPattern[i].begin(); it2 != dictDefectsForPattern[i].end(); it2++, nIndex++)
					{
						CString strSingleDefect = L"";
						CString strPartSingleDefect = L"";
						CString strPatternName = CA2T(it2->defCommInfo["Frame"].c_str());
						nCrePatternIndex = gSystemMgr.PgPatternNameMap(strPatternName.GetString());
						strPatternName=(*ptrDictSeqImg)[nCrePatternIndex].m_wstrType.c_str();
						CString strPatternIndex;
						strPatternIndex.Format(L"%d", nCrePatternIndex);
						if (0 == nIndex)
						{
							m_logDetectResult.InsertItem(0, L"");
							strPatternName += L"(" + strPatternIndex + L")";
							m_logDetectResult.SetItemText(0, 1, strPatternName);//pattern name(pattern index)
							gSystemMgr.m_detectLogger.QueueUpLog(strPatternName.GetBuffer());
							gSystemMgr.m_detectLogger.FlushLogCache();
						}
						strPartSingleDefect.Format(L"缺陷 %d:{X=%.2f", nIndex + 1, boost::lexical_cast<float>(it2->defSpeInfo["PicPos"]["X"]));
						strSingleDefect.Append(strPartSingleDefect);
						strSingleDefect.Append(L";");
						strPartSingleDefect.Format(L"Y=%.2f", boost::lexical_cast<float>(it2->defSpeInfo["PicPos"]["Y"]));
						strSingleDefect.Append(strPartSingleDefect);
						strSingleDefect.Append(L";");
						strPartSingleDefect.Format(L"Width=%.2f", boost::lexical_cast<float>(it2->defSpeInfo["PicPos"]["Width"]));
						strSingleDefect.Append(strPartSingleDefect);
						strSingleDefect.Append(L";");
						strPartSingleDefect.Format(L"Height=%.2f", boost::lexical_cast<float>(it2->defSpeInfo["PicPos"]["Height"]));
						strSingleDefect.Append(strPartSingleDefect);
						strSingleDefect.Append(L"}-");
						int nRegionIndex = JudgeDefectRegionNumber(it2->defSpeInfo["PicPos"]);
						if (-1 != nRegionIndex)
						{
							strSingleDefect.AppendFormat(L"区域%d;", nRegionIndex);
						}
						strCrtPatternDefects.Append(strSingleDefect);
						m_dictPatternsDefectForDisp[nCrePatternIndex][nIndex] = strSingleDefect.GetString();
					}
					m_logDetectResult.SetItemText(0, 2, strCrtPatternDefects);//display one defSpeInfo of one pattern
					EVENT_PACKAGE eventPackage;
					eventPackage.m_wstrTimestamp = L"";
					eventPackage.m_wstrDescription = strCrtPatternDefects.GetString();
					eventPackage.m_level = LV_ERROR;
					m_resultEventBuffer.push_back(eventPackage);
					m_logDetectResult.Invalidate();
					gSystemMgr.m_detectLogger.QueueUpLog(strCrtPatternDefects.GetBuffer());
					gSystemMgr.m_detectLogger.FlushLogCache();
				}
			else//this pattern has no defect
			{
				CString strOkPatternIndex;
				strOkPatternIndex.Format(L"%d",i);
				CString strOkPatternName = (*ptrDictSeqImg)[i].m_wstrType.c_str();
				m_logDetectResult.InsertItem(0, L"");
				strOkPatternName += L"(" + strOkPatternIndex + L")";
				m_logDetectResult.SetItemText(0, 1, strOkPatternName);//pattern name(pattern index)
				m_logDetectResult.SetItemText(0, 2, L"OK");

				EVENT_PACKAGE eventPackage;
				eventPackage.m_wstrTimestamp = L"";
				eventPackage.m_wstrDescription = (strOkPatternName + L":OK").GetString();
				eventPackage.m_level = LV_OK;
				m_resultEventBuffer.push_back(eventPackage);
				m_logDetectResult.Invalidate();
				gSystemMgr.m_detectLogger.QueueUpLog(eventPackage.m_wstrDescription);
				gSystemMgr.m_detectLogger.FlushLogCache();
			}
		}
	}	
	//else//no defect
	//{
	//	m_dictPatternsDefectForDisp.clear();
	//	CString strPatternName;
	//	CString strPatternIndex;
	//	for (int i = 0; i < gSystemMgr.m_crtProject->m_vPattern.size(); i++)
	//	{
	//		strPatternIndex.Format(L"%d", i);
	//		strPatternName=(*ptrDictSeqImg)[i].m_wstrType.c_str();
	//		m_logDetectResult.InsertItem(0, L"");
	//		strPatternName += L"(" + strPatternIndex + L")";
	//		m_logDetectResult.SetItemText(0, 1, strPatternName);//pattern name(pattern index)
	//		m_logDetectResult.SetItemText(0, 2, L"OK");
	//		EVENT_PACKAGE eventPackage;
	//		eventPackage.m_wstrTimestamp = L"";
	//		eventPackage.m_wstrDescription =( strPatternName+L":OK").GetString();
	//		eventPackage.m_level = LV_OK;
	//		m_resultEventBuffer.push_back(eventPackage);
	//		m_logDetectResult.Invalidate();
	//		gSystemMgr.m_detectLogger.QueueUpLog(eventPackage.m_wstrDescription);
	//		gSystemMgr.m_detectLogger.FlushLogCache();
	//	}
	//}
	int indexItem = m_logDetectResult.GetItemCount();
	if (indexItem >= 100)
	{
		m_logDetectResult.DeleteItem(100);
	}
	return 0;
}


void CAOIDlg::OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* lplvcd = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	int iRow = int(lplvcd->nmcd.dwItemSpec);
	if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		 return;
	} 
    if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		lplvcd->clrTextBk = RGB(200, 220, 250);
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}
	else if (lplvcd->nmcd.dwDrawStage == (CDDS_SUBITEM |CDDS_PREPAINT|CDDS_ITEMPREPAINT))
	{
		COLORREF crText;
		if (lplvcd->iSubItem == 2)
		{
			EVENT_PACKAGE eventPackage;
			eventPackage = m_eventBuffer.at(m_logList.GetItemCount() - iRow - 1);
			switch (eventPackage.m_level)
			{
				case( LV_ERROR):
				    crText = RGB(250,0,0);
				    break;
				case(LV_WARNING):
				    crText = RGB(150, 150, 0);
				    break;
				case(LV_EVENT):
				    crText = RGB(50, 50, 50);
				    break;
				default:
					break;
			}
				lplvcd->clrText = crText;
				*pResult = CDRF_DODEFAULT;
				return;
		}
	}
	*pResult = 0;
}

void CAOIDlg::OnNMCustomdrawListResultInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	NMLVCUSTOMDRAW* lplvcd = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	int iRow = int(lplvcd->nmcd.dwItemSpec);
	if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		lplvcd->clrTextBk = RGB(200, 220, 250);
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}
	else if (lplvcd->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEMPREPAINT))
	{
		COLORREF crText;
		if (lplvcd->iSubItem == 2)
		{
			EVENT_PACKAGE eventPackage;
			eventPackage = m_resultEventBuffer.at(m_logDetectResult.GetItemCount() - iRow - 1);
			switch (eventPackage.m_level)
			{
			case(LV_ERROR):
				crText = RGB(250, 0, 0);
				break;
			case(LV_WARNING):
				crText = RGB(150, 150, 0);
				break;
			case(LV_EVENT):
				crText = RGB(50, 50, 50);
				break;
			case(LV_OK):
				crText = RGB(0, 250,0);
				break;
			default:
				break;
			}
		    lplvcd->clrText = crText;
			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
	*pResult = 0;
}

LRESULT CAOIDlg::OnDisplayImageOnMainScreen(WPARAM wParam, LPARAM lParam)
{
	if (WINDOW_MAIN_FINAL_DISP == wParam&&NULL == lParam)//final result display
	{
		Mat tmpMat1, tmpMat2;
		tmpMat1 = gSystemMgr.m_finalDispSrc.clone();
		tmpMat2 = gSystemMgr.m_finalDispImg.clone();
		if (tmpMat1.data&&tmpMat2.data)
		{
			IplImage pCrtImg1(tmpMat1);
			m_cImgWndMainLeft->ClearImage();
			m_cImgWndMainLeft->LoadImageDataReset(&pCrtImg1, IMAGE_GRAY8);
			m_cImgWndMainLeft->ShowText(L"主相机", 50, 50, L"Green");
			if (gSystemMgr.m_dBgValue > 0)
			{
				CString strValue;
				strValue.Format(L"平均灰度值=%.3f", gSystemMgr.m_dBgValue);
				m_cImgWndMainLeft->ShowText(strValue, 50, 300, L"Green");
			}
			IplImage pCrtImg2(tmpMat2);
			m_cImgWndMainRight->ClearImage();
			m_cImgWndMainRight->LoadImageDataReset(&pCrtImg2, IMAGE_GRAY8);
			SegmentResultImageRegion(m_cImgWndMainRight, pCrtImg2.width, pCrtImg2.height);

			/*BASEINFO curInfo = gSystemMgr.m_cfResultInfo.back();
			map<string, string> commInfo = curInfo.m_commInfo;
			for (auto it = commInfo.begin(); it != commInfo.end(); it++)
			{

				CString str1 = CA2T((it->first).c_str());
				CString str2 = CA2T((it->second).c_str());
				if (str1 == L"Status")
				{
					if (str2 == L"OK")
					{
						m_cImgWndMainRight->ShowText(L"检测结果：OK", 100, 100, L"Green");
					}
					else
					{
						m_cImgWndMainRight->ShowText(L"检测结果：NG", 100, 100, L"Red");
					}
					return 0;
				}
			}*/
		}
	}

	if (WINDOW_MAIN_RESULT==lParam&&WINDOW_MAIN_GRAY_VALUE == wParam)//display pattern0 gray value
			{
				if (gSystemMgr.m_dBgValue > 0)
				{
					CString strValue;
					strValue.Format(L"平均灰度值=%.3f", gSystemMgr.m_dBgValue);
					m_cImgWndMainLeft->ShowText(strValue, 50, 250, L"Green");
				}
			}

	if (NULL != wParam&&wParam != WINDOW_MAIN_RESULT&&wParam != WINDOW_MAIN_GRAY_VALUE
		&&wParam != WINDOW_MAIN_CLEAR&&wParam !=WINDOW_MAIN_FINAL_DISP&&wParam != WINDOW_MAIN_SELECTED_ONE_DEFECT
		&&lParam != WINDOW_MAIN_RESULT)//not debug mode,src image(camera name ,if main camera)
			{
				CString strCameraName = (BSTR)wParam;
				SysFreeString((BSTR)wParam);
				if (gSystemMgr.m_dictImageSource.find(strCameraName.GetString()) != gSystemMgr.m_dictImageSource.end())
				{
					IplImage pCrtImg(gSystemMgr.m_dictImageSource[strCameraName.GetString()].m_image);
					eCAMERA_IMAGE_TYPE eImgType = gSystemMgr.m_dictImageSource[strCameraName.GetString()].m_eType;
					CImgShowWnd* cImgWnd = NULL;
					CString strWndName = L"";
					if (true == (bool)lParam)//main camera
					{
						cImgWnd = m_cImgWndMainLeft;
						strWndName = L"主相机";
					}
					else//sub camera
					{
						CString strCamIndex = strCameraName.Right(1);
						if (strCamIndex == L"1")
						{
							cImgWnd = m_cImgWndSub1;//(up)
							strWndName = L"斜拍相机1（上）";
						}
						else if (strCamIndex == L"2")
						{
							cImgWnd = m_cImgWndSub2;//(right)
							strWndName = L"斜拍相机2（右）";
						}
						else if (strCamIndex == L"3")
						{
							cImgWnd = m_cImgWndSub3;//(down)
							strWndName = L"斜拍相机3（下）";
						}
						else if (strCamIndex == L"4")
						{
							cImgWnd = m_cImgWndSub4;//(left)
							strWndName = L"斜拍相机4（左）";
						}
					}
					if (NULL != cImgWnd)
					{
						cImgWnd->ClearImage();
						cImgWnd->LoadImageDataReset(&pCrtImg, eImgType);
						cImgWnd->ShowText(strWndName, 50, 50, L"Green");
					}
				}
				return 0;
			}

	if (NULL != wParam&&wParam != WINDOW_MAIN_RESULT&&wParam != WINDOW_MAIN_GRAY_VALUE
		&&wParam != WINDOW_MAIN_CLEAR&&wParam != WINDOW_MAIN_FINAL_DISP&&wParam != WINDOW_MAIN_SELECTED_ONE_DEFECT
		&&lParam == WINDOW_MAIN_RESULT)//not debug mode,result image(camera name ,reult image flag)
	{
		CString strCameraName = (BSTR)wParam;
		SysFreeString((BSTR)wParam);
		if (gSystemMgr.m_dictResultImg.find(strCameraName.GetString()) != gSystemMgr.m_dictResultImg.end())
		{
			if (gSystemMgr.m_dictResultImg[strCameraName.GetString()].m_image.data)
			{
				IplImage pCrtImg(gSystemMgr.m_dictResultImg[strCameraName.GetString()].m_image);
				eCAMERA_IMAGE_TYPE eImgType = gSystemMgr.m_dictResultImg[strCameraName.GetString()].m_eType;
				CImgShowWnd* cImgWnd = NULL;
				CString strWndName = L"";
				{
					CString strCamIndex = strCameraName.Right(1);
					if (strCamIndex == L"1")
					{
						cImgWnd = m_cImgWndSub1;//(up)
						strWndName = L"斜拍相机1（上）";
					}
					else if (strCamIndex == L"2")
					{
						cImgWnd = m_cImgWndSub2;//(right)
						strWndName = L"斜拍相机2（右）";
					}
					else if (strCamIndex == L"3")
					{
						cImgWnd = m_cImgWndSub3;//(down)
						strWndName = L"斜拍相机3（下）";
					}
					else if (strCamIndex == L"4")
					{
						cImgWnd = m_cImgWndSub4;//(left)
						strWndName = L"斜拍相机4（左）";
					}
					else//main camera
					{
						cImgWnd = m_cImgWndMainRight;
						strWndName = L"结果图";
						//SegmentResultImageRegion(m_cImgWndMainRight, pCrtImg.width, pCrtImg.height);
						//m_cImgWndMainRight->ShowText(L"NG", 100, 100, L"Red");
						//m_cImgWndMainRight->ShowRectangle(1000, 2000, 300, 700, L"Red", 2);
						//m_cImgWndMainRight->ShowRectangle(5900, 3300, 1500, 800, L"Green", 2);
					}
				}
				if (NULL != cImgWnd)
				{
					cImgWnd->ClearImage();
					cImgWnd->LoadImageDataReset(&pCrtImg, eImgType);
					cImgWnd->ShowText(strWndName, 50, 50, L"Green");
				}
				return 0;
			}
		}
	}

	if (WINDOW_MAIN_CLEAR == wParam&&NULL == lParam)//clear window image
	{
		for (auto it = m_vImgWindow.begin(); it != m_vImgWindow.end(); it++)
		{
			(*it)->ClearImage();
		}
	}

	if (WINDOW_MAIN_SELECTED_ONE_DEFECT == wParam)
	{
		if (m_cImgWndMainRight->CheckImageLoad())
		{
			if (m_dictPatternsDefectForDisp.find(m_nCrtPatternIndex) != m_dictPatternsDefectForDisp.end())
			{
				std::map<int, wstring> dictSelectPatternDefect = m_dictPatternsDefectForDisp[m_nCrtPatternIndex];
				wstring wstrSelectedDefect = dictSelectPatternDefect[int(lParam)];
				int nStart = (int)wstrSelectedDefect.find(L"{");
				int nEnd = (int)wstrSelectedDefect.find(L"}");
				wstrSelectedDefect = wstrSelectedDefect.substr(nStart + 1, nEnd - nStart - 1);
				vector<wstring> vPosDetail = localUtils::utils::SplitString(wstrSelectedDefect, L";");
				map<wstring, float> dictRectPos;
				for (auto it = vPosDetail.begin(); it != vPosDetail.end(); it++)
				{
					vector<wstring> vPosItem = localUtils::utils::SplitString((*it), L"=");
					if (vPosItem.size() == 2)
					{
						dictRectPos[vPosItem[0]] = boost::lexical_cast<float>(vPosItem[1]);
					}
				}
				m_cImgWndMainRight->ShowRectangle((dictRectPos[L"X"] + dictRectPos[L"Width"] / 2), (dictRectPos[L"Y"] + dictRectPos[L"Height"] / 2),
					abs(dictRectPos[L"Width"]), abs(dictRectPos[L"Height"]), L"Yellow", 1);

			}
		}
	}
	return -1;
}

LRESULT CAOIDlg::OnReceiveVieworksCameraData(WPARAM wParam, LPARAM lParam)
{
	int nSz = int(wParam);
	byte* ptr = (byte*)lParam;
	std::vector<char> vTmp;
	for (int i = 0; i < nSz; i++)
	{
		vTmp.push_back((*(ptr + i)));
	}
	string strVieworksData;
	strVieworksData.insert(strVieworksData.begin(), vTmp.begin(), vTmp.end());
	CString cstrVieworksData(strVieworksData.c_str());
	gSystemLogger.QueueUpLogAndReport(cstrVieworksData.GetString(),LV_WARNING);
	return 0;
}

LRESULT CAOIDlg::OnUpdateStationConfig(WPARAM wParam, LPARAM lParam)
{
	auto it = m_dictStationIndex_Ptr.begin();
	for (int i=0;it!= m_dictStationIndex_Ptr.end();it++,i++)
	{
		(*it).second->m_stationConfig = (*gConfigFileMgr.GetStaionConfigArray())[i];
	}
	return 0;
}

BOOL CAOIDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CAOIDlg::GrantSuperAdmin()
{
    UAC crtUser;
    crtUser.m_uacLevel = SADMIN;
    gSystemMgr.SetCurrentUser(crtUser);
    m_btnSysConfig.SetLabelImage(IMG_PNG_SYS_CONFIG);
    m_btnSysConfig.EnableWindow(TRUE);
    m_btnLanguage.SetLabelImage(IMG_PNG_LANGUAGE);
    m_btnLanguage.EnableWindow(TRUE);
    m_btnUAC.SetLabelImage(IMG_PNG_UAC);
    m_btnUAC.EnableWindow(TRUE);
    m_btnLogon.SetLabelImage(IMG_PNG_LOG_ON);
    m_btnDebug.EnableWindow(TRUE);
    m_comboProject.EnableWindow(TRUE);
    m_btnShutdown.SetLabelImage(IMG_PNG_SHUTDOWN);
    m_btnShutdown.EnableWindow(TRUE);


    m_btnLogon.Invalidate();
    m_btnUAC.Invalidate();
    m_btnSysConfig.Invalidate();
    m_btnLanguage.Invalidate();
    m_btnShutdown.Invalidate();
}
	
void CAOIDlg::ResetCountValue()
{
	wstring wstrCrtProjectName = gSystemMgr.m_wstrCrtProjectName;
	string strProjectName = localUtils::utils::Unicode2Utf8(wstrCrtProjectName);
	strProjectName = gSystemMgr.TransDBTableName(strProjectName);
	if (gSystemMgr.m_sysDatabase.TableExists(strProjectName.c_str()))
	{
		char   sql[1024];

		CString cstrQueryBeginDate(L"2019-02-12 11:00"), cstrQueryEndDate;
		SYSTEMTIME dt;
		::GetLocalTime(&dt);
		cstrQueryEndDate.Format((_T("%d-%.2d-%.2d %.2d:00")), dt.wYear, dt.wMonth, dt.wDay, dt.wHour);
		USES_CONVERSION;
		char* strQueryBeginDate = T2A(cstrQueryBeginDate);
		char* strQueryEndDate = T2A(cstrQueryEndDate);
		sprintf_s(sql, SQL_QUERY_RECORD_DATA, strProjectName.c_str(), strQueryBeginDate, strQueryEndDate);
		CppSQLite3Query QueryData = gSystemMgr.m_sysDatabase.Query(sql);

		int nTotal(0), nOkCount(0);
		double dTotalTime(0.0);
		for (int i = 0; !QueryData.eof(); i++)
		{
			const char*  charTotal = QueryData.fieldValue("total");
			const char*  charOkCount = QueryData.fieldValue("OkCount");
			const char*  charCrtTime = QueryData.fieldValue("CrtTime");

			nTotal = atoi(charTotal);
			nOkCount = atoi(charOkCount);

			double dTempTime = atof(charCrtTime);
			dTotalTime += dTempTime;
			QueryData.NextRow();
		}
		gSystemMgr.m_nTotal = nTotal;
		gSystemMgr.m_dTotalTiming = dTotalTime;
		gSystemMgr.m_nOkCount = nOkCount;
	}
	else
	{
		gSystemMgr.m_nTotal = 0;
		gSystemMgr.m_dTotalTiming = 0.0;
		gSystemMgr.m_nOkCount = 0;
	}
}

void CAOIDlg::OnRClickListBoxLog(NMHDR * pNMHDR, LRESULT * pResult)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_logList.ScreenToClient(&point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;
    m_logList.SubItemHitTest(&lvinfo);
	m_strCopyLog = m_logList.GetItemText(lvinfo.iItem, lvinfo.iSubItem);
	if (m_strCopyLog != _T(""))
	{
		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		menu.LoadMenu(IDR_MENU_LOGCOPY);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN, pt.x, pt.y, this);
	}
}

void CAOIDlg::OnDbClickResultListBox(NMHDR * pNMHDR, LRESULT * pResult)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_logDetectResult.ScreenToClient(&point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;
	m_logDetectResult.SubItemHitTest(&lvinfo);
	m_strPatternIndex = m_logDetectResult.GetItemText(lvinfo.iItem, 1);
	int nStart = m_strPatternIndex.Find(L"(");
	int nEnd= m_strPatternIndex.Find(L")");
	CString strIndex= m_strPatternIndex.Mid(nStart+1, nEnd-nStart-1);
	if (m_strPatternIndex != _T("")&& -1!= nStart && -1 != nEnd&&m_strPatternIndex&&localUtils::utils::IsNumericString(strIndex))
	{
		m_labelScreenName.SetWindowTextW(strIndex);
		m_labelScreenName.Invalidate();
		m_nCrtPatternIndex = _ttoi(strIndex);
		m_cImgWndMainRight->ClearImage();
		Mat resultMat = gSystemMgr.m_dictResultImg2[m_nCrtPatternIndex].clone();
		if (resultMat.data)
		{
			IplImage pImg(resultMat);
			m_cImgWndMainRight->LoadImageDataReset(&pImg, IMAGE_RGB8);
			SegmentResultImageRegion(m_cImgWndMainRight, pImg.width, pImg.height);
		}
		if (m_dictPatternsDefectForDisp.find(m_nCrtPatternIndex) != m_dictPatternsDefectForDisp.end())
		{
			if (m_cImgWndMainRight->CheckImageLoad())
			{
				CString strResultFlag=L"结果图";
				strResultFlag.AppendFormat(L"（%d）:NG", m_nCrtPatternIndex);
				m_cImgWndMainRight->ShowText(strResultFlag, 100, 100, L"Red");
				map<int, wstring> dictPatternDefectInfo = m_dictPatternsDefectForDisp[m_nCrtPatternIndex];
				CString strDefectNum;
				strDefectNum.Format(L"%d", (int)dictPatternDefectInfo.size());
				m_labelDefectNum.SetWindowTextW(strDefectNum);
				m_labelDefectNum.Invalidate();
				int nDefectIndex=0;
				for (auto it = dictPatternDefectInfo.begin(); it != dictPatternDefectInfo.end(); it++,nDefectIndex++)
				{
					wstring strDefectItem = it->second;
					if (strDefectItem.size() > 3)
					{
						int nStart =(int)strDefectItem.find(L"{");
						int nEnd = (int)strDefectItem.find(L"}");
						strDefectItem=strDefectItem.substr(nStart + 1, nEnd - nStart - 1);
						vector<wstring> vPosDetail = localUtils::utils::SplitString(strDefectItem, L";");
						map<wstring,float> dictRectPos;
						for (auto it1 = vPosDetail.begin(); it1 != vPosDetail.end(); it1++)
						{
							vector<wstring> vPosItem= localUtils::utils::SplitString((*it1), L"=");
							if (vPosItem.size() == 2)
							{
								dictRectPos[vPosItem[0]] =boost::lexical_cast<float>(vPosItem[1]);
							}
						}
						m_cImgWndMainRight->ShowRectangle((dictRectPos[L"X"] + dictRectPos[L"Width"]/2), (dictRectPos[L"Y"] + dictRectPos[L"Height"]/2), 
							abs(dictRectPos[L"Width"]), abs(dictRectPos[L"Height"]), L"Red", 1);
						strDefectNum.Format(L"%d", nDefectIndex + 1);
						m_cImgWndMainRight->ShowText(strDefectNum, (dictRectPos[L"X"]-200), (dictRectPos[L"Y"]-200),L"Red");
					}
				}
			}
			else
			{
				m_labelDefectNum.SetWindowTextW(L"0");
				m_labelDefectNum.Invalidate();
			}
		}
		else
		{
			CString strResultFlag = L"结果图";
			strResultFlag.AppendFormat(L"（%d）:OK", m_nCrtPatternIndex);
			m_cImgWndMainRight->ShowText(strResultFlag, 100, 100, L"Green");
			m_labelDefectNum.SetWindowTextW(L"0");
			m_labelDefectNum.Invalidate();
		}
	}
}

void CAOIDlg::OnRClickResultListBox(NMHDR * pNMHDR, LRESULT * pResult)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_logDetectResult.ScreenToClient(&point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;
	m_logDetectResult.SubItemHitTest(&lvinfo);
	CString strPatternIndex = m_logDetectResult.GetItemText(lvinfo.iItem, 1);
	int nStart = strPatternIndex.Find(L"(");
	int nEnd = strPatternIndex.Find(L")");
	CString strIndex = strPatternIndex.Mid(nStart + 1, nEnd - nStart - 1);
	if (strPatternIndex != _T("") && -1 != nStart && -1 != nEnd&&strPatternIndex&&localUtils::utils::IsNumericString(strIndex))
	{
		if (m_dictPatternsDefectForDisp.find(_ttoi(strIndex)) != m_dictPatternsDefectForDisp.end()&& strPatternIndex != L"OK")
		{
			if (NULL == m_ptrDefectDlg)
			{
				m_ptrDefectDlg = new CDispResultInfoDlg;
				m_ptrDefectDlg->Create(IDD_DIALOG_DEFECTS_DISPLAY, this);
			}
			if(m_ptrDefectDlg->m_listBox.GetCount()>0)
			{
				m_ptrDefectDlg->m_listBox.ResetContent();
			}
			m_ptrDefectDlg->ShowWindow(SW_SHOW);
			CRect rect;
			rect.top = m_rcResultList.top;
			rect.bottom = m_rcResultList.bottom;
			rect.left = m_rcResultList.right;
			rect.right = m_rcWndSub1.right;
			m_ptrDefectDlg->MoveWindow(rect, FALSE);

			std::map<int, wstring> dictPatternDefect = m_dictPatternsDefectForDisp[_ttoi(strIndex)];
			for (auto it = dictPatternDefect.begin(); it != dictPatternDefect.end(); it++)
			{
				m_ptrDefectDlg->m_listBox.AddString((it->second).c_str());
			}
		}
		else
		{
			if (NULL != m_ptrDefectDlg&&m_ptrDefectDlg->GetSafeHwnd())
			{
				m_ptrDefectDlg->ShowWindow(SW_HIDE);
			}
		}
	}
	else
	{
		if (NULL != m_ptrDefectDlg&&m_ptrDefectDlg->GetSafeHwnd())
		{
			m_ptrDefectDlg->ShowWindow(SW_HIDE);
		}
	}
}

void CAOIDlg::OnCopylog()
{
	if (!OpenClipboard())
	{
		//OpenClipboard();
	}
	HGLOBAL hClipbuffer;
	char * cBuffer;
	EmptyClipboard();
	USES_CONVERSION;
	const char* cSrc = T2A(m_strCopyLog);
	size_t nLen = strlen(cSrc) + 1;
	hClipbuffer = GlobalAlloc(GMEM_DDESHARE, nLen);
	cBuffer = (char*)GlobalLock(hClipbuffer);
	strcpy_s(cBuffer,nLen, cSrc);
	GlobalUnlock(hClipbuffer);
	SetClipboardData(CF_TEXT, hClipbuffer);
	CloseClipboard();
}

void CAOIDlg::CloseExternalProgram(CString strProgram)
{
	HANDLE handleList;
	HANDLE handleClose;
	handleList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 *processInfo;
	processInfo = new PROCESSENTRY32;
	processInfo->dwSize = sizeof(PROCESSENTRY32);
	Process32First(handleList, processInfo);
	while (Process32Next(handleList, processInfo) != FALSE)
	{
		CString strTmp = processInfo->szExeFile;
		if (strProgram.CompareNoCase(processInfo->szExeFile) == 0)
		{
			handleClose = OpenProcess(PROCESS_TERMINATE, FALSE, processInfo->th32ProcessID);
			TerminateProcess(handleClose, 0);
		}
	}
	delete processInfo;
	CloseHandle(handleList);
}