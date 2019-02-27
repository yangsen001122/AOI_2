#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost\lexical_cast.hpp>
#include "TypeDef.h"

using namespace std;
using namespace boost;

class CSystemLogger
{
public:
	CSystemLogger();
	CSystemLogger(wstring wstrFolderName);
	~CSystemLogger();
	HWND m_hViewHandle;
	void QueueUpLog(wstring strMsg, bool bHighResolution = true, eEVENT_LEVEL nLevel=LV_EVENT);
	void FlushLogCache();
	void QueueUpLogAndReport(wstring wstrMsg, eEVENT_LEVEL nLevel);
private:
	wstring          m_wstrPath;
	wstring          m_wstrLastLog;
	CString          m_sLastLogMsg;
	CRITICAL_SECTION m_csLogger;
	std::multimap<wstring, wstring> m_dictLogMsg;
};

