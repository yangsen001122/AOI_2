#pragma once
#include "resource.h"   
#include "AOIAlgo_i.h"
#include <vector>
#include <string>
#include <tuple>
#include <map>
#include "HalconCpp.h"
#include "AlgoSpy.h"
#include "MainProcess.h"
#include "SubProcess.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace ATL;
using namespace HalconCpp;
using namespace cv;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ERROR....."
#endif

void MyHalconExceptionHandler(const HalconCpp::HException& except)
{
	throw except;
}

class ATL_NO_VTABLE CAOIAlgo :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAOIAlgo, &CLSID_AOIAlgo>,
	public IDispatchImpl<IAOIAlgo, &IID_IAOIAlgo, &LIBID_AOIAlgoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{      
public:
	CAOIAlgo():
		m_loggerAlgo(L"Algo"),
		m_isWriteLog(1),
		m_imageType(2),
		m_isSaveInternalImage(0),
		m_errorNum(0),
		m_strImgFolder(""),
		m_nCalculateMainFlag(0),
		m_nRotateAngle(0),
		m_nResultOK(2),
		m_nChangeKey(0),
		m_dBGValue(-1.0)
	{

	}

DECLARE_REGISTRY_RESOURCEID(IDR_MURAALGO1)
BEGIN_COM_MAP(CAOIAlgo)
	COM_INTERFACE_ENTRY(IAOIAlgo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		
	}
	//Halcon与Opencv切换参数//
private:
	Mat m_CtrImg;
	Mat m_MainResultImg;
	Mat m_SubResultImg;
	LONG m_nChangeKey;//0:halcon;1:opencv
	MainProcess m_MainProcess;
	SubProcess m_SubProcess;
	Mat m_SrcMat;
	Mat m_MainWImg;
	Mat m_SubWImg;
	Mat m_resultMat;
private:
	//Input Img
	HObject  m_crtImg;
	//Output Img 
	HObject m_ResultImg;
	//Input MainImg and SubImg
	HObject m_MainWilteImg;
	HObject m_SubWilteImg;
	//MainMatrix and SubMatrix
	HTuple  m_HTMatrix;
	//if recalculate main matrix again -0:no;1:yes
	int   m_nCalculateMainFlag;
	//subMatrix rotate angle
	int   m_nRotateAngle;
	LONG  m_nResultOK;//2:OK;1:NG;
	double m_dBGValue;
	// 异常
	CAlgoSpy  m_algoSpy;
	// 日志
	CSystemLogger m_loggerAlgo;
	// 自动生成存图文件夹
	string m_strImgFolder;
	// 其他参数
	int     m_isWriteLog;
	int     m_imageType;
	int     m_isSaveInternalImage;
	int     m_errorNum;
	//
private:
	void    SubMatrix2D(HObject ho_ImageSource, HObject *ho_ImageTrans, HTuple hv_Thresh, HTuple hv_SmoothCont, HTuple hv_MaxLineDist1, HTuple hv_MaxLineDist2, HTuple hv_newWidth, HTuple hv_newHeight, HTuple hv_Delta);
	void    FindStraightEdge(HObject ho_Image, HTuple hv_Row, HTuple hv_Col, HTuple hv_Len1,
		HTuple hv_Len2, HTuple hv_Phi, HTuple hv_NumEdges, HTuple *hv_RowEdges, HTuple *hv_ColEdges,
		HTuple *hv_Row1, HTuple *hv_Col1, HTuple *hv_Row2, HTuple *hv_Col2);
	void    MainMatrix2D(HObject ho_ImageSrc, HTuple hv_ThParam, HTuple hv_MaskSizeParam,
		HTuple hv_w, HTuple hv_h, HTuple *hv_HomMat2D);
	void    MainNormalizeSize(HObject ho_ImageSrc, HObject *ho_ImageDst, HTuple hv_Hom2DMat,
		HTuple hv_Width, HTuple hv_Height);
	void    SubNormalizeSize(HObject ho_ImageSrc, HObject *ho_ImageDst, HTuple hv_RotateScale);
	void    MainEstamateBackGround(HObject ho_ImageSrc, HTuple hv_Model, HTuple *hv_GrayValue);

	// 增加自动白平衡算法
private:
	void Calc_Sign(HTuple hv_Number, HTuple *hv_sign);
	void Calc_WhiteBalance(HObject ho_Image, HTuple *hv_RGain, HTuple *hv_GGain, HTuple *hv_BGain);
	void Convert_Image_to_Tuple(HObject ho_Image, HTuple *hv_tupleImage);
	void Convert_Image_to_YCbCr(HTuple hv_Rorg, HTuple hv_Gorg, HTuple hv_Borg, HTuple *hv_Lu, HTuple *hv_Cb, HTuple *hv_Cr);

public:
	//H
	STDMETHOD(SetCurrentImage)(LONGLONG* pImg);
	STDMETHOD(GetResultImg)(LONGLONG* pRgn,LONG* nResultOK);
	STDMETHOD(GetMainMatrix2D)(BYTE rgnID, VARIANT* ThVaule, VARIANT* MaskSize, VARIANT* MapWSize, VARIANT* MapHSize);
	STDMETHOD(GetMainMatrix2DHelp)(BSTR* pHelpStr);
	STDMETHOD(GetSubMatrix2D)(BYTE RgnID, VARIANT* ThParam, VARIANT* ShParam, VARIANT* MaxDisParam, VARIANT* MaxDisParam2, VARIANT* LhParam, VARIANT* WhParam, VARIANT* DaParam);
	STDMETHOD(GetSubMatrix2DHelp)(BSTR* pHelpStr);
	//C
public:
	STDMETHOD(SetCalculateMainMatrixFlag)(BYTE nFlag);
	STDMETHOD(SetSubMatrixRotateFlag)(LONG nRotateAngle);
	STDMETHOD(VAGetMainMatrix2D)(BYTE rgnID, VARIANT* MaskSize, VARIANT* MeasureThs, VARIANT* MapWSize, VARIANT* MapHSize);
	STDMETHOD(VAGetMainMatrix2DHelp)(BSTR* pHelpStr);
	STDMETHOD(VAGetSubMatrix2D)(BYTE rgnID, VARIANT* MaskSize, VARIANT* LineMinLength, VARIANT* MapWSize, VARIANT* MapHSize);
	STDMETHOD(VAGetSubMatrix2DHelp)(BSTR* pHelpStr);
	STDMETHOD(VAGetResultImage)(LONGLONG* pImg);
	STDMETHOD(VASetInputImage)(LONGLONG* pImg);
	STDMETHOD(VACheckResultImage)(LONG* nResultOK);
	STDMETHOD(GetBackGroundValue)(DOUBLE* dBgValue);
};
OBJECT_ENTRY_AUTO(__uuidof(AOIAlgo), CAOIAlgo)

