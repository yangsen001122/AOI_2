#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <unordered_map>
#include "bounded_buffer.h"
#include "TypeDef.h"
#include "CameraBase.h"
#include "PatternProcessor.h"
#include <boost/circular_buffer.hpp>

using namespace std;
using namespace boost;


class CDemuraStation
{
public:
	CDemuraStation(STATION_CONFIG stationConfig);
	~CDemuraStation();
	void Switch2Project(wstring wstrProjectName);
	void StartCurrentProject();
	void StopCurrentProject();
	void OnShutdownEvent();
	void SetCurrentPatternIndex(int nPatternIndex);
	void SetStationConfig(wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern,eCAMERAPARAM_TYPE sParaType);
	void SetStationConfig(wstring strProjectName, int nPatternIndex, wstring strCameraname, int nCrtActionGpIndex, std::map<int, FRAME_PATTERN> newFramPattern, int m_nMainCameraHeight);

	int GetStationStatus();
	wstring GetStationCrtProjectName();
	long GetStationCrtCameraHeight();
	void SetStationCrtCameraHeight(long nCameraHeight);
	std::map<wstring, CAMERA_SETTING> GetStationCameras();
	std::vector<SERVO_AXIS_SETTING> GetServoAxisSetting();
	std::map<int,FRAME_PATTERN> GetCurrentProjectPatternArray();
	std::map<int, CAMERA_FRAME_PATTERN>* GetProjectFramePatternForCamera(wstring wstrCameraName);
    std::map<int, wstring> GetCameraIndexNameDict();
	std::map<int, CAMERA_FRAME_PATTERN> GenerateProjectFramePatternForCamera(wstring wstrCameraName);
	std::unordered_map<wstring, std::shared_ptr<CCameraBase>>* GetStationCameraPtr();
	bool WaitForImageReceivedEvent(int nPatternIndex);
	void StopUpdateDebugImageWindow();
	CPatternProcessor* GetPatternProcessor(wstring wstrCameraName);
	void ControlCamera(bool bRun);
	void AbortProcessor();
	STATION_CONFIG  m_stationConfig;
private:
	void InitializeStationCamera();

private:
	long long       m_nTotalCount;
	int             m_nStationIndex;
	wstring         m_wstrCrtCameraName;
	wstring         m_wstrCrtProjectName;
	std::vector<HANDLE>   m_vThreadHandle;
	std::unordered_map<wstring, std::shared_ptr<CCameraBase>>   m_dictCameraName_CameraPtr;
	std::unordered_map<wstring, CPatternProcessor*>             m_dictCamera_ProcessorPtr;
	std::unordered_map<wstring, map<int, CAMERA_FRAME_PATTERN>> m_dictCamera_PatternArray;
public:
	std::map<int, wstring>                                      m_dictCameraIndex_Name; // for Debug Screen
};

