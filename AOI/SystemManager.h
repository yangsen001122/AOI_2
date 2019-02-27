#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include "TypeDef.h"
#include "BaslerCamera.h"
#include "HikCamera.h"
#include "RadProc.h"
#include "SqliteDB.h"
#include "MessageDlg.h"
#include "bounded_buffer.h"
#include <fstream>
#include <iostream>
#include <WinSock2.h>
#include "FileMapping.h"
#include "HttpHelper.h"
#include "SystemLogger.h"
using namespace std;

class CDemuraStation;
class CAOIDlg;

class CSystemManager
{
public:
	CSystemManager();
	~CSystemManager();
	wstring                      m_wstrCrtDirectory;
	CWnd*                        m_ptrMainUi;
	FileMapping					 m_fileMapping;
	CHttpHelper					 m_httpHelper;
	HANDLE                       m_hAutoRunFlag;
	HANDLE                       m_hCaptureFinishFlag;
	HANDLE                       m_hManualGrabOKFlag;
	DeviceInfoList_t             m_baslerDeviceInfoList;
	MV_CC_DEVICE_INFO_LIST       m_hikDeviceInfoList;
	std::map<wstring, bool>      m_dictCamera_State;
	std::vector<unsigned char>   m_vRadConfigData;
	std::map<wstring, CAMERA_IMG_SOURCE>  m_dictImageSource;//for source image display on main screen
	std::map<wstring, CAMERA_IMG_SOURCE>  m_dictResultImg;  //for result image display on main and debug screen
	bounded_buffer<Mat>                   m_debugResultImgBuffer; //for debug result iamge display on main and debug screen
	boost::circular_buffer<BASEINFO>      m_cfResultInfo;//for every pattern result information display on main screen
	std::map<int, Mat>                    m_dictResultImg2; //for result iamge display on main screen(according to which pattern index clicked on list)
	Mat                                   m_finalDispImg;
	Mat                                   m_finalDispSrc;
	double                                m_dBgValue;
	int                          m_nPixelWidth;
	int                          m_nPixelHeight;
	CSqliteDB<CppSQLite3Query>   m_sysDatabase;
	CppSQLite3Query              m_queryExpData;
	float                        m_fRatioH;   // Horizontal
	float                        m_fRatioV;   // Vertical
	volatile bool                m_bProjectChanged;
	volatile bool                m_bSaveRawCsv;
	volatile bool                m_bAlgoSaveImg;
	volatile bool                m_MsgResult;
	// For statistics;
	unsigned long  m_nTotal;
	unsigned long  m_nOkCount;
	double         m_dTotalTiming;
	wstring        m_wstrCrtTiming;
	wstring        m_wstrAvgTiming;
	wstring        m_wstrTotalTime;
	chronograph    m_sysTimer;
	wstring        m_wstrCrtImgRepository;
	//ADD RECORD
	unsigned long  m_nAddTotal;
	unsigned long  m_nAddOkCount;
	wstring        m_wstrAddTotalTime;
	string         m_strScreenIDBarcode;
	string         m_strScreenColorLevel;
	PROJECT*       m_crtProject;
	CSystemLogger  m_detectLogger;
	CString        m_strOperatorID;
	wstring        m_wstrDefaultUserName;
	wstring        m_wstrCrtProjectName;
	volatile bool  ShowMsg(int nStrID, WPARAM wParam=MESSAGE_CONFIRM);
	void CollectStationPtr(int nIndex, CDemuraStation* ptr);
	bool CreateFileMappingServe();
	bool ConnectVIQServe();
	bool CreateHttpServe();
	void HttpTest();
	void StartSaveSrcImageThread();
	void StartSaveExtractImgThread();
	void QueueUpPendingRawImage(Mat img,int nptnIndex, wstring strCameraName,int nArrayIndex);
	void QueueUpPendingExtractImage(Mat img, int nptnIndex, wstring strCameraName, int nArrayIndex);
	void QueueUpPendingCSV(int nPatternIndex,int nArrayIndex, double* ptrData, int nWidth=1440,int nHeight=2992);
	CDemuraStation* GetStation(int nStationIndex);
	//void ListAllAvailableBaslerCamera(std::map<wstring, bool>& mapNameState);
	void ListAllAvailableHikCamera(std::map<wstring, bool>& mapNameState);
	void StopWorkerThread();
	void SetStartedFlag(bool bStarted);
	void SetMainUiPtr(CWnd* ptrMainUi);
	void CreateScreenFileFolder(CString barcode);
	void SetCurrentPatternIndex(int nPatternIndex);
	UAC GetCurrentUser();
	LOGVIEWER GetCurrentLogViewer();
	void CreateUACTable();
	void CreateCountTable();
	string TransDBTableName(string strTableName);
	void SetCurrentUser(const UAC& crtUser);
	void SetCurrentLogViewer(const LOGVIEWER& crtLogViewer);
	wstring ParseRawImageName(int currentImageIndex);
	void ParseRawImageCameraParam(int nCurrentImageIndex,wstring wstrCameraName);
	int ParseCameraIndex(wstring wstrCameraName);
	void InitPatternImgNums();
	wstring CreatePatternImageRepository(string codeInfo, int ptnIndex);
	void SaveRecord(string ctrProj, int nTotal, int nOkCount,double dCrtTime);
	void ExpRecordHint();
	void TransExpRecord(const char*field, const char* crtProj);
	void UpdateStatictics();
	void UpdateCrtProjectPatterns(std::map<int, FRAME_PATTERN> dictProjectPattern);
	int  PgPatternNameMap(wstring wstrPatternName);
	void InspectDiskSpace(string  strLinuxPath);
	void ExpFileHint(string  strFilePath);
	unsigned int __stdcall TriggerSignalSentryThread();
	unsigned int __stdcall FileMappingHeartBeatThread();
	unsigned int __stdcall OnHttpRecieved();
	unsigned int __stdcall WaitForProcessResult0();
	unsigned int __stdcall WaitForProcessResult1();
private:
	void GetCrtDirectory();
	unsigned int __stdcall SaveSrcImageThread();
	unsigned int __stdcall SaveExtractImgThread();
	bool WaitForCaptureFinished(int nPatternIndex,int nStationIndex = -1);// -1 means waiting for all two stations to finish.
	bool ResendHttpMetadata(string codeInfo, int ptnIndex);
	void InitCountValue();
private:
	UAC                            m_crtUser;
	std::map<int, CDemuraStation*> m_dictIndex_StationPtr;
	std::map<int, FRAME_PATTERN>   m_dictIndex_Pattern;
	long                           m_nPatternCount;
	volatile bool                  m_bStarted;
	LOGVIEWER                      m_crtLogViewer;
	bounded_buffer<PENDING_RAW_IMAGE_DATA> m_rawImageBuffer; // for saving src images
	bounded_buffer<PENDING_RAW_IMAGE_DATA> m_extractImageBuffer; // for saving src images
	bounded_buffer<PENDING_CSV_DATA>       m_csvBuffer1;
	wstring                                m_wstrTargetFolder;
	wstring                                m_wstrIDFolder;

	map<int,int>						 m_patternImgCount;		// 每个画面传送图像计数
	map<int, int>						 m_patternImgNums;		// 每个画面需要传送图片的总数
	BASEINFO							 m_metadata;			// D05传送的Metadata
	map<wstring, string>				 m_postCamName;			// Metadata发送给VIQ的相机名，AOI定义
	map<wstring, string>				 m_postPhotoName;		// Metadata发送给VIQ的照片名，已由IBM定义好
	map<int, string>				     m_postFrameName;       // Metadata发送给VIQ的画面名，上位机触发时采集
	map<string, string>					 m_postRuleID;			// Metadata中由画面名映射的规则名
	map<int,vector<map<string, string>>> m_patternInfo;			// Metadata中的Pics对应的数据结构
	map<int, wstring>					 m_patternPath;         // 当前画面本地保存路径
	vector<HANDLE>						 m_vTrigHandle;
	deque<bool>						     m_vTrigFlag;
	vector<WAITINFO>					 m_vWaitInfo;

	vector<STATIONINFO>					 m_stationInfo;         // VIQ图像处理完成信号
	map<string, int>					 m_patternIndex;		// D06重传数据时，需根据反馈画面名称得到对应的编号
	bool								 m_fileMappingActive;
	
	vector<string>						 m_patternName;
	// Device control thread 
	std::vector<HANDLE>    m_vWorkerThread; 
	volatile unsigned int  m_nExitFlag; 
	std::vector<int>       m_vImgCameraParam ;
	union _Proc
	{
		unsigned(_stdcall * ThreadProc)(void*);
		unsigned(_stdcall CSystemManager::*MemProc)();
	}SavingSrcImgProc, SavingExtractImgProc,OnFileMappingRecieve,OnHttpRecieve,OnFileMappmingHeartBeat,WaitForStation0Result, WaitForStation1Result;

};


