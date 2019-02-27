#include "stdafx.h"
#include "VieworksCamera.h"
#include "PatternProcessor.h"
#include "SystemManager.h"
#include "utils.h"
#include "HalconCPP.h"
#include "opencv\cv.hpp"
#define MBUFFER 2

using namespace HalconCpp;
using namespace cv;
extern CSystemManager gSystemMgr;

CVieworksCamera::CVieworksCamera()
	:m_ptrAcquisition(nullptr),
	m_ptrBuffers(nullptr),
	m_ptrXfer(nullptr),
	m_ptrSerialPort(nullptr),
	m_bCameraRun(false)
{
	m_nResourceIndex = 0;
	m_nStationIndex = -1;
}

CVieworksCamera::~CVieworksCamera()
{
	m_crtImg.release();
}

void CVieworksCamera::XferCallback(SapXferCallbackInfo* pInfo)
{
# pragma region based on opencv
	CVieworksCamera* ptr = (CVieworksCamera*)pInfo->GetContext();
	ptr->m_compBuff.Copy(ptr->m_ptrBuffers.get());
	PUINT8 pData;
	ptr->m_compBuff.GetAddress((void**)&pData);
	int nWidth = ptr->m_imgSz.cx;
	int nHeight = ptr->m_imgSz.cy;

	Mat cMat;
	cMat.create(nHeight,nWidth, CV_8UC1);
	memcpy(cMat.data, pData, nWidth*nHeight);
	ptr->m_crtImg = cMat.clone();
	SetEvent(gSystemMgr.m_hManualGrabOKFlag);
	if (!ptr->m_bCameraRun)
	{
		return;
	}
	if (ptr->m_crtImg.empty() || !ptr->m_crtImg.data)
	{
		return;
	}
	if (!ptr->m_bIndependentGrab)
	{
		IMG_PACKAGE crtProduct(ptr->m_crtImg, ptr->m_strCameraName,ptr->IsMainCamera());
		ptr->m_ptrPatternProcessor->ReceiveImage(crtProduct);
	}
	if (gSystemMgr.m_ptrMainUi&&gSystemMgr.m_ptrMainUi->GetSafeHwnd())
	{
		try
		{
			if (ptr->m_crtImg.data)
			{
				CAMERA_IMG_SOURCE cameraImgSrc(ptr->m_strCameraName,ptr->IsMainCamera(), ptr->m_crtImg.clone(), IMAGE_GRAY8);
				gSystemMgr.m_dictImageSource[ptr->m_strCameraName] = cameraImgSrc;
				CString strCameraName = ptr->m_strCameraName.c_str();
				bool bMainCamera = ptr->m_cameraSetting.m_bMainCamera;
				BSTR bstr = strCameraName.AllocSysString();
				::PostMessage(gSystemMgr.m_ptrMainUi->m_hWnd, WM_UPDATE_DISP_MAIN, WPARAM(bstr), LPARAM(bMainCamera));
				if (NULL != ptr->m_ptrPatternProcessor->m_hDebugHandle)
				{
					if (ptr->m_bIndependentGrab)
					{
						::PostMessage(ptr->m_ptrPatternProcessor->m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(INDEPENDENT_GRAB), LPARAM(0));
					}
				}
			}
		}
		catch (cv::Exception cvExp)
		{
			gSystemLogger.QueueUpLogAndReport(L"cv::Disp Exception--XferCallback!",LV_ERROR);
		}
	}
# pragma endregion
}

void CVieworksCamera::OnImageGrabbed()
{                               
	//TODO:
}

bool CVieworksCamera::FindCamera(std::map<string, int>& retDict)
{
	int nServerCount = SapManager::GetServerCount();
	if (nServerCount <= 0)
	{
		return false;
	}
	string strCameraName = localUtils::utils::Unicode2Utf8(m_cameraSetting.m_wstrCameraName);
	for (int serverIndex = 0; serverIndex < nServerCount; serverIndex++)
	{
		if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
		{
			char serverName[CORSERVER_MAX_STRLEN];
			memset(serverName, 0, sizeof(char)*CORSERVER_MAX_STRLEN);
			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
			m_strServerCardName = serverName;

			int deviceCount = SapManager::GetResourceCount(m_strServerCardName.c_str(), SapManager::ResourceAcq);
			std::vector<string> vDeviceName;
			for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
			{
				char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
				memset(deviceName, 0, sizeof(char)*CORPRM_GETSIZE(CORACQ_PRM_LABEL));
				SapManager::GetResourceName(m_strServerCardName.c_str(), SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
				vDeviceName.push_back(deviceName);
				string tmpName = deviceName;
				if (strCameraName == tmpName)
				{
					m_nResourceIndex = deviceIndex;
				}
			}
			string strName = localUtils::utils::Unicode2Utf8(m_cameraSetting.m_wstrCameraName);
			auto retIt = std::find(vDeviceName.begin(), vDeviceName.end(), strName);
			int acqDeviceID = (int)std::distance(vDeviceName.begin(), retIt);
			retDict[vDeviceName[acqDeviceID]] = acqDeviceID;
		}
	}
	return (!retDict.empty());
}

bool CVieworksCamera::Initialize(const CAMERA_SETTING& cameraSetting)
{
	//波特率19200，奇偶校验No Parity，数据位8 bit，停止位1 stop bit
	//初始化相机配置,先初始化COM端口
	m_ptrSerialPort.reset(new CSerialPort(cameraSetting.m_nSerialPort, cameraSetting.m_nBuffSize));
	//if (!m_ptrSerialPort->InitPort(115200, NULL, 8, 1, EV_RXCHAR))//71M Camera
	if (!m_ptrSerialPort->InitPort(cameraSetting.m_nBaudrate, NULL, cameraSetting.m_nDatabits, cameraSetting.m_nStopsbits, EV_RXCHAR))
	{
		m_ptrSerialPort->ClosePort();
		//TODO: log this event;]
		CString CameraPort;
		CameraPort.Format(_T("Camera Port %d init fail "),cameraSetting.m_nSerialPort);
		gSystemLogger.QueueUpLogAndReport(CameraPort.GetBuffer(),LV_ERROR);
		return false;
	}
	m_ptrSerialPort->StartMonitoring();
	m_ptrSerialPort->SetOwnerWindowPtr(gSystemMgr.m_ptrMainUi);
	m_cameraSetting = cameraSetting;
	TCHAR configPath[MAX_PATH];
	memset(configPath, 0, sizeof(TCHAR)*MAX_PATH);
	GetEnvironmentVariable(_T("SAPERADIR"), configPath,MAX_PATH);
	CString sConfigFile = configPath;
	sConfigFile.Append(_T("\\CamFiles\\User\\"));
	m_strConfigFilePath = localUtils::utils::Unicode2Utf8(sConfigFile.GetString());
	string strFileName = localUtils::utils::Unicode2Utf8(m_cameraSetting.m_wstrConfigFile);
	m_strConfigFilePath.append(strFileName);
	//std::map<string, int> retDict;
	//if (!FindCamera(retDict))
	//{
	//	CString CameraPort;
	//	CameraPort.Format(_T("Dalsa Camera %d init fail "), cameraSetting.m_nSerialPort - 1);
	//	gSystemLogger.QueueUpLogAndReport(CameraPort.GetBuffer(), LV_ERROR);
	//	return false;
	//}
	SapLocation Location(1,m_nResourceIndex);
	m_ptrAcquisition.reset(new SapAcquisition(Location,m_strConfigFilePath.c_str()));
	m_ptrBuffers.reset(new SapBufferWithTrash(MBUFFER, m_ptrAcquisition.get()));
	m_ptrXfer.reset(new SapAcqToBuf(m_ptrAcquisition.get(), m_ptrBuffers.get(), XferCallback, this));//Create buffer object

	if (m_ptrAcquisition == nullptr || !m_ptrAcquisition->Create())
	{
		return false;
	}
	if (m_ptrBuffers == nullptr || !m_ptrBuffers->Create())
	{
		return false;
	}
	if (m_ptrXfer == nullptr || !m_ptrXfer->Create())//Create transfer object
	{
		return false;
	}

	m_imgSz.cx = (long)m_ptrBuffers->GetWidth();
	m_imgSz.cy = (long)m_ptrBuffers->GetHeight();
	SapBuffer::Type retType = m_ptrBuffers->GetType();
	m_compBuff.SetParameters(1, m_imgSz.cx, m_imgSz.cy, SapFormatMono8, retType);
	m_compBuff.SetPixelDepth(8);
	m_compBuff.Create();
	SetTriggerMode(1);
	wstring strMsg;
	m_bCameraOk = true;
	ResetEvent(gSystemMgr.m_hManualGrabOKFlag);
	return m_bCameraOk;
}

bool CVieworksCamera::OpenCamera()
{
	return false;
}

bool CVieworksCamera::CloseCamera() 
{
	m_bCameraRun = false;
	m_bCameraOk = false;
	if (m_ptrXfer != nullptr)
		m_ptrXfer->Destroy();
	if (m_ptrBuffers != nullptr)
		m_ptrBuffers->Destroy();
	if (m_ptrAcquisition != nullptr)
		m_ptrAcquisition->Destroy();
	return true;
}

bool CVieworksCamera::Start()
{
	if (m_bCameraOk)
	{
		m_bCameraRun = true;
		return true;
	}
	return false;
}

bool CVieworksCamera::Pause()
{
	m_bCameraRun = false;
	return true;
}

bool CVieworksCamera::SetTriggerMode(int nMode)
{
	CString msg0;
	string msg1;
	msg0.Format(_T("stm %d \r"), nMode);
	msg1 = CT2A(msg0);
	m_ptrSerialPort->SendCmdToPort(msg1);//这是设置相机端触发模式
	return true;
}

bool CVieworksCamera::SetExposureTime(int nExposureTime)
{
	CString msg2;
	string msg3;
	msg2.Format(_T("set %d \r"), nExposureTime);
	msg3 = CT2A(msg2);
	m_ptrSerialPort->SendCmdToPort(msg3);
	return true;
}

bool CVieworksCamera::GetExposureTime()
{
	CString msg2;
	string msg3;
	msg2.Format(_T("get \r"));
	msg3 = CT2A(msg2);
	m_ptrSerialPort->SendCmdToPort(msg3);
	return true;
}

bool CVieworksCamera::SetTapGain(int nGainValue)
{
	CString msg0;
	string msg1;
	msg0.Format(_T("sag %d \r"), nGainValue);
	msg1 = CT2A(msg0);
	m_ptrSerialPort->SendCmdToPort(msg1);

	return true;
}

bool CVieworksCamera::SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB)
{
	return false;
}

bool CVieworksCamera::SetDelayTime(int nDelay)
{
	CString msg0;
	string msg1;
	msg0.Format(_T("sso %d \r"), nDelay);
	msg1 = CT2A(msg0);
	m_ptrSerialPort->SendCmdToPort(msg1);
	return true;
}

bool CVieworksCamera::IsMainCamera()
{
	return m_cameraSetting.m_bMainCamera;
}

bool CVieworksCamera::IsCameraOk()
{
	return m_bCameraOk;
}

bool CVieworksCamera::SendTriggerSignal(bool bIndependentGrab)
{
	ResetEvent(gSystemMgr.m_hManualGrabOKFlag);
	if (m_bCameraOk)
	{
		m_bIndependentGrab = bIndependentGrab;
		m_ptrXfer->Snap();
		//m_ptrAcquisition->SetParameter(CORACQ_CAP_SOFTWARE_TRIGGER, TRUE);
		m_ptrAcquisition->SoftwareTrigger(CORACQ_VAL_SOFTWARE_TRIGGER_EXT_FRAME);
		return true;
	}
	return false;
}

void CVieworksCamera::Attach2ImageProcessor(CPatternProcessor * ptrProcessor)
{
	m_ptrPatternProcessor = ptrProcessor;
}

void CVieworksCamera::DalsaConnectCom(CWnd *ptrUI)
{
	m_ptrSerialPort->SetOwnerWindowPtr(ptrUI);
}

void CVieworksCamera::SetStationIndex(int nStationIndex)
{
	m_nStationIndex = nStationIndex;
}

wstring CVieworksCamera::GetCameraName()
{
	return m_strCameraName;
}

void CVieworksCamera::SetCameraName(wstring strCameraName)
{
	m_strCameraName = strCameraName;
}