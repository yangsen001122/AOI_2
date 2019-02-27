#include "stdafx.h"
#include "WindowbackgroundBrushHelper.h"


WindowBackgroundBrushHelper::WindowBackgroundBrushHelper()
{
}


WindowBackgroundBrushHelper::~WindowBackgroundBrushHelper()
{
	for (auto it = m_dictWnd_Brush.begin(); it != m_dictWnd_Brush.end(); ++it)
	{
		ATLASSERT(it->second != NULL);
		::DeleteObject(it->second);
	}
	m_dictWnd_Brush.clear();
}

void WindowBackgroundBrushHelper::AddSubWindow(CWnd* ptrParent, CWnd* ptrChild, HBITMAP parent_background)
{
	RECT rect;
	RECT parent_window_rect;
	RECT parent_client_rect;
	ptrParent->GetWindowRect(&parent_window_rect);
	ptrParent->GetClientRect(&parent_client_rect);

	ptrChild->GetWindowRect(&rect);
	ptrParent->ScreenToClient(&rect);

	long width = rect.right - rect.left;
	long height = rect.bottom - rect.top;

	CDC sdc;
	CDC ddc;
	sdc.CreateCompatibleDC(NULL);
	ddc.CreateCompatibleDC(NULL);
	CBitmap destBmp;
	destBmp.CreateCompatibleBitmap(ptrParent->GetDC(), width, height);
	sdc.SelectObject(parent_background);
	ddc.SelectObject(destBmp);
	ddc.BitBlt(0, 0, width, height, &sdc, rect.left, rect.top, SRCCOPY);

	CBrush bs;
	bs.CreatePatternBrush(&destBmp);
	m_dictWnd_Brush[ptrChild->m_hWnd] = bs;

	bs.Detach();
}

HBRUSH WindowBackgroundBrushHelper::GetHBRUSH(HWND hwnd)
{
	ATLASSERT(hwnd);

	if (m_dictWnd_Brush.find(hwnd) != m_dictWnd_Brush.end())
	{
		return m_dictWnd_Brush[hwnd];
	}
	return NULL;
}
