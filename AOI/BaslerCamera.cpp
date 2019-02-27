#include "stdafx.h"
#include "BaslerCamera.h"
#include "PatternProcessor.h"
#include "SystemManager.h"


extern CSystemManager gSystemMgr;

Pylon::PylonAutoInitTerm autoInitTerm;

CBaslerCamera::CBaslerCamera()
{
	Pylon::PylonInitialize();
	m_baslerCamera.RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
	m_baslerCamera.RegisterConfiguration(new CAcquireContinuousConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
	m_baslerCamera.RegisterImageEventHandler(this, RegistrationMode_Append, Cleanup_None);
	m_bColorCamera = false;
}


CBaslerCamera::~CBaslerCamera()
{
	Pylon::PylonTerminate();
	m_crtImg.release();
}

void CBaslerCamera::OnImageGrabbed(CInstantCamera & camera,  const CGrabResultPtr & ptrGrabResult)
{                                  // 一个即时相机对象  ,这个智能指针将接收抓取结果数据。
#pragma region based on opencv
	m_nImgCount++;
	if (ptrGrabResult->GrabSucceeded())// Image grabbed successfully?
	{
		if (m_baslerCamera.IsGrabbing())//检索到图片时。	
		{
			int nWidth = ptrGrabResult->GetWidth();           
			int nHeight = ptrGrabResult->GetHeight();
			Mat cMat;
			if (!m_bColorCamera)
			{
				cMat.create(nHeight, nWidth, CV_8UC1);
				memcpy(cMat.data, (unsigned char*)ptrGrabResult->GetBuffer(), nWidth*nHeight);
				if (m_crtImg.data)
				{
					m_crtImg.release();
				}
				m_crtImg = cMat.clone();
			}
			else
			{
				m_Converter.OutputPixelFormat = PixelType_RGB8planar;
				m_Converter.Convert(m_TargetImage, (IImage&)ptrGrabResult);
				m_DestR.CopyImage(m_TargetImage.GetPlane(0));
				m_DestG.CopyImage(m_TargetImage.GetPlane(1));
				m_DestB.CopyImage(m_TargetImage.GetPlane(2));
				unsigned char* ptrDataR = (unsigned char*)m_DestR.GetBuffer();
				unsigned char* ptrDataG = (unsigned char*)m_DestG.GetBuffer();
				unsigned char* ptrDataB = (unsigned char*)m_DestB.GetBuffer();
				vector<Mat> dictMatChannel(3);
				cMat.create(nHeight, nWidth, CV_8UC3);
				dictMatChannel[0].create(nHeight, nWidth, CV_8UC1);
				dictMatChannel[1].create(nHeight, nWidth, CV_8UC1);
				dictMatChannel[2].create(nHeight, nWidth, CV_8UC1);
				memcpy(dictMatChannel[2].data, ptrDataR, nWidth*nHeight);//Opencv中，像素排列顺序为BGR，不是RGB
				memcpy(dictMatChannel[1].data, ptrDataG, nWidth*nHeight);
				memcpy(dictMatChannel[0].data, ptrDataB, nWidth*nHeight);
				cv::merge(dictMatChannel, cMat);
				Mat tempMat;
				tempMat.create(nHeight, nWidth, CV_8UC3);
				for (int i = 0; i < nHeight; i++)//图像镜像
				{
					for (int j = 0; j < nWidth; j++)
					{
						tempMat.data[(nHeight - 1 - i)*(nWidth * 3) + j * 3 + 0] = cMat.data[i*nWidth * 3 + j * 3 + 0];
						tempMat.data[(nHeight - 1 - i)*(nWidth * 3) + j * 3 + 1] = cMat.data[i*nWidth * 3 + j * 3 + 1];
						tempMat.data[(nHeight - 1 - i)*(nWidth * 3) + j * 3 + 2] = cMat.data[i*nWidth * 3 + j * 3 + 2];
					}
				}
				m_crtImg = tempMat.clone();
			}
			if (m_crtImg.empty() || !m_crtImg.data)
			{
				return;
			}
			IMG_PACKAGE product(m_crtImg, m_strCameraName,m_bMainCamera);
			m_ptrPatternProcessor->ReceiveImage(product);
			if (gSystemMgr.m_ptrMainUi&&gSystemMgr.m_ptrMainUi->GetSafeHwnd())
			{
				try
				{
					if (m_crtImg.data)
					{
						CAMERA_IMG_SOURCE cameraImgSrc(m_strCameraName,m_bMainCamera,m_crtImg.clone(),IMAGE_RGB8);
						gSystemMgr.m_dictImageSource[m_strCameraName] = cameraImgSrc;
						CString strCameraName = m_strCameraName.c_str();
						BSTR bstr = strCameraName.AllocSysString();
						::PostMessage(gSystemMgr.m_ptrMainUi->m_hWnd, WM_UPDATE_DISP_MAIN, WPARAM(bstr), LPARAM(NULL));
						if (NULL != m_ptrPatternProcessor->m_hDebugHandle&&strCameraName.Right(1)==m_ptrPatternProcessor->m_nCameraIndex)
						{
							::PostMessage(m_ptrPatternProcessor->m_hDebugHandle, WM_UPDATE_DISP_DEBUG, WPARAM(m_ptrPatternProcessor->m_nCameraIndex), LPARAM(NULL));
						}
					}
				}
				catch (cv::Exception &except)
				{
					string strError = except.msg;//TODO:Log this event;
				}
		    }
		}
#pragma endregion
	}
	else // The image is incompletely grabbed
	{
		auto strDesc = ptrGrabResult->GetErrorDescription();
		uint32_t nErrCode =  ptrGrabResult->GetErrorCode();
		if (!camera.IsCameraDeviceRemoved()) // No not handle errors caused by device removal!
		{
			//TODO:log this event;
		}
		else // camera has been removed 
		{
		}
	}
}

void CBaslerCamera::DestroyImageEventHandler()
{
	//rewrite this function and DO NOT do anything.
}


bool CBaslerCamera::FindCamera(wstring strName, int& nDeviceIndex)
{
	CString sMsg;
	CString strCameraName = strName.c_str();
	nDeviceIndex = 0;
	bool bFind = false;
	for (DeviceInfoList_t::iterator it = gSystemMgr.m_baslerDeviceInfoList.begin(); it != gSystemMgr.m_baslerDeviceInfoList.end(); it++)
	{
		CString name((*it).GetUserDefinedName().c_str());//获取basler相机名称
		if (name == strCameraName)
		{
			bFind = true;
			break;
		}
		nDeviceIndex++;
	}
	if (!bFind)
	{
		//TODO:log this event.
	}
	return bFind;
}


bool CBaslerCamera::Initialize(const CAMERA_SETTING& cameraSetting)
{
	if (m_baslerCamera.IsOpen())
	{
		m_baslerCamera.Close();
	}
	m_cameraSetting = cameraSetting;
	m_bCameraOk = OpenCamera();
	if (m_bCameraOk)
	{
		m_imgSz.cx = (long)m_baslerCamera.Width.GetValue();
		m_imgSz.cy = (long)m_baslerCamera.Height.GetValue();
		//m_baslerCamera.TriggerSelector.SetValue(TriggerSelector_AcquisitionStart);
	    m_baslerCamera.TriggerMode.SetValue(TriggerMode_On);
		//m_baslerCamera.AcquisitionMode.SetValue(AcquisitionMode_SingleFrame);
		m_baslerCamera.TriggerSource.SetValue(TriggerSource_Software);
		m_baslerCamera.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
	}
	return m_bCameraOk;
}

bool CBaslerCamera::OpenCamera()
{
	try
	{
		int deviceIndex = -1;
		if (!FindCamera(m_cameraSetting.m_wstrCameraName, deviceIndex))//check 相机名字和工站是否统一
			return false;
		m_baslerCamera.DestroyDevice();//销毁代表分离摄像机设备的Pylon设备。
		if (Pylon::CTlFactory::GetInstance().IsDeviceAccessible(gSystemMgr.m_baslerDeviceInfoList[deviceIndex]))
		{
			m_baslerCamera.Attach(Pylon::CTlFactory::GetInstance().CreateDevice(gSystemMgr.m_baslerDeviceInfoList[deviceIndex]));//创建pylon设备
			m_baslerCamera.Open();
			m_bCameraOk = true;
			PixelFormatEnums eFormat = m_baslerCamera.PixelFormat.GetValue();
			m_bColorCamera = (eFormat > PixelFormat_Mono16 ? true : false);
			wstring strMsg;
            bool bRet = true;
			if (!bRet)                              
			{
				//TODO:Log this event.strMsg
				m_baslerCamera.Close();
				m_bCameraOk = false;
			}
			return bRet;
		}
		else
		{
			m_bCameraOk = false;
			//TODO:log thid event.
			return false;
		}
	}
	catch (GenICam::GenericException &e)		
	{
		string strError("OpenCamera failed --");
		strError.append(e.GetDescription());
		wstring sLog = localUtils::utils::Utf8ToUnicode(strError);

		return false;
	}
	return true;
}

bool CBaslerCamera::SetExposureTime(int nExposureTime)
{
	if (m_bCameraOk && IsWritable(m_baslerCamera.ExposureTimeAbs))
	{
		m_baslerCamera.ExposureTimeAbs.SetValue(nExposureTime);
		return true;
	}
	return false;
}


bool CBaslerCamera::SetTapGain(int nGainValue)
{
	if (m_bCameraOk && IsWritable(m_baslerCamera.GainRaw))
	{
		m_baslerCamera.GainRaw.SetValue(nGainValue);
		return true;
	}
	return false;
}

bool CBaslerCamera::SetWhiteBalance(int nBalanceValueR, int nBalanceValueG, int nBalanceValueB)
{
	return false;
}


bool CBaslerCamera::SetDelayTime(int nDelay)
{
	if (m_bCameraOk && IsWritable(m_baslerCamera.TriggerDelayAbs))
	{
		m_baslerCamera.TriggerDelayAbs.SetValue(nDelay);
		return true;
	}
	return false;
}


bool CBaslerCamera::Start()
{
	if (m_baslerCamera.IsPylonDeviceAttached())// If the camera has been found.
	{
		if (!m_bCameraOk)
		{
			m_bCameraOk = OpenCamera();
		}
		try
		{
			m_baslerCamera.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);//图像被处理按照他们的到来顺序。
			return true;
		}
		catch (GenICam::GenericException& e)
		{
			string strError = e.GetDescription();
			//TODO: Write this error message into log file.
			return false;
		}
	}
	return false;
}


bool CBaslerCamera::Pause()
{
	if (m_baslerCamera.IsGrabbing())
	{
		m_baslerCamera.StopGrabbing();
	}
	return true;
}


bool CBaslerCamera::IsMainCamera()
{
	return m_cameraSetting.m_bMainCamera;
}


bool CBaslerCamera::IsCameraOk()
{
	return m_bCameraOk;
}

bool CBaslerCamera::SendTriggerSignal(bool bIndependentGrab)
{
	if (!m_bCameraOk)
	{
		return false;
	}
	if (m_baslerCamera.IsGrabbing())
	{
		//m_baslerCamera.ExecuteSoftwareTrigger();
		m_baslerCamera.TriggerSoftware.Execute();
	}
	return true;
}

void CBaslerCamera::Attach2ImageProcessor(CPatternProcessor * ptrProcessor)
{
	m_ptrPatternProcessor = ptrProcessor;
}


void CBaslerCamera::Reset()
{

}

wstring CBaslerCamera::GetCameraName()
{
	return m_strCameraName;
}

void CBaslerCamera::SetCameraName(wstring strCameraName)
{
	m_strCameraName = strCameraName;
}



