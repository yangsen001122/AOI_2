#include "stdafx.h"
#include "SystemLogger.h"


CSystemLogger::CSystemLogger(wstring wstrFolderName)
{
	TCHAR  moduleFileName[MAX_PATH];
	::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	(_tcsrchr(moduleFileName, _T('\\')))[1] = 0;
	CString strPath = moduleFileName;
	strPath.Append(_T("log\\"));
	m_wstrPath = strPath.GetString();
	m_wstrPath.append(wstrFolderName);
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}
	::InitializeCriticalSection(&m_csLogger);
}


CSystemLogger::~CSystemLogger()
{

}

void CSystemLogger::CreateJigLogFolder(int nJigFolder)
{
	wostringstream woss;
	woss << m_wstrPath << L"\\" <<L"JIG#" << nJigFolder;

	m_wstrPath = woss.str();
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}

	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString strDate;
	strDate.Format(_T("%d-%02d-%02d"), dt.wYear, dt.wMonth, dt.wDay);
	m_wstrPath = m_wstrPath + L"\\" + strDate.GetString() + L".log";
	m_wstrLastLog = L"";
}

string CSystemLogger::Unicode2Utf8(const wstring& unicodeStr)
{
	std::string retStr("");
	int strLen = (int)::WideCharToMultiByte(CP_UTF8, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), NULL, 0, NULL, NULL);
	if (strLen == 0)
		return retStr;
	std::vector<char> strBuffer(strLen, '\0');
	::WideCharToMultiByte(CP_UTF8, NULL, unicodeStr.c_str(), (int)wcslen(unicodeStr.c_str()), &strBuffer[0], strLen, NULL, NULL);

	retStr.assign(strBuffer.begin(), strBuffer.end());
	return retStr;
}

void CSystemLogger::QueueUpLog(wstring strNote, double dDur)
{
	size_t sz = m_dictLogMsg.size();
	if (sz > 256)
		return;

	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	ATL::CString sTimestamp;
	sTimestamp.Format(_T("%02d:%02d:%02d:%03d: "), dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
	wstring strTimestamp = ATL::CT2W(sTimestamp);
	if (m_dictLogMsg.find(strTimestamp) != m_dictLogMsg.end())
		return;
	std::wostringstream woss;
	woss << strTimestamp << strNote << L" -- " << dDur;
	wstring strLog = woss.str();
	m_dictLogMsg[strTimestamp] = strLog;
	m_wstrLastLog = strLog;
}


void CSystemLogger::FlushLogCache()
{
	if (m_dictLogMsg.empty())
	{
		return;
	}
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	ATL::CString strNewDate;
	strNewDate.Format(_T("%d-%02d-%02d"), dt.wYear, dt.wMonth, dt.wDay);
	wstring wstrLogFile = m_wstrPath + L"\\" + strNewDate.GetString() + L".log";
	string strFile = Unicode2Utf8(wstrLogFile);
	FILE* fp = nullptr;
	fopen_s(&fp, strFile.c_str(), "a");
	for (auto it = m_dictLogMsg.begin(); it != m_dictLogMsg.end(); it++)
	{
		string strLog = Unicode2Utf8(it->second);
		fputs(strLog.c_str(), fp);
		fputs("\r\n", fp);
	}
	fclose(fp);
	m_dictLogMsg.clear();
}

void CSystemLogger::QueueUpLogAndReport(wstring wstrMsg)
{
	::EnterCriticalSection(&m_csLogger);
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString sTimestamp;
	sTimestamp.Format(_T("%02d:%02d:%02d:%03d:"), dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
	wstring strTimestamp = ATL::CT2W(sTimestamp);
	m_wstrLastLog = strTimestamp + wstrMsg;
	m_dictLogMsg.insert(pair<wstring, wstring>(strTimestamp, m_wstrLastLog));

	CString sLog = m_wstrLastLog.c_str();
	BSTR bstr = sLog.AllocSysString();
	::LeaveCriticalSection(&m_csLogger);
}
