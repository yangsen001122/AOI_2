#pragma once

#include <vector>
#include <algorithm>
#include "CameraBase.h"
#include "opencv2/opencv.hpp"
#include "MvCameraControl.h"
#include "TlFactory.h"
#include "MvDeviceBase.h"
#include "CameraParams.h"
#include "GenICam/Base/GCException.h"
#include "ImgShowWnd.h"

using namespace std;
using namespace cv;
using namespace MvCamCtrl;
using namespace GENICAM_NAMESPACE;

const unsigned long MAX_BUFFER_SIZE_HIKCAMERA = 1024 * 1024 * 50;

class CHikCamera : public CCameraBase
{
public:
	CHikCamera();
	~CHikCamera();
	virtual void OnImageGrabbed();
	//通用接口
	bool Initialize(const CAMERA_SETTING& cameraSetting);
	bool Start();
	bool Pause();
	bool SetExposureTime(int nExposureTime);
	bool SetTapGain(int nGainValue);
    bool SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB);
	bool SetDelayTime(int nDelay);
	bool IsMainCamera();
	bool IsCameraOk();
	bool SetTriggerMode(int nTriggerMode);//0：关闭；1：打开
	bool SendTriggerSignal(bool bIndependentGrab);
	void Attach2ImageProcessor(CPatternProcessor* ptrProcessor);
	void Reset();
	wstring GetCameraName();
	void SetCameraName(wstring strCameraName);
private:
	//内部接口
	static int EnumDevices(MV_CC_DEVICE_INFO_LIST*ptrDeviceList);
	int  Open(MV_CC_DEVICE_INFO*ptrDeviceInfo);
	
	int  StartGrabbing();
	int  StopGrabbing();
	int  RegisterImageCallBack(void(__stdcall* ptrOutPut)(unsigned char* ptrData, MV_FRAME_OUT_INFO_EX* ptrFrameInfo, void*ptrUser), void* ptrUser);
	int  GetIntValue(IN const char* ptrKey, OUT unsigned int *ptrValue);
	int  SetIntValue(IN const char* ptrKey, IN unsigned int nValue);
	int  GetFloatValue(IN const char* ptrKey, OUT float *ptrValue);
	int  SetFloatValue(IN const char* ptrKey, IN float  fValue);
	int  GetEnumValue(IN const char* ptrKey, OUT unsigned int *ptrValue);
	int  SetEnumValue(IN const char* ptrKey, IN unsigned int nValue);
	int  GetBoolValue(IN const char* ptrKey, OUT bool *ptrValue);
	int  SetBoolValue(IN const char* ptrKey, IN bool bValue);
	int  GetStringValue(IN const char* ptrKey, IN OUT char* ptrValue, IN unsigned int nSize);
	int  SetStringValue(IN const char* ptrKey, IN const char * ptrValue);
	int  CommandExecute(IN const char*ptrKey);
	int  GetOptimalPacketSize();
	bool RawToRGB(void *devHandle, unsigned char *ptrSrcData, int nHeight, int nWidth, MvGvspPixelType nPixelType, std::vector<unsigned char>& vDstData);
	bool RawToOpenCV(void* devHandle, unsigned char *ptrSrcData, int nHeight, int nWidth, MvGvspPixelType nPixelType, std::vector<unsigned char>& vDstData);
	static  void  __stdcall ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX * pFrameInfo, void*pUser);//回调函数
private:
	bool OpenCamera();
	bool FindCamera(wstring stName, int& nDeviceIndex);
public:
	int  Close();
private:
	void*        m_hDevHandle;
	unsigned int m_nTLayerType;
	IMvDevice*   m_hikCamera;
	bool         m_bInGrabbing;
	bool         m_bColorCamera;
	Mat          m_crtImage;

    std::vector<unsigned char> m_vDataB;
    std::vector<unsigned char> m_vDataG;
    std::vector<unsigned char> m_vDataR;
	std::vector<Mat> m_dictMatChannel;
	chronograph m_timer;
public:
	//unsigned char* m_ptrRGBDataBuffer;//暂存转换数据
    std::vector<unsigned char> m_vRgbDataBuffer;//暂存转换数据
};
