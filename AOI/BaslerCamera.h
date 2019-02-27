#pragma once
#include <map>
#include "CameraBase.h"
#include "TypeDef.h"
#include <boost/atomic.hpp>
#include <pylon/PylonIncludes.h>
#include <genapi\INode.h>
#include <pylon/PylonGUI.h>
#include <pylon\gige\_BaslerGigECameraParams.h>
#include <pylon\gige\BaslerGigEInstantCamera.h>
#include <pylon\ConfigurationEventHandler.h>
#include <pylon\ThreadPriority.h>

using namespace std;
using namespace boost;
using namespace Pylon;
using namespace Basler_GigECamera;

extern Pylon::PylonAutoInitTerm autoInitTerm;

class CBaslerCamera :public CCameraBase,public CImageEventHandler
{
public:
	CBaslerCamera();
	~CBaslerCamera();
	
	void CBaslerCamera::OnImageGrabbed(CInstantCamera & camera, const CGrabResultPtr & ptrGrabResult);
	virtual void DestroyImageEventHandler();

	bool Initialize(const CAMERA_SETTING& cameraSetting);
	bool Start();
	bool Pause();
	bool SetExposureTime(int nExposureTime);
	bool SetTapGain(int nGainValue);
	bool SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB);
	bool SetDelayTime(int nDelay);
	bool IsMainCamera();
	bool IsCameraOk();
	bool SendTriggerSignal(bool bIndependentGrab);
	void Attach2ImageProcessor(CPatternProcessor* ptrProcessor);
	void Reset();
	wstring GetCameraName();
	void SetCameraName(wstring strCameraName);
private:
	bool OpenCamera();
	bool FindCamera(wstring strName, int& nDeviceIndex);
private: 
	Mat                      m_crtImg;
	bool                     m_bColorCamera;
	CPylonImage				 m_TargetImage;
	CPylonBitmapImage        m_DestR;
	CPylonBitmapImage        m_DestG;
	CPylonBitmapImage        m_DestB;
	CImageFormatConverter	 m_Converter;
	CBaslerGigEInstantCamera m_baslerCamera;  
	CGrabResultPtr           m_baslerCameraBuffer;
};

