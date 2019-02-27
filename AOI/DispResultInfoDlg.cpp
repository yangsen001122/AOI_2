
#include "stdafx.h"
#include "AOI.h"
#include "DispResultInfoDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CDispResultInfoDlg, CDialogEx)

CDispResultInfoDlg::CDispResultInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DEFECTS_DISPLAY, pParent)
{

}

CDispResultInfoDlg::~CDispResultInfoDlg()
{

}

void CDispResultInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX_DEFECT_DETAIL, m_listBox);
}

BEGIN_MESSAGE_MAP(CDispResultInfoDlg, CDialogEx)
	ON_LBN_DBLCLK(IDC_LIST_BOX_DEFECT_DETAIL, &CDispResultInfoDlg::OnLbnDblclkListBoxDefectDetail)
END_MESSAGE_MAP()

BOOL CDispResultInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//TODO:
	return TRUE;  			
}


void CDispResultInfoDlg::OnLbnDblclkListBoxDefectDetail()
{
	int nIndex = m_listBox.GetCurSel();
	if (0 <= nIndex&&nIndex <= m_listBox.GetCount())
	{
		CString strText;
		m_listBox.GetText(nIndex, strText);
		if (strText != L"")
		{
			::PostMessage(gSystemMgr.m_ptrMainUi->m_hWnd, WM_UPDATE_DISP_MAIN, WPARAM(WINDOW_MAIN_SELECTED_ONE_DEFECT), LPARAM(nIndex));
		}
	}
}
