#include "stdafx.h"
#include "AOI.h"
#include "AOIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAOIApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CAOIApp::CAOIApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	// TODO: 
	// 将所有重要的初始化放置在 InitInstance 中
}

CAOIApp theApp;
ConfigFileManager gConfigFileMgr;
CSystemLogger     gSystemLogger;
CSystemManager    gSystemMgr;
ui_decorator      gUiDecorator;

BOOL CAOIApp::InitInstance()
{
	//m_LoadingDlg.SetWindowPos(&wndTopMost, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	m_hSingleton = ::CreateMutexW(0, true, _T("Singleton"));
	if (m_hSingleton != NULL && ERROR_ALREADY_EXISTS == GetLastError())
	{
		::ReleaseMutex(m_hSingleton);
		CString sMsg;
		sMsg.LoadString(IDS_STRING_CANNOT_RUN_TWO_APP);
		AfxMessageBox(sMsg, MB_OK | MB_ICONWARNING, NULL);
		return FALSE;
	}

	CShellManager *pShellManager = new CShellManager;

	BOOL ret = ::SetPriorityClass(::GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS);
	if (ret == FALSE)
	{
		//TODO:
	}
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("LeadChina - Mura Detector System"));
	//PylonInitialize();
	if (FALSE == AfxSocketInit())
	{
		AfxMessageBox(_T("AfxSocketInit Failed."), MB_OK | MB_ICONWARNING, NULL);
		return FALSE;
	}
	//SetSystem("thread_num", 8);
	//gConfigFileMgr.ParseConfigFile();
	CAOIDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	return FALSE;
}

int CAOIApp::ExitInstance()
{
	PylonTerminate();
	return 0;
}

