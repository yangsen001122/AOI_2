#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
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