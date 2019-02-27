#include "stdafx.h"
#include "AOI.h"
#include "LoadingDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CLoadingDlg, CDialogEx)

CLoadingDlg::CLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOADING, pParent), 
	m_nProgressPos(0),
	m_nProgressMin(0),
	m_nProgressMax(0),
	m_nProgressStep(0),
	m_nLoadingTime(30)
{

}

CLoadingDlg::~CLoadingDlg()
{
}

void CLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_CProg);
}


BEGIN_MESSAGE_MAP(CLoadingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CLoadingDlg::SetLoadingTime(int nloadingTime)
{
	m_nLoadingTime = nloadingTime;
}
void CLoadingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	HMODULE module = GetModuleHandle(0);
	TCHAR pFileName[MAX_PATH];
	GetModuleFileName(module, pFileName, MAX_PATH);
	CString csFullPath(pFileName);
	int nPos = csFullPath.ReverseFind(_T('\\'));
	if (nPos <= 0)
		return;
	CString path = csFullPath.Left(nPos);
	CRect rc;
	GetWindowRect(&rc);
	ATL::CImage m_image;
	if (m_image.Load(path + _T("\\ui_pictures\\bkground.png")) == S_OK)
	{
		m_image.Draw(GetDC()->GetSafeHdc(), CRect(0, 0, rc.Width(), rc.Height()));
	}
}

void CLoadingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_FOR_PROGRESS_BAR)
	{
		if (m_nProgressPos == m_nProgressMax)
		{
			CDialogEx::OnCancel();
		}
		m_CProg.StepIt();
		m_nProgressPos = m_CProg.GetPos();
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CLoadingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rcScreen.Width(), rcScreen.Height(), SWP_SHOWWINDOW);
	ModifyStyle(WS_CAPTION, 0, 0);
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	m_nProgressPos = 0;
	m_nProgressMin = 0;
	m_nProgressMax = 500;
	m_nProgressStep = 1;
	//m_CProg.MoveWindow(210, 880, 1500, 20);
	m_CProg.MoveWindow(int(rcScreen.Width()*0.1), int(rcScreen.Height()*0.8), int(rcScreen.Width()*0.8), int(rcScreen.Height()*0.018));
	m_CProg.SetRange(m_nProgressMin, m_nProgressMax);
	m_CProg.SetStep(m_nProgressStep);
	m_CProg.SetPos(m_nProgressPos);
	//m_CProg.OffsetPos(-100);
	int nTimeStep = (int)m_nLoadingTime * 1000 / m_nProgressMax;
	SetTimer(TIMER_ID_FOR_PROGRESS_BAR, nTimeStep, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
