#pragma once
#include "CameraBase.h"
#include <string>
#include <boost\atomic.hpp>
#include <boost\atomic\atomic.hpp>

#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "SerialPort.h"

using namespace std;
using namespace boost;

class CVieworksCamera :public CCameraBase
{
public:
	CVieworksCamera();
	~CVieworksCamera();
	virtual void OnImageGrabbed();

public:
	bool Initialize(const CAMERA_SETTING& cameraSetting);
	bool Start();
	bool Pause();
	bool SetTriggerMode(int nMode);
	bool SetExposureTime(int nExposureTime);
	bool SetTapGain(int nGainValue);
	bool SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB);
	bool SetDelayTime(int nDelay);
	bool IsMainCamera();
	bool IsCameraOk();
	bool SendTriggerSignal(bool bIndependentGrab);
	void Attach2ImageProcessor(CPatternProcessor* ptrProcessor);
	bool FindCamera(std::map<string, int>& retDict);
	bool OpenCamera();
	bool CloseCamera();
	bool GetExposureTime();
	void DalsaConnectCom(CWnd *ptrUI);
	void SetStationIndex(int nStationIndex);
	wstring GetCameraName();
	void SetCameraName(wstring strCameraName);
	static void XferCallback(SapXferCallbackInfo* pInfo);
	static bool GetAllAcquisitionDevice(std::map<wstring, int>& retDict)
	{
		int serverCount = SapManager::GetServerCount();
		if (serverCount == 0)
		{
			return false;
		}
		string DEVICE_OPTION = "CameraLink 80-Bit Packed/8-Bit RGB";
		for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
		{
			if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
			{
				char serverName[CORSERVER_MAX_STRLEN];
				SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
				string strServerName = serverName;

				int deviceCount = SapManager::GetResourceCount(strServerName.c_str(), SapManager::ResourceAcq);
				std::vector<string> vDeviceName;
				for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
				{
					char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
					SapManager::GetResourceName(strServerName.c_str(), SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
					vDeviceName.push_back(deviceName);
				}
				auto retIt = std::find(vDeviceName.begin(), vDeviceName.end(), DEVICE_OPTION);
				int acqDeviceID = (int)std::distance(vDeviceName.begin(), retIt);
				wstring wstrServerName = localUtils::utils::Utf8ToUnicode(strServerName);
				retDict[wstrServerName] = acqDeviceID;
			}
		}
		return true;
	}
private:
    unique_ptr<SapAcquisition>      m_ptrAcquisition;
	int                             m_nResourceIndex;
	int                             m_nStationIndex;
	std::unique_ptr<SapBuffer>      m_ptrBuffers;
	SapBuffer                       m_compBuff;
	std::unique_ptr<SapTransfer>    m_ptrXfer;
	std::unique_ptr<CSerialPort>    m_ptrSerialPort;
	volatile bool                   m_bCameraRun;
	string                          m_strServerCameraName;
	string                          m_strServerCardName;
	string                          m_strConfigFilePath;
	Mat                             m_crtImg;
};

