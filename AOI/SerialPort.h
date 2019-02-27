#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "typedef.h"
using namespace std;

#define  WM_COMM_BREAK_DETECTED		WM_USER + 1//
#define  WM_COMM_CTS_DETECTED		WM_USER + 2//
#define  WM_COMM_DSR_DETECTED		WM_USER + 3//
#define  WM_COMM_ERR_DETECTED		WM_USER + 4//
#define  WM_COMM_RING_DETECTED      WM_USER + 5//
#define  WM_COMM_RLSD_DETECTED      WM_USER + 6//
//#define  WM_COMM_RXCHAR_DETECTED    WM_USER + 7//
#define  WM_COMM_RXFLAG_DETECTED    WM_USER + 8//
#define  WM_COMM_TXEMPTY_DETECTED   WM_USER + 9//
#define  WM_COMM_WRTOK_DETECTED     WM_USER + 10//

const size_t READ_BUFFER_SIZE  = 64;

class CChannelControlPanel;

class CSerialPort
{
public:
	CSerialPort(int nPortNum,int rxBuffSize = 5);
	virtual ~CSerialPort();
    bool  InitPort(UINT baud = 9600, char parity = 'N', UINT databits = 8, UINT atopsbits = 1, DWORD dwCommEvents = EV_RXCHAR|EV_CTS);
	void  SetOwnerWindowPtr(CWnd* pWnd);
	bool  StartMonitoring();
	bool  RestartMonitoring();
	void  SendCmdToPort(const std::vector<byte>& cmd);
	void  SendCmdToPort(const std::string& strCmd);
	void  ReceiveChar(COMSTAT comstat);
	bool  GetReadFinishedFlag();
	void  ClosePort();
	bool  SetPortDCB(UINT baud = 9600,UINT databits = 8, UINT atopsbits = 1);

private:
	unsigned int __stdcall SerialPortThreadFunc();
	void TryToShutdown();
	void ProcessErrorMessage(char* ErrorText);
	void WritePort();

private:
	bool				 m_bThreadAlive;
	HANDLE				 m_hShutdownEvent;
	HANDLE				 m_hComm;
	HANDLE				 m_hWriteEvent;
	HANDLE				 m_hEventArray[3];
	HANDLE				 m_hThread;
	UINT				 m_iThreadId;
	OVERLAPPED			 m_ov;
	DCB					 m_dcb;
	COMMTIMEOUTS		 m_CommTimeouts;
	CRITICAL_SECTION	 m_csCommunicationSync;
	int				     m_nPortNum;
	std::string          m_writeBuffer;
	bool                 m_bReadingPortFinished;
	int                  m_nRxBuffSize;
	std::vector<byte>    m_RXBuff;
	CWnd*                m_pOwnerWnd;

	union _SerialPortProc
	{
		unsigned (_stdcall * ThreadProc)(void*);
		unsigned (_stdcall CSerialPort::*MemProc)();
	} SerialPortProc;
};