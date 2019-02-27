#include "stdafx.h"
#include <map>
#include "AOI.h"
#include "ConfigFileCpyDlg.h"
#include "afxdialogex.h"
#include "ConfigFileManager.h"
#include "Resource.h"
extern CSystemManager gSystemMgr;
extern ConfigFileManager gConfigFileMgr;
IMPLEMENT_DYNAMIC(ConfigFileCpyDlg, CDialogEx)


ConfigFileCpyDlg::ConfigFileCpyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CONFIGFILE, pParent),
	m_bOpRslt(false)
{
	m_smallFont.CreateFont(int(26 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_txtFont.CreateFont(int(36 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
}

ConfigFileCpyDlg::~ConfigFileCpyDlg()
{
}

void ConfigFileCpyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_BtnloadConfig);
	DDX_Control(pDX, IDC_BUTTON2, m_BtnSaveConfig);
}


BEGIN_MESSAGE_MAP(ConfigFileCpyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(LOAD_CONFIGUREFILE_BNT, &ConfigFileCpyDlg::OnBnClickedImportConfigFile)
	ON_BN_CLICKED(SAVE_CONFIGUREFILE_BNT, &ConfigFileCpyDlg::OnBnClickedExportConfigFile)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


void ConfigFileCpyDlg::OnPaint()
{
	CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ClientToScreen(&rcWindow);
	CBitmap bakImg;
	if (bakImg.LoadBitmap(IDB_BITMAP_SYS_CONFIG_DLG_BK))
	{
		BITMAP bitmap;
		bakImg.GetBitmap(&bitmap);
		CBitmap* pPngOld = dcMem.SelectObject(&bakImg);
		CRect rcParentWindow;
		GetParent()->GetWindowRect(&rcParentWindow);
		ClientToScreen(&rcParentWindow);
		dc.StretchBlt(0, 0, rect.Width(), rect.Height(),&dcMem,
			rcWindow.left - rcParentWindow.left, rcWindow.top - rcParentWindow.top, rect.Width(), rect.Height(), SRCCOPY);
	}
}


BOOL ConfigFileCpyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rcClient;
	GetClientRect(&rcClient);
	int nGapH = int(30 * gSystemMgr.m_fRatioH);
	int nGapV = int(25 * gSystemMgr.m_fRatioV);
	int nLen = int(240 * gSystemMgr.m_fRatioH);
	int nHeight = int(50 * gSystemMgr.m_fRatioV);
	int nCtrlLen = int(320 * gSystemMgr.m_fRatioH);//combo length
	int nCtrllistLen = int(520 * gSystemMgr.m_fRatioH);

	CRect rcPos, rcCheck, rcRotate;

	rcPos.left = rcClient.left + nGapH;
	rcPos.top = rcClient.top + 2 * nGapV;
	rcPos.right = rcPos.left + nLen;
	rcPos.bottom = rcPos.top + nHeight;
	InitializeCtrl(LOAD_CONFIGUREFILE_BNT, rcPos);

	rcPos.top = rcPos.bottom + nGapV;
	rcPos.bottom = rcPos.top + nHeight;
	InitializeCtrl(SAVE_CONFIGUREFILE_BNT, rcPos);

	rcPos.right += 200;
	rcPos.top = rcPos.bottom + nGapV;
	rcPos.bottom = rcPos.top + 2 * nHeight;
	InitializeCtrl(IDC_STATIC_PROMPT_MSG_PANEL, rcPos);
	return TRUE;  
}

void ConfigFileCpyDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
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

void ConfigFileCpyDlg::OnBnClickedImportConfigFile()
{
	CString strFilter(_T("Configuration File (*.config)|*.config||"));
	CFileDialog fileDlg(TRUE, _T("config"), _T("*.config"), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, strFilter, this);
	if (fileDlg.DoModal() == IDOK)
	{
		CString strSrcFile = fileDlg.GetPathName();
		wstring strSrcConfigFile = CT2W(strSrcFile);
		wstring strDstFileName = ConfigFileManager::GetConfigFilePath();

		CString sMsg;
		BOOL ret = ::CopyFileW(strSrcConfigFile.c_str(), strDstFileName.c_str(), false);
		bool bOk = true;
		if (ret == FALSE)
		{
			sMsg.LoadString(IDS_STRING_OPERATION_FAILED);
			bOk = false;
		}
		else
		{
			sMsg.LoadString(IDS_STRING_OPERATION_OK);
		}
		CString sReboot;
		sReboot.LoadStringW(IDS_STRING_REBOOT_PROMPT);
		sMsg.Append(sReboot);
		DisplayPromptMessage(sMsg, bOk);
	}
}

void ConfigFileCpyDlg::DisplayPromptMessage(CString sMsg, bool bOpRslt)
{
	SetDlgItemTextW(IDC_STATIC_PROMPT_MSG_PANEL, _T(""));
	RedrawWindow();
	m_bOpRslt = bOpRslt;
	SetDlgItemTextW(IDC_STATIC_PROMPT_MSG_PANEL, sMsg);
	SetTimer(TIMER_ID_CONFIG_FILE_DLG, 5000, NULL);
}

void ConfigFileCpyDlg::OnBnClickedExportConfigFile()
{
	wstring wstrConfigPath = ConfigFileManager::GetConfigFilePath();

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Config(*.config)|*.config||"));
	if (dlg.DoModal() == IDOK)
	{
		CString sPath = dlg.GetPathName();
		CString sFile = sPath + _T(".config");

		wstring wstrNewConfigFile = sFile.GetString();
		BOOL ret = ::CopyFileW(wstrConfigPath.c_str(), wstrNewConfigFile.c_str(), false);
		CString sMsg;
		if (ret == FALSE)
		{
			sMsg.LoadString(IDS_STRING_OPERATION_FAILED);
			DisplayPromptMessage(sMsg, false);
		}
		else
		{
			sMsg.LoadString(IDS_STRING_OPERATION_OK);
			DisplayPromptMessage(sMsg, true);
		}
	}
}


HBRUSH ConfigFileCpyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	UINT id = pWnd->GetDlgCtrlID();
	if (id == IDC_STATIC_PROMPT_MSG_PANEL)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (id == IDC_STATIC_PROMPT_MSG_PANEL )
		{
			if (m_bOpRslt)
				pDC->SetTextColor(RGB(50, 220, 40));
			else
				pDC->SetTextColor(RGB(200, 20, 20));
		}
		else
		{
			pDC->SetTextColor(RGB(250, 250, 250));
		}
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}


void ConfigFileCpyDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_CONFIG_FILE_DLG)
	{
		SetDlgItemTextW(IDC_STATIC_PROMPT_MSG_PANEL, _T(""));
		RedrawWindow();
		KillTimer(TIMER_ID_CONFIG_FILE_DLG);
	}
	CDialogEx::OnTimer(nIDEvent);
}

