#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "bounded_buffer.h"
#include "TypeDef.h"
#include <boost/circular_buffer.hpp>
#include <boost/atomic.hpp>
#include <afxpriv.h>
#include "SystemLogger.h"
#include "RadProc.h"

using namespace std;
using namespace boost;

#import "AOIAlgo.dll" no_namespace

class CCameraBase;

class CPatternProcessor
{
public:
	CPatternProcessor(int nStaionIndex,wstring wstrCameraName,bool bMainProcessor,const std::map<int, CAMERA_FRAME_PATTERN>& ptrDictPattern);
	~CPatternProcessor();

	void StartProcessThread();
	void ExitProcessThread(bool bShutDown = true);
	HANDLE GetThreadHandle();
	void SetCameraPtr(CCameraBase* ptrCamera);
	void SetCurrentPatternIndex(int nPatternIndex);
	void SetCurrentActionGroupIndex(int nActionGpIndex);
	void SetSizeInformation(long nWidth, long nHeight, long nCameraWidth, long nCameraHeight);
	void SetDebugFlag(bool bDebug);
	void ReceiveImage(IMG_PACKAGE& imgPac);
	void InsertDebugImage(int nPatternIndex, IMG_PACKAGE& debugImg);
	std::map<int, CAMERA_FRAME_PATTERN>* GetCameraPatternArray();
	void UpdateDictIndexPatternDebug(int nStationIndex, wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern,  eCAMERAPARAM_TYPE sParaType);
	void ApplyNewParam(int nPatternIndex,int nActionIndex,int nFuncIndex);
	void SetInternalImgIndex(int nInternalImgIndex);
	bool WaitForProcessingDone();
	void ResetInnerData();
	void SetImageReceivedEvent();
	void GetImageFromCisternBuffer(std::vector<IMG_PACKAGE>& vImgPac);
	void FlushProcessorLog();
	void SetBatchMode(bool bBatchMode);
	void GrabOneImage(int nPatternIndex);
	CCameraBase*  m_ptrCamera;
	int           m_nCrtAlgoIndex;
	int		      m_nResolutionWidth_S;
	int           m_nResolutionHeight_S;
	int           m_nCameraWidth;
	int           m_nCameraHeight;
	volatile bool m_bExeAllAlgo;
	volatile bool m_bCrtTestFunc;
	volatile bool m_bInChecking;
	int           m_nInternalImgIndex;
	HWND          m_hDebugHandle;
	int           m_nCameraIndex;
	std::map<int, CAMERA_FRAME_PATTERN> m_dictIndex_Pattern;      // Pattern Index <==> Action Group
private:
	
	bool ParseAlgoFunctions(LPVOID pInstance);
	unsigned int __stdcall PatternInspectingThread();	
	void TriggerCamera();
	bool InspectImage(IMG_PACKAGE& imgPac, std::vector<FUNC_INFO>& vFunc);
	void DisplayResultOnDebugScreen();//result image
	void DisplayResultOnMainScreen(); //result image
	void UpdateDebugScreenImageWindow();
	void UpdateMainScreenImageWindow();
	void SetCameraParameter(int nExposureTime, int nGain,int nWhiteBalanceR, int nWhiteBalanceG, int nWhiteBalanceB);
	void ParsePatternColorParam(int nPatternIndex,long &nPatternColor, long &nPatternGrayLevel);
	bool Demura();
private:
	volatile int   m_nPatternLoopIndex;
	int            m_nPatternLoopCount;
	CSystemLogger       m_processorLogger;
	CComPtr<IAOIAlgo>  m_ptrMuraCOM;
	HANDLE              m_hEndFlag;
	wstring             m_wstrCameraName;
	bool                m_bMainProcessor;
	bool                m_bProcessorOk;
	volatile bool       m_bDebugMode;
	volatile bool       m_bBatchMode; // for image batch processing
	bool                m_bDisplayImg;
	const int           m_nStationIndex;
	int                 m_nCrtPatternIndex;
	int                 m_nCrtActionGpIndex; //For debugging - 
	long long           m_nTotalCount;
	long long           m_nNgCount;
	int                 m_nOldExposureTime;
	int                 m_nOldGainValue;
	int                 m_nOldWhiteBalanceR;
	int                 m_nOldWhiteBalanceG;
	int                 m_nOldWhiteBalanceB;
	double			    m_dTotalProcessAlgTime;             
	
	std::map<int, CString>              m_mapShowText;
	IMG_PACKAGE                         m_crtImgPac;
	bounded_buffer<int>                 m_patternIndexBuffer;
	bounded_buffer<IMG_PACKAGE>         m_imgBuffer;
	circular_buffer<IMG_PACKAGE>        m_cisternImgBuffer;
	// Thread Function
	HANDLE  m_hThread;
	std::map<int, PG_IMAGE>*            m_ptrDictSeqImg;
	union _Proc
	{
		unsigned(_stdcall * ThreadProc)(void*);
		unsigned(_stdcall CPatternProcessor::*MemProc)();
	} Proc;
	
	//Timing
	chronograph m_timer;
	std::tuple<int, int, float> m_timingBuffer; // Pattern Index, Function Index, Timing Value;
	//For updating configuration file
	std::set<std::tuple<int, int, int>> m_modifiedFuncBuffer; // Pattern Index, Action Index, Function Index;
	Mat m_resultMat;
	Mat m_crtMat;
	HObject m_resultHObj;
	HObject m_crtHObj;
	LONG    m_nResultOK;
};

