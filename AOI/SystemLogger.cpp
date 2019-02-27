#include "stdafx.h"
#include "SystemLogger.h"
#include "utils.h"
#include "SystemManager.h"


extern CSystemManager gSystemMgr;

CSystemLogger::CSystemLogger()
{
	TCHAR  moduleFileName[MAX_PATH];
	::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	(_tcsrchr(moduleFileName, _T('\\')))[1] = 0;
	CString strPath = moduleFileName;
	strPath.Append(_T("\log\\"));
	m_wstrPath = strPath.GetString();
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}
	m_wstrPath.append(L"system");
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}
	m_wstrLastLog = L"";
	::InitializeCriticalSection(&m_csLogger);
}

CSystemLogger::CSystemLogger(wstring wstrFolderName)
{
	TCHAR  moduleFileName[MAX_PATH];
	::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	(_tcsrchr(moduleFileName, _T('\\')))[1] = 0;
	CString strPath = moduleFileName;
	strPath.Append(_T("\log\\"));
	m_wstrPath = strPath.GetString();
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}
	m_wstrPath.append(wstrFolderName);
	if (!_wmkdir(m_wstrPath.c_str()))
	{
		//TODO
	}
	m_wstrLastLog = L"";
	::InitializeCriticalSection(&m_csLogger);
}


CSystemLogger::~CSystemLogger()
{
	::DeleteCriticalSection(&m_csLogger);
}

void CSystemLogger::QueueUpLog(wstring strMsg, bool bHighResolution, eEVENT_LEVEL nLevel)
{
	::EnterCriticalSection(&m_csLogger);
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString sTimestamp;
	if (!bHighResolution)
		sTimestamp.Format(_T("%02d:%02d:%02d"), dt.wHour, dt.wMinute, dt.wSecond);
	else
		sTimestamp.Format(_T("%02d:%02d:%02d:%03d"), dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
	wstring strTimestamp = CT2W(sTimestamp);
	wstring wstrLevel = L"INVALID";
	switch (nLevel)
	{
	case(LV_EVENT):
		wstrLevel = L"EVENT";
		break;
	case(LV_WARNING):
		wstrLevel = L"WARNING";
		break;
	case(LV_ERROR):
		wstrLevel = L"ERROR";
		break;
	default:
		break;
	}
	wstring strLog = strTimestamp+L"$" +wstrLevel + L"$" +gSystemMgr.m_strOperatorID.GetBuffer()+L":"+strMsg;
	m_dictLogMsg.insert(pair<wstring, wstring>(strTimestamp, strLog));
	m_wstrLastLog = strLog;
	::LeaveCriticalSection(&m_csLogger);
}

void CSystemLogger::QueueUpLogAndReport(wstring wstrMsg, eEVENT_LEVEL nLevel)
{
	::EnterCriticalSection(&m_csLogger);
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString sTimestamp;
	sTimestamp.Format(_T("%02d:%02d:%02d:%03d"), dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
	wstring strTimestamp = CT2W(sTimestamp);
	wstring wstrLevel = L"INVALID";
	switch (nLevel)
	{
	case(LV_EVENT):
		wstrLevel = L"EVENT";
		break;
	case(LV_WARNING):
		wstrLevel = L"WARNING";
		break;
	case(LV_ERROR):
		wstrLevel = L"ERROR";
		break;
	default:
		break;
	}
	m_wstrLastLog = strTimestamp + L"$" + wstrLevel + L"$" + gSystemMgr.m_strOperatorID.GetBuffer() + L":" + wstrMsg;
	m_dictLogMsg.insert(pair<wstring, wstring>(strTimestamp, m_wstrLastLog));

	CString sLog = m_wstrLastLog.c_str();
	BSTR bstr = sLog.AllocSysString();
	::PostMessage(m_hViewHandle, WM_SEND_LOG_MSG_TO_MAIN_UI, (WPARAM)bstr, (LPARAM)nLevel);
	::LeaveCriticalSection(&m_csLogger);
}

CMutex cMutlog(false, NULL);
void CSystemLogger::FlushLogCache() 
{
	std::multimap<wstring, wstring> tmpMsgArray;
	::EnterCriticalSection(&m_csLogger);
	tmpMsgArray = m_dictLogMsg;
	m_dictLogMsg.clear();
	::LeaveCriticalSection(&m_csLogger);

	if (tmpMsgArray.empty())
	{
		return;
	}
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString strNewDate;
	strNewDate.Format(_T("%d-%02d-%02d"), dt.wYear, dt.wMonth, dt.wDay);
	wstring wstrLogFile = m_wstrPath +L"\\"+ strNewDate.GetString() + L".log";
	string strFile = localUtils::utils::Unicode2Utf8(wstrLogFile);
	cMutlog.Lock();
	FILE* fp1 = nullptr;
	fopen_s(&fp1, strFile.c_str(), "a");
	for (auto it = tmpMsgArray.begin(); it != tmpMsgArray.end(); it++)
	{
	   string strLog = localUtils::utils::Unicode2Utf8(it->second);
	   fputs(strLog.c_str(), fp1);
	   fputs("\r\n", fp1);
	}
	fclose(fp1);
	cMutlog.Unlock();
}

