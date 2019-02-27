#pragma once
#include <Windows.h>
#include <map>

using namespace std;

class WindowBackgroundBrushHelper
{
public:
	WindowBackgroundBrushHelper();
	~WindowBackgroundBrushHelper();

	void AddSubWindow(CWnd* ptrParent,CWnd* ptrChild, HBITMAP parent_background);
	HBRUSH GetHBRUSH(HWND hwnd);
private:
	std::map<HWND, HBRUSH> m_dictWnd_Brush;
};

