// dllmain.cpp: DllMain ��ʵ�֡�

#include "stdafx.h"
#include "resource.h"
#include "AOIAlgo_i.h"
#include "dllmain.h"

CAOIAlgoModule _AtlModule;

// DLL ��ڵ�
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
