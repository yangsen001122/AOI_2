#include "stdafx.h"
#include "PatternProcessor.h"
#include "CameraBase.h"
#include "utils.h"
#include "SystemManager.h"
#include "ConfigFileManager.h"

extern CSystemManager    gSystemMgr;
extern ConfigFileManager gConfigFileMgr;
extern CSystemLogger     gSystemLogger;

CPatternProcessor::CPatternProcessor(int nStaionIndex, wstring wstrCameraName, bool bMainProcessor, const std::map<int, CAMERA_FRAME_PATTERN>& dictPattern)
	:m_wstrCameraName(wstrCameraName),
	m_processorLogger(wstrCameraName),
	m_ptrCamera(nullptr),
	m_bMainProcessor(bMainProcessor),
	m_bProcessorOk(false),
	m_bDebugMode(false),
	m_bBatchMode(false),
	m_nStationIndex(nStaionIndex),
	m_nCrtPatternIndex(-1),
	m_nCrtActionGpIndex(0),
	m_nOldExposureTime(-1),
	m_nOldGainValue(-1),
	m_nOldWhiteBalanceR(-1),
	m_nOldWhiteBalanceG(-1),
	m_nOldWhiteBalanceB(-1),
	m_dTotalProcessAlgTime(0),
	m_patternIndexBuffer(9),
	m_imgBuffer(9),
	m_cisternImgBuffer(9),
	m_nCrtAlgoIndex(-1),
	m_bDisplayImg(false),
	m_hThread(NULL),
	m_hDebugHandle(NULL),
	m_nCameraIndex(-1),
	m_nPatternLoopIndex(9),
	m_nPatternLoopCount(9),
	m_bExeAllAlgo(true),
	m_bCrtTestFunc(true),
	m_bInChecking(false),
	m_nInternalImgIndex(0),
	m_nResultOK(2)
{
	m_dictIndex_Pattern = dictPattern;
	m_hEndFlag = ::CreateEventW(NULL, TRUE, FALSE, wstrCameraName.c_str());

	m_nPatternLoopCount = (int)m_dictIndex_Pattern.size();

}

CPatternProcessor::~CPatternProcessor()
{

}

void CPatternProcessor::StartProcessThread()
{
	ResetInnerData();
	Proc.MemProc = &CPatternProcessor::PatternInspectingThread;
	UINT threadId;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Proc.ThreadProc, (LPVOID)this, 0, &threadId);
	if (m_hThread == NULL)
	{
		AfxMessageBox(_T("Starting pattern processing thread failed"), MB_OK);
	}
	::SetThreadPriority(m_hThread, THREAD_PRIORITY_HIGHEST);
}

void CPatternProcessor::ExitProcessThread(bool bShutDown)
{
	m_patternIndexBuffer.push_front(-1);
	IMG_PACKAGE lastImg;
	lastImg.m_bExitFlag = true;
	lastImg.m_bDebugFlag = true;
	m_imgBuffer.push_front(lastImg);
}

HANDLE CPatternProcessor::GetThreadHandle()
{
	return m_hThread;
}

void CPatternProcessor::SetCameraPtr(CCameraBase * ptrCamera)
{
	m_ptrCamera = ptrCamera;
}

void CPatternProcessor::SetCurrentPatternIndex(int nPatternIndex)
{
	if (nPatternIndex == 0) // new oled screen is in position
	{
		m_nPatternLoopIndex = m_nPatternLoopCount;
	}
	ResetEvent(m_hEndFlag);
	if (m_ptrCamera && m_ptrCamera->IsCameraOk())
	{
		m_patternIndexBuffer.push_front(nPatternIndex);
	}
}

void CPatternProcessor::SetCurrentActionGroupIndex(int nActionGpIndex)
{
	m_nCrtActionGpIndex = nActionGpIndex;
}

void CPatternProcessor::SetSizeInformation(long nWidth, long nHeight, long nCameraWidth, long nCameraHeight)
{
	m_nResolutionWidth_S = nWidth;
	m_nResolutionHeight_S = nHeight;
	m_nCameraWidth = nCameraWidth;
	m_nCameraHeight = nCameraHeight;
}

void CPatternProcessor::SetDebugFlag(bool bDebug)
{
	m_bDebugMode = bDebug;
}

void CPatternProcessor::ReceiveImage(IMG_PACKAGE& imgPac)
{
	if (!m_bProcessorOk || !m_ptrCamera->IsCameraOk())
	{
		return;
	}
	if (m_imgBuffer.is_not_full())
	{
		m_imgBuffer.push_front(imgPac);
	}
	else
	{
		if (imgPac.m_bExitFlag)
		{
			IMG_PACKAGE tmpPac;
			m_imgBuffer.pop_back(&tmpPac);
			m_imgBuffer.push_back(imgPac);
			m_processorLogger.QueueUpLog(L"Image processing thread is exiting!");
		}
		else
		{
			m_processorLogger.QueueUpLog(L"Image buffer is full !", true);
			
		}
	}
}

void CPatternProcessor::InsertDebugImage(int nPatternIndex, IMG_PACKAGE & debugImg)
{
	m_nCrtPatternIndex = nPatternIndex;
	m_patternIndexBuffer.push_front(nPatternIndex);
	m_imgBuffer.push_front(debugImg);
}

std::map<int, CAMERA_FRAME_PATTERN>* CPatternProcessor::GetCameraPatternArray()
{
	return &m_dictIndex_Pattern;
}

void CPatternProcessor::UpdateDictIndexPatternDebug(int nStationIndex, wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern, eCAMERAPARAM_TYPE sParaType)
{
	switch (sParaType)
	{
	case(PARA_EXPOSURE):
		m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nExposureTime = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nExposureTime;
		break;
	case (PARA_GAIN):
		m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nGain = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nGain;
		break;
	case (PARA_DELAY):
		//m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nFocalPos = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nFocalPos;
		break;
	case (PARA_WHITE_BALANCE_R):
		m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nWhiteBalanceR = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nWhiteBalanceR;
		break;
	case (PARA_WHITE_BALANCE_G):
		m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nWhiteBalanceG = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nWhiteBalanceG;
		break;
	case (PARA_WHITE_BALANCE_B):
		m_dictIndex_Pattern[nPatternIndex].m_vActionGroup[nCrtActionGpIndex].m_nWhiteBalanceB = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex].m_nWhiteBalanceB;
		break;
	default:
		break;
	}
}

void CPatternProcessor::ApplyNewParam(int nPatternIndex, int nActionIndex, int nFuncIndex)
{
	if (m_dictIndex_Pattern.find(nPatternIndex) != m_dictIndex_Pattern.end())
	{
		CAMERA_FRAME_PATTERN& pattern = m_dictIndex_Pattern[nPatternIndex];

		if (nActionIndex < pattern.m_vActionGroup.size() && nFuncIndex < pattern.m_vActionGroup[nActionIndex].m_vAlgoFunc.size())
		{
			pattern.m_vActionGroup[nActionIndex].m_vAlgoFunc[nFuncIndex].CreateComParamArray();
			std::tuple<int, int, int> tmpTuple = std::make_tuple(nPatternIndex, nActionIndex, nFuncIndex);
			m_modifiedFuncBuffer.insert(tmpTuple);
		}
	}
}

bool CPatternProcessor::WaitForProcessingDone()
{
	if (m_bProcessorOk && m_ptrCamera->IsCameraOk())
	{
		DWORD ret = ::WaitForSingleObject(m_hEndFlag, 10000);
		if (ret == WAIT_TIMEOUT)
		{
			m_processorLogger.QueueUpLog(L"Waiting for capture done TIME OUT.", true);
			return false;
		}
		return true;
	}
	return false;
}

void CPatternProcessor::ResetInnerData()
{
	while (m_imgBuffer.is_not_empty())
	{
		IMG_PACKAGE tmpPac;
		m_imgBuffer.pop_back(&tmpPac);
	}
	while (m_patternIndexBuffer.is_not_empty())
	{
		int tmpIndex;
		m_patternIndexBuffer.pop_back(&tmpIndex);
	}
	m_nCrtPatternIndex = -1;
	m_nPatternLoopIndex = m_nPatternLoopCount;
}

void CPatternProcessor::SetImageReceivedEvent()
{
	while (m_imgBuffer.is_not_empty())
	{
		IMG_PACKAGE tmpPac;
		m_imgBuffer.pop_back(&tmpPac);
	}
	while (m_patternIndexBuffer.is_not_empty())
	{
		int tmpIndex;
		m_patternIndexBuffer.pop_back(&tmpIndex);
	}

	IMG_PACKAGE tmpPac;
	tmpPac.m_bExitFlag = true;
	m_imgBuffer.push_back(tmpPac);

	m_nPatternLoopIndex = m_nPatternLoopCount;
}

void CPatternProcessor::GetImageFromCisternBuffer(std::vector<IMG_PACKAGE>& vImgPac)
{
	vImgPac.clear();
	vImgPac.assign(m_cisternImgBuffer.begin(), m_cisternImgBuffer.end());
}

void CPatternProcessor::FlushProcessorLog()
{
	m_processorLogger.FlushLogCache();
}

bool CPatternProcessor::ParseAlgoFunctions(LPVOID pInstance)
{
	auto ptrThis = (CPatternProcessor*)pInstance;
	for (auto it = m_dictIndex_Pattern.begin(); it != m_dictIndex_Pattern.end(); it++)
	{
		int nPatternIndex = it->first;
		CAMERA_FRAME_PATTERN& framePattern = it->second;

		vector<SNAP_ACTION>& vActionGroup = framePattern.m_vActionGroup;

		for (size_t t = 0; t < vActionGroup.size(); t++)
		{
			if (!vActionGroup[t].ParseSelf(m_ptrMuraCOM))
			{
				// TODO:log this event
				return false;
			}
		}
	}
	return true;
}

unsigned int CPatternProcessor::PatternInspectingThread()
{
	HRESULT hr = ::CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = m_ptrMuraCOM.CoCreateInstance(__uuidof(AOIAlgo));
		if (!localUtils::utils::CheckResult(hr) || !ParseAlgoFunctions(this))
		{
			m_processorLogger.QueueUpLog(L"CoCreateInstance falied", true);
			m_ptrMuraCOM.Release();
			::CoUninitialize();
			return -1;
		}
	}
	wstring  wstrCamera = m_ptrCamera->GetCameraName();
	CString cstrCamera;
	cstrCamera.Format(wstrCamera.c_str());
	cstrCamera.Append(L" pattern processor is ready!");
	gSystemLogger.QueueUpLogAndReport(cstrCamera.GetBuffer(),LV_EVENT);

	m_bProcessorOk = true;
	while (true)
	{
		int nPatternIndex = -1;
	    m_patternIndexBuffer.pop_back(&nPatternIndex);
		m_nCrtPatternIndex = nPatternIndex;
		m_bInChecking = true;
		if (nPatternIndex != -1)
		{
			if (m_dictIndex_Pattern.find(nPatternIndex) != m_dictIndex_Pattern.end())
		    {
				CAMERA_FRAME_PATTERN& cameraPattern = m_dictIndex_Pattern[nPatternIndex];
				std::vector<SNAP_ACTION>& vActionGp = cameraPattern.m_vActionGroup;
				//BASEINFO curBaseInfo;
				if (!m_bDebugMode)
				{
					SNAP_ACTION& action = vActionGp[0];
					if (nPatternIndex == 0)
					{
						Sleep(100);
					}
					SetCameraParameter(action.m_nExposureTime, action.m_nGain, action.m_nWhiteBalanceR, action.m_nWhiteBalanceG, action.m_nWhiteBalanceB);
					Sleep(100);
					if (nPatternIndex == 0)
					{
						Sleep(100);
					}
					TriggerCamera();
					m_imgBuffer.pop_back(&m_crtImgPac);
					if (m_crtImgPac.m_bExitFlag) // Abort Event;
					{
						while (m_patternIndexBuffer.is_not_empty())
						{
							int tmpIndex;
							m_patternIndexBuffer.pop_back(&tmpIndex);
						}
						continue;
					}
					m_crtImgPac.m_nPatternIndex = nPatternIndex;
					m_cisternImgBuffer.push_back(m_crtImgPac);
					if (m_crtImgPac.m_img.data)
					{
						gSystemMgr.QueueUpPendingRawImage(m_crtImgPac.m_img, nPatternIndex,m_ptrCamera->GetCameraName(),(m_nStationIndex + 1));
					}
					SetEvent(m_hEndFlag);
					int nFlag = (nPatternIndex == 0) ? 1 : 0;

					m_ptrMuraCOM->SetCalculateMainMatrixFlag(nFlag);
					if (!m_crtImgPac.m_bMainCamera)
					{
						CString strCameraName = m_crtImgPac.m_strCameraName.c_str();
						int nSubCamIndex=_ttoi(strCameraName.Right(1));
						int nRotateAngle=0;
						switch (nSubCamIndex)
						{
						case 1:
							nRotateAngle = 0;
							break;
						case 2:
							nRotateAngle =270;
							break;
						case 3:
							nRotateAngle = 180;
							break;
						case 4:
							nRotateAngle = 90;
							break;
						default:
							break;
						}
						m_ptrMuraCOM->SetSubMatrixRotateFlag(nRotateAngle);
					}
					InspectImage(m_crtImgPac, action.m_vAlgoFunc);//´¦ÀíÍ¼Æ¬
					if (nPatternIndex == 0)
					{
						m_ptrMuraCOM->GetBackGroundValue((DOUBLE*)&gSystemMgr.m_dBgValue);
						if (gSystemMgr.m_dBgValue > 0)
						{
							::PostMessage(gSystemMgr.m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_DISP_MAIN, WPARAM(WINDOW_MAIN_GRAY_VALUE), LPARAM(WINDOW_MAIN_RESULT));
						}
					}
					if (m_resultMat.data)
					{
						gSystemMgr.QueueUpPendingExtractImage(m_resultMat, nPatternIndex, m_ptrCamera->GetCameraName(), (m_nStationIndex + 1));
						if (nPatternIndex == 3)
						{
							gSystemMgr.m_finalDispImg = m_resultMat.clone();
							gSystemMgr.m_finalDispSrc = m_crtImgPac.m_img.clone();
						}
						if (nPatternIndex == m_dictIndex_Pattern.size() - 1)
						{
							//select one result to display on right window as final display
							SetEvent(gSystemMgr.m_hCaptureFinishFlag);
							Sleep(300);
							::PostMessage(gSystemMgr.m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_DISP_MAIN, WPARAM(WINDOW_MAIN_FINAL_DISP), LPARAM(NULL));
						}
					}
				}
				else
				{
					m_imgBuffer.pop_back(&m_crtImgPac);
					m_crtImgPac.m_nPatternIndex = nPatternIndex;
					SNAP_ACTION &actionGp = vActionGp[m_nCrtActionGpIndex];
					int nFlag = (nPatternIndex == 0) ? 1 : 0;
					m_ptrMuraCOM->SetCalculateMainMatrixFlag(nFlag);
					if (!m_crtImgPac.m_bMainCamera)
					{
						CString strCameraName = m_crtImgPac.m_strCameraName.c_str();
						int nSubCamIndex = _ttoi(strCameraName.Right(1));
						int nRotateAngle = 0;
						switch (nSubCamIndex)
						{
						case 1:
							nRotateAngle = 0;
							break;
						case 2:
							nRotateAngle = 270;
							break;
						case 3:
							nRotateAngle = 180;
							break;
						case 4:
							nRotateAngle = 90;
							break;
						default:
							break;
						}
						m_ptrMuraCOM->SetSubMatrixRotateFlag(nRotateAngle);
					}
					InspectImage(m_crtImgPac, actionGp.m_vAlgoFunc);
					//-------------------------------------------------
					if (nPatternIndex == 0)
					{
						m_ptrMuraCOM->GetBackGroundValue((DOUBLE*)&gSystemMgr.m_dBgValue);
					}
					if (m_resultMat.data)
					{
						if (nPatternIndex == 3)
						{
							gSystemMgr.m_finalDispImg = m_resultMat.clone();
							gSystemMgr.m_finalDispSrc = m_crtImgPac.m_img.clone();
					        
					     	//m_type
							BASEINFO curBaseInfo;
							curBaseInfo.m_type = AOI_SERVE_REC_D04;
							//m_commInfo
							std::map<string, string> commInfo;
							commInfo["AOIID"] = "AOI0001";
							commInfo["TaskID"] = "Task1";
							commInfo["MaterielID"] = "VPGMYF-17C272-ABA1P88N966L0122018-11-16TM070RDHP0";
							commInfo["Status"] = "NG";
							curBaseInfo.m_commInfo = commInfo;
							//m_defectsInfo 0
							std::map<string, vector<DEFECTINFO>> defectsInfo;
							std::vector<DEFECTINFO> vDefectInfo;
							DEFECTINFO defectInfo1;
							std::map<string, string> defCommInfo1;
							defCommInfo1["DefectID"] = "1";
							defCommInfo1["Confidence"] = "0.85";
							defCommInfo1["Camera"] = "#0";
							defCommInfo1["Frame"] = "White";
							defCommInfo1["Photo"] = "0";

							std::map<string, map<string, string>> defSpeInfo1;
							std::map<string, string>  picPos1;
							picPos1["X"] = "5100.5";
							picPos1["Y"] = "5100.5";
							picPos1["Width"] = "100.5";
							picPos1["Height"] = "100.5";
							std::map<string, string>  scrPos1;
							scrPos1["X"] = "111";
							scrPos1["Y"] = "111";
							scrPos1["Width"] = "211";
							scrPos1["Height"] = "211";

							defSpeInfo1["PicPos"] = picPos1;

							defectInfo1.defCommInfo = defCommInfo1;
							defectInfo1.defSpeInfo = defSpeInfo1;
							vDefectInfo.push_back(defectInfo1);

							std::map<string, string> defCommInfo2;
							defCommInfo2["DefectID"] = "2";
							defCommInfo2["Confidence"] = "0.86";
							defCommInfo2["Camera"] = "#0";
							defCommInfo2["Frame"] = "White";
							defCommInfo2["Photo"] = "0";

							std::map<string, map<string, string>> defSpeInfo2;
							std::map<string, string>  picPos2;
							picPos2["X"] = "1101.5";
							picPos2["Y"] = "1101.5";
							picPos2["Width"] = "101";
							picPos2["Height"] = "101";
							std::map<string, string>  scrPos2;
							scrPos2["X"] = "3102";
							scrPos2["Y"] = "3102";
							scrPos2["Width"] = "102";
							scrPos2["Height"] = "102";

							defSpeInfo2["PicPos"] = picPos2;

							DEFECTINFO defectInfo2;
							defectInfo2.defCommInfo = defCommInfo2;
							defectInfo2.defSpeInfo = defSpeInfo2;
							vDefectInfo.push_back(defectInfo2);
							//-----------------------------
							std::vector<DEFECTINFO> vDefectInfo11;
							//pattern1
							std::map<string, string> defCommInfo11;
							defCommInfo11["DefectID"] = "1";
							defCommInfo11["Confidence"] = "0.87";
							defCommInfo11["Camera"] = "#0";
							defCommInfo11["Frame"] = "Gray";
							defCommInfo11["Photo"] = "1";

							std::map<string, map<string, string>> defSpeInfo11;
							std::map<string, string>  picPos11;
							picPos11["X"] = "1103";
							picPos11["Y"] = "1103";
							picPos11["Width"] = "103";
							picPos11["Height"] = "103";
							std::map<string, string>  scrPos11;
							scrPos11["X"] = "3102";
							scrPos11["Y"] = "3102";
							scrPos11["Width"] = "102";
							scrPos11["Height"] = "102";

							defSpeInfo11["PicPos"] = picPos11;

							DEFECTINFO defectInfo11;
							defectInfo11.defCommInfo = defCommInfo11;
							defectInfo11.defSpeInfo = defSpeInfo11;
							vDefectInfo11.push_back(defectInfo11);
							vDefectInfo.push_back(defectInfo11);
							//-----------------------------

							//pattern2
							std::map<string, string> defCommInfo21;
							defCommInfo21["DefectID"] = "1";
							defCommInfo21["Confidence"] = "0.87";
							defCommInfo21["Camera"] = "#0";
							defCommInfo21["Frame"] = "Dark";
							defCommInfo21["Photo"] = "1";

							std::map<string, map<string, string>> defSpeInfo21;
							std::map<string, string>  picPos21;
							picPos21["X"] = "1104";
							picPos21["Y"] = "1104";
							picPos21["Width"] = "104";
							picPos21["Height"] = "104";
							std::map<string, string>  scrPos21;
							scrPos21["X"] = "3102";
							scrPos21["Y"] = "3102";
							scrPos21["Width"] = "102";
							scrPos21["Height"] = "102";

							defSpeInfo21["PicPos"] = picPos21;

							DEFECTINFO defectInfo21;
							defectInfo21.defCommInfo = defCommInfo21;
							defectInfo21.defSpeInfo = defSpeInfo21;
							vDefectInfo.push_back(defectInfo21);
							//-----------------------------
							defectsInfo["Defects"] = vDefectInfo;

							curBaseInfo.m_defectsInfo = defectsInfo;
							gSystemMgr.m_cfResultInfo.clear();
							gSystemMgr.m_cfResultInfo.push_back(curBaseInfo);
							if (NULL != gSystemMgr.m_ptrMainUi&&gSystemMgr.m_ptrMainUi->GetSafeHwnd())
							{
								::PostMessage(gSystemMgr.m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_RESULT_INFO, WPARAM(NULL), LPARAM(NULL));
							}
						}
					}
					//---------------------------------------------------------
					if (nPatternIndex == m_dictIndex_Pattern.size() - 1)
					{

					}
					if (NULL != m_hDebugHandle )
					{
						if (m_bBatchMode)
						{
							::PostMessage(m_hDebugHandle, WM_ONE_MORE_IMAGE, WPARAM(NULL), LPARAM(NULL));
						}
					}
				}
			}
			else // This camera will not snap this pattern
			{
				SetEvent(m_hEndFlag);
			}
			if (m_patternIndexBuffer.is_empty())
			{
				m_bInChecking = false;
			}
		}
		else
		{
			SetEvent(m_hEndFlag);
			SetEvent(gSystemMgr.m_hCaptureFinishFlag);
			m_processorLogger.QueueUpLog(L"Receive exit flag - nPatternIndex == -1");
			break;
		}
	}
	m_ptrMuraCOM.Release();
	::CoUninitialize();
	m_bProcessorOk = false;
	return 0;
}

void CPatternProcessor::SetCameraParameter(int nExposureTime, int nGain, int nWhiteBalanceR, int nWhiteBalanceG, int nWhiteBalanceB)
{
	if (m_ptrCamera == nullptr)
	{
		//TODO:
		return;
	}
	if (m_nOldExposureTime != nExposureTime)
	{
		m_ptrCamera->SetExposureTime(nExposureTime);
		m_nOldExposureTime = nExposureTime;

	}
	if (m_nOldGainValue != nGain)
	{
		Sleep(200);
		m_ptrCamera->SetTapGain(nGain);
		m_nOldGainValue = nGain;
	}

	if (m_nOldWhiteBalanceR != nWhiteBalanceR|| m_nOldWhiteBalanceG != nWhiteBalanceG|| m_nOldWhiteBalanceB != nWhiteBalanceB)
	{
		m_ptrCamera->SetWhiteBalance(nWhiteBalanceR, nWhiteBalanceG, nWhiteBalanceB);
		m_nOldWhiteBalanceR = nWhiteBalanceR;
		m_nOldWhiteBalanceG = nWhiteBalanceG;
		m_nOldWhiteBalanceB = nWhiteBalanceB;
	}
}

void CPatternProcessor::TriggerCamera()
{
	if (m_ptrCamera && m_ptrCamera->IsCameraOk())
	{
		m_ptrCamera->SendTriggerSignal(false);
	}
	else
	{
		m_processorLogger.QueueUpLog(L"Trigger camera failed.", true);
	}
}

void CPatternProcessor::ParsePatternColorParam(int nPatternIndex,long &nPatternColor, long &nPatternGrayLevel)
{
	m_ptrDictSeqImg = gConfigFileMgr.GetPgSeqSetting();
	wstring strColorParam = (*m_ptrDictSeqImg)[m_dictIndex_Pattern[nPatternIndex].m_nSeqIndex].m_wstrName;
	wstring strTemp;
	if (strColorParam == _T("Dark"))
	{
		nPatternColor = -1;
		nPatternGrayLevel = -1;
	}
	if (strColorParam.substr(4, 1) == _T("R"))
	{
		nPatternColor = 1;
		strTemp = strColorParam.substr(0, 3);
		nPatternGrayLevel = stol(strTemp, 0, 10);
	}
	else if (strColorParam.substr(4, 1) == _T("G"))
	{
		nPatternColor = 2;
		strTemp = strColorParam.substr(0, 3);
		nPatternGrayLevel = stol(strTemp, 0, 10);
	}
	else if (strColorParam.substr(4, 1) == _T("B"))
	{
		nPatternColor = 3;
		strTemp = strColorParam.substr(0, 3);
		nPatternGrayLevel = stol(strTemp, 0, 10);
	}
	else if (strColorParam.substr(4, 1) == _T("W"))
	{
		nPatternColor = 4;
		strTemp = strColorParam.substr(0, 3);
		nPatternGrayLevel = stol(strTemp, 0, 10);
	}
}

bool CPatternProcessor::Demura()
{
	//TODO
	return true;
}

bool CPatternProcessor::InspectImage(IMG_PACKAGE & imgPac, std::vector<FUNC_INFO>& vFunc)
{
	HRESULT hr = E_FAIL;
	if (imgPac.m_img.data)
		{
			m_crtHObj = localUtils::utils::MatToHobj(imgPac.m_img);
		}
		if (m_crtHObj.IsInitialized())
		{
			hr = m_ptrMuraCOM->SetCurrentImage((long long*)&m_crtHObj);
		}
	//{
	//	hr = m_ptrMuraCOM->VASetInputImage((long long*)&imgPac.m_img);
	//}
	if (!localUtils::utils::CheckResult(hr))
	{
		return false;
	}
	m_resultMat.release();
	if (m_bExeAllAlgo &&!m_bDebugMode)//execute all  algorithm and show result(last out)image
	{
		m_bCrtTestFunc = true;
		for (auto it = vFunc.begin(); it != vFunc.end(); it++)
		{
			FUNC_INFO& fi = *it;
			DISPPARAMS dispParams = { &fi.m_vComParam[0],NULL,(UINT)fi.m_vComParam.size(),0 };
			//¼ì²â
			CComVariant retValue;
			m_timer.start();// algorithm process time
			hr = m_ptrMuraCOM->Invoke(fi.m_dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &retValue, NULL, NULL);
			fi.m_dProcessTime = m_timer.duration();
			hr = m_ptrMuraCOM->GetResultImg((long long*)&m_resultHObj,(LONG *)&m_nResultOK);
			//hr = m_ptrMuraCOM->VACheckResultImage((LONG *)&m_nResultOK);
			//if(m_nResultOK==2)
			//{
			//	hr = m_ptrMuraCOM->VAGetResultImage((long long*)&m_resultMat);
			//}
			if (m_resultHObj.IsInitialized() && m_nResultOK == 2)
			{
				m_resultMat = localUtils::utils::HobjToMat(m_resultHObj);
			}
			else
			{
				gSystemLogger.QueueUpLogAndReport(L"result image from preAlgorithm not ok", LV_ERROR);
			}
			int errRet = retValue.intVal;
			if (!localUtils::utils::CheckResult(hr))
			{
				//TODO:
			}
			if (m_resultMat.data)
			{
				gSystemMgr.m_dictResultImg[imgPac.m_strCameraName].m_image = m_resultMat.clone();
				if (imgPac.m_bMainCamera)
				{
					gSystemMgr.m_dictResultImg2[imgPac.m_nPatternIndex]= m_resultMat.clone();
				}
				DisplayResultOnMainScreen();
				DisplayResultOnDebugScreen(); 
			}
		}
	}
	if (m_bDebugMode)
	{
		FUNC_INFO& fi = vFunc[0];
		DISPPARAMS dispParams = { &fi.m_vComParam[0],NULL,(UINT)fi.m_vComParam.size(),0 };
		//¼ì²â
		m_bCrtTestFunc = true;
		CComVariant retValue;
		m_timer.start();// algorithm process time
		hr = m_ptrMuraCOM->Invoke(fi.m_dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &retValue, NULL, NULL);
		if (!localUtils::utils::CheckResult(hr))
		{
			return false;
		}
		fi.m_dProcessTime = m_timer.duration();
		int errRet = retValue.intVal;
		
		hr = m_ptrMuraCOM->GetResultImg((long long*)&m_resultHObj, (LONG *)&m_nResultOK);

		if (!localUtils::utils::CheckResult(hr))
		{
			return false;
		}
		if (m_resultHObj.IsInitialized() && m_nResultOK == 2)
		{
			m_resultMat = localUtils::utils::HobjToMat(m_resultHObj);
		}
		//hr = m_ptrMuraCOM->VACheckResultImage((LONG *)&m_nResultOK);
		//if (m_nResultOK == 2)
		//{
		//	hr = m_ptrMuraCOM->VAGetResultImage((long long*)&m_resultMat);
		//}
		else
		{
			gSystemLogger.QueueUpLogAndReport(L"result image from preAlgorithm not ok", LV_ERROR);
		}
		if (m_resultMat.data)
		{
			gSystemMgr.m_debugResultImgBuffer.push_back(m_resultMat.clone());
			
			gSystemMgr.m_dictResultImg2[imgPac.m_nPatternIndex] = m_resultMat.clone();
			
			DisplayResultOnDebugScreen();
		}	
	}
	return true;
}

void CPatternProcessor::UpdateDebugScreenImageWindow()
{
	//TODO:
}

void CPatternProcessor::UpdateMainScreenImageWindow()
{
	//TODO:
}

void CPatternProcessor::DisplayResultOnDebugScreen()
{
	if (NULL != m_hDebugHandle)
	{
		if (!m_bDebugMode)
		{
			CString strCameraName = m_crtImgPac.m_strCameraName.c_str();
			BSTR bstr = strCameraName.AllocSysString();
			::PostMessage(m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(WINDOW_NDEBUG_RESULT), LPARAM(bstr));
		}
		else
		{
			::PostMessage(m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(WINDOW_DEBUG_RESULT), LPARAM(m_nCrtPatternIndex));
		}
	}
}

void CPatternProcessor::DisplayResultOnMainScreen()
{
	if (gSystemMgr.m_ptrMainUi&&gSystemMgr.m_ptrMainUi->GetSafeHwnd())
	{
		CString strCameraName = m_crtImgPac.m_strCameraName.c_str();
		BSTR bstr = strCameraName.AllocSysString();
		::PostMessage(gSystemMgr.m_ptrMainUi->GetSafeHwnd(), WM_UPDATE_DISP_MAIN, WPARAM(bstr), LPARAM(WINDOW_MAIN_RESULT));
	}
}

void CPatternProcessor::SetBatchMode(bool bBatchMode)
{
	m_bBatchMode = bBatchMode;
}

void CPatternProcessor::SetInternalImgIndex(int nInternalImgIndex)
{
	m_nInternalImgIndex = nInternalImgIndex;
}

void CPatternProcessor::GrabOneImage(int nPatternIndex)
{
	if (m_patternIndexBuffer.is_not_full())
	{
		m_patternIndexBuffer.push_front(nPatternIndex);
	}
}

