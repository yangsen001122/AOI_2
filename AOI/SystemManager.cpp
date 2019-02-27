#include "stdafx.h"
#pragma   warning(disable:4503) 
#include "SystemManager.h"
#include"DemuraStation.h"
#include "ConfigFileManager.h"
#include "TypeDef.h"
#include"CreateBMP.h"

extern CSystemLogger     gSystemLogger;
extern ConfigFileManager gConfigFileMgr;

CSystemManager::CSystemManager() :
	m_nPixelWidth(720),
	m_nPixelHeight(1080),
	m_fRatioH(1.0f),
	m_fRatioV(1.0f),
	m_nExitFlag(0),
	m_nPatternCount(0),
	m_bStarted(false),
	m_bProjectChanged(false),
	m_ptrMainUi(nullptr),
	m_rawImageBuffer(9),
	m_extractImageBuffer(9),
	m_csvBuffer1(9),
	m_nTotal(0),
    m_nOkCount(0),
	m_dTotalTiming(0.0),
	m_wstrCrtTiming(L""),
    m_wstrAvgTiming(L""),
	m_wstrTotalTime(L""),
	m_wstrCrtImgRepository(L""),
	m_MsgResult(false),
	m_strScreenIDBarcode(""),
	m_strScreenColorLevel(""),
	m_fileMappingActive(true),
	m_debugResultImgBuffer(13),
	m_dBgValue(-1.0),
	m_detectLogger(L"DetectResult"),
	m_strOperatorID(L""),
	m_wstrCrtProjectName(L"")
{
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	m_fRatioH = 1.0f * rcScreen.Width() / BK_IMG_WIDTH;
	m_fRatioV = 1.0f * rcScreen.Height() / BK_IMG_HEIGHT;
	m_hAutoRunFlag = ::CreateEventW(NULL, TRUE, TRUE, _T("AutoRunFlag"));
	m_hCaptureFinishFlag = ::CreateEventW(NULL, TRUE, TRUE, _T("CaptureFinishFlag"));
	m_hManualGrabOKFlag= ::CreateEventW(NULL, TRUE, TRUE, _T("ManualGrabOKFlag"));
	//ListAllAvailableBaslerCamera(m_dictCamera_State);
	ListAllAvailableHikCamera(m_dictCamera_State);
	m_wstrDefaultUserName = gConfigFileMgr.GetCommonSetting(L"DEFAULT_USER_NAME");;
	GetCrtDirectory();
	m_vRadConfigData.reserve(DEMURA_CFG_SIZE);
	wstring wstrSaveRawCsv= gConfigFileMgr.GetCommonSetting(L"SAVE_RAW_CSV");
	if (wstrSaveRawCsv==L"1")
	{
		m_bSaveRawCsv = true;
	}
	else
	{
		m_bSaveRawCsv = false;
	} 

	wstring wstrAlgoSaveImg = gConfigFileMgr.GetCommonSetting(L"ALGO_SAVE_IMG");
	if (wstrAlgoSaveImg == L"1")
	{
		m_bAlgoSaveImg = true;
	}
	else
	{
		m_bAlgoSaveImg = false;
	}

	wstring wstrRawImgFolder = gConfigFileMgr.GetCommonSetting(L"IMAGE_FOLDER");
	if (!localUtils::utils::CheckDirExist(wstrRawImgFolder))
	{
		if (!_wmkdir(wstrRawImgFolder.c_str()))
		{
			//TODO:
		}
	}
	m_wstrTargetFolder = wstrRawImgFolder;
	m_crtProject = gConfigFileMgr.GetCurrentProjectConfig();
	vector<string> patnames={"Dark","Gray","Red","Blue","Green","White","Chess","Panel","Dust"};
	m_patternName.swap(patnames);
	for (int i = 0; i < (int)m_patternName.size(); i++)
	{
		m_postRuleID[m_patternName[i]] = "R00000";
	}
	InitPatternImgNums();
	STATIONINFO initInfo;
	vector<STATIONINFO> stationInfo{ initInfo ,initInfo};
	vector<HANDLE> trigHandle{NULL,NULL};
	deque<bool> trigFlg{ false,false };
	m_vTrigFlag = trigFlg;
	WAITINFO initWaitInfo;
	vector<WAITINFO> waitInfo{ initWaitInfo, initWaitInfo };
	for (int i = 0; i < (int)stationInfo.size(); i++)
	{
		wstring wstrName =L"WaitSignal"+::to_wstring(i);
		stationInfo[i].m_hProcessFinish = CreateEventW(NULL, TRUE, FALSE, wstrName.c_str());
		wstring wstrName1 = L"TrigHandle"+ ::to_wstring(i);
		trigHandle[i] = CreateEventW(NULL, TRUE, FALSE, wstrName1.c_str());
	}
	m_stationInfo = stationInfo;
	m_vTrigHandle = trigHandle;
	m_vWaitInfo = waitInfo;
	int nPatternCount= boost::lexical_cast<int>(m_crtProject->m_vPattern.size());
	m_cfResultInfo.resize(nPatternCount);
	m_vImgCameraParam = vector<int>(2,-1);

	InitCountValue();
}

CSystemManager::~CSystemManager()
{
	WSACleanup();
	m_dictImageSource.clear();
}

volatile bool  CSystemManager::ShowMsg(int nStrID, WPARAM wParam)
{
	CMessageDlg DeleteDlg(nStrID, wParam);
	DeleteDlg.DoModal(); 
	return 	m_MsgResult;
}


void CSystemManager::CollectStationPtr(int nIndex, CDemuraStation * ptr)
{
	m_dictIndex_StationPtr[nIndex] = ptr;
	if (m_dictIndex_Pattern.empty())
	{
		m_dictIndex_Pattern = ptr->GetCurrentProjectPatternArray();
		m_nPatternCount     = (long)m_dictIndex_Pattern.size();
	}
}

bool CSystemManager::CreateFileMappingServe()
{
	if (!m_fileMapping.CreatFileMapping())
	{
		return false;
	}
	else
	{
		UINT threadId;
		OnFileMappingRecieve.MemProc = &CSystemManager::TriggerSignalSentryThread;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, OnFileMappingRecieve.ThreadProc, (LPVOID)this, 0, &threadId);
		OnFileMappmingHeartBeat.MemProc = &CSystemManager::FileMappingHeartBeatThread;
		HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, OnFileMappmingHeartBeat.ThreadProc, (LPVOID)this, 0, &threadId);
		if (hThread == NULL|| hThread1==NULL)
		{
			AfxMessageBox(_T("Starting FileMapping Recieve Thread  Failed"), MB_OK);
		}
		return true;
	}
}

bool CSystemManager::ConnectVIQServe()
{
	if (!m_httpHelper.ConnectVIQServer())
	{
		return false;
	}                         
	else                                   
	{
		BASEINFO curProdInfo;
		string errcode;
		if (!m_httpHelper.GetProductInfomation(curProdInfo, errcode))
		{
			gSystemLogger.QueueUpLogAndReport(localUtils::utils::Utf8ToUnicode(errcode), LV_ERROR);
			return false;
		}
		for (auto it = curProdInfo.m_specifiedInfo.begin(); it != curProdInfo.m_specifiedInfo.end(); it++)
		{
			int nSize = (int)it->second.size();
			for (int i = 0; i < nSize; i++)
			{
				if (it->second[i]["ProdID"] == /*"P00001"*/localUtils::utils::Unicode2Utf8(m_wstrCrtProjectName))
				{
					BASEINFO curRuleInfo;
					string errcode;
					curRuleInfo.m_commInfo["ProdID"] = /*"P00001"*/localUtils::utils::Unicode2Utf8(m_wstrCrtProjectName);
					if (!m_httpHelper.GetDetectRules(curRuleInfo, errcode))
					{
						gSystemLogger.QueueUpLogAndReport(localUtils::utils::Utf8ToUnicode(errcode), LV_ERROR);
						return false;
					}
					for (auto it1 = curRuleInfo.m_specifiedInfo.begin(); it1 != curRuleInfo.m_specifiedInfo.end(); it1++)
					{
						int nSize1 = (int)it1->second.size();
						for (int j = 0; j < nSize1; j++)
						{
							for (int k = 0; k < (int)m_patternName.size(); k++)
							{
								m_postRuleID[m_patternName[k]] = it1->second[j]["RuleID"];
							}
							/*	std::vector<string> vFrame = localUtils::utils::SplitString(it1->second[j]["Frames"], ",");
							int nSize2 = (int)vFrame.size();
							for(int k=0;k<nSize2;k++)
							{
								m_postRuleID[vFrame[k]] = it1->second[j]["RuleID"];
							}*/
						}
					}
					UINT threadId;
					WaitForStation0Result.MemProc = &CSystemManager::WaitForProcessResult0;
					HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WaitForStation0Result.ThreadProc, (LPVOID)this, 0, &threadId);
					UINT threadId1;
					WaitForStation1Result.MemProc = &CSystemManager::WaitForProcessResult1;
					HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, WaitForStation1Result.ThreadProc, (LPVOID)this, 0, &threadId1);
					if (hThread == NULL || hThread1 == NULL)
					{
						AfxMessageBox(_T("Wait Station Result Thread Start Failed!"), MB_OK);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool CSystemManager::CreateHttpServe()
{
	if (!m_httpHelper.CreatAOIServer())
	{
		return false;
	}
	else
	{
		OnHttpRecieve.MemProc = &CSystemManager::OnHttpRecieved;
		UINT threadId;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, OnHttpRecieve.ThreadProc, (LPVOID)this, 0, &threadId);
		if (hThread == NULL)
		{
			AfxMessageBox(_T("Starting Http Recieve Thread  Failed"), MB_OK);
		}
		return true;
	}
}

UAC CSystemManager::GetCurrentUser()
{
	return m_crtUser;
}

LOGVIEWER CSystemManager::GetCurrentLogViewer()
{
	return  m_crtLogViewer;
}

void CSystemManager::CreateUACTable()
{
	const char* indexstr = "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, username char(5), key char(6), level char(5)";
	if (m_sysDatabase.Connect())
	{
		m_sysDatabase.CreateTable("UAC", indexstr);
		string strDefaultUserName = localUtils::utils::Unicode2Utf8(m_wstrDefaultUserName);
		if (m_sysDatabase.QueryRow(strDefaultUserName.c_str()) <= 0)
			m_sysDatabase.saveData("UAC", strDefaultUserName.c_str(), "1", "engineer");
	}
}

void CSystemManager::CreateCountTable()
{
	const char* indexnamestr = "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,date char(10),total char(10),OkCount char(10),OkCountRate char(10),CrtTime char(10)";
	if (m_sysDatabase.Connect())
	{
		std::map<int, STATION_CONFIG>* ptrDictRet = gConfigFileMgr.GetStaionConfigArray();
		if (!ptrDictRet->empty())
		{
			STATION_CONFIG config = ptrDictRet->begin()->second;
			wstring wstrCrtProjectName = config.m_wstrCrtProjectName;
			for (auto it = config.m_dictProject.begin(); it != config.m_dictProject.end(); it++)
			{
				wstring wstrName = it->first;
				string strName = localUtils::utils::Unicode2Utf8(wstrName);
				strName = TransDBTableName(strName);
				m_sysDatabase.CreateTable(strName.c_str(), indexnamestr);
			}
		}
	}
}

std::string CSystemManager::TransDBTableName(string strTableName)
{
	for (int i = 0; i < strTableName.length(); i++)
	{
		if ((strTableName[i] >= '0') && (strTableName[i] <= '9') && (i == 0))
		{
			strTableName[i] += 49;
		}
		else if ((strTableName[i] == '.') || (strTableName[i] == '-'))
		{
			strTableName[i] = '_';
		}
	}
	return strTableName;
}

void CSystemManager::SetCurrentUser(const UAC & crtUser)
{
	m_crtUser = crtUser;
}

void CSystemManager::SetCurrentLogViewer(const LOGVIEWER& crtLogViewer)
{
	m_crtLogViewer = crtLogViewer;
}

void CSystemManager::StartSaveSrcImageThread()
{
	SavingSrcImgProc.MemProc = &CSystemManager::SaveSrcImageThread;
	UINT threadId1;
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, SavingSrcImgProc.ThreadProc, (LPVOID)this, 0, &threadId1);
	if (hThread1 == NULL)
	{
		AfxMessageBox(_T("Starting Saving Src Images Thread Failed"), MB_OK);
	}
	else
	{
		m_vWorkerThread.push_back(hThread1);
	}
}

void CSystemManager::StartSaveExtractImgThread()
{

	SavingExtractImgProc.MemProc = &CSystemManager::SaveExtractImgThread;
	UINT threadId2;
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, SavingExtractImgProc.ThreadProc, (LPVOID)this, 0, &threadId2);
	if (hThread2 == NULL)
	{
		AfxMessageBox(_T("Starting Saving Extract Images Thread  Failed"), MB_OK);
	}
	else
	{
		m_vWorkerThread.push_back(hThread2);
	}
}

void CSystemManager::QueueUpPendingRawImage(Mat img, int nptnIndex, wstring strCameraName, int nArrayIndex)
{
	if (m_bSaveRawCsv)
	{
		CString strRad;
		wstring wstrFileName = ParseRawImageName(nptnIndex);
		PENDING_RAW_IMAGE_DATA rawData(wstrFileName, img, nptnIndex, strCameraName);
		if (!img.data)
		{
			strRad.Format(_T("%d picture initial fail"), nArrayIndex);
			gSystemLogger.QueueUpLogAndReport(strRad.GetString(), LV_ERROR);
		}
		if (nArrayIndex == 1)
		{
			m_rawImageBuffer.push_front(rawData);
		}
	}
}

void CSystemManager::QueueUpPendingExtractImage(Mat img, int nptnIndex, wstring strCameraName, int nArrayIndex)
{
	if (m_bSaveRawCsv)
	{
		CString strRad;
		wstring wstrFileName = ParseRawImageName(nptnIndex);
		PENDING_RAW_IMAGE_DATA extractData(wstrFileName, img, nptnIndex, strCameraName);
		if (!img.data)
		{
			strRad.Format(_T("%d picture initial fail"), nArrayIndex);
			gSystemLogger.QueueUpLogAndReport(strRad.GetString(), LV_ERROR);
		}
		if (nArrayIndex == 1)
		{
			m_extractImageBuffer.push_front(extractData);
		}
	}
}

void CSystemManager::QueueUpPendingCSV(int nPatternIndex, int nArrayIndex, double* ptrData, int nWidth, int nHeight)
{
	if (m_bSaveRawCsv)
	{
		wstring wstrFileName = ParseRawImageName(nPatternIndex);
		PENDING_CSV_DATA csvData(wstrFileName, ptrData, nPatternIndex);

		if (nArrayIndex == 1)
		{
			m_csvBuffer1.push_front(csvData);
		}
	}
}

CDemuraStation * CSystemManager::GetStation(int nStationIndex)
{
	return m_dictIndex_StationPtr[nStationIndex];
}

unsigned int CSystemManager::SaveSrcImageThread()
{
	string m_strCodeID = "";
	while (m_nExitFlag == 0)
	{
		PENDING_RAW_IMAGE_DATA rawImg;
		m_rawImageBuffer.pop_back(&rawImg);
		if (rawImg.m_nIndex == -1)
		{
			break;
		}
		Mat retImg = rawImg.m_rawImage.clone();
		if (retImg.data)
		{
			if (rawImg.m_nIndex == 0)
			{
				m_strCodeID = m_strScreenIDBarcode;
			}
		    ParseRawImageCameraParam(rawImg.m_nIndex, rawImg.m_wstrCameraName);
			SYSTEMTIME dt;
			::GetLocalTime(&dt);
			CString sTimestamp;
			sTimestamp.Format(_T("%04d-%02d-%02d-%02d-%02d-%02d-%03d"), dt.wYear, dt.wMonth, dt.wDay, dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
			wostringstream woss;
			woss << m_wstrIDFolder << L"\\" << m_crtProject->m_screenParam.m_strStationName << L"_" << m_crtProject->m_screenParam.m_strProductName
				<< L"_" << localUtils::utils::Utf8ToUnicode(m_strCodeID) << L"_"
				<< sTimestamp.GetBuffer() << L"_Pattern" << rawImg.m_nIndex
				<< L"_" << rawImg.m_wstrCameraName << L"_" << m_vImgCameraParam[0] << L"_" << m_vImgCameraParam[1] << L"_"<<localUtils::utils::Utf8ToUnicode(m_postFrameName[rawImg.m_nIndex])
				<< L"_Original" << L".bmp";
			string strFile = localUtils::utils::Unicode2Ansi(woss.str());
			imwrite(strFile.c_str(), retImg);
		}
	}
	return 0;
}

unsigned int CSystemManager::SaveExtractImgThread()
{
	string m_strTaskID = "";
	string m_strCodeID = "";
	while (m_nExitFlag == 0)
	{
		PENDING_RAW_IMAGE_DATA extractImg;
		m_extractImageBuffer.pop_back(&extractImg);
		if (extractImg.m_nIndex == -1)
		{
			break;
		}
		Mat retImg = extractImg.m_rawImage.clone();
		if (retImg.data)
		{
		    ParseRawImageCameraParam(extractImg.m_nIndex, extractImg.m_wstrCameraName);
			SYSTEMTIME dt;
			::GetLocalTime(&dt);
			CString sTimestamp;
			sTimestamp.Format(_T("%04d-%02d-%02d-%02d-%02d-%02d-%03d"), dt.wYear, dt.wMonth, dt.wDay, dt.wHour, dt.wMinute, dt.wSecond, dt.wMilliseconds);
			
			string strlocal="";
			if (localUtils::utils::CheckDirExist(m_patternPath[extractImg.m_nIndex]))
			{
				// 重传图像不写入本地
				if (extractImg.m_bSave)
				{
					wostringstream localPath;
					localPath << m_patternPath[extractImg.m_nIndex] << L"\\" << extractImg.m_wstrCameraName << L".bmp";
				    strlocal = localUtils::utils::Unicode2Ansi(localPath.str());
					imwrite(strlocal.c_str(), retImg);//write into local path  						 
				}
			}
			else
			{
				gSystemLogger.QueueUpLogAndReport(L"Check Direction Exist Failed", LV_ERROR);
			}

			if (extractImg.m_nIndex == 0)
			{
				m_strTaskID = localUtils::utils::TChar2Utf8(sTimestamp);
				m_strCodeID = m_strScreenIDBarcode;
			}
			wstring  strLinuxPath = L"\\\\193.168.70.1\\aoidisk1";
			wostringstream linuxPath, linuxImgName;
			linuxImgName << m_crtProject->m_screenParam.m_strStationName << L"_" << m_crtProject->m_screenParam.m_strProductName
				<< L"_" << localUtils::utils::Utf8ToUnicode(m_strCodeID) << L"_"
				<< sTimestamp.GetBuffer() << L"_Pattern" << extractImg.m_nIndex
				<< L"_" << localUtils::utils::AnsiToUnicode(m_postCamName[extractImg.m_wstrCameraName]) << L"_" << m_vImgCameraParam[0] << L"_" << m_vImgCameraParam[1] << L"_" << localUtils::utils::Utf8ToUnicode(m_postFrameName[extractImg.m_nIndex])
				<< L"_Extract" << L".bmp";
			linuxPath << strLinuxPath << L"\\" << m_crtProject->m_screenParam.m_strStationName << L"_" << m_crtProject->m_screenParam.m_strProductName
				<< L"_" << localUtils::utils::Utf8ToUnicode(m_strCodeID) << L"_"
				<< sTimestamp.GetBuffer() << L"_Pattern" << extractImg.m_nIndex
				<< L"_" << localUtils::utils::AnsiToUnicode(m_postCamName[extractImg.m_wstrCameraName]) << L"_" << m_vImgCameraParam[0] << L"_" << m_vImgCameraParam[1] << L"_" << localUtils::utils::Utf8ToUnicode(m_postFrameName[extractImg.m_nIndex])
				<< L"_Extract" << L".bmp";
			InspectDiskSpace((localUtils::utils::Unicode2Ansi(strLinuxPath)));
			CCreateBMP::RawData2BMP((BYTE*)retImg.data, retImg.cols, retImg.rows, (localUtils::utils::Unicode2Ansi(linuxPath.str())).c_str(), (retImg.channels() == 1) ? false : true); //write into linux
            // 填充结构体单次数据
			map<string, string> crtImageInfo;
			crtImageInfo["Camera"] = m_postCamName[extractImg.m_wstrCameraName];
			crtImageInfo["Photo"] = m_postPhotoName[extractImg.m_wstrCameraName];
			crtImageInfo["Filename"] = (localUtils::utils::Unicode2Ansi(linuxImgName.str())).c_str();
			m_patternInfo[extractImg.m_nIndex].push_back(crtImageInfo);
			m_patternImgCount[extractImg.m_nIndex]++;
			// 发送当前Pattern结构体
			if (m_patternImgCount[extractImg.m_nIndex] == m_patternImgNums[extractImg.m_nIndex])
			{
				string errcode="";
				m_metadata.m_commInfo["AOIID"] = "AOI001";
				m_metadata.m_commInfo["TaskID"] = m_strTaskID;
				m_metadata.m_commInfo["MaterielID"] = /*"Materiel_Demo_01"*/m_strCodeID;
				m_metadata.m_commInfo["ProdID"] = /*"P00001"*/localUtils::utils::Unicode2Ansi(m_wstrCrtProjectName);
				m_metadata.m_commInfo["RuleID"] = /*"R00001"*/m_postRuleID[m_postFrameName[extractImg.m_nIndex]];
				m_metadata.m_commInfo["Frame"] = m_postFrameName[extractImg.m_nIndex];
				m_metadata.m_commInfo["TranType"] = (extractImg.m_nIndex == (m_crtProject->m_vPattern.size() - 1))?"finish":(extractImg.m_bSave?"normal":"again");
				m_metadata.m_specifiedInfo["Pics"] = m_patternInfo[extractImg.m_nIndex];
				if (!m_httpHelper.SendMetaData(m_metadata, errcode))
				{
					 gSystemLogger.QueueUpLogAndReport(localUtils::utils::Utf8ToUnicode(errcode), LV_ERROR);
				}
				// 等待及超时处理
				if (extractImg.m_nIndex == (m_crtProject->m_vPattern.size() - 1))
				{
					for (int i = 0; i < (int)m_stationInfo.size(); i++)
					{
						if (m_strCodeID == m_stationInfo[i].m_strCode)
						{
							m_vWaitInfo[i].m_strCode = m_strCodeID;
							m_vWaitInfo[i].m_strTask = m_strTaskID;
							m_vTrigFlag[i] = true;
							//SetEvent(m_vTrigHandle[i]);
							break;
						}
					}
					
				}
			}
		}
	}
	return 0;
}

void CSystemManager::SetCurrentPatternIndex(int nPatternIndex)
{
	for (auto it = m_dictIndex_StationPtr.begin(); it != m_dictIndex_StationPtr.end(); it++)
	{
		it->second->SetCurrentPatternIndex(nPatternIndex);
	}
}

bool CSystemManager::WaitForCaptureFinished(int nPatternIndex,int nStationIndex)
{
	bool bResult = true;
	if (m_dictIndex_StationPtr.find(nStationIndex) != m_dictIndex_StationPtr.end())
	{
		CDemuraStation* pStation = m_dictIndex_StationPtr[nStationIndex];
		return pStation->WaitForImageReceivedEvent(nPatternIndex);
	}
	else
	{
		for (auto it = m_dictIndex_StationPtr.begin(); it != m_dictIndex_StationPtr.end(); it++)
		{
			if (!it->second->WaitForImageReceivedEvent(nPatternIndex))
			{
				bResult = false;
			}
		}
		return bResult;
	}
}

//void CSystemManager::ListAllAvailableBaslerCamera(std::map<wstring, bool>& mapNameState)
//{
//	mapNameState.clear();
//	try
//	{
//		Pylon::PylonInitialize(); //初始化
//		Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();//获取传输层工厂。
//		if (tlFactory.EnumerateDevices(m_baslerDeviceInfoList) == 0)//枚举所有的相机
//			return;
//
//		int index = 0;
//		for (DeviceInfoList_t::iterator it = m_baslerDeviceInfoList.begin(); it != m_baslerDeviceInfoList.end(); it++)
//		{
//			wstring name = CA2W((*it).GetUserDefinedName().c_str());
//			bool bAccessible = false;
//			if (Pylon::CTlFactory::GetInstance().IsDeviceAccessible(m_baslerDeviceInfoList[index]))
//			{
//				bAccessible = true;
//			}
//			index++;
//			mapNameState[name] = bAccessible;
//		}
//	}
//	catch (GenICam::GenericException& e)
//	{
//		string strDescription = e.GetDescription();
//		int nStop = 90;
//	}
//}

void CSystemManager::ListAllAvailableHikCamera(std::map<wstring, bool>& mapNameState)
{
	mapNameState.clear();
	try
	{
		int nRet = -1;
		MvCamCtrl::CTlFactory& tlFactory = MvCamCtrl::CTlFactory::GetInstance();//获取传输层工厂
		unsigned int nSupportedTls = tlFactory.EnumerateTls();
		if (MV_GIGE_DEVICE == (nSupportedTls & MV_GIGE_DEVICE))
		{
			memset(&m_hikDeviceInfoList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
			nRet = tlFactory.EnumDevices(MV_GIGE_DEVICE, m_hikDeviceInfoList);//枚举所有相机
			if (MV_OK != nRet)
			{
				return;
			}
			else
			{
				for (int i = 0; i< (int)(m_hikDeviceInfoList.nDeviceNum); i++)
				{
					wstring strUserName;//获取相机名称
					if (strcmp("",(LPCSTR)(m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName))!= 0)
					{
						strUserName =localUtils::utils::AnsiToUnicode((LPCSTR)(m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName));//采用用户定义名(需先设定),推荐使用
					}
					else
					{
						strUserName = localUtils::utils::AnsiToUnicode((LPCSTR)(m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chSerialNumber));//采用相机序列号
					}
					bool bAccessible = false;
					if (tlFactory.IsDeviceAccessible(*(m_hikDeviceInfoList.pDeviceInfo[i])))
					{
						bAccessible = true;
						mapNameState[strUserName] = bAccessible;
					}
				}
			}
		}
	}
	catch (...)
	{
		int nStop = 90;
	}
}

void CSystemManager::StopWorkerThread()
{
	::InterlockedIncrement(&m_nExitFlag);

	// 退出fileMappingThread
	m_fileMappingActive = false;
	ReleaseSemaphore(m_fileMapping.m_Read[1], 1, NULL);

	// 退出HttpRecieveThread
	closesocket(m_httpHelper.m_srvSocket);

	// 退出SaveSrcImageThread
	PENDING_RAW_IMAGE_DATA exitData;
	m_rawImageBuffer.push_front(exitData);
	m_extractImageBuffer.push_front(exitData);
	size_t  m = m_vWorkerThread.size();
	CString stt;
	stt.Format(L"%d", (int)m);
	gSystemLogger.QueueUpLogAndReport(stt.GetBuffer(), LV_ERROR);
	::WaitForMultipleObjects((DWORD)m_vWorkerThread.size(), &m_vWorkerThread[0], TRUE, INFINITE);
}

void CSystemManager::SetStartedFlag(bool bStarted)
{
	m_bStarted = bStarted;
}

void CSystemManager::SetMainUiPtr(CWnd * ptrMainUi)
{
	m_ptrMainUi = ptrMainUi;
}

void CSystemManager::CreateScreenFileFolder(CString barcode)
{
	wstring wstrPath = m_wstrCrtDirectory;
	wstrPath.append(L"\data");
	wstrPath.append(L"\\Screen#");
	wstrPath.append(barcode);
	if (!_wmkdir(wstrPath.c_str()))
	{
		//TODO:
	}
}

void CSystemManager::GetCrtDirectory()
{
	TCHAR  moduleFileName[MAX_PATH];
	::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	(_tcsrchr(moduleFileName, _T('\\')))[1] = 0;
	CString strPath = moduleFileName;
	m_wstrCrtDirectory = strPath.GetString();

	wstring wstrDataPath = m_wstrCrtDirectory;
	wstrDataPath.append(L"\data");
	if (!_wmkdir(wstrDataPath.c_str()))
	{
		//TODO:
	}
}

wstring CSystemManager::ParseRawImageName(int currentImageIndex)
{
	wstring wstrRet;
	auto ptrDictSeqImg = gConfigFileMgr.GetPgSeqSetting();
	if (m_dictIndex_Pattern.find(currentImageIndex) != m_dictIndex_Pattern.end())
	{
		FRAME_PATTERN framePattern = m_dictIndex_Pattern[currentImageIndex];
		wstrRet = (*ptrDictSeqImg)[framePattern.m_nSeqIndex].m_wstrType;
	}
	return  wstrRet;
}

void CSystemManager::ParseRawImageCameraParam(int nCurrentImageIndex, wstring wstrCameraName)
{
	auto crtPattern = m_crtProject->m_vPattern;
	for (auto it = crtPattern.begin(); it != crtPattern.end(); it++)
	{
		if (it->m_nIndex == nCurrentImageIndex)
		{
			auto cameraConfig = it->m_dictCameraName_SAA;
			if (cameraConfig.find(wstrCameraName) != cameraConfig.end())
			{
				auto config = cameraConfig[wstrCameraName];
				int nParaValue = config.begin()->m_nExposureTime;
				m_vImgCameraParam[0]= nParaValue;
				nParaValue = config.begin()->m_nGain;
				m_vImgCameraParam[1] = nParaValue;
			}
		}
	}	
}

int CSystemManager::ParseCameraIndex(wstring wstrCameraName)
{
	auto crtCamera = m_dictIndex_StationPtr[0]->m_dictCameraIndex_Name;
	for (auto it = crtCamera.begin(); it != crtCamera.end(); it++)
	{
		if (wstrCameraName == it->second)
		{
			return it->first;
		}
	}
	return 0;
}

void CSystemManager::InitPatternImgNums()
{
	auto crtPattern = m_crtProject->m_vPattern;
	for (auto it = crtPattern.begin(); it != crtPattern.end(); it++)
	{
		m_patternImgNums[it->m_nIndex] = (int)it->m_dictCameraName_SAA.size();
		m_patternImgCount[it->m_nIndex] = 0;
		map<string, string> mp;
		mp["0"] = "0";
		vector<map<string, string>> vec;
		vec.push_back(mp);
		m_patternInfo[it->m_nIndex] = vec;
		m_patternPath[it->m_nIndex] = L"";
	}
	std::map<int, STATION_CONFIG>* crtStationConfig = gConfigFileMgr.GetStaionConfigArray();
	std::map<wstring, CAMERA_SETTING> cameraNames = (*crtStationConfig)[0].m_dictCamara;
	for (auto it = cameraNames.begin(); it != cameraNames.end(); it++)
	{
		m_postCamName[it->first] = localUtils::utils::Unicode2Utf8(it->first);
		CString strCameraName = (it->first).c_str();
		CString s=strCameraName.Right(1);
		if (!localUtils::utils::IsNumericString(strCameraName.Right(1)))
		{
			m_postPhotoName[it->first] = "main";
		}
		else
		{
			int nCameraIndex = boost::lexical_cast<int>(strCameraName.Right(1).GetString());
			switch (nCameraIndex)
			{
			case 1:
				m_postPhotoName[it->first] = "top";
				break;
			case 2:
				m_postPhotoName[it->first] = "right";
				break;
			case 3:
				m_postPhotoName[it->first] = "bottom";
				break;
			case 4:
				m_postPhotoName[it->first] = "left";
				break;
			}
		}
	}
	
}

wstring CSystemManager::CreatePatternImageRepository(string codeInfo,int ptnIndex)
{
	wstring imagePath = m_wstrTargetFolder;
	imagePath.append(localUtils::utils::AnsiToUnicode(codeInfo));
	if (!localUtils::utils::CheckDirExist(imagePath))
	{
		_wmkdir(imagePath.c_str());
	}
	string imgPath = "\\"+ to_string(ptnIndex);
	imagePath.append(localUtils::utils::AnsiToUnicode(imgPath));
	_wmkdir(imagePath.c_str());
	//--------------------------------------------------------
	if (ptnIndex == 0)
	{
		if (!localUtils::utils::CheckDirExist(m_wstrTargetFolder))
		{
			if (!_wmkdir(m_wstrTargetFolder.c_str()))
			{
				//TODO:
			}
		}
		string strIDFolder = localUtils::utils::Unicode2Utf8(m_wstrTargetFolder);
		strIDFolder.append(codeInfo);
		m_wstrIDFolder = localUtils::utils::Utf8ToUnicode(strIDFolder);
		if (!localUtils::utils::CheckDirExist(m_wstrIDFolder))
		{
			if (!_wmkdir(m_wstrIDFolder.c_str()))
			{
				//TODO:
			}
		}
	}
	//--------------------------------------------------------
	return imagePath;
}

void CSystemManager::SaveRecord(string ctrProj, int nTotal, int nOkCount,double dCrtTime)
{
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString   cstrDate,sTmp, sValue, sOKRate,cstrOkCount;
	cstrDate.Format((_T("%d-%.2d-%.2d %.2d:%.2d")), dt.wYear, dt.wMonth, dt.wDay, dt.wHour, dt.wMinute);
	sTmp.Format(_T("%.2f"), dCrtTime);
	wstring wstrCrtTime = sTmp.GetString();
	wstrCrtTime.append(L"s");
	sValue.Format(_T("%d"), nTotal);
	if (nTotal != 0)
	{
		sOKRate.Format(_T("%.1f"), 1.0f*nOkCount / nTotal * 100);
	}
	else
	{
		sOKRate.Format(_T("%.1f"),0);
	}
	sOKRate.Append(_T("%"));
	cstrOkCount.Format(_T("%d"), nOkCount);

	string strDate = localUtils::utils::Unicode2Utf8(cstrDate.GetString());
	string strTotal = localUtils::utils::Unicode2Utf8(sValue.GetString());
	string strOkCount = localUtils::utils::Unicode2Utf8(cstrOkCount.GetString());
	string strCrtTime = localUtils::utils::Unicode2Utf8(wstrCrtTime);
	string strOKRate = localUtils::utils::Unicode2Utf8(sOKRate.GetString());
	m_sysDatabase.saveCountData(ctrProj.c_str(), strDate.c_str(), strTotal.c_str(), strOkCount.c_str(), strOKRate.c_str(), strCrtTime.c_str());
}

void CSystemManager::ExpRecordHint()
{
	SYSTEMTIME dt;
	::GetLocalTime(&dt);
	CString  strExpDate, strCrtDate;
	if (dt.wMonth != 1)
	{
		strExpDate.Format((_T("%d-%.2d-%.2d %.2d:00")), dt.wYear, (dt.wMonth - 1), dt.wDay, dt.wHour);
	}
	else
	{
		strExpDate.Format((_T("%d-%.2d-%.2d %.2d:00")), (dt.wYear - 1), 12, dt.wDay, dt.wHour);
	}
	strCrtDate.Format((_T("%d-%.2d-%.2d %.2d:00")), dt.wYear, (dt.wMonth - 1), dt.wDay, dt.wHour);
	USES_CONVERSION;
	char* m_strExpDate = T2A(strExpDate);
	char* m_strCrtDate = T2A(strCrtDate);
	wstring wstrCrtProjectName = gConfigFileMgr.GetCurrentProjectName();
	string strProjectName = localUtils::utils::Unicode2Utf8(wstrCrtProjectName);
	strProjectName = TransDBTableName(strProjectName);
	if (m_sysDatabase.QueryRecordRow(m_strExpDate, strProjectName.c_str()) > 0)
	{
		if (IDYES == AfxMessageBox(IDS_STRING_QUERY_HINT, MB_YESNO))
		{
			TransExpRecord(m_strCrtDate, strProjectName.c_str());
		}
		m_sysDatabase.DeleteExpRecord(m_strCrtDate, strProjectName.c_str());
	}
}

void CSystemManager::TransExpRecord(const char*field, const char* crtProj)
{
	char   sql[1024];
	sprintf_s(sql, SQL_QUERY_RECORD, crtProj, field);
	m_queryExpData= m_sysDatabase.Query(sql);
	CFile m_file;
	CString str_totalPath;
	CString strFilter = _T("CSV Files (*.csv)|*.csv||");
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		str_totalPath = dlg.GetPathName();
		str_totalPath += ".csv";
		USES_CONVERSION;
		char* Filename = T2A(str_totalPath);
	}
	if (m_file.Open(str_totalPath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
	{
		CString strHead = _T("ID,DATE,TOTAL,OKCOUNT,OKRATE,CRTTIME\r\n");
		int length = strHead.GetLength();
		m_file.Write(strHead, strHead.GetLength() * 2);
		for (int i = 0; !m_queryExpData.eof(); i++)
		{
			CString strData;
			strData += m_queryExpData.fieldValue("id");
			strData += _T(",");
			strData += m_queryExpData.fieldValue("date");
			strData += _T(",");
			strData += m_queryExpData.fieldValue("total");
			strData += _T(",");
			strData += m_queryExpData.fieldValue("OkCount");
			strData += _T(",");
			strData += m_queryExpData.fieldValue("OkCountRate");
			strData += _T(",");
			strData += m_queryExpData.fieldValue("CrtTime");
			strData += _T("\r\n");
			m_file.Write(strData, strData.GetLength() * 2);
			m_queryExpData.NextRow();
		}
	}
}

void CSystemManager::UpdateStatictics()
{
	m_nTotal += 1;
	double dDur = m_sysTimer.duration();
	CString sTmp;
	sTmp.Format(_T("%.2f"), dDur / 1000.0);
	double dCrtTiming = (unsigned long)dDur / 1000.0;
	m_wstrCrtTiming = sTmp.GetString();
	m_wstrCrtTiming.append(L"s");

	double dTotalTiming = ((unsigned long)dDur / 1000.0) * 1 + m_dTotalTiming;
	m_dTotalTiming = dTotalTiming;
	double dAvg = 0;
	if (m_nTotal != 0) 
	{
		dAvg = (m_dTotalTiming*1.0) / m_nTotal;
	}
	wstring wstrCrtProjectName = gConfigFileMgr.GetCurrentProjectName();
	string strProjectName = localUtils::utils::Unicode2Utf8(wstrCrtProjectName);
	strProjectName = TransDBTableName(strProjectName);
	SaveRecord(strProjectName, m_nTotal, m_nOkCount,dCrtTiming);
	sTmp.Format(_T("%.2f"), dAvg);
	m_wstrAvgTiming = sTmp.GetString();
	m_wstrAvgTiming.append(L"s");
	sTmp.Format(_T("%.2f"), dTotalTiming);
	m_wstrTotalTime = sTmp.GetString();
	m_wstrTotalTime.append(L"s");

	::PostMessage(m_ptrMainUi->m_hWnd, WM_UPDATE_STATISTICS, NULL, NULL);
}

void CSystemManager::UpdateCrtProjectPatterns(std::map<int, FRAME_PATTERN> dictProjectPattern)
{
	std::vector<FRAME_PATTERN> vPattern;
	for (int i = 0; i < dictProjectPattern.size(); i++)
	{
		vPattern.push_back(dictProjectPattern[i]);
	}
	m_crtProject->m_vPattern=vPattern;
}

int CSystemManager::PgPatternNameMap(wstring wstrPatternName)
{
	int nMapIndex = -1;
	std::map<int, PG_IMAGE>* ptrDictImg = gConfigFileMgr.GetPgSetting();
	auto it = ptrDictImg->begin();
	for (; it != ptrDictImg->end(); it++)
	{
		if (wstrPatternName == it->second.m_wstrName)
		{
			nMapIndex = it->second.m_nIndex;
			break;
		}
	}
	return nMapIndex;
}

void CSystemManager::InspectDiskSpace(string strLinuxPath)
{
	long lFreeSpace = localUtils::utils::GetDesignatedDiskSpace(strLinuxPath);
	if (lFreeSpace <= (2 << 10))
	{
		SYSTEMTIME dt;
		GetLocalTime(&dt);
		CString strYear;
		strYear.Format(L"_%d", dt.wYear);
		CString strRemoveName;
		CString strBeforeNameTime = L"9999-99-99-99-99-99-99";
		CString strPath = (localUtils::utils::Utf8ToUnicode(strLinuxPath)).c_str();
		bool bNeedRemove = false;
		CFileFind cFinder;
		BOOL bFile = cFinder.FindFile(strPath + L"\\*.*");
		while (bFile)
		{
			bFile = cFinder.FindNextFile();
			if (cFinder.IsDots())
			{
				continue;
			}
			if (!cFinder.IsDirectory())
			{
				CString strFullName = cFinder.GetFilePath();
				if (!strFullName.Find(L".bmp") || strFullName.GetLength() < 50)
				{
					continue;
				}
				else
				{
					int nStart = strFullName.Find(strYear);
					int nEnd = strFullName.Find(L"Pattern");
					CString  strImgTime = strFullName.Mid(nStart + 1, nEnd - nStart - 2);
					if (strImgTime <= strBeforeNameTime)
					{
						strBeforeNameTime = strImgTime;
						strRemoveName = strFullName;
					}
					bNeedRemove = true;
				}
			}
		}
		if (bNeedRemove)
		{
			CFile tempFile;
			tempFile.Remove(strRemoveName);
		}
	}
}

void CSystemManager::ExpFileHint(string strFilePath)
{
		SYSTEMTIME dt;
		GetLocalTime(&dt);
		CString strRemoveName, strBeforeNameTime;
		if (dt.wMonth != 1)
		{
			strBeforeNameTime.Format((_T("%d-%02d-%02d")), dt.wYear, (dt.wMonth - 1), dt.wDay);

		}
		else
		{
			strBeforeNameTime.Format((_T("%d-%02d-%02d")), (dt.wYear - 1), 12, dt.wDay);
		}
		CString strPath = (localUtils::utils::Utf8ToUnicode(strFilePath)).c_str();
		bool bNeedRemove = false;
		CFileFind cFinder;
		BOOL bFile = cFinder.FindFile(strPath + L"\\*.*");
		while (bFile)
		{
			bFile = cFinder.FindNextFile();
			if (cFinder.IsDots())
			{
				continue;
			}
			if (!cFinder.IsDirectory())
			{
				CString strFullName = cFinder.GetFilePath();
				if (!strFullName.Find(L".log"))
				{
					continue;
				}
				else
				{
					int nEnd = strFullName.Find(L"log");
						CString  strImgTime = strFullName.Mid(nEnd+11,10);
						if (strImgTime!=L"")
						{
							if (strImgTime <= strBeforeNameTime)
							{
								strRemoveName = strFullName;
								bNeedRemove = true;
							}
							if (bNeedRemove)
							{
								CFile tempFile;
								tempFile.Remove(strRemoveName);
								bNeedRemove = false;
							}
						}
				}
			}
		}
}

unsigned int CSystemManager::TriggerSignalSentryThread()
{
	int curCapIndex = 0;
    CString strErrCode;
	string strScreenIDBarcodeOld = "A";
	int stationIndex = -1;
	while (true)
	{
		WaitForSingleObject(m_fileMapping.m_Read[1], INFINITE);
		if (!m_fileMappingActive)
		{
			break;
		}
		curCapIndex = ((FILEMAPPING_COMM_INFO*)m_fileMapping.m_pview[1])->m_nCurCapIndex;
		if (0 == curCapIndex)
		{
			m_sysTimer.start();
			::PostMessage(m_ptrMainUi->m_hWnd, WM_UPDATE_DISP_MAIN, WPARAM(WINDOW_MAIN_CLEAR), LPARAM(NULL));
		}

		string curCode = ((FILEMAPPING_COMM_INFO*)m_fileMapping.m_pview[1])->m_Codestr.data();
		CString cstrCodeInfo;
		cstrCodeInfo = curCode.c_str();
		cstrCodeInfo.Trim();
		
		//get current pattern frame color name
		m_strScreenColorLevel= ((FILEMAPPING_COMM_INFO*)m_fileMapping.m_pview[1])->m_ChScreenID.data();
		CString cstrScreenColorInfo;
		cstrScreenColorInfo = m_strScreenColorLevel.c_str();
		cstrScreenColorInfo.Trim();
		m_strScreenColorLevel = localUtils::utils::TChar2Utf8(cstrScreenColorInfo);
		
		ReleaseSemaphore(m_fileMapping.m_Write[1], 1, NULL);
		ReleaseSemaphore(m_fileMapping.m_SendResult[1], 1, NULL);
		if (m_bStarted)
		{
			if (curCapIndex == 0)
			{
				m_strScreenIDBarcode = localUtils::utils::TChar2Utf8(cstrCodeInfo);
				stationIndex = (stationIndex++)%2 == 0 ? 0 : 1;
				m_stationInfo[stationIndex].m_strCode = m_strScreenIDBarcode;
			}
			strErrCode.Format(L"Pattern index - %d!", curCapIndex);
			gSystemLogger.QueueUpLogAndReport(strErrCode.GetString(), LV_EVENT);
			m_patternImgCount[curCapIndex] = 0;
			m_postFrameName[curCapIndex] = m_strScreenColorLevel;
			m_patternIndex[m_strScreenColorLevel] = curCapIndex;
			m_patternInfo[curCapIndex].clear();
			m_patternPath[curCapIndex] = CreatePatternImageRepository(m_strScreenIDBarcode, curCapIndex);
			SetCurrentPatternIndex(curCapIndex);
			FILEMAPPING_COMM_INFO respondMsg(CAPTURERESULT, WaitForCaptureFinished(curCapIndex, 0), false);
			if (!m_fileMapping.SendMsg(respondMsg))
			{
				gSystemLogger.QueueUpLogAndReport(L"向上位机发送拍照完成信号失败!", LV_ERROR);
			}
		}
	}
	return 0;
}

unsigned int CSystemManager::FileMappingHeartBeatThread()
{
	bool losed = false;
	while (m_nExitFlag == 0)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_fileMapping.m_HeartBeat[0], 4000))
		{
			if (!losed)
			{
				gSystemLogger.QueueUpLogAndReport(L"上位机软件连接异常!", LV_ERROR);
			}
			losed = true;
			::PostMessage(m_ptrMainUi->GetSafeHwnd(), WM_SEND_STATE_TO_MAIN_UI, WPARAM(STOP_STATE), LPARAM(NULL));
		}
		else
		{
			if (losed)
			{
				losed = false;
				::PostMessage(m_ptrMainUi->GetSafeHwnd(), WM_SEND_STATE_TO_MAIN_UI, WPARAM(READY_STATE), LPARAM(NULL));
				gSystemLogger.QueueUpLogAndReport(L"上位机软件连接成功!", LV_EVENT);
			}
		}
	}
	return true;
}

unsigned int CSystemManager::OnHttpRecieved()
{
	SOCKET	 acpt_soc;
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	string rspStr = "";
	while (true)
	{
		acpt_soc = accept(m_httpHelper.m_srvSocket, (sockaddr*)&clientAddr, &clientAddrSize);
		if (acpt_soc == INVALID_SOCKET)
		{
			break;
		}
		string strInfoRec = "";
		int result = 0;
		for (int i = 0; i < 2; i++)
		{
			char recv_buf[HTTP_READ_BUFFER_SIZE + 1] = { 0 };
			result = recv(acpt_soc, recv_buf, HTTP_READ_BUFFER_SIZE, 0);
			if (result == SOCKET_ERROR)
			{
				closesocket(acpt_soc);
				break;
			}
			string curInfoRec(recv_buf);
			strInfoRec += curInfoRec;
			i = result < HTTP_HEADER_SIZE ? 0 : 1;
		}
		if (result == SOCKET_ERROR)
		{
			// 接收异常报错
			gSystemLogger.QueueUpLogAndReport(L"接收VIQ数据异常!", LV_ERROR);
			continue;
		}
		BASEINFO curInfo;
		bool recSuc = m_httpHelper.ParseRecieveInfo(strInfoRec, curInfo);
		// 发送http反馈
		string headStr = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: ";
		string headEndstr = "\r\n\r\n";
		string bodyStr = recSuc ? "{ \"status\":\"SUCCESS\"}":"{ \"status\":\"ERROR\"}";
		rspStr = headStr + to_string(bodyStr.length()) + headEndstr + bodyStr;
		send(acpt_soc, rspStr.c_str(), (int)rspStr.length(), 0);
		// 成功接收则解析处理
		if (recSuc)
		{
			switch (curInfo.m_type)
			{
			case AOI_SERVE_REC_D04:
			{
				FILEMAPPING_COMM_INFO respondMsg(PROCESSRESULT, false, (curInfo.m_commInfo["Status"] == "OK"));
				if (!m_fileMapping.SendMsg(respondMsg))
				{
					gSystemLogger.QueueUpLogAndReport(L"向上位机发送检测结果失败!", LV_ERROR);
				}
				for (int i = 0; i < (int)m_stationInfo.size(); i++)
				{
					if (m_stationInfo[i].m_strCode == curInfo.m_commInfo["MaterielID"])
					{
						SetEvent(m_stationInfo[i].m_hProcessFinish);
						break;
					}
				}
				if (curInfo.m_commInfo["Status"] == "OK")
				{
					m_nOkCount += 1;
				}
				UpdateStatictics();

				m_cfResultInfo.push_back(curInfo);
				if (NULL != m_ptrMainUi&&m_ptrMainUi->GetSafeHwnd())
				{
					::PostMessage(m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_RESULT_INFO, WPARAM(NULL), LPARAM(NULL));
				}
			}
				break;
			case AOI_SERVE_REC_D06:
			{
				// 读取图片文件 重传
				if(!ResendHttpMetadata(curInfo.m_commInfo["MaterielID"], m_patternIndex[curInfo.m_commInfo["Frame"]]))
				{
					gSystemLogger.QueueUpLogAndReport(L"重传数据失败!", LV_ERROR);
				}
			}
				break;
			}
		}
	}
	return 0;
}

unsigned int CSystemManager::WaitForProcessResult0()
{
	while (m_nExitFlag == 0)
	{
		if (m_vTrigFlag[0])
		{
			m_vTrigFlag[0] = false;
			if (WAIT_TIMEOUT == ::WaitForSingleObject(m_stationInfo[0].m_hProcessFinish, 60000))
			{
				gSystemLogger.QueueUpLogAndReport(L"等待处理结果超时，开始查询...", LV_ERROR);

				BASEINFO curInfo;
				string errcode = "";
				curInfo.clear();
				curInfo.m_commInfo["AOIID"] = "AOI001";
				curInfo.m_commInfo["TaskID"] = m_vWaitInfo[0].m_strTask;
				curInfo.m_commInfo["MaterielID"] = /*"Materiel_Demo_01"*/ m_vWaitInfo[0].m_strCode;
				if (!m_httpHelper.GetDetectResult(curInfo, errcode))
				{
					// 查询结果出错
					gSystemLogger.QueueUpLogAndReport(localUtils::utils::Utf8ToUnicode(errcode), LV_ERROR);
				}
				else
				{
					m_cfResultInfo.push_back(curInfo);
					if (NULL != m_ptrMainUi&&m_ptrMainUi->GetSafeHwnd())
					{
						::PostMessage(m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_RESULT_INFO, WPARAM(NULL), LPARAM(NULL));
					}
				}
			}
			else
			{
				ResetEvent(m_stationInfo[0].m_hProcessFinish);
			}
		}
		//::WaitForSingleObject(m_vTrigHandle[0], INFINITE);
		//ResetEvent(m_vTrigHandle[0]);
		Sleep(50);
	}
	return 0;
}

unsigned int CSystemManager::WaitForProcessResult1()
{
	while (m_nExitFlag == 0)
	{
		if (m_vTrigFlag[1])
		{
			m_vTrigFlag[1] = false;
			if (WAIT_TIMEOUT == ::WaitForSingleObject(m_stationInfo[1].m_hProcessFinish, 60000))
			{
				gSystemLogger.QueueUpLogAndReport(L"等待处理结果超时，开始查询...", LV_ERROR);

				BASEINFO curInfo;
				string errcode = "";
				curInfo.clear();
				curInfo.m_commInfo["AOIID"] = "AOI001";
				curInfo.m_commInfo["TaskID"] = m_vWaitInfo[1].m_strTask;
				curInfo.m_commInfo["MaterielID"] = /*"Materiel_Demo_01"*/ m_vWaitInfo[1].m_strCode;
				if (!m_httpHelper.GetDetectResult(curInfo, errcode))
				{
					// 查询结果出错
					gSystemLogger.QueueUpLogAndReport(localUtils::utils::Utf8ToUnicode(errcode), LV_ERROR);
				}
				else
				{
					m_cfResultInfo.push_back(curInfo);
					if (NULL != m_ptrMainUi&&m_ptrMainUi->GetSafeHwnd())
					{
						::PostMessage(m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_RESULT_INFO, WPARAM(NULL), LPARAM(NULL));
					}
				}
			}
			else
			{
				ResetEvent(m_stationInfo[1].m_hProcessFinish);
			}
		}
		//::WaitForSingleObject(m_vTrigHandle[1], INFINITE);
		//ResetEvent(m_vTrigHandle[1]);
		Sleep(50);
	}
	return 0;
}

bool CSystemManager::ResendHttpMetadata(string codeInfo,int ptnIndex)
{
	/*for (int ptnIndex = 0; ptnIndex < m_crtProject->m_vPattern.size(); ptnIndex++)
	{*/
		for (auto it = m_crtProject->m_vPattern.begin(); it != m_crtProject->m_vPattern.end(); it++)
		{
			wstring ptnPath = m_wstrTargetFolder + L"\\" + localUtils::utils::AnsiToUnicode(codeInfo) + L"\\" + localUtils::utils::AnsiToUnicode(to_string(ptnIndex));
			if (!localUtils::utils::CheckDirExist(ptnPath))
			{
				return false;
			}
			if (ptnIndex == it->m_nIndex)
			{
				for (auto it1 = it->m_dictCameraName_SAA.begin(); it1 != it->m_dictCameraName_SAA.end(); it1++)
				{
					wostringstream woss;
					woss << ptnPath << L"\\" << it1->first << L".bmp";
					string strFile = localUtils::utils::Unicode2Ansi(woss.str());
					PENDING_RAW_IMAGE_DATA extractImg;
					extractImg.m_bSave = false;
					extractImg.m_wstrCameraName = it1->first;
					extractImg.m_nIndex = ptnIndex;
					extractImg.m_rawImage = imread(strFile.c_str(),IMREAD_UNCHANGED);
					m_extractImageBuffer.push_front(extractImg);
				}
				break;
			}
		}
	/*}*/
	return true;
}

void CSystemManager::InitCountValue()
{
	wstring wstrCrtProjectName = m_wstrCrtProjectName;
	string strProjectName = localUtils::utils::Unicode2Utf8(wstrCrtProjectName);
	strProjectName = TransDBTableName(strProjectName);
	if (m_sysDatabase.TableExists(strProjectName.c_str()))
	{
		char   sql[1024];

		CString cstrQueryBeginDate(L"2019-02-12 11:00"), cstrQueryEndDate;
		SYSTEMTIME dt;
		::GetLocalTime(&dt);
		cstrQueryEndDate.Format((_T("%d-%.2d-%.2d %.2d:00")), dt.wYear, dt.wMonth, dt.wDay, dt.wHour);
		USES_CONVERSION;
		char* strQueryBeginDate = T2A(cstrQueryBeginDate);
		char* strQueryEndDate = T2A(cstrQueryEndDate);
		sprintf_s(sql, SQL_QUERY_RECORD_DATA, strProjectName.c_str(), strQueryBeginDate, strQueryEndDate);
		CppSQLite3Query QueryData = m_sysDatabase.Query(sql);

		int nTotal(0), nOkCount(0);
		double dTotalTime(0.0);
		for (int i = 0; !QueryData.eof(); i++)
		{
			const char*  charTotal = QueryData.fieldValue("total");
			const char*  charOkCount = QueryData.fieldValue("OkCount");
			const char*  charCrtTime = QueryData.fieldValue("CrtTime");

			nTotal = atoi(charTotal);
			nOkCount = atoi(charOkCount);

			double dTempTime = atof(charCrtTime);
			dTotalTime += dTempTime;
			QueryData.NextRow();
		}
		m_nTotal = nTotal;
		m_dTotalTiming = dTotalTime;
		m_nOkCount = nOkCount;
	}
	else
	{
		m_nTotal = 0;
		m_dTotalTiming = 0.0;
		m_nOkCount = 0;
	}
}

void CSystemManager::HttpTest()
{
//	int a = m_httpHelper.ConnectVIQServer();
//
//	BASEINFO curInfo;
//	string errcode;
//#pragma region HTTP客户端测试
//	curInfo.clear();
//	m_httpHelper.GetProductInfomation(curInfo, errcode);
//
//	curInfo.clear();
//	curInfo.m_commInfo["ProdID"] = "P00001";
//	m_httpHelper.GetDetectRules(curInfo, errcode);
//
//	curInfo.clear();
//	curInfo.m_commInfo["AOIID"] = "AOI001";
//	curInfo.m_commInfo["TaskID"] = "T844944e4c6bb4a35b421d73548ac6bd1";
//	curInfo.m_commInfo["MaterielID"] = "XXX-XXXX-YYYY-10000";
//	m_httpHelper.GetDetectResult(curInfo, errcode);
//
//	curInfo.clear();
//	curInfo.m_commInfo["AOIID"] = "AOI001";
//	curInfo.m_commInfo["TaskID"] = "T7dbb5c3582fd43689b8624ed8176020a";
//	curInfo.m_commInfo["MaterielID"] = "XXX-XXXX-YYYY-10000";
//	curInfo.m_commInfo["ProdID"] = "P00004";
//	curInfo.m_commInfo["RuleID"] = "R000100";
//	curInfo.m_commInfo["Frame"] = "Dark";
//	curInfo.m_commInfo["TranType"] = "normal";
//	vector<map<string, string>> info1(5);
//	info1[0]["Camera"] = "main";
//	info1[0]["Photo"] = "main";
//	info1[0]["Filename"] = "ET3_7Inch_T1-M11891309659A04_2018-11-19-20-31-46-538_Pattern1_main_56000_Gray_Original.bmp";
//	info1[1]["Camera"] = "gige1";
//	info1[1]["Photo"] = "top";
//	info1[1]["Filename"] = "ET3_7Inch_T1-M11891309659A04_2018-11-19-20-31-46-438_Pattern1_gige1_46000_Gray_Original.bmp";
//	info1[2]["Camera"] = "gige2";
//	info1[2]["Photo"] = "bottom";
//	info1[2]["Filename"] = "ET3_7Inch_T1-M11891309659A04_2018-11-19-20-31-46-338_Pattern1_gige2_36000_Gray_Original.bmp";
//	info1[3]["Camera"] = "gige3";
//	info1[3]["Photo"] = "left";
//	info1[3]["Filename"] = "ET3_7Inch_T1-M11891309659A04_2018-11-19-20-31-46-238_Pattern1_gige3_26000_Gray_Original.bmp";
//	info1[4]["Camera"] = "gige4";
//	info1[4]["Photo"] = "right";
//	info1[4]["Filename"] = "ET3_7Inch_T1-M11891309659A04_2018-11-19-20-31-46-138_Pattern1_gige4_16000_Gray_Original.bmp";
//	curInfo.m_specifiedInfo["Pics"] = info1;
//	m_httpHelper.SendMetaData(curInfo, errcode);
#pragma endregion

#pragma region HTTP服务器测试
	/*curInfo.clear();
	curInfo.m_type = TEST_CLIENT_SEND_D04;
	curInfo.m_commInfo["TaskID"] = "0";
	curInfo.m_commInfo["MaterielID"] = "0";
	curInfo.m_commInfo["Status"] = "0";
	vector<DEFECTINFO> info(20);
	for (int i = 0; i < 20; i++)
	{
	info[i].defCommInfo["DefectID"] = "0";
	info[i].defCommInfo["Confidence"] = "0";
	info[i].defCommInfo["Camera"] = "0";
	info[i].defCommInfo["Frame"] = "0";
	info[i].defCommInfo["Photo"] = "0";
	map<string, int> info1;
	info1["X1"] = 10;
	info1["Y1"] = 20;
	info1["X2"] = 30;
	info1["Y2"] = 40;
	info[i].defSpeInfo["PicPos"] = info1;
	info[i].defSpeInfo["ScrPos"] = info1;
	}
	curInfo.m_defectsInfo["Defects"] = info;
	m_httpHelper.PostDetectResult(curInfo);

	curInfo.clear();
	curInfo.m_type = AOI_SERVE_REC_D06;
	curInfo.m_commInfo["TaskID"] = "0";
	curInfo.m_commInfo["MaterielID"] = "0";
	m_httpHelper.PostReSendInfo(curInfo);*/
#pragma endregion
}
