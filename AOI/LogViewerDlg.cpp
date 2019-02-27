#include "stdafx.h"
#include "AOI.h"
#include "LogViewerDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CLogViewerDlg, CDialogEx)

CLogViewerDlg::CLogViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOGVIEWER, pParent)
	, m_nLevel(0)
	, m_nLogSource(0)
	, m_eventBuffer(200)
	, m_strCopyLog("")
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
	m_logFont.CreateFont(int(16 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_bgImg.LoadBitmap(IDB_BITMAP_SYS_CONFIG_DLG_BK);
	m_penFrame.CreatePen(PS_SOLID, 6, RGB(200, 220, 250));
	m_penTitle.CreatePen(PS_SOLID, 3, RGB(4, 40, 94));
}

CLogViewerDlg::~CLogViewerDlg()
{
}

void CLogViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_DATETIMEPICKER_LOGQUERY, m_LogDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDTIME, m_EndTime);
	DDX_Radio(pDX, IDC_EVENT, m_nLevel);
	DDX_Radio(pDX, IDC_SYSTEAM, m_nLogSource);
	CDialogEx::DoDataExchange(pDX);
}


void CLogViewerDlg::DisplayBackgroundImage(CPaintDC& dc)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	BITMAP bitmap;
	m_bgImg.GetBitmap(&bitmap);
	CBitmap* pPngOld = dcMem.SelectObject(&m_bgImg);
	long nWidth = (rect.Width() < bitmap.bmWidth ? rect.Width() : bitmap.bmWidth);
	long nHeight = (rect.Height() < bitmap.bmHeight ? rect.Height() : bitmap.bmHeight);
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, nWidth, nHeight, SRCCOPY);
}

BOOL CLogViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	gSystemLogger.m_hViewHandle = this->GetSafeHwnd();
	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_CONFIG_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_CONFIG_UI_HEIGHT * fRatioV);

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
BEGIN_MESSAGE_MAP(CLogViewerDlg, CDialogEx)
	ON_STN_CLICKED(BTN_CLOSE, &CLogViewerDlg::OnStnClickedClose)
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_EVENT, &CLogViewerDlg::OnRadioBtnLevelClicked)
	ON_BN_CLICKED(IDC_WARNING, &CLogViewerDlg::OnRadioBtnLevelClicked)
	ON_BN_CLICKED(IDC_ERROR, &CLogViewerDlg::OnRadioBtnLevelClicked)
	ON_BN_CLICKED(IDC_SYSTEAM, &CLogViewerDlg::OnRadioBtnSourceClicked)
	ON_BN_CLICKED(IDC_DETECT_RESULT, &CLogViewerDlg::OnRadioBtnSourceClicked)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_LOGVIEWER, &CLogViewerDlg::OnNMCustomdrawListLog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOGVIEWER, &CLogViewerDlg::OnRClickListBoxLog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &CLogViewerDlg::OnBnClickedButtonConfirm)
	ON_COMMAND(ID_ADD_COPYLOG, &CLogViewerDlg::OnCopylog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_STARTTIME, &CLogViewerDlg::OnDtnDatetimechangeDatetimepickerStarttime)
END_MESSAGE_MAP()

void CLogViewerDlg::CreateLogo(CRect& rcClient)
{
	CRect rcBtn;
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

void CLogViewerDlg::CreateCloseButton(CRect& rcClient)
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

void CLogViewerDlg::CreateFunctionButton(CRect& rcClient)
{
	CRect rcStatic, rcEdit, rcBtn, rcGroup, rcLevelGroup, rcBtnTime, rcMsg;
	int nGapH = int(30 * gSystemMgr.m_fRatioH);
	int nBtnGapH= int(20 * gSystemMgr.m_fRatioH);
	int nGapV = int(20 * gSystemMgr.m_fRatioV);
	int nLen = int(55 * gSystemMgr.m_fRatioH);
	int nBtnLen = int(50 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);
	//QUERYLOG GROUP
	rcGroup.left = rcClient.left + 2 * nGapH;
	rcGroup.right = rcClient.right - 2 * nGapH;
	rcGroup.top = rcClient.top + int(5 * nGapV);
	rcGroup.bottom = rcGroup.top + int(220 * gSystemMgr.m_fRatioV);
	CString str;
	str.LoadStringW(IDS_STRING_LOG_GROUP);
	CStatic*pStatic = (CStatic*)GetDlgItem(IDC_LOG_QUERY);
	pStatic->MoveWindow(rcGroup);
	pStatic->SetWindowTextW(str);
	pStatic->SetFont(&m_smallFont);
	SetWindowTheme(pStatic->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pStatic, m_bgImg);

	//QUERYTIME
	rcStatic.left = rcGroup.left + nGapH;
	rcStatic.top = rcGroup.top + 2 * nGapV;
	rcStatic.right = rcStatic.left + 6 * nLen;
	rcStatic.bottom = rcStatic.top + 4*nHeight;

	str.LoadStringW(IDS_STRING_QUERYLOG_TIME);
	pStatic = (CStatic*)GetDlgItem(IDC_STATIC_TIME_GROUP);
	pStatic->MoveWindow(rcStatic);
	pStatic->SetWindowTextW(str);
	pStatic->SetFont(&m_smallFont);
	SetWindowTheme(pStatic->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pStatic, m_bgImg);


	//DATATIME PICKER
	m_LogDate.SetFormat(_T("yyyy/MM/dd"));
	m_StartTime.SetFormat(_T("HH:mm"));
	m_EndTime.SetFormat(_T("HH:mm"));
	rcBtnTime.left = rcStatic.left + nGapH;
	rcBtnTime.right = rcBtnTime.left+ 5* nLen;
	rcBtnTime.top = rcStatic.top +2* nGapV;
	rcBtnTime.bottom = rcBtnTime.top + nHeight;
	InitializeCtrl(IDC_DATETIMEPICKER_LOGQUERY, rcBtnTime, true);


	rcBtnTime.right = rcBtnTime.left + 2*nLen;
	rcBtnTime.top = rcBtnTime.bottom + nGapV;
	rcBtnTime.bottom = rcBtnTime.top + nHeight;
	InitializeCtrl(IDC_DATETIMEPICKER_STARTTIME, rcBtnTime, true);

	rcBtnTime.right = rcBtnTime.left + 5 * nLen;
	rcBtnTime.left = rcBtnTime.right -2* nLen;
	InitializeCtrl(IDC_DATETIMEPICKER_ENDTIME, rcBtnTime, true);


	//levelgroup
	rcLevelGroup.left = rcStatic.right + 5*nGapH;
	rcLevelGroup.right = rcLevelGroup.left + 7 * nBtnLen;
	rcLevelGroup.top = rcStatic.top;
	rcLevelGroup.bottom = rcLevelGroup.top + 2*nHeight;

	str.LoadStringW(IDS_STRING_LOG_LEVEL);
	pStatic = (CStatic*)GetDlgItem(IDC_STATIC_LEVEL_GROUP);
	pStatic->MoveWindow(rcLevelGroup);
	pStatic->SetWindowTextW(str);
	pStatic->SetFont(&m_smallFont);
	SetWindowTheme(pStatic->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pStatic, m_bgImg);

	rcBtn.left = rcLevelGroup.left + nBtnGapH;
	rcBtn.right = rcBtn.left +2* nBtnLen;
	rcBtn.top = rcLevelGroup.top + int(35 * gSystemMgr.m_fRatioV);
	rcBtn.bottom = rcBtn.top + nHeight;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_EVENT);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LOG_EVENT);
	pBtn->SetWindowTextW(str);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);

	rcBtn.left = rcBtn.right + nBtnGapH;
	rcBtn.right = rcBtn.left + 2 * nBtnLen;
    pBtn = (CButton*)GetDlgItem(IDC_WARNING);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LOG_WARNING);
	pBtn->SetWindowTextW(str);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);

	rcBtn.left = rcBtn.right + nBtnGapH;
	rcBtn.right = rcBtn.left + int(80 * gSystemMgr.m_fRatioH);
	pBtn = (CButton*)GetDlgItem(IDC_ERROR);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LOG_ERROR);
	pBtn->SetWindowTextW(str);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
	//
	rcLevelGroup.bottom = rcStatic.bottom;
	rcLevelGroup.top = rcLevelGroup.bottom - 2 * nHeight;
	str.LoadStringW(IDS_STRING_LOG_TYPE);
	pStatic = (CStatic*)GetDlgItem(IDC_STATIC_SOURCE_GROUP);
	pStatic->MoveWindow(rcLevelGroup);
	pStatic->SetWindowTextW(str);
	pStatic->SetFont(&m_smallFont);
	SetWindowTheme(pStatic->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pStatic, m_bgImg);

	//
	rcBtn.left = rcLevelGroup.left + nBtnGapH;
	rcBtn.right = rcBtn.left + 2 * nBtnLen;
	rcBtn.top = rcLevelGroup.top + int(35 * gSystemMgr.m_fRatioV);
	rcBtn.bottom = rcBtn.top + nHeight;
	pBtn = (CButton*)GetDlgItem(IDC_SYSTEAM);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LOG_SYSTEAM);
	pBtn->SetWindowTextW(str);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);


	rcBtn.left = rcBtn.right + nBtnGapH;
	rcBtn.right = rcBtn.left + 4 * nBtnLen;
	pBtn = (CButton*)GetDlgItem(IDC_DETECT_RESULT);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LOG_DETECT_RESULT);
	pBtn->SetWindowTextW(str);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
	//btn
	rcEdit.right = rcGroup.right - nGapH;
	rcEdit.left = rcEdit.right - 2 * nLen;
	rcEdit.bottom = rcLevelGroup.bottom;
	rcEdit.top = rcEdit.bottom - nHeight;
	InitializeCtrl(IDC_BUTTON_CONFIRM, rcEdit, IDS_STRING_LOG_CONFIRM);
	//msg
	
	rcMsg.left = rcEdit.left;
	rcMsg.right = rcMsg.left+ 3 * nLen;
	rcMsg.top = rcStatic.top + 2 * nGapV;
	rcMsg.bottom = rcMsg.top+ nHeight;
	InitializeCtrl(IDC_STATIC_MSG, rcMsg, -1, true);


	//listLog
	CRect rcListBox;
	rcListBox.left = rcClient.left + 2 * nGapH;
	rcListBox.right = rcClient.right - 2 * nGapH;
	rcListBox.top = rcGroup.bottom + nGapV;
	rcListBox.bottom = rcClient.bottom - nGapV;
	m_logList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT, rcListBox, this, IDC_LIST_LOGVIEWER);
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
	m_logList.InsertColumn(1, strLog, LVCFMT_LEFT, int(200 * gSystemMgr.m_fRatioH));
	strLog.LoadStringW(IDS_STRING_LOG_DESCRIBE);
	m_logList.InsertColumn(2, strLog, LVCFMT_LEFT, int(1000 * gSystemMgr.m_fRatioH));
	m_logList.ShowWindow(SW_HIDE);
	m_logList.EnableWindow(TRUE);
	m_logList.Invalidate();
	m_logList.ShowWindow(SW_SHOW);
	m_logList.BringWindowToTop();
}



void CLogViewerDlg::InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID /*= -1*/, bool bSmallFont /*= false*/)
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

void CLogViewerDlg::DisplayMessage(CString sMsg)
{
	CStatic* pMsgPanel = (CStatic*)GetDlgItem(IDC_STATIC_MSG);
	pMsgPanel->ShowWindow(SW_HIDE);
	pMsgPanel->SetWindowText(sMsg);
	pMsgPanel->ShowWindow(SW_SHOW);
}

void CLogViewerDlg::OnStnClickedClose()
{
	CDialogEx::OnCancel();
}

void CLogViewerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DisplayBackgroundImage(dc);
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
HBRUSH CLogViewerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_MSG)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 10, 10));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID == IDC_EVENT ||
		ctrlID == IDC_WARNING ||
		ctrlID == IDC_ERROR ||
		ctrlID == IDC_STATIC_LEVEL_GROUP ||
		ctrlID == IDC_STATIC_SOURCE_GROUP||
		ctrlID == IDC_SYSTEAM||
		ctrlID == IDC_DETECT_RESULT||
		ctrlID == IDC_STATIC_TIME_GROUP||
		ctrlID == IDC_LOG_QUERY
		)
	{
		pDC->SetTextColor(RGB(250, 250, 250));
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH brush = m_bgBrushHelper.GetHBRUSH(pWnd->m_hWnd);
		if (brush)
		{
			return brush;
		}
	}

	return hbr;
}


// CLogViewerDlg 消息处理程序


void CLogViewerDlg::OnRadioBtnLevelClicked()
{
	UpdateData(TRUE);
	LOGVIEWER crtLogViewer;
	crtLogViewer = gSystemMgr.GetCurrentLogViewer();
	switch (m_nLevel)
	{
	case 0:
		crtLogViewer.m_logLevel = LV_EVENT;
		break;
	case 1:
		crtLogViewer.m_logLevel = LV_WARNING;
		break;
	case 2:
		crtLogViewer.m_logLevel = LV_ERROR;
		break;
	}
	gSystemMgr.SetCurrentLogViewer(crtLogViewer);
}


void CLogViewerDlg::OnRadioBtnSourceClicked()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LOGVIEWER crtLogViewer;
	crtLogViewer = gSystemMgr.GetCurrentLogViewer();
	switch (m_nLogSource)
	{
	case 0:
		crtLogViewer.m_logSource = SYSTEAM;
		break;
	case 1:
		crtLogViewer.m_logSource = DETECT_RESULT;
		break;
	}
	gSystemMgr.SetCurrentLogViewer(crtLogViewer);
}


void CLogViewerDlg::OnBnClickedButtonConfirm()
{
	Empty();
	CString sTime, sMsg;
	sMsg = "";
	DisplayMessage(sMsg);
	m_LogDate.GetTime(m_QueryBeginDate);
	sTime.Format(_T("%d-%02d-%02d"), m_QueryBeginDate.GetYear(), m_QueryBeginDate.GetMonth(), m_QueryBeginDate.GetDay());
	TCHAR  moduleFileName[MAX_PATH];
	::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	(_tcsrchr(moduleFileName, _T('\\')))[1] = 0;
	CString strPath = moduleFileName;
	LOGVIEWER crtLogViewer;
	crtLogViewer = gSystemMgr.GetCurrentLogViewer();
	if (crtLogViewer.m_logSource==SYSTEAM)
	{
		strPath.Append(_T("\log\\system"));
	}
	if (crtLogViewer.m_logSource == DETECT_RESULT)
	{
		strPath.Append(_T("\log\\DetectResult"));
	}
	wstring wstrFilePath = strPath.GetString();

	strPath = strPath + L"\\" + sTime.GetString() + L".log";
	FILE* fp = nullptr;
	if (0 != fopen_s(&fp, localUtils::utils::TChar2Utf8(strPath).c_str(), "r"))
	{
		sMsg.LoadStringW(IDS_STRING_LOGFILE_NOT_EXIST);
		DisplayMessage(sMsg);
		return;
	}
	gSystemMgr.ExpFileHint(localUtils::utils::Unicode2Utf8(wstrFilePath));
	char buf[1024];
	memset(&buf, 0, sizeof(buf));
	while (NULL != fgets(buf, sizeof(buf), fp))
	{
		string strValue(buf);
		wstring wstrValue = localUtils::utils::Utf8ToUnicode(strValue);
		if (!(ShowLogList(wstrValue)))
		{
		   break;
		}
	}
	fclose(fp);
}
LRESULT CLogViewerDlg::OnNcHitTest(CPoint point)
{
	CRect rect;
	ScreenToClient(&point);//屏幕坐标转换为窗口坐标
	if ((point.x > 19 && point.x < 1175) && (point.y > 84 && point.y < 599))//测试当前鼠标位置是否在客户区
	{
		return HTCLIENT;//返回客户区标志位
	}
	return HTCAPTION;//返回标题栏标志位
}
BOOL CLogViewerDlg::ShowLogList(wstring wstrLogMsg)
{
	CString sMsg,sBeginTime,sEndTime;
	std::vector<wstring> vLogItem = localUtils::utils::SplitString(wstrLogMsg, L"$");
	if (vLogItem.size() != 3)
	{
		sMsg.LoadStringW(IDS_STRING_LOGFILE_READ_FAIL);
		DisplayMessage(sMsg);
		return false;
	}
	wstring wstrTime = vLogItem[0];
	wstring wstrLevel = vLogItem[1];
	wstring wstrMsg = vLogItem[2];
	m_StartTime.GetTime(m_QueryBeginTime);
	m_EndTime.GetTime(m_QueryEndTime);
	sBeginTime.Format(_T("%02d:%02d:%02d:%03d"), m_QueryBeginTime.GetHour(), m_QueryBeginTime.GetMinute(), 0, 0);
	sEndTime.Format(_T("%02d:%02d:%02d:%03d"), m_QueryEndTime.GetHour(), m_QueryEndTime.GetMinute(), 0, 0);
	if (m_QueryBeginTime >= m_QueryEndTime)
	{
		sMsg.LoadStringW(IDS_STRING_QUERY_ERROR);
		DisplayMessage(sMsg);
		Empty();
		return false;
	}
	if ((wstrTime >= sBeginTime.GetString()) && (wstrTime <= sEndTime.GetString()))
	{
		LOGVIEWER crtLogViewer;
		crtLogViewer = gSystemMgr.GetCurrentLogViewer();
		eEVENT_LEVEL crtlogLevel = LV_INVALID;
		if (wstrLevel == L"WARNING")
		{
			crtlogLevel = LV_WARNING;
		}
		else if (wstrLevel == L"ERROR")
		{
			crtlogLevel = LV_ERROR;
		}
		else
		{
			crtlogLevel = LV_EVENT;
		}
		if (crtLogViewer.m_logLevel == crtlogLevel)
		{
			int indexItem = m_logList.GetItemCount();
			m_logList.InsertItem(0, L"");
			m_logList.SetItemText(0, 1, wstrTime.c_str());
			m_logList.SetItemText(0, 2, wstrMsg.c_str());
			EVENT_PACKAGE eventPackage;
			eventPackage.m_level = crtlogLevel;
			eventPackage.m_wstrTimestamp = wstrTime;
			eventPackage.m_wstrDescription = wstrMsg;
			m_eventBuffer.push_back(eventPackage);
			m_logList.Invalidate();
			if (indexItem >= 200)
			{
				m_logList.DeleteItem(200);
			}
		}
	}
	return true;
	
}

void CLogViewerDlg::Empty()
{
	m_logList.SetRedraw(FALSE);
	m_logList.DeleteAllItems();
	m_logList.SetRedraw(TRUE);
	m_logList.Invalidate();
	m_logList.UpdateWindow();
	m_eventBuffer.clear();
}

void CLogViewerDlg::OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult)
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
			eventPackage = m_eventBuffer.at(m_logList.GetItemCount() - iRow - 1);
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

void CLogViewerDlg::OnRClickListBoxLog(NMHDR *pNMHDR, LRESULT *pResult)
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

void CLogViewerDlg::OnCopylog()
{
	if (!OpenClipboard())
	{
		//OpenClipboard();
	}
	HGLOBAL clipbuffer;
	char * buffer;
	EmptyClipboard();
	USES_CONVERSION;
	const char* csrc = T2A(m_strCopyLog);
	size_t iLen = strlen(csrc) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, iLen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy_s(buffer, iLen, csrc);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
}


void CLogViewerDlg::OnDtnDatetimechangeDatetimepickerStarttime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
