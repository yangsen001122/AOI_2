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
	DWORD dwStyle = GetStyle();//��ȡ����ʽ  
	DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	dwNewStyle &= dwStyle;//��λ�뽫����ʽȥ��  
	SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//���ó��µ���ʽ  
	DWORD dwExStyle = GetExStyle();//��ȡ����չ��ʽ  
	DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	dwNewExStyle &= dwExStyle;//��λ�뽫����չ��ʽȥ��  
	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//�����µ���չ��ʽ  
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//��ʽ�ı��ˣ�����λ�úʹ�С����ԭ�����䣡  
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
				  // �쳣: OCX ����ҳӦ���� FALSE
	//wangl
// 	DWORD dwStyle = GetStyle();//��ȡ����ʽ  
// 	DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
// 	dwNewStyle &= dwStyle;//��λ�뽫����ʽȥ��  
// 	SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//���ó��µ���ʽ  
// 	DWORD dwExStyle = GetExStyle();//��ȡ����չ��ʽ  
// 	DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
// 	dwNewExStyle &= dwExStyle;//��λ�뽫����չ��ʽȥ��  
// 	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);//�����µ���չ��ʽ  
//	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);//��ʽ�ı��ˣ�����λ�úʹ�С����ԭ�����䣡  

// 	int cx = GetSystemMetrics(SM_CXSCREEN);//��Ļ���ؿ��
// 	int cy = GetSystemMetrics(SM_CYSCREEN);//��Ļ���ظ߶�
// 	SetWindowPos(NULL, cx + 1, 0, 720, 1280, 0);//�ڶ���Ļ�������Ҳ�
// 	//	ShowWindow(SW_MAXIMIZE);

// 	//wangl

}
void DebugOledDlg::DrawRgbImg(COLORREF picColor)
{
	CWnd *pWnd = this; //�ؼ�ID
	CRect rect;
	pWnd->GetClientRect(&rect);
	CDC *pDC = pWnd->GetDC();//ѡ����ͼDC

	CPen MyPen, *OldPen;//���߿�
	MyPen.CreatePen(PS_DASH, 2, picColor);
	OldPen = pDC->SelectObject(&MyPen);

	CBrush MyBrush, *OldBrush;//��ˢ
	MyBrush.CreateSolidBrush(picColor);
	OldBrush = pDC->SelectObject(&MyBrush);

	//���������
	CPoint point1(0, 0);
	CPoint point2(720, 1280);

	//���ƾ���
	pDC->Rectangle(point1.x, point1.y, point2.x, point2.y);

	//���
	pDC->SelectObject(OldPen);
	MyPen.DeleteObject();
	pDC->SelectObject(OldBrush);
	MyBrush.DeleteObject();
}