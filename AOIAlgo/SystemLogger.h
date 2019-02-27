#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost\lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <atlstr.h>
#include <atlconv.h>

using ATL::CString;

using namespace std;
using namespace boost;

class CSystemLogger
{
public:
	CSystemLogger(wstring wstrFolderName);
	~CSystemLogger();
	HWND m_hViewHandle;
	string Unicode2Utf8(const wstring& unicodeStr);
	void QueueUpLog(wstring strNote, double dDur);
	void FlushLogCache();
	void QueueUpLogAndReport(wstring wstrMsg);
	void CreateJigLogFolder(int nJigFolder);
private:
	wstring m_wstrPath;
	wstring m_wstrLastLog;
	std::map<wstring, wstring> m_dictLogMsg;
	CRITICAL_SECTION m_csLogger;
};

