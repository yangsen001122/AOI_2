#include "stdafx.h"
#include "CameraBase.h"



CCameraBase::CCameraBase()
	:m_bCameraOk(false),
	m_bMainCamera(false),
	m_bIndependentGrab(false),
	m_ptrPatternProcessor(nullptr),
	m_nImgCount(0)
{
}


CCameraBase::~CCameraBase()
{
}


bool CCameraBase::IsMainCamera()
{
	return m_bMainCamera;
}


bool CCameraBase::IsCameraOk()
{
	return m_bCameraOk;
}
