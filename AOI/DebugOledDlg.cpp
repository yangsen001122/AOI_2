#include "stdafx.h"
#include "AOI.h"
#include "DebugOledDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(DebugOledDlg, CDialogEx)

DebugOledDlg::DebugOledDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DEBUG_OLED, pParent)
{

}

DebugOledDlg::~DebugOledDlg()
{
}

void DebugOledDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DebugOledDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL DebugOledDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwStyle = GetStyle();//获取旧样式  
	DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	dwNewStyle &= dwStyle;//按位与将旧样式去掉  
	SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式  
	DWORD dwExStyle = GetExStyle();//获取旧扩展样式  
	DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	dwNewExStyle &= dwExStyle;//按位与将旧扩展样式去掉  
	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式  
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//样式改变了，窗口位置和大小保持原来不变！  
	//get screen size
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	::SetWindowPos(m_hWnd, HWND_TOP, nWidth + 4, 0,720,1280, SWP_SHOWWINDOW);
	//::SetWindowPos(m_hWnd, HWND_TOP, nWidth + 1, 0, 1280, 720, SWP_SHOWWINDOW);
	ModifyStyle(WS_CAPTION, 0, 0);  // hide caption
	//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	GetWindowRect(rcScreen);
	rcScreenOled = rcScreen;
	GetClientRect(rcScreen);
	rcScreenOledClient = rcScreen;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
	//wangl
// 	DWORD dwStyle = GetStyle();//获取旧样式  
// 	DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
// 	dwNewStyle &= dwStyle;//按位与将旧样式去掉  
// 	SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式  
// 	DWORD dwExStyle = GetExStyle();//获取旧扩展样式  
// 	DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
// 	dwNewExStyle &= dwExStyle;//按位与将旧扩展样式去掉  
// 	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式  
//	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//样式改变了，窗口位置和大小保持原来不变！  

// 	int cx = GetSystemMetrics(SM_CXSCREEN);//屏幕像素宽度
// 	int cy = GetSystemMetrics(SM_CYSCREEN);//屏幕像素高度
// 	SetWindowPos(NULL, cx + 1, 0, 720, 1280, 0);//第二屏幕在主屏右侧
// 	//	ShowWindow(SW_MAXIMIZE);

// 	//wangl

}
void DebugOledDlg::DrawRgbImg(COLORREF picColor)
{
	CWnd *pWnd = this; //控件ID
	CRect rect;
	pWnd->GetClientRect(&rect);
	CDC *pDC = pWnd->GetDC();//选定画图DC

	CPen MyPen, *OldPen;//画边框
	MyPen.CreatePen(PS_DASH, 2, picColor);
	OldPen = pDC->SelectObject(&MyPen);

	CBrush MyBrush, *OldBrush;//画刷
	MyBrush.CreateSolidBrush(picColor);
	OldBrush = pDC->SelectObject(&MyBrush);

	//定义坐标点
	CPoint point1(0, 0);
	CPoint point2(720, 1280);

	//绘制矩形
	pDC->Rectangle(point1.x, point1.y, point2.x, point2.y);

	//清除
	pDC->SelectObject(OldPen);
	MyPen.DeleteObject();
	pDC->SelectObject(OldBrush);
	MyBrush.DeleteObject();
}