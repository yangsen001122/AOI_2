// MessageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AOI.h"
#include "MessageDlg.h"
#include "afxdialogex.h"

class CLabelCtrl;
extern CSystemManager gSystemMgr;
// CMessageDlg 对话框

IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)


CMessageDlg::CMessageDlg(int nStrID, WPARAM wParam, CWnd* pParent /*= NULL*/)
	: CDialogEx(IDD_DIALOG_MESSAGE, pParent),
	m_MsgConfirm(false),
	m_MsgChoose(false),
	m_StrId(-1)
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
	m_penTitle.CreatePen(PS_DASH, 1, RGB(4, 40, 94));
	m_penFrame.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));

	m_StrId = nStrID;
	int MsgType = int(wParam);
	if (MsgType == MESSAGE_CHOOSE)
	m_MsgChoose = true;
	
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BOOL CMessageDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_RETURN)
	{
		CWnd* ctrlID = GetFocus();
		if (ctrlID != NULL)
		{
			/*	if (IDC_EDIT_PASSWORD == ctrlID->GetDlgCtrlID())
				{
					OnBnClickedButtonLogin();
				}*/
		}
		return TRUE;
	}
	if (param == VK_ESCAPE || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CMessageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_MESSAGE_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_MESSAGE_UI_HEIGHT * fRatioV);

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
	CreateFunctionButton(rcClient);
	DisplayMessage(m_StrId);
	return TRUE;
}

void CMessageDlg::CreateFunctionButton(CRect& rcClient)
{
	CRect rcBtn,rcMsg,rcPic;
	int nGapH = int(20 * gSystemMgr.m_fRatioH);
	int nGapV = int(10 * gSystemMgr.m_fRatioV);
	int nLen = int(80 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);
	rcBtn.right = rcClient.right - nGapH;
	rcBtn.left = rcBtn.right - nLen;
	rcBtn.bottom = rcClient.bottom - nGapV;
	rcBtn.top = rcBtn.bottom - nHeight;
	InitializeCtrl(IDC_BUTTON_OK, rcBtn, IDS_STRING_MESSAGE_OK);
	rcPic.left = rcClient.left;
	rcPic.right= rcPic.left+ int(128 * gSystemMgr.m_fRatioH);
	rcPic.top=rcClient.top+ int(100 * gSystemMgr.m_fRatioV);
	rcPic.bottom=rcClient.bottom- int(100 * gSystemMgr.m_fRatioV);
	m_labelMsg.Create(_T(""), WS_CHILD | WS_VISIBLE, rcPic, this, NULL);
	CStatic* pMsgPanel = (CStatic*)GetDlgItem(IDC_BUTTON_CANCEL);
	pMsgPanel->ShowWindow(SW_HIDE);
	if (m_MsgChoose)
	{
		pMsgPanel->ShowWindow(SW_SHOW);
		rcBtn.right = rcBtn.left - nGapH;
		rcBtn.left = rcBtn.right - nLen;
		InitializeCtrl(IDC_BUTTON_CANCEL, rcBtn, IDS_STRING_MESSAGE_CANCEL);
		m_labelMsg.SetLabelImage(IMG_PNG_ENQUIRE);
	}
	else
	{
		m_labelMsg.SetLabelImage(IMG_PNG_WARNING);
	}
	m_labelMsg.BringWindowToTop();
	m_labelMsg.EnableWindow(TRUE);
	m_labelMsg.Invalidate();
	rcMsg.left = rcClient.left+ int(128 * gSystemMgr.m_fRatioH);
	rcMsg.right = rcMsg.left+ int(384 * gSystemMgr.m_fRatioH);
	rcMsg.top = rcClient.top+ int(140 * gSystemMgr.m_fRatioV);
	rcMsg.bottom = rcMsg.top+ int(40 * gSystemMgr.m_fRatioV);
	InitializeCtrl(IDC_STATIC_MESSAGE_SHOW, rcMsg, -1, false);
}


void CMessageDlg::InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID /*= -1*/, bool bSmallFont /*= false*/)
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


void CMessageDlg::DisplayMessage(int nCtrlID)
{
	if (nCtrlID != -1)
	{
		CStatic* pMsgPanel = (CStatic*)GetDlgItem(IDC_STATIC_MESSAGE_SHOW);
		CString sMsg;
		sMsg.LoadStringW(nCtrlID);
		pMsgPanel->ShowWindow(SW_HIDE);
		pMsgPanel->SetWindowText(sMsg);
		pMsgPanel->ShowWindow(SW_SHOW);
	}

}

HBRUSH CMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_MESSAGE_SHOW)
	{
		pDC->SetTextColor(RGB(10, 10, 90));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CMessageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rcClient;
	GetClientRect(&rcClient);
	CPen* pOldPen = dc.SelectObject(&m_penTitle);
	dc.MoveTo(rcClient.left, rcClient.top +60);
	dc.LineTo(rcClient.right, rcClient.top + 60);
	dc.MoveTo(rcClient.left , rcClient.bottom-60);
	dc.LineTo(rcClient.right, rcClient.bottom - 60);
	dc.SelectObject(pOldPen);

	pOldPen = dc.SelectObject(&m_penFrame);
	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.right, rcClient.top);

	dc.MoveTo(rcClient.TopLeft());
	dc.LineTo(rcClient.left, rcClient.bottom);
	dc.MoveTo(rcClient.BottomRight());
	dc.LineTo(rcClient.right, rcClient.top);
	dc.MoveTo(rcClient.BottomRight());
	dc.LineTo(rcClient.left, rcClient.bottom);
	dc.SelectObject(pOldPen);

}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OK, &CMessageDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CMessageDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CMessageDlg 消息处理程序


void CMessageDlg::OnBnClickedButtonOk()
{
	if (m_MsgChoose)
		gSystemMgr.m_MsgResult = true;
	CDialogEx::OnOK();
}



void CMessageDlg::OnBnClickedButtonCancel()
{
	CDialogEx::OnOK();
}
