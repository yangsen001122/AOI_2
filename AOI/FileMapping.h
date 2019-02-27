#pragma once
#include <winsock2.h>
#include <windows.h>
#include <string>
#include "TypeDef.h"

using namespace std;

class FileMapping
{
public:
	FileMapping();
	~FileMapping();

	PVOID m_pview[2];
	HANDLE	m_Read[2], m_Write[2];
	HANDLE	m_SendResult[2];
	HANDLE	m_HeartBeat[2];
	
	int m_CommOutTime;
	bool CreatFileMapping();
	bool SendMsg(FILEMAPPING_COMM_INFO msg);
	
private:
	CString m_FileMappingName[2];
	CString m_ReadSemaphoreName[2];
	CString m_WriteSemaphoreName[2]; 
	CString m_SendResultName[2];
	CString m_HeartBeatName[2];

};
