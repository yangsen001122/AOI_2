#pragma once

#include "TypeDef.h"
class CPatternProcessor;
class CCameraBase
{
public:
	CCameraBase();
	~CCameraBase();
	virtual bool Initialize(const CAMERA_SETTING& cameraSetting) = 0;
	virtual bool Start() = 0;
	virtual bool Pause() = 0;
	virtual bool SetExposureTime(int nExposureTime) = 0;
	virtual bool SetTapGain(int nGainValue) = 0;
	virtual bool SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB) = 0;
	virtual bool SetDelayTime(int nDelay) = 0;
	virtual bool IsMainCamera();
	virtual bool IsCameraOk();
	virtual bool SendTriggerSignal(bool bIndependentGrab) = 0;
	virtual void Attach2ImageProcessor(CPatternProcessor* ptrProcessor) = 0;
	virtual wstring GetCameraName()=0;
	virtual void SetCameraName(wstring strCameraName) = 0;
protected:
	CSize                 m_imgSz;
	CAMERA_SETTING        m_cameraSetting;
	volatile bool         m_bCameraOk;
	volatile bool         m_bMainCamera;
	volatile bool         m_bIndependentGrab;
	CPatternProcessor*    m_ptrPatternProcessor;
	volatile long long    m_nImgCount;
	wstring               m_strCameraName;
};
