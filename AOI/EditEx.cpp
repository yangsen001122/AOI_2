#include "stdafx.h"
#include "EditEx.h"

IMPLEMENT_DYNAMIC(CEditEx,CEdit)
CEditEx::CEditEx()
{

}


CEditEx::~CEditEx()
{

}

BOOL CEditEx::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)

END_MESSAGE_MAP()