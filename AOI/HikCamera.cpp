#include "stdafx.h"
#include "HikCamera.h"
#include "PatternProcessor.h"
#include "SystemManager.h"

extern CSystemManager gSystemMgr;

CHikCamera::CHikCamera()
	:m_hDevHandle(NULL)
	, m_nTLayerType(0)
	, m_bInGrabbing(false)
{
	m_bColorCamera = true;
}


CHikCamera::~CHikCamera()
{
	m_crtImage.release();	
	m_dictMatChannel.clear();
}

void CHikCamera::OnImageGrabbed()
{
	//TODO
}

void __stdcall CHikCamera::ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX * pFrameInfo,void*pUser)
{
#pragma region based on opencv
	
	CHikCamera* ptr = (CHikCamera*)pUser;
	if (NULL == ptr)
	{
		return;
	}
	if (!pFrameInfo || !(ptr->m_bInGrabbing))
	{
		return;
	}
	ptr->m_nImgCount++;
	int nWidth = pFrameInfo->nWidth;
	int nHeight = pFrameInfo->nHeight;
	if (!ptr->RawToOpenCV(ptr->m_hDevHandle, pData, nHeight, nWidth, pFrameInfo->enPixelType, ptr->m_vRgbDataBuffer))
	{
		return;
	}
	if (ptr->m_crtImage.empty()||!ptr->m_crtImage.data)
	{
		return;
	}
	if (!ptr->m_bIndependentGrab)
	{
		IMG_PACKAGE product(ptr->m_crtImage, ptr->m_strCameraName, ptr->IsMainCamera());
		ptr->m_ptrPatternProcessor->ReceiveImage(product);
	}
	
	if (gSystemMgr.m_ptrMainUi&&gSystemMgr.m_ptrMainUi->GetSafeHwnd())
	{
		try
		{
			CAMERA_IMG_SOURCE cameraImgSrc(ptr->m_strCameraName, ptr->IsMainCamera(), ptr->m_crtImage.clone(), IMAGE_RGB8);
			gSystemMgr.m_dictImageSource[ptr->m_strCameraName] = cameraImgSrc;
			CString strCameraName = ptr->m_strCameraName.c_str();
			BSTR bstr = strCameraName.AllocSysString();
			bool bMainCamera = ptr->m_cameraSetting.m_bMainCamera;
			::PostMessage(gSystemMgr.m_ptrMainUi->m_hWnd, WM_UPDATE_DISP_MAIN, WPARAM(bstr), LPARAM(bMainCamera));
			//if (NULL != ptr->m_ptrPatternProcessor->m_hDebugHandle&&strCameraName.Right(1) == ptr->m_ptrPatternProcessor->m_nCameraIndex)
			//{
			//	::PostMessage(ptr->m_ptrPatternProcessor->m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(ptr->m_ptrPatternProcessor->m_nCameraIndex), LPARAM(NULL));
			//}
			if (NULL != ptr->m_ptrPatternProcessor->m_hDebugHandle)
			{
				if (ptr->IsMainCamera())
				{
					::PostMessage(ptr->m_ptrPatternProcessor->m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(INDEPENDENT_GRAB), LPARAM(0));
				}
				else
				{
					int nCameraIndex = boost::lexical_cast<int>(strCameraName.Right(1).GetString());
					::PostMessage(ptr->m_ptrPatternProcessor->m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(INDEPENDENT_GRAB), LPARAM(nCameraIndex));
				}
			}
		}
		catch (cv::Exception &except)
	    {
		    string strError = except.msg;//TODO:Log this event;
	    }
    }
#pragma endregion
}

bool CHikCamera::Initialize(const CAMERA_SETTING & cameraSetting)
{
	m_cameraSetting = cameraSetting;
	m_bCameraOk = OpenCamera();
	if (m_bCameraOk)
	{
		unsigned int nWidth,nHeight;
		(long)GetIntValue("WidthMax", &nWidth);
		(long)GetIntValue("HeightMax", &nHeight);
		m_imgSz.cx = (long)nWidth;
		m_imgSz.cy = (long)nHeight;

        unsigned long nSize = nWidth * nHeight;
        m_vDataB = std::vector<unsigned char>(nSize, 0);
        m_vDataG = std::vector<unsigned char>(nSize, 0);
        m_vDataR = std::vector<unsigned char>(nSize, 0);

		m_dictMatChannel = std::vector<Mat>(3);
		m_dictMatChannel[0].create(nHeight, nWidth, CV_8UC1);
		m_dictMatChannel[1].create(nHeight, nWidth, CV_8UC1);
		m_dictMatChannel[2].create(nHeight, nWidth, CV_8UC1);

        unsigned long nLen = std::max(nSize, MAX_BUFFER_SIZE_HIKCAMERA);

        m_vRgbDataBuffer = std::vector<unsigned char>(nLen, 0);

		SetTriggerMode(MV_TRIGGER_MODE_ON);
		SetEnumValue("TriggerSource",MV_TRIGGER_SOURCE_SOFTWARE);
		SetIntValue("GevHeartbeatTimeout", 8000);
		SetEnumValue("BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_OFF);
		RegisterImageCallBack(ImageCallBackEx,this);
		int nRet=StartGrabbing();
		if (MV_OK == nRet)
		{
			m_bInGrabbing = true;
		}
	}
	return m_bCameraOk;
}

bool CHikCamera::Start()
{
	//if (NULL!=m_hikCamera)
	{
		if (!m_bCameraOk)
		{
			m_bCameraOk = OpenCamera();
		}
		try
		{
			StartGrabbing();
			m_bInGrabbing = true;
			return true;
		}
		catch (GENICAM_NAMESPACE::GenericException& e)
		{
			string strError = e.GetDescription();
			return false;
		}
	}
	return false;
}

bool CHikCamera::Pause()
{
	if (/*m_hikCamera->IsOpen()&&*/m_bInGrabbing)
	{
		StopGrabbing();
	}
	m_bInGrabbing = false;
	return true;
}

bool CHikCamera::SetExposureTime(int nExposureTime)
{
	if (m_bCameraOk)
	{
		int nRet = SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
		if (MV_OK != nRet)
		{
			return false;
		}
		nRet = SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
		nRet = SetFloatValue("ExposureTime", (float)nExposureTime);
		if (MV_OK != nRet)
		{
			return false;
		}
	}
	return true;
}

bool CHikCamera::SetTapGain(int nGainValue)
{
	if (m_bCameraOk)
	{
		int nRet = SetEnumValue("GainAuto", MV_GAIN_MODE_OFF);
		if(MV_OK == nRet)
		{
			nRet = SetFloatValue("Gain", float(nGainValue));
		}
		else
		{
			//TODO:
		}
		if (MV_OK != nRet)
		{
			return false;
		}
	}
	return true;
}

bool CHikCamera::SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB)
{
	if (m_bCameraOk)
	{
		int nRet = SetEnumValue("BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_OFF);
		nRet = SetEnumValue("BalanceRatioSelector", 0);//red
		nRet = SetIntValue("BalanceRatio", nBalanceValueR);
		nRet = SetEnumValue("BalanceRatioSelector", 1);//green
		nRet = SetIntValue("BalanceRatio", nBalanceValueG);
		nRet = SetEnumValue("BalanceRatioSelector", 2);//blue
		nRet = SetIntValue("BalanceRatio", nBalanceValueB);
		if (MV_OK != nRet)
		{
			return false;
		}
		return true;
	}

	return false;
}

bool CHikCamera::SetDelayTime(int nDelay)
{
	if (m_bCameraOk)
	{
		int nRet = SetBoolValue("FrameTimeoutEnable", 1);
		nRet = SetIntValue("FrameTimeoutTime", nDelay);
		if (MV_OK != nRet)
		{
			return false;
		}
	}
	return true;
}

bool CHikCamera::IsMainCamera()
{
	return m_cameraSetting.m_bMainCamera;
}

bool CHikCamera::IsCameraOk()
{
	return m_bCameraOk;
}

bool CHikCamera::SetTriggerMode(int nTriggerMode)
{
	int nRet =SetEnumValue("TriggerMode", nTriggerMode);
	if (MV_OK != nRet)
	{
		return false;
	}
	return true;
}

bool CHikCamera::SendTriggerSignal(bool bIndependentGrab)
{
	if (!m_bCameraOk)
	{
		return false;
	}
	m_bIndependentGrab = bIndependentGrab;
	if (m_bInGrabbing)
	{
		int nRet = CommandExecute("TriggerSoftware");
		if (MV_OK != nRet)
		{
			return false;
		}
	}
	return true;
}

void CHikCamera::Attach2ImageProcessor(CPatternProcessor * ptrProcess)
{
	m_ptrPatternProcessor = ptrProcess;
}

void CHikCamera::Reset()
{
}

wstring CHikCamera::GetCameraName()
{
	return m_strCameraName;
}

void CHikCamera::SetCameraName(wstring strCameraName)
{
	m_strCameraName = strCameraName;
}

int CHikCamera::EnumDevices(MV_CC_DEVICE_INFO_LIST * ptrDeviceList)
{
	int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, ptrDeviceList);
	if (nRet != MV_OK)
	{
		return nRet;
	}
	return MV_OK;
}

int CHikCamera::Open(MV_CC_DEVICE_INFO * ptrDeviceInfo)
{
	if (NULL == ptrDeviceInfo)
	{
		return MV_E_PARAMETER;
	}
	int nRet = MV_OK;
	if (NULL == m_hDevHandle)
	{
		nRet = MV_CC_CreateHandle(&m_hDevHandle, ptrDeviceInfo);
	}
	if (nRet != MV_OK)
	{
		return nRet;
	}
	nRet = MV_CC_OpenDevice(m_hDevHandle);
	if (nRet != MV_OK)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = NULL;
		return nRet;
	}
	return MV_OK;
}

int CHikCamera::Close()
{
	m_bInGrabbing = false;
	int nRet = MV_OK;
	if (NULL == m_hDevHandle)
	{
		return MV_E_PARAMETER;
	}
	nRet=MV_CC_StopGrabbing(m_hDevHandle);
	if (MV_OK != nRet)
	{
		return nRet;	
	}
	nRet = MV_CC_CloseDevice(m_hDevHandle);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	nRet = MV_CC_DestroyHandle(m_hDevHandle);
	m_hDevHandle = NULL;
	return nRet;
}

int CHikCamera::StartGrabbing()
{
	return MV_CC_StartGrabbing(m_hDevHandle);
}

int CHikCamera::StopGrabbing()
{
	return MV_CC_StopGrabbing(m_hDevHandle);
}

int CHikCamera::RegisterImageCallBack(void(__stdcall* ptrOutPut)(unsigned char* ptrData, MV_FRAME_OUT_INFO_EX* ptrFrameInfo, void*ptrUser), void* ptrUser)
{
	return MV_CC_RegisterImageCallBackEx(m_hDevHandle, ptrOutPut, ptrUser);
}

int CHikCamera::GetIntValue(IN const char * ptrKey, OUT unsigned int * ptrValue)
{
	if (NULL == ptrKey || NULL == ptrValue)
	{
		return MV_E_PARAMETER;
	}
	MVCC_INTVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_INTVALUE));
	int nRet = MV_CC_GetIntValue(m_hDevHandle, ptrKey, &stParam);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	*ptrValue = stParam.nCurValue;
	return MV_OK;
}

int CHikCamera::SetIntValue(IN const char * ptrKey, IN unsigned int nValue)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetIntValue(m_hDevHandle, ptrKey, nValue);
}

int CHikCamera::GetFloatValue(IN const char * ptrKey, OUT float * ptrValue)
{
	if (NULL == ptrKey || NULL == ptrValue)
	{
		return MV_E_PARAMETER;
	}
	MVCC_FLOATVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_FLOATVALUE));
	int nRet = MV_CC_GetFloatValue(m_hDevHandle, ptrKey, &stParam);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	*ptrValue = stParam.fCurValue;
	return MV_OK;
}

int CHikCamera::SetFloatValue(IN const char * ptrKey, IN float fValue)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetFloatValue(m_hDevHandle, ptrKey, fValue);
}

int CHikCamera::GetEnumValue(IN const char * ptrKey, OUT unsigned int * ptrValue)
{
	if (NULL == ptrKey || NULL == ptrValue)
	{
		return MV_E_PARAMETER;
	}
	MVCC_ENUMVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_ENUMVALUE));
	int nRet = MV_CC_GetEnumValue(m_hDevHandle, ptrKey, &stParam);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	*ptrValue = stParam.nCurValue;
	return MV_OK;
}

int CHikCamera::SetEnumValue(IN const char * ptrKey, IN unsigned int nValue)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetEnumValue(m_hDevHandle, ptrKey, nValue);
}

int CHikCamera::GetBoolValue(IN const char * ptrKey, OUT bool * ptrValue)
{
	if (NULL == ptrKey || NULL == ptrValue)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_GetBoolValue(m_hDevHandle, ptrKey, ptrValue);
}

int CHikCamera::SetBoolValue(IN const char * ptrKey, IN bool bValue)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetBoolValue(m_hDevHandle, ptrKey, bValue);
}

int CHikCamera::GetStringValue(IN const char * ptrKey, IN OUT char *ptrValue, IN unsigned int nSize)
{
	if (NULL == ptrKey || NULL == ptrValue)
	{
		return MV_E_PARAMETER;
	}
	MVCC_STRINGVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_STRINGVALUE));
	int nRet = MV_CC_GetStringValue(m_hDevHandle, ptrKey, &stParam);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	strcpy_s(ptrValue, nSize, stParam.chCurValue);
	return MV_OK;
}

int CHikCamera::SetStringValue(IN const char * ptrKey, IN const char * ptrValue)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetStringValue(m_hDevHandle, ptrKey, ptrValue);
}

int CHikCamera::CommandExecute(IN const char * ptrKey)
{
	if (NULL == ptrKey)
	{
		return MV_E_PARAMETER;
	}
	return MV_CC_SetCommandValue(m_hDevHandle, ptrKey);
}

int CHikCamera::GetOptimalPacketSize()
{
	return MV_CC_GetOptimalPacketSize(m_hDevHandle);
}

bool CHikCamera::RawToRGB(void * devHandle, unsigned char * ptrSrcData, int nHeight, int nWidth, MvGvspPixelType nPixelType, std::vector<unsigned char>& vDstData)
{
	bool m_bSupportType = false;
	switch (nPixelType)
	{
		case PixelType_Gvsp_RGB8_Packed://该种格式不用转换
			m_bSupportType = false;
			break;
		case PixelType_Gvsp_Mono8: 
		case PixelType_Gvsp_Mono10:
		case PixelType_Gvsp_Mono10_Packed:
		case PixelType_Gvsp_Mono12:
		case PixelType_Gvsp_Mono12_Packed: 
		case PixelType_Gvsp_BayerGR8: 
		case PixelType_Gvsp_BayerGR10: 
		case PixelType_Gvsp_BayerGR12:
		case PixelType_Gvsp_BayerGR10_Packed:
		case PixelType_Gvsp_BayerGR12_Packed: 
		case PixelType_Gvsp_YUV422_Packed:
		case PixelType_Gvsp_YUV422_YUYV_Packed:
			m_bSupportType = true;
			break;
		default:
			m_bSupportType = false;
			break;
	}
	if (m_bSupportType == true)//不是RGB格式的需要先转换为RGB格式
	{
		MV_CC_PIXEL_CONVERT_PARAM pixelConvertParam;
		int nRet = STATUS_OK;
		pixelConvertParam.pSrcData = ptrSrcData;
		if (NULL==pixelConvertParam.pSrcData)
		{
			//TODO:log this event
			return false;
		}
		pixelConvertParam.nWidth = nWidth;
		pixelConvertParam.nHeight = nHeight;
		pixelConvertParam.enSrcPixelType = nPixelType;
		pixelConvertParam.nSrcDataLen = (unsigned int)(nWidth * nHeight * ((((unsigned int)nPixelType) >> 16) & 0x00ff) >> 3);
		pixelConvertParam.nDstBufferSize = (unsigned int)(nWidth * nHeight * ((((unsigned int)PixelType_Gvsp_RGB8_Packed) >> 16) & 0x00ff) >> 3);
		pixelConvertParam.pDstBuffer = &vDstData[0];
		pixelConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed;
		pixelConvertParam.nDstBufferSize = nWidth * nHeight * 3;
		nRet = MV_CC_ConvertPixelType(devHandle, &pixelConvertParam);
		if (nRet != MV_OK)
		{
			return false;
			//TODO:log this event
		}
	}
	else
	{
		memcpy(&vDstData[0], ptrSrcData, nWidth * nHeight * 3);
	}
	return true;
}

bool  CHikCamera::RawToOpenCV(void * devHandle, unsigned char * ptrSrcData, int nHeight, int nWidth, MvGvspPixelType nPixelType, std::vector<unsigned char>& vDstData)
{
	if (RawToRGB(devHandle, ptrSrcData, nHeight, nWidth, nPixelType, vDstData) != true)
	{
		//TODO:log this event
		return false;
	}
	//int nSupWidth = (nWidth + 3) & 0xfffffffc;  //宽度补齐为4的倍数
    unsigned long nLen = nHeight*nWidth;
	m_timer.start();
    for (unsigned long i = 0; i < 3*nLen; i += 3)
    {
        int nIndex = int(i / 3);
        m_vDataR[nIndex] = vDstData[i];
        m_vDataG[nIndex] = vDstData[i + 1];
        m_vDataB[nIndex] = vDstData[i + 2];
    }
	Mat cMat;
	cMat.create(nHeight,nWidth, CV_8UC3);
	memcpy(m_dictMatChannel[2].data, &m_vDataR[0], nLen);//Opencv中，像素排列顺序为BGR，不是RGB
	memcpy(m_dictMatChannel[1].data, &m_vDataG[0], nLen);
	memcpy(m_dictMatChannel[0].data, &m_vDataB[0], nLen);

	cv::merge(m_dictMatChannel, cMat);
	if (m_crtImage.data)
	{
		m_crtImage.release();
	}
    m_crtImage = cMat.clone();
	cMat.release();
	double dTime = m_timer.duration();
	CString stime;
	stime.Format(L" time=%.3f", dTime);
	gSystemLogger.QueueUpLogAndReport(m_cameraSetting.m_wstrCameraName + stime.GetBuffer(), LV_EVENT);
	return true;
}

bool CHikCamera::OpenCamera()
{
	try
	{
		int nDevIndex = -1;
		if (!FindCamera(m_cameraSetting.m_wstrCameraName, nDevIndex))
		{
			return false;
		}
		int nRet = MV_OK;
		MvCamCtrl::CTlFactory& tlFactory = MvCamCtrl::CTlFactory::GetInstance();
		if (tlFactory.IsDeviceAccessible(*(gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[nDevIndex])))
		{
			m_nTLayerType = gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[nDevIndex]->nTLayerType;
			nRet=Open(gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[nDevIndex]);
			if (gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[nDevIndex]->nTLayerType == MV_GIGE_DEVICE)
			{
				int nPacketSize = GetOptimalPacketSize();
				if (nPacketSize > 0)
				{
					SetIntValue("GevSCPSPacketSize", nPacketSize);
				}
			}
		}
		if (MV_OK == nRet)
		{
			MV_CC_SetPixelFormat(m_hDevHandle, PixelType_Gvsp_RGB8_Packed);
			wstring strMsg;
			m_bCameraOk = true;
		}
		else
		{
			nRet = Close();
			m_bCameraOk = false;
			return false;
		}
	}
	catch (GENICAM_NAMESPACE::GenericException &e)
	{
		string strError("OpenCamera failed --");
		strError.append(e.GetDescription());
		wstring sLog = localUtils::utils::Utf8ToUnicode(strError);
		return false;
	}
	return true;
}

bool CHikCamera::FindCamera(wstring strName, int & nDeviceIndex)
{
	CString sMsg;
	wstring strCameraName = strName;
	nDeviceIndex = 0;
	bool  bFind = false;
	for (int i = 0; i <(int)(gSystemMgr.m_hikDeviceInfoList.nDeviceNum); i++)
	{
		wstring strUserName;//获取相机名称
		if (strcmp("", (LPCSTR)(gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
		{
			strUserName = localUtils::utils::AnsiToUnicode((LPCSTR)(gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName));//采用用户定义名(需先设定),推荐使用
		}
		else
		{
			strUserName = localUtils::utils::AnsiToUnicode((LPCSTR)(gSystemMgr.m_hikDeviceInfoList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chSerialNumber));//采用相机序列号
		}
		if (strCameraName == strUserName)
		{
			bFind = true;
			break;
		}
		nDeviceIndex++;
	}
	if (!bFind)
	{
		//TODO
	}
	return bFind;
}
