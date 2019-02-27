#include "stdafx.h"
#include "resource.h"
#include "DemuraStation.h"
#include "SystemManager.h"
#include "BaslerCamera.h"
#include "VieworksCamera.h"

extern CSystemManager    gSystemMgr;
extern CSystemLogger     gSystemLogger;
 

CDemuraStation::CDemuraStation(STATION_CONFIG stationConfig)
	:m_stationConfig(stationConfig),
	m_nTotalCount(0),
	m_nStationIndex(stationConfig.m_nStationIndex),
	m_wstrCrtProjectName(stationConfig.m_wstrCrtProjectName)
{
	InitializeStationCamera();
	StartCurrentProject();
	gSystemMgr.CollectStationPtr(stationConfig.m_nStationIndex, this);
}

CDemuraStation::~CDemuraStation()
{

}

void CDemuraStation::Switch2Project(wstring wstrProjectName)
{
	if (m_wstrCrtProjectName == wstrProjectName)
		return;
	StopCurrentProject();
	m_wstrCrtProjectName = wstrProjectName;
	StartCurrentProject();
}

void CDemuraStation::InitializeStationCamera()
{
	for (auto it = m_stationConfig.m_dictCamara.begin(); it != m_stationConfig.m_dictCamara.end(); it++)
	{
		wstring wstrCameraName = it->first;
		CAMERA_SETTING setting = it->second;
		bool bMainCamera = setting.m_bMainCamera;

		m_dictCameraIndex_Name[setting.m_nIndex] = wstrCameraName;
		if (bMainCamera)
		{
			m_dictCameraName_CameraPtr[wstrCameraName].reset(new CVieworksCamera());
			auto ptrCamera = m_dictCameraName_CameraPtr[wstrCameraName].get();
			CVieworksCamera* ptrViewWorks = (CVieworksCamera*)ptrCamera;
			ptrViewWorks->SetStationIndex(m_nStationIndex);
			if (ptrCamera)
			{
				ptrCamera->SetCameraName(wstrCameraName);
				if (!ptrCamera->Initialize(setting))
				{
#ifdef NDEBUG
                    //m_dictCameraName_CameraPtr.erase(wstrCameraName);
#endif
					CString sMsg;
					sMsg.LoadString(IDS_STRING_CREATE_CAMERA_FALIED);
					sMsg.Append(setting.m_wstrCameraName.c_str());
					gSystemLogger.QueueUpLogAndReport(sMsg.GetString(), LV_ERROR);
				}
			}
			else
			{
				//TODO:log this event;
			}
		}
		else
		{
			m_dictCameraName_CameraPtr[wstrCameraName].reset(new CHikCamera());
			auto ptrCamera = m_dictCameraName_CameraPtr[wstrCameraName].get();
			if (ptrCamera)
			{
				ptrCamera->SetCameraName(wstrCameraName);
				if (!ptrCamera->Initialize(setting))
				{
#ifdef NDEBUG
                   // m_dictCameraName_CameraPtr.erase(wstrCameraName);
#endif
					CString sMsg;
					sMsg.LoadString(IDS_STRING_CREATE_CAMERA_FALIED);
					sMsg.Append(setting.m_wstrCameraName.c_str());
					gSystemLogger.QueueUpLogAndReport(sMsg.GetString(), LV_ERROR);
				}
			}
			else
			{
				//TODO:log this event;
			}
		}
	}
}

void CDemuraStation::StartCurrentProject()
{
	for (auto it = m_stationConfig.m_dictCamara.begin(); it != m_stationConfig.m_dictCamara.end(); it++)
	{
		wstring wstrCameraName = it->first;
		bool bMainCamera       = it->second.m_bMainCamera;
		CCameraBase* ptrCamera = nullptr;

		std::map<int, CAMERA_FRAME_PATTERN> cameraPatternList = GenerateProjectFramePatternForCamera(wstrCameraName);
        if (cameraPatternList.empty())
        {
            continue;
        }
        if (m_dictCameraName_CameraPtr.find(wstrCameraName) == m_dictCameraName_CameraPtr.end())
        {
            continue;
        }

		m_dictCamera_PatternArray[wstrCameraName] = cameraPatternList;
		CPatternProcessor* ptrProcessor = new CPatternProcessor(m_nStationIndex,wstrCameraName,bMainCamera, m_dictCamera_PatternArray[wstrCameraName]);
		
		if (ptrProcessor == nullptr)
		{
			//TODO:
			return;
		}
		m_dictCamera_ProcessorPtr[wstrCameraName] = ptrProcessor;

		long nWidthS = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_screenParam.m_nResolutionWidth_S;
		long nHeightS = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_screenParam.m_nResolutionHeight_S;
		long nCameraWidth = m_stationConfig.m_dictCamara[wstrCameraName].m_nWidth;
		long nCameraHeight = m_stationConfig.m_dictCamara[wstrCameraName].m_nHeight;
		ptrProcessor->SetSizeInformation(nWidthS, nHeightS, nCameraWidth, nCameraHeight);

		ptrProcessor->SetCameraPtr(m_dictCameraName_CameraPtr[wstrCameraName].get());
		ptrProcessor->StartProcessThread();

		if (m_dictCameraName_CameraPtr.find(wstrCameraName) != m_dictCameraName_CameraPtr.end())
		{
			ptrCamera = m_dictCameraName_CameraPtr[wstrCameraName].get();
			ptrCamera->Attach2ImageProcessor(ptrProcessor);
		}
		HANDLE hThread = ptrProcessor->GetThreadHandle();
		if (hThread == NULL)
		{
			//TODO:Log this event;
		}
		else
		{
			m_vThreadHandle.push_back(hThread);
		}
	}
}

void CDemuraStation::StopCurrentProject()
{
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
		it->second->ExitProcessThread(false);
	}
	DWORD sz = (DWORD)m_dictCamera_ProcessorPtr.size();
	::WaitForMultipleObjects(sz, &m_vThreadHandle[0], TRUE, INFINITE);
	m_vThreadHandle.clear();
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
		delete it->second;
	}
	m_dictCamera_ProcessorPtr.clear();
}

void CDemuraStation::OnShutdownEvent()
{
	if (!m_dictCameraName_CameraPtr.empty())
	{
		for (auto itCamera = m_dictCameraName_CameraPtr.begin(); itCamera != m_dictCameraName_CameraPtr.end(); itCamera++)
		{
			itCamera->second->Pause();
		}
		for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
		{
			it->second->ExitProcessThread();
			Sleep(300);
		}
		DWORD sz = (DWORD)m_dictCamera_ProcessorPtr.size();
		::WaitForMultipleObjects(sz, &m_vThreadHandle[0], TRUE, INFINITE);//ºÄÊ±¼ä
		m_vThreadHandle.clear();
		for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
		{
			it->second->FlushProcessorLog();
			delete it->second;
		}
		m_dictCamera_ProcessorPtr.clear();
	}
}

void CDemuraStation::SetCurrentPatternIndex(int nPatternIndex)
{
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
        it->second->SetCurrentPatternIndex(nPatternIndex);
	}
}

std::map<wstring, CAMERA_SETTING> CDemuraStation::GetStationCameras()
{
	return m_stationConfig.m_dictCamara;
}

std::vector<SERVO_AXIS_SETTING> CDemuraStation::GetServoAxisSetting()
{
	return m_stationConfig.m_vServoAxisSetting;
}

int CDemuraStation::GetStationStatus()
{
	if (!m_dictCameraName_CameraPtr.empty())
	{
		for (auto it = m_dictCameraName_CameraPtr.begin(); it != m_dictCameraName_CameraPtr.end(); it++)
		{
			bool bCameraOk = it->second->IsCameraOk();
			if (!bCameraOk)
			{
				return STATUS_ERROR;
			}
		}
		return STATUS_OK;
	}
	else
	{
		return STATUS_ERROR;
	}
	
}

wstring CDemuraStation::GetStationCrtProjectName()
{
	return m_stationConfig.m_wstrCrtProjectName;
}

long CDemuraStation::GetStationCrtCameraHeight()
{
	return m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_nMainCameraHeight;
}

//void CDemuraStation::GetStationCrtScreenParam(int &nScreenWidth, int &nScreenHeight)
//{
//	nScreenWidth = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_screenParam.m_nResolutionWidth_S;
//	nScreenHeight = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_screenParam.m_nResolutionHeight_S;
//}

void CDemuraStation::SetStationCrtCameraHeight(long nCameraHeight)
{
	m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_nMainCameraHeight = nCameraHeight;
}

std::map<int, FRAME_PATTERN> CDemuraStation::GetCurrentProjectPatternArray()
{
	PROJECT crtProject = m_stationConfig.m_dictProject[m_wstrCrtProjectName];
	std::map<int, FRAME_PATTERN> retDict;

	for (size_t t = 0; t < crtProject.m_vPattern.size(); t++)
	{
		FRAME_PATTERN& pattern = crtProject.m_vPattern[t];
		retDict[pattern.m_nIndex] = pattern;
	}
	return retDict; 
}

void CDemuraStation::SetStationConfig(wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern,int m_nMainCameraHeight = -1)
{
	SNAP_ACTION& target = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_vPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex];
	SNAP_ACTION& source = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex];
	if (m_nMainCameraHeight != -1)
	{
		m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_nMainCameraHeight = m_nMainCameraHeight;
	}
}

void CDemuraStation::SetStationConfig(wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern, eCAMERAPARAM_TYPE sParaType)
{
	SNAP_ACTION& target = m_stationConfig.m_dictProject[m_wstrCrtProjectName].m_vPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex];
	SNAP_ACTION& source = newFramPattern[nPatternIndex].m_dictCameraName_SAA[strCameraname][nCrtActionGpIndex];
	switch (sParaType)
	{
	    case(PARA_EXPOSURE):
			target.m_nExposureTime = source.m_nExposureTime;
			break;
		case(PARA_GAIN):
			target.m_nGain = source.m_nGain; 
			break;
		case(PARA_DELAY):
			//target.m_nFocalPos = source.m_nFocalPos;
			break;
		case(PARA_WHITE_BALANCE_R):
			target.m_nWhiteBalanceR = source.m_nWhiteBalanceR;
			break;
		case(PARA_WHITE_BALANCE_G):
			target.m_nWhiteBalanceG = source.m_nWhiteBalanceG;
			break;
		case(PARA_WHITE_BALANCE_B):
			target.m_nWhiteBalanceB = source.m_nWhiteBalanceB;
			break;
		default:break;
	}
}

bool CDemuraStation::WaitForImageReceivedEvent(int nPatternIndex)
{
	bool result = true;
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
		if (it->second->m_dictIndex_Pattern.find(nPatternIndex) != it->second->m_dictIndex_Pattern.end())
		{
			if (!(it->second->WaitForProcessingDone()))
			{
				CString sMsg;
				sMsg.LoadString(IDS_STRING_CAPTURE_FAILD);
				sMsg = it->first.c_str() + sMsg;
				gSystemLogger.QueueUpLogAndReport(sMsg.GetString(), LV_ERROR);
				result = false;
			}
		}
	}
	return result;
}

void CDemuraStation::StopUpdateDebugImageWindow()
{
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
		it->second->m_hDebugHandle = NULL;
	}
}

CPatternProcessor * CDemuraStation::GetPatternProcessor(wstring wstrCameraName)
{
	if (m_dictCamera_ProcessorPtr.find(wstrCameraName) != m_dictCamera_ProcessorPtr.end())
	{
		return m_dictCamera_ProcessorPtr[wstrCameraName];
	}
	return nullptr;
}

void CDemuraStation::ControlCamera(bool bRun)
{
	for (auto it = m_dictCameraName_CameraPtr.begin(); it != m_dictCameraName_CameraPtr.end(); it++)
	{
		if (bRun)
		{
			it->second->Start();
		}
		else
		{
			it->second->Pause();
		}
	}
}

void CDemuraStation::AbortProcessor()
{
	for (auto it = m_dictCamera_ProcessorPtr.begin(); it != m_dictCamera_ProcessorPtr.end(); it++)
	{
		it->second->SetImageReceivedEvent();
	}
}

std::map<int, CAMERA_FRAME_PATTERN>* CDemuraStation::GetProjectFramePatternForCamera(wstring wstrCameraName)
{
	if (m_dictCamera_PatternArray.find(wstrCameraName) == m_dictCamera_PatternArray.end())
	{
		return nullptr;
	}
	return &m_dictCamera_PatternArray[wstrCameraName];
}

std::map<int, wstring> CDemuraStation::GetCameraIndexNameDict()
{
    return m_dictCameraIndex_Name;
}

std::map<int, CAMERA_FRAME_PATTERN> CDemuraStation::GenerateProjectFramePatternForCamera(wstring wstrCameraName)
{
	std::map<int, CAMERA_FRAME_PATTERN> retDict;
	if (m_stationConfig.m_dictProject.find(m_wstrCrtProjectName) != m_stationConfig.m_dictProject.end())
	{
		PROJECT& crtProject = m_stationConfig.m_dictProject[m_wstrCrtProjectName];
        CAMERA_FRAME_PATTERN ret;
		std::for_each(crtProject.m_vPattern.begin(), crtProject.m_vPattern.end(), [&](FRAME_PATTERN& framePattern)
		{
            if (framePattern.GetCameraFramePattern(wstrCameraName, ret))
            {
                retDict[framePattern.m_nIndex] = ret;
            }
		});
	}
	return retDict;
}

std::unordered_map<wstring, std::shared_ptr<CCameraBase>>* CDemuraStation::GetStationCameraPtr()
{
	return &m_dictCameraName_CameraPtr;
}
