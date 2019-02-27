#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"
#include "ConfigFileManager.h"
#include "SystemLogger.h"
#include "SystemManager.h"
#include "ui_decorator.h"
#include "SystemUserManagementDlg.h"

class CAOIApp : public CWinApp
{
public:
	CAOIApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_hSingleton;
};

extern CAOIApp theApp;
extern ConfigFileManager gConfigFileMgr;
extern CSystemLogger     gSystemLogger;
extern CSystemManager    gSystemMgr;
extern ui_decorator      gUiDecorator;