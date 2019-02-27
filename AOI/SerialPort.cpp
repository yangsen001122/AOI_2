#include "StdAfx.h"
#include "SerialPort.h"
#include <assert.h>
#include <process.h>
#include <afxwin.h>


//extern CSystemLogger gSystemLogger;

CSerialPort::CSerialPort(int nPortNum, int rxBuffSize)
	:m_nPortNum(nPortNum),
	m_nRxBuffSize(rxBuffSize),
	m_hComm(NULL),
	m_hThread(NULL),
	m_hWriteEvent(0),
	m_hShutdownEvent(0),
	m_bReadingPortFinished(false),
	m_bThreadAlive(false),
	m_pOwnerWnd(nullptr)
{
	::memset((void*)&m_ov, 0x00, sizeof(OVERLAPPED));

	m_ov.hEvent		 = CreateEventW(NULL,TRUE,FALSE,NULL);  assert(NULL != m_ov.hEvent);//�Ƕ�д�¼�
	m_hWriteEvent	 = CreateEventW(NULL,TRUE,FALSE,NULL);  assert(NULL != m_hWriteEvent);
	m_hShutdownEvent = CreateEventW(NULL,TRUE,FALSE,NULL);  assert(NULL != m_hShutdownEvent);

	m_hEventArray[0] = m_hShutdownEvent;
	m_hEventArray[1] = m_hWriteEvent;
	m_hEventArray[2] = m_ov.hEvent;

	m_CommTimeouts.ReadIntervalTimeout			= 100;//�������ʱ
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= 100;//��ʱ��ϵ��
	m_CommTimeouts.ReadTotalTimeoutConstant		= 100;//��ʱ�䳣��
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= 100;//��ʱ��ϵ��
	m_CommTimeouts.WriteTotalTimeoutConstant	= 100;//дʱ�䳣��

	InitializeCriticalSection(&m_csCommunicationSync);
	m_RXBuff = std::vector<byte>(m_nRxBuffSize,0x00);
}

CSerialPort::~CSerialPort()
{
	TryToShutdown();
	m_bThreadAlive = FALSE;
	TRACE("Thread ended\n");
	::DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT baudrate, char parity, UINT databits, UINT stopsbits, DWORD dwCommEvents)
{                       //������19200����żУ��No Parity������λ8 bit��ֹͣλ1 stop bit
	if (m_bThreadAlive)
	{
		TryToShutdown();
	}
	if(m_nPortNum < 0)
	{
		return false;
	}
	ResetEvent(m_ov.hEvent);
	ResetEvent(m_hWriteEvent);
	ResetEvent(m_hShutdownEvent);

	EnterCriticalSection(&m_csCommunicationSync);

	if (NULL != m_hComm)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	std::wostringstream oss1, oss2;
	oss1 << L"\\\\.\\COM" << m_nPortNum;
	oss2 << L"baud=" << baudrate <<L" parity=" << parity <<L" data=" << databits <<L" stop=" << stopsbits;
	wstring wstrPort = oss1.str();
	wstring wstrBaud = oss2.str();
	m_hComm = CreateFileW(//�򿪴���
		wstrPort.c_str(), 
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		OPEN_EXISTING, // ����CreateFile�����ļ�֮��������豸ʱ������ʹ��OPEN_EXISTING���� ������
		FILE_FLAG_OVERLAPPED|FILE_ATTRIBUTE_NORMAL,//�ص���ʽ
		NULL);

	DWORD retError= GetLastError();
	if (INVALID_HANDLE_VALUE == m_hComm)//��comʧ��
	{
		return false;
	}
	bool bOk = true;
	if(!SetCommTimeouts(m_hComm,&m_CommTimeouts))//���ó�ʱ
	{
		ProcessErrorMessage("SetCommTimeouts"); bOk = false;
		goto LastW;
	}
	if(!SetCommMask(m_hComm,dwCommEvents))//���ô���ͨ���¼�
	{
		ProcessErrorMessage("SetCommMask"); bOk = false;
		goto LastW;
	}
	if(!GetCommState(m_hComm,&m_dcb))//��ȡ��������(������,У��,ֹͣλ,����λ��).
	{
		ProcessErrorMessage("GetCommState"); bOk = false;
		goto LastW;
	}
	SetPortDCB(baudrate,databits, stopsbits);
	//m_dcb.EvtChar     = 'q'; /* �յ��¼��ַ�        */
	//m_dcb.fRtsControl = RTS_CONTROL_ENABLE;/*��������               */
	//if(!BuildCommDCB(wstrBaud.c_str(), &m_dcb))//��亯��
	//{
	//	ProcessErrorMessage("BuildCommDCB"); bOk = false;
	//	goto LastW;
	//}
	//if (!SetCommState(m_hComm,&m_dcb))//���õ�ǰ�������ú���
	//{
	//	ProcessErrorMessage("SetCommState"); bOk = false;
	//	goto LastW;
	//}
	LastW:
	PurgeComm(m_hComm, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_TXABORT|PURGE_RXABORT);//��ջ�����
	LeaveCriticalSection(&m_csCommunicationSync);

	return bOk;
}

void CSerialPort::SetOwnerWindowPtr(CWnd* pWnd)//����ָ��
{
	m_pOwnerWnd = pWnd;
}

unsigned int CSerialPort::SerialPortThreadFunc()
{
	DWORD Event = 0, CommEvent = 0, dwError = 0;
	COMSTAT comstat;
	::memset((void*)&comstat, 0x00, sizeof(COMSTAT));
	PurgeComm(m_hComm, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_TXABORT|PURGE_RXABORT);//��ջ�����

    for (;;)
    {
		if (!WaitCommEvent(m_hComm, &Event, &m_ov))//�����ж���SetCommMask()�������õĴ���ͨ���¼��Ƿ��ѷ�����
		{
			dwError = GetLastError();
			if(ERROR_IO_PENDING != dwError && ERROR_INVALID_PARAMETER != dwError)
			{
				ProcessErrorMessage("WaitCommEvent");
			}
		}
		else// �����ջ����Ƿ������ݴ�����������������־��
		{   
			ClearCommError(m_hComm, &dwError, &comstat);
			//�ú������ͨ�Ŵ��󲢱��洮�ڵĵ�ǰ״̬��ͬʱ���ú���������ڵĴ����־�Ա�������롢���������
			if (0 == comstat.cbInQue)
			{
				continue;
			}
		}
		Event = WaitForMultipleObjects(3, m_hEventArray, FALSE, INFINITE);
		if(0 == Event) // Shutdown Event
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
			m_bThreadAlive = FALSE;
			return 0;
		}
		else if(1 == Event) // Write Event
		{
			WritePort();
		}
		else if(2 == Event)  // Read Event
		{
			// �ú�����ȡ���ڷ�����ͨ���¼�
			GetCommMask(m_hComm, &CommEvent);
			if (CommEvent & EV_RXCHAR)
			{
				ReceiveChar(comstat);
			}
			else
			{
				//TODO:
			}
		}
		else
		{
		}
    }
	return 0;
}

void CSerialPort::ReceiveChar(COMSTAT comstat)
{
	BOOL bRead      = TRUE;
	BOOL bResult    = TRUE;
	DWORD dwError   = 0;
	DWORD BytesRead = 0;
	m_RXBuff = std::vector<byte>(m_nRxBuffSize,0x00);

	for (;;)
	{
		EnterCriticalSection(&m_csCommunicationSync);
		bResult = ClearCommError(m_hComm, &dwError, &comstat);
		LeaveCriticalSection(&m_csCommunicationSync);
		if(0 == comstat.cbInQue) 
			break; // Break out if all bytes have been read 

		EnterCriticalSection(&m_csCommunicationSync);
		bResult = ReadFile(m_hComm, &m_RXBuff[0], m_nRxBuffSize, &BytesRead, &m_ov);
		//ReadFile:������ڴ棬�ֽڣ�����ʵ�ʶ����ֵ��ͬ������ΪNULL��
		if (!bResult) //ERROR_IO_PENDING ��ʾI/O�����Ѿ����ɹ�����ӵ����豸��������Ķ����У��ȴ�����
		{
			if(ERROR_IO_PENDING == GetLastError())//GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж�����
			{
				bRead = FALSE;
			}
			else
			{
				ProcessErrorMessage("ReadFile");
			}
		}
		else
		{
			bRead = TRUE;
		}
		if (!bRead)
		{
			if (!GetOverlappedResult(m_hComm, &m_ov, &BytesRead, TRUE))
			{
				ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}
			bRead = TRUE;
		}
		LeaveCriticalSection(&m_csCommunicationSync);

		if(m_pOwnerWnd != nullptr) // Notify the parent window that a byte was received.
		{
			::SendMessage(m_pOwnerWnd->m_hWnd, WM_COMM_RXCHAR_DETECTED, (WPARAM)m_nRxBuffSize, (LPARAM)&m_RXBuff[0]);
		}
	}
	m_bReadingPortFinished = true;
}

void CSerialPort::SendCmdToPort(const std::vector<byte>& cmd)
{
	assert(m_hComm != 0);
	m_writeBuffer.clear();
	m_writeBuffer.assign(cmd.begin(), cmd.end());
	SetEvent(m_hWriteEvent);
}

void  CSerialPort::SendCmdToPort(const std::string& strCmd)
{
	m_writeBuffer.clear();
	m_writeBuffer = strCmd;
	SetEvent(m_hWriteEvent);
}

bool CSerialPort::StartMonitoring()
{
	UINT nThreadID;
	SerialPortProc.MemProc = &CSerialPort::SerialPortThreadFunc;

	m_hThread = (HANDLE)_beginthreadex(NULL,0,SerialPortProc.ThreadProc, this, 0, &nThreadID);
	if (m_hThread)
	{
		m_bThreadAlive = TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool CSerialPort::RestartMonitoring()
{
	DWORD dwRet = ::ResumeThread(m_hThread);
	if (0xFFFFFFFF == dwRet)
	{
		LPVOID lpMsgBuf;
		UINT flags = (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS);
		FormatMessage(flags, NULL, GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, NULL);
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );
	}
	return TRUE;
}

void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );
	wstring wstrErrText = CA2W(ErrorText);
	wstring wstrBuff = CT2W((LPTSTR)lpMsgBuf);
	std::wostringstream oss;
	oss << "WARNING: " << wstrErrText.c_str() << L" Failed with following error:\n" << wstrBuff.c_str() << L"Port:" << m_nPortNum;
	wstring errorDesc = oss.str();
	MessageBoxW( NULL, errorDesc.c_str(), _T("Error"), MB_OK | MB_ICONINFORMATION );
	LocalFree( lpMsgBuf );
}

void CSerialPort::WritePort()
{
	if(m_writeBuffer.empty())
	{
		return;
	}
	UINT portNum	= m_nPortNum;
	BOOL bWrite		= TRUE;
	DWORD BytesSent = 0;
	ResetEvent(m_hWriteEvent);

	EnterCriticalSection(&m_csCommunicationSync);

	m_ov.Offset     = 0;
	m_ov.OffsetHigh = 0;
	PurgeComm(m_hComm, PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_TXABORT|PURGE_RXABORT);//��ջ�����
	BOOL bResult = WriteFile(m_hComm, m_writeBuffer.c_str(), (DWORD)m_writeBuffer.length(), &BytesSent, &m_ov);
	//��ȡ���ݡ��������ַ���ֽڣ�ָ��ָ��һ��DWORD��ֵ������ֵ����ʵ��д����ֽ���
	if (!bResult)
	{
		DWORD nErrCode = GetLastError();
		if(ERROR_IO_PENDING == nErrCode)
		{
			BytesSent = 0;
			bWrite = FALSE;
		}
		else
		{
			ProcessErrorMessage("WriteFile");
		}
	}
	else
	{
		LeaveCriticalSection(&m_csCommunicationSync);
	}
	if(!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(m_hComm, &m_ov, &BytesSent, TRUE);
		          //�����// ָ���ص�������ʼʱָ����OV// ָ��һ��32λ�������ñ�����ֵ����ʵ�ʶ�д����������ֽ����� 
		//// �ò�������ָ�������Ƿ�һֱ�ȵ��ص����������� // ����ò���ΪTRUE������ֱ�����������ŷ��ء�
		LeaveCriticalSection(&m_csCommunicationSync);
		if (!bResult)
		{
			//ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}
	}
}

void CSerialPort::TryToShutdown()
{
	do 
	{
		if(m_hShutdownEvent)
			SetEvent(m_hShutdownEvent);
	} while (m_bThreadAlive);
}

bool CSerialPort::GetReadFinishedFlag()
{
	return m_bReadingPortFinished;
}

void CSerialPort::ClosePort()
{
	if (m_bThreadAlive)
	{
		SetEvent(m_hShutdownEvent);
	}
    WaitForSingleObject(m_hThread,5000);////ʹ��WaitForSingleObject�����ȴ���ֱ����������ɻ���ʱ�Ѵﵽ2����
    CloseHandle(m_hThread);
	m_bThreadAlive = FALSE;
}

bool CSerialPort::SetPortDCB(UINT baud ,UINT databits, UINT atopsbits)//����DCB,���ô�������,���ô��ڲ����ʡ�����Ĭ�ϡ�
{
	memset(&m_dcb, 0, sizeof(m_dcb));
	if (!GetCommState(m_hComm, &m_dcb))//��ȡ��ǰDCB����
		return FALSE;
	m_dcb.DCBlength = sizeof(m_dcb);
	/* ---------- ���ж˿����� ------- */
	m_dcb.BaudRate = baud;//������Ϊ9600--BaudRate
	m_dcb.Parity = NOPARITY;//dcb.Parity = EVENPARITY żУ��    NOPARITY ��У��     MARKPARITY ���У��    ODDPARITY ��У��
	m_dcb.fParity = 0;
	if (atopsbits==1)
	{
		m_dcb.StopBits = ONESTOPBIT;//ONESTOPBIT һλ//dcb.StopBits = 2;����ֹͣλ//--STOP
		//ONESTOPBIT 1 λֹͣλ TWOSTOPBITS 2 λֹͣλONE5STOPBITS 1.5λֹͣλ 
	}
	m_dcb.ByteSize = databits;//����λ��Ϊ8 λ--DATA
	m_dcb.fOutxCtsFlow = 0;
	m_dcb.fOutxDsrFlow = 0;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = 0;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutX = 0;
	m_dcb.fInX = 0;
	/* ----------------- �������� ----- */
	m_dcb.fErrorChar = 0;
	m_dcb.fBinary = 1;
	m_dcb.fNull = 0;
	m_dcb.fAbortOnError = 0;
	m_dcb.wReserved = 0;
	m_dcb.XonLim = 2;
	m_dcb.XoffLim = 4;
	m_dcb.XonChar = 0x13;
	m_dcb.XoffChar = 0x19;
	m_dcb.EvtChar = 0;
	// set DCB
	if (!SetCommState(m_hComm, &m_dcb))
	{
		return false;
	}	
	else
	{
		return true;
	}
}
