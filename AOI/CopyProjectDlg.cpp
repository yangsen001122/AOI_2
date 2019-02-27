#include "stdafx.h"
#include "AOI.h"
#include "CopyProjectDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CCopyProjectDlg, CDialogEx)

CCopyProjectDlg::CCopyProjectDlg(CString sSrcName, wstring& wstrRet, CWnd* pParent /*=NULL*/)
	: m_sSourceName(sSrcName),
	m_wstrTargetName(wstrRet),
	CDialogEx(IDD_DIALOG_COPY_PROJECT, pParent)
{
	m_txtFont.CreateFont(int(26 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_bgImg.LoadBitmap(IDB_BITMAP_SYS_CONFIG_DLG_BK);
}

CCopyProjectDlg::~CCopyProjectDlg()
{
}

void CCopyProjectDlg::InitializeCtrl(int nCtrlID, int nStrID)
{
	CWnd* pWnd = GetDlgItem(nCtrlID);
	if (pWnd != nullptr)
	{
		pWnd->SetFont(&m_txtFont);
	}
	if (nStrID != -1)
	{
		CString str;
		str.LoadStringW(nStrID);
		pWnd->SetWindowTextW(str);
	}
}

void CCopyProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CCopyProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitializeCtrl(IDC_STATIC_SRC_PROJECT,IDS_STRING_SRC_PROJECT);
	InitializeCtrl(IDC_EDIT_SRC_PROJECT);
	InitializeCtrl(IDC_STATIC_NEW_PROJECT, IDS_STRING_TARGET_PROJECT);
	InitializeCtrl(IDC_EDIT_NEW_PROJECT);
	InitializeCtrl(IDOK,IDS_STRING_CONFIRM);
	InitializeCtrl(IDCANCEL,IDS_STRING_CANCEL);

	SetDlgItemTextW(IDC_EDIT_SRC_PROJECT,m_sSourceName);
	return TRUE;
}

BOOL CCopyProjectDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CCopyProjectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCopyProjectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCopyProjectDlg::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


void CCopyProjectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString sTargetName;
	GetDlgItemTextW(IDC_EDIT_NEW_PROJECT, sTargetName);
	m_wstrTargetName = sTargetName.GetString();
	CDialogEx::OnOK();
}


void CCopyProjectDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CCopyProjectDlg::OnPaint()
{
	CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ClientToScreen(&rcWindow);
	BITMAP bitmap;
	m_bgImg.GetBitmap(&bitmap);
	CBitmap* pPngOld = dcMem.SelectObject(&m_bgImg);
	CRect rcParentWindow;
	GetParent()->GetWindowRect(&rcParentWindow);
	ClientToScreen(&rcParentWindow);
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(),
		&dcMem,
		rcWindow.left - rcParentWindow.left, rcWindow.top - rcParentWindow.top, rect.Width(), rect.Height(), SRCCOPY);
}


HBRUSH CCopyProjectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_SRC_PROJECT ||
		ctrlID == IDC_STATIC_NEW_PROJECT)
	{
		pDC->SetBkMode(TRANSPARENT);
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}
