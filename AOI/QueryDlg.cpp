#include "stdafx.h"
#include "AOI.h"
#include "AOIDlg.h"
#include "QueryDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"


IMPLEMENT_DYNAMIC(CQueryDlg, CDialogEx)

CQueryDlg::CQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_QUERY, pParent)
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

	m_txtFont.CreateFont(int(32 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
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
	gSystemMgr.ExpRecordHint();

}

CQueryDlg::~CQueryDlg()
{
}

void CQueryDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
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

void CQueryDlg::DisplayMessage(CString sMsg)
{
	CStatic* pMsgPanel = (CStatic*)GetDlgItem(IDC_STATIC_MSG);
	pMsgPanel->ShowWindow(SW_HIDE);
	pMsgPanel->SetWindowText(sMsg);
	pMsgPanel->ShowWindow(SW_SHOW);
}

void CQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dateCtrl2);
	DDX_Control(pDX, IDC_BUTTON_QUERY, m_btnConfirm);
	DDX_Control(pDX, IDC_LIST_QUERY_DATA, m_listQueryData);
	DDX_Control(pDX, IDC_BUTTON_CSV, m_btnCSV);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDDATE, m_EndDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDTIME, m_EndTime);
	DDX_Control(pDX, IDC_EDIT_REPORT_TOTAL, m_ReportTotal);
	DDX_Control(pDX, IDC_EDIT_REPORT_OKRATE, m_ReportOkRate);
	DDX_Control(pDX, IDC_EDIT_REPORT_AVGTIME, m_AvgTime);
	DDX_Control(pDX, IDC_COMBO_PROJ_SWITCH, m_cboxProj);
}

BOOL CQueryDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CQueryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CDialogEx::SetBackgroundImage(IDB_BITMAP_SYS_CONFIG_DLG_BK);

	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_QUERY_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_QUERY_UI_HEIGHT * fRatioV);

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
	CreateQueryReportDisplayingWidget(rcClient);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CQueryDlg, CDialogEx)
	ON_STN_CLICKED(BTN_CLOSE, &CQueryDlg::OnStnClickedClose)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CQueryDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_CSV, &CQueryDlg::OnBnClickedButtonCsv)
END_MESSAGE_MAP()

void CQueryDlg::CreateLogo(CRect & rcClient)
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

	rcTitle.left = rcClient.left + int(430 * gSystemMgr.m_fRatioH);
	rcTitle.top = rcClient.top + int(20 * gSystemMgr.m_fRatioV);
	rcTitle.right = rcTitle.left + int(620 * gSystemMgr.m_fRatioH);
	rcTitle.bottom = rcTitle.top + int(64 * gSystemMgr.m_fRatioV);
	wstring wstrTitle = gConfigFileMgr.GetCommonSetting(L"CUSTOMISED_TITLE");
	CWnd* pCtrl = (CWnd*)GetDlgItem(IDC_STATIC_TITLE);
	pCtrl->SetFont(&m_titleFont);
	pCtrl->MoveWindow(&rcTitle);
	pCtrl->SetWindowTextW(wstrTitle.c_str());
	
}

void CQueryDlg::CreateCloseButton(CRect & rcClient)
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

void CQueryDlg::CreateFunctionButton(CRect& rcClient)
{

	int nGapH = int(30 * gSystemMgr.m_fRatioH);
	int nGapHs= int(15 * gSystemMgr.m_fRatioH);
	int nGapV = int(25 * gSystemMgr.m_fRatioV);
	int nLen = int(140 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);
	int nCtrlLen = int(200 * gSystemMgr.m_fRatioH);//combo length
	int nCtrllistLen = int(840 * gSystemMgr.m_fRatioH);
	CRect rcDate,rcButton,rcMsg,rcList;
	m_dateCtrl2.SetFormat(_T("yyyy/MM/dd"));
	m_EndDate.SetFormat(_T("yyyy/MM/dd"));
	m_StartTime.SetFormat(_T("HH:00"));
	m_EndTime.SetFormat(_T("HH:00"));
	//------------------------------------QueryStartDate
	rcDate.left = rcClient.left + int(60 * gSystemMgr.m_fRatioH);
	rcDate.top = rcClient.top + int(120 * gSystemMgr.m_fRatioV);
	rcDate.right = rcDate.left + nLen;
	rcDate.bottom = rcDate.top + nHeight;
	InitializeCtrl(IDC_STATIC_QUERY_DATE, rcDate, IDS_STRING_QUERY_BEGINTIME);
	rcButton.left = rcDate.right + nGapHs;
	rcButton.top = rcDate.top;
	rcButton.right = rcButton.left + nCtrlLen;
	rcButton.bottom= rcButton.top+ nHeight;
	InitializeCtrl(IDC_DATETIMEPICKER1, rcButton);
	//------------------------------------QueryStartTime
	rcButton.left = rcButton.right + nGapHs;
	rcButton.right = rcButton.left + nLen;
	InitializeCtrl(IDC_DATETIMEPICKER_STARTTIME, rcButton);

	//------------------------------------QueryEndDate
	rcDate.left = rcClient.left + int(60 * gSystemMgr.m_fRatioH);
	rcDate.top = rcDate.bottom + nGapHs;
	rcDate.right = rcDate.left + nLen;
	rcDate.bottom = rcDate.top + nHeight;
	InitializeCtrl(IDC_STATIC_QUERY_ENDTIME, rcDate, IDS_STRING_QUERY_ENDTIME);
	rcButton.left = rcDate.right + nGapHs;
	rcButton.top = rcDate.top;
	rcButton.right = rcButton.left + nCtrlLen;
	rcButton.bottom = rcButton.top + nHeight;
	InitializeCtrl(IDC_DATETIMEPICKER_ENDDATE, rcButton);
	//------------------------------------QueryEndTime
	rcButton.left = rcButton.right + nGapHs;
	rcButton.right = rcButton.left + nLen;
	InitializeCtrl(IDC_DATETIMEPICKER_ENDTIME, rcButton);
	//------------------------------------QueryProject
	rcDate.top = rcDate.bottom + nGapHs;
	rcDate.bottom = rcDate.top + nHeight;
	InitializeCtrl(IDC_STATIC_QUERY_PROJECT, rcDate, IDS_STRING_QUERY_PROJECT);
	rcButton.left = rcDate.right + nGapHs;
	rcButton.top = rcDate.top;
	rcButton.right = rcButton.left + int(355 * gSystemMgr.m_fRatioH);
	rcButton.bottom = rcButton.top + nHeight;
	InitializeCtrl(IDC_COMBO_PROJ_SWITCH, rcButton);
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_PROJ_SWITCH);
	pCombo->SendMessage(CB_SETITEMHEIGHT, -1, 33);
	pCombo->SendMessage(CB_SETITEMHEIGHT, 0, 33);
	ListAllProjectName();
	//------------------------------------CSV
	rcButton.top = rcButton.bottom + nGapHs;
	rcButton.bottom = rcButton.top + nHeight;
	rcButton.left = rcButton.right - nLen;
	InitializeCtrl(IDC_BUTTON_CSV, rcButton, IDS_STRING_QUERY_CSV);
	//------------------------------------QueryConfirm
	rcButton.right = rcButton.left - nGapHs;
	rcButton.left = rcButton.right - nLen;
	InitializeCtrl(IDC_BUTTON_QUERY, rcButton, IDS_STRING_QUERY_CONFIRM);

	//------------------------------------QueryMsg
	rcMsg.left = rcClient.left + int(60 * gSystemMgr.m_fRatioH);
	rcMsg.right = rcMsg.left + nCtrllistLen;
	rcMsg.top = rcDate.bottom + nGapHs;
	rcMsg.bottom = rcMsg.top + nHeight;
	InitializeCtrl(IDC_STATIC_MSG, rcMsg, -1, true);
	//------------------------------------QueryList
	rcList.right= rcClient.right - int(60 * gSystemMgr.m_fRatioH);
	rcList.left= rcList.right - nCtrllistLen;
	rcList.top= rcClient.top + int(120 * gSystemMgr.m_fRatioV);
	rcList.bottom = rcList.top + 12* nHeight;
	InitializeCtrl(IDC_LIST_QUERY_DATA, rcList,true);
	CString strDateTitle, strTotalTitle, strOkCountTitle,strOkRateTitle,strTotalTimeTitle;
	strDateTitle.Format(IDS_STRING_QUERY_DATETIME);
	strTotalTitle.Format(IDS_STRING_TOTAL);
	strOkCountTitle.Format(IDS_STRING_OK_COUNT);
	strOkRateTitle.Format(IDS_STRING_OK_COUNT_RATE);
	strTotalTimeTitle.Format(IDS_STRING_LAST_TIMING);
	m_listQueryData.SetExtendedStyle(m_listQueryData.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listQueryData.InsertColumn(0, strDateTitle, LVCFMT_LEFT, rcList.Width() / 3, 0);
	m_listQueryData.InsertColumn(1, strTotalTitle, LVCFMT_LEFT, rcList.Width() / 8, 1);
	m_listQueryData.InsertColumn(2, strOkCountTitle, LVCFMT_LEFT, rcList.Width()/ 8, 2);
	m_listQueryData.InsertColumn(3, strOkRateTitle, LVCFMT_LEFT, rcList.Width() / 6, 3);
	m_listQueryData.InsertColumn(4, strTotalTimeTitle, LVCFMT_LEFT, rcList.Width() / 4, 4);
	m_listQueryData.ShowScrollBar(SB_VERT, TRUE);
}

void CQueryDlg::CreateQueryReportDisplayingWidget(CRect& rcClient)
{
	CRect  rcGroup,rcName, rcValue;
	int nGapH = int(15 * gSystemMgr.m_fRatioV);
	int nGapHs = int(15 * gSystemMgr.m_fRatioH);
	int nLen = int(140 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);
	int nCtrlLen = int(200 * gSystemMgr.m_fRatioH);
	//------------------------------------ReportGroup
	rcGroup.left= rcClient.left + int(60 * gSystemMgr.m_fRatioH);
	rcGroup.right= rcGroup.left+ int(510 * gSystemMgr.m_fRatioH);
	rcGroup.top=rcClient.top+ int(320 * gSystemMgr.m_fRatioV);
	rcGroup.bottom= rcGroup.top+ int(280 * gSystemMgr.m_fRatioV);
	InitializeCtrl(IDC_STATIC_GROUP_REPORT, rcGroup, IDS_STRING_QUERY_REPORT_GROUP);
	rcName.left= rcGroup.left+ int(25 * gSystemMgr.m_fRatioH);
	rcName.right = rcName.left + nCtrlLen;
	rcName.top= rcGroup.top+ int(55 * gSystemMgr.m_fRatioV);
	rcName.bottom = rcName.top + nHeight;
	InitializeCtrl(IDC_STATIC_REPORT_TOTAL, rcName, IDS_STRING_TOTAL);
	rcName.top = rcName.bottom + nGapH;
	rcName.bottom = rcName.top + nHeight;
	InitializeCtrl(IDC_STATIC_REPORT_OKRATE, rcName, IDS_STRING_OK_COUNT_RATE);
	rcName.top = rcName.bottom + nGapH;
	rcName.bottom = rcName.top + nHeight;
	InitializeCtrl(IDC_STATIC_REPORT_AVGTIME, rcName, IDS_STRING_AVG_TIMING);
	//------------------------------------ReportValue
	rcValue.left = rcName.right + int(35 * gSystemMgr.m_fRatioH);
	rcValue.right = rcValue.left + nCtrlLen;
	rcValue.top= rcClient.top + int(375 * gSystemMgr.m_fRatioV);
	rcValue.bottom = rcValue.top + nHeight;
	InitializeCtrl(IDC_EDIT_REPORT_TOTAL, rcValue);
	rcValue.top = rcValue.bottom + nGapH;
	rcValue.bottom = rcValue.top + nHeight;
	InitializeCtrl(IDC_EDIT_REPORT_OKRATE, rcValue);
	rcValue.top = rcValue.bottom + nGapH;
	rcValue.bottom = rcValue.top + nHeight;
	InitializeCtrl(IDC_EDIT_REPORT_AVGTIME, rcValue);

}

void CQueryDlg::OnStnClickedClose()
{
	CDialogEx::OnCancel();
}


HBRUSH CQueryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_TITLE)
	{
		pDC->SetTextColor(RGB(10, 10, 90));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID == IDC_STATIC_MSG)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 10, 10));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID==IDC_STATIC_REPORT_TOTAL||
		ctrlID==IDC_STATIC_REPORT_OKRATE||
		ctrlID==IDC_STATIC_REPORT_AVGTIME||
		ctrlID== IDC_STATIC_QUERY_DATE||
		ctrlID== IDC_STATIC_QUERY_ENDTIME||
		ctrlID== IDC_STATIC_QUERY_PROJECT
		)
	{
		pDC->SetTextColor(RGB(250, 250, 250));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}

	return hbr;
}

void CQueryDlg::OnPaint()
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


void CQueryDlg::OnBnClickedButtonQuery()
{
	CString cstrQueryBeginDate,cstrQueryEndDate,strReportTotalValue,strReportOkRateValue,strReportAvgTimeValue, sMsg(L"");
	DisplayMessage(sMsg);
	Empty();
	m_dateCtrl2.GetTime(m_QueryBeginDate);
	m_StartTime.GetTime(m_QueryBeginTime);
	m_EndDate.GetTime(m_QueryEndDate);
	m_EndTime.GetTime(m_QueryEndTime);
	if ((m_QueryBeginDate > m_QueryEndDate)|| ((m_QueryBeginTime >= m_QueryEndTime) && (m_QueryBeginDate == m_QueryEndDate)))
	{
		sMsg.LoadStringW(IDS_STRING_QUERY_ERROR);
		DisplayMessage(sMsg);
		Empty();
		return;
	}
	cstrQueryBeginDate.Format((_T("%d-%.2d-%.2d %.2d:00")), m_QueryBeginDate.GetYear(), m_QueryBeginDate.GetMonth(), m_QueryBeginDate.GetDay(), m_QueryBeginTime.GetHour());
	cstrQueryEndDate.Format((_T("%d-%.2d-%.2d %.2d:00")), m_QueryEndDate.GetYear(), m_QueryEndDate.GetMonth(), m_QueryEndDate.GetDay(), m_QueryEndTime.GetHour());
	USES_CONVERSION;
	char* strQueryBeginDate = T2A(cstrQueryBeginDate);
	char* strQueryEndDate = T2A(cstrQueryEndDate);
	int crtIndex = m_cboxProj.GetCurSel();
	CString sProjectName;
	m_cboxProj.GetLBText(crtIndex, sProjectName);
	wstring wstrName = sProjectName.GetString();
	string strProjectName = localUtils::utils::Unicode2Utf8(wstrName);
	strProjectName = gSystemMgr.TransDBTableName(strProjectName);

	int  nLastId;
	if (gSystemMgr.m_sysDatabase.TableExists(strProjectName.c_str()))
	{
		char   sql[1024];
		sprintf_s(sql, SQL_QUERY_RECORD_DATA, strProjectName.c_str(), strQueryBeginDate, strQueryEndDate);
		QueryData = gSystemMgr.m_sysDatabase.Query(sql);
		m_listQueryData.SetRedraw(FALSE);
		for (int i=0; !QueryData.eof();i++)
		{
			QueryCountData[i].Id = QueryData.fieldValue("id");
			QueryCountData[i].Date= QueryData.fieldValue("date");
			QueryCountData[i].Total = QueryData.fieldValue("total");
			QueryCountData[i].OkCount = QueryData.fieldValue("OkCount");
			QueryCountData[i].OkCountRate = QueryData.fieldValue("OkCountRate");
			QueryCountData[i].CrtTime = QueryData.fieldValue("CrtTime");
		
			nLastId = atoi(QueryCountData[i].Id);

			wstring wstrId = localUtils::utils::Utf8ToUnicode(QueryCountData[i].Id);
			wstring wstrDate= localUtils::utils::Utf8ToUnicode(QueryCountData[i].Date);
			wstring wstrTotal = localUtils::utils::Utf8ToUnicode(QueryCountData[i].Total);
			wstring wstrOKCount = localUtils::utils::Utf8ToUnicode(QueryCountData[i].OkCount);
			wstring wstrOKCountRate = localUtils::utils::Utf8ToUnicode(QueryCountData[i].OkCountRate);
			wstring wstrCrtTime = localUtils::utils::Utf8ToUnicode(QueryCountData[i].CrtTime);
			CString UserManagement;
			UserManagement.Format(_T("%d"), i + 1);
			m_listQueryData.InsertItem(i, UserManagement);
			m_listQueryData.SetItemText(i, 0, wstrDate.c_str());
			m_listQueryData.SetItemText(i, 1, wstrTotal.c_str());
			m_listQueryData.SetItemText(i, 2, wstrOKCount.c_str());
			m_listQueryData.SetItemText(i, 3, wstrOKCountRate.c_str());
			m_listQueryData.SetItemText(i, 4, wstrCrtTime.c_str());
			QueryData.NextRow();
		}
		m_listQueryData.SetRedraw(TRUE);
		m_listQueryData.Invalidate();
		m_listQueryData.UpdateWindow();
		//report
		if (m_listQueryData.GetItemCount() >= 1)
		{
			int nTotalValue = 0;
			int nOkValue = 0;
			double dTime = 0;
			CString cstrTotalFirst,cstrTotalLast, cstrOkCountFirst, cstrOkCountLast;

			int nListNum = m_listQueryData.GetItemCount() - 1;
			if (nListNum == 0)
			{
				cstrTotalFirst = L"0";
				cstrTotalLast = L"1";

				char   sql[1024];
				if (nLastId>=1)
				{
					sprintf_s(sql, SQL_QUERY_RECORD_DATA_FROM_ID, strProjectName.c_str(), nLastId - 1);
				}
				else
				{
					sprintf_s(sql, SQL_QUERY_RECORD_DATA_FROM_ID, strProjectName.c_str(), 0);
				}
				CppSQLite3Query tempQueryData = gSystemMgr.m_sysDatabase.Query(sql);
				const char* charOkCountFirst = tempQueryData.fieldValue("OkCount");
				wstring wstrOkCountFirst = localUtils::utils::Utf8ToUnicode(charOkCountFirst);
				cstrOkCountFirst = wstrOkCountFirst.c_str();
				cstrOkCountLast = m_listQueryData.GetItemText(nListNum, 2);
			}
			else
			{
				cstrTotalFirst = m_listQueryData.GetItemText(0, 1);
				cstrOkCountFirst = m_listQueryData.GetItemText(0, 2);
				cstrTotalLast = m_listQueryData.GetItemText(nListNum, 1);
				cstrOkCountLast = m_listQueryData.GetItemText(nListNum, 2);
			}
			string strTotalFirst = localUtils::utils::Unicode2Ansi(cstrTotalFirst.GetString());
			string strOkCountFirst = localUtils::utils::Unicode2Ansi(cstrOkCountFirst.GetString());
			string strTotalLast = localUtils::utils::Unicode2Ansi(cstrTotalLast.GetString());
			string strOkCountLast = localUtils::utils::Unicode2Ansi(cstrOkCountLast.GetString());

			int nTotalFirst = atoi(strTotalFirst.c_str());
			int nTotalLast = atoi(strTotalLast.c_str());
			int nOkCountFirst = atoi(strOkCountFirst.c_str());
			int nOkCountLast = atoi(strOkCountLast.c_str());

			nTotalValue = nTotalLast - nTotalFirst;
			nOkValue = nOkCountLast - nOkCountFirst;

			for (int i=0;i<nListNum;i++)
			{
				CString cstrTestTime = m_listQueryData.GetItemText(i, 4);
				string strTestTime = localUtils::utils::Unicode2Ansi(cstrTestTime.GetString());
				double dTempTime = atof(strTestTime.c_str());
				dTime += dTempTime;
			}

			if (nTotalValue !=0)
			{
				double Avgtime = dTime / nTotalValue;
				strReportAvgTimeValue.Format(_T("%.2f"), Avgtime);
				strReportAvgTimeValue.Append(_T("s"));
				strReportOkRateValue.Format(_T("%d /"), nOkValue);
				//strReportOkRateValue.Append(cstrOkCountRateLast);
			}
			else
			{
				AfxMessageBox(L"查询结果总数为0");
				return;
			}
			strReportTotalValue.Format(_T("%d"), nTotalValue);
			m_ReportTotal.SetWindowText(strReportTotalValue);
			m_ReportOkRate.SetWindowText(strReportOkRateValue);
			m_AvgTime.SetWindowText(strReportAvgTimeValue);
		}
	}
	else
	{
		sMsg.LoadStringW(IDS_STRING_QUERY_MSG);
		DisplayMessage(sMsg);
		Empty();
	}
}

void CQueryDlg::Empty()
{
	m_ReportTotal.SetWindowText(_T(""));
	m_ReportOkRate.SetWindowText(_T(""));
	m_AvgTime.SetWindowText(_T(""));
	m_listQueryData.SetRedraw(FALSE);
	m_listQueryData.DeleteAllItems();
	m_listQueryData.SetRedraw(TRUE);
	m_listQueryData.Invalidate();
	m_listQueryData.UpdateWindow();
}

void CQueryDlg::OnBnClickedButtonCsv()
{
	if (!(m_listQueryData.GetItemCount() > 0))
		return;
	CFile m_file;
	CString str_totalPath;
	CString strFilter = _T("CSV Files (*.csv)|*.csv||");
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
	if (dlg.DoModal() == IDOK)
	{
	    str_totalPath = dlg.GetPathName();
		str_totalPath += ".csv";
		USES_CONVERSION;
		char* Filename = T2A(str_totalPath);
	}

	if (m_file.Open(str_totalPath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
	{
		CString strHead = _T("DATETIME,TOTAL,OKRATE,AVGTIME\r\n");
		int length = strHead.GetLength();
		m_file.Write(strHead, strHead.GetLength() * 2);
		for (int nItem = 0; nItem<m_listQueryData.GetItemCount(); nItem++)
		{
			CString strData;
			for (int i = 0; i<4; i++)
			{
				strData += m_listQueryData.GetItemText(nItem, i);
				if (i == 3)
				{
					strData += _T("\r\n");
				}
				else
				{
					strData += _T(",");
				}
			}
			m_file.Write(strData, strData.GetLength() * 2);
		}
//Report
		CString strReportHead = _T("TOTAL,OKRATE,AVGTIME\r\n");
		m_file.Write(strReportHead, strReportHead.GetLength() * 2);
		CString total, okrate, avgtime, strReport;
		 m_ReportTotal.GetWindowText(total);
		 m_ReportOkRate.GetWindowText(okrate);
		 m_AvgTime.GetWindowText(avgtime);
		 total += _T(",");
		 okrate += _T(",");
		 strReport += total;
		 strReport += okrate;
		 strReport += avgtime;
		 m_file.Write(strReport, strReport.GetLength() * 2);
		m_file.Close();
	}




}

void CQueryDlg::ListAllProjectName()
{
	m_cboxProj.ResetContent();
	std::map<int, STATION_CONFIG>* ptrDictRet = gConfigFileMgr.GetStaionConfigArray();
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
			m_cboxProj.AddString(wstrName.c_str());
			if (wstrName == wstrCrtProjectName)
			{
				nCrtIndex = nIndex;
			}
		}
		m_cboxProj.SetCurSel(nCrtIndex);
	}
}
