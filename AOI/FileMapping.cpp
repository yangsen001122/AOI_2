#include "stdafx.h"
#include "FileMapping.h"

FileMapping::FileMapping()
{
	CString strName = L"";
	for (int i = 0; i < 2; i++)
	{
		strName.Format(_T("ShareMem%d"), i);
		m_FileMappingName[i] = strName;
		strName.Format(_T("ReadSemaphore%d"), i);
		m_ReadSemaphoreName[i] = strName;
		strName.Format(_T("WriteSemaphore%d"), i);
		m_WriteSemaphoreName[i] = strName;
		strName.Format(_T("SendResult%d"), i);
		m_SendResultName[i] = strName;
		strName.Format(_T("HeartBeat%d"), i);
		m_HeartBeatName[i] = strName;
	}
	m_CommOutTime = 100;
}

FileMapping::~FileMapping()
{
	for (int i = 0; i < 2; i++)
	{
		::UnmapViewOfFile(m_pview[i]);
		CloseHandle(m_Read[i]);
		CloseHandle(m_Write[i]);
		CloseHandle(m_SendResult[i]);
		CloseHandle(m_HeartBeat[i]);
	}
}

bool FileMapping::CreatFileMapping()
{
	for (int i = 0; i < 2; i++)
	{
		HANDLE hfile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 65535, m_FileMappingName[i]);
		m_pview[i] = ::MapViewOfFile(hfile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
		m_Read[i] = ::CreateSemaphore(NULL, 0, 1, m_ReadSemaphoreName[i]);
		m_Write[i] = ::CreateSemaphore(NULL, 1, 1, m_WriteSemaphoreName[i]);
		m_SendResult[i] = ::CreateSemaphore(NULL, 0, 1, m_SendResultName[i]);
		m_HeartBeat[i] = ::CreateSemaphore(NULL, 0, 1, m_HeartBeatName[i]);
	}
	return true;
}

bool FileMapping::SendMsg(FILEMAPPING_COMM_INFO msg)
{
	if (WAIT_TIMEOUT==WaitForSingleObject(m_Write[0], m_CommOutTime))
	{
		return false;
	}
	memcpy_s(m_pview[0], 1024, &msg, sizeof(msg));
	ReleaseSemaphore(m_Read[0], 1, NULL);
	return (WAIT_TIMEOUT != WaitForSingleObject(m_SendResult[0], m_CommOutTime));
}

