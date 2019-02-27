#include "stdafx.h"
#include "resource.h"
#include "AOIAlgo_i.h"
#include "dllmain.h"
#include <ppl.h>
#include <string.h>
#include <iostream>
#include "SystemLogger.h"
#include <concrt.h>
#include <concurrent_vector.h>
#include <vector>
#include <algorithm>

using namespace std;
using namespace Concurrency;
using namespace ATL;

#pragma region KEEP BELOW BLOCK UNTOUCHED

STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


STDAPI DllRegisterServer(void)
{
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)

		{
			ATL::AtlSetPerUserRegistration(true);

		}
	}

	if (bInstall)

	{
		hr = DllRegisterServer();
		if (FAILED(hr))

		{
			DllUnregisterServer();

		}
	}
	else
	{
		hr = DllUnregisterServer();
	}
	return hr;
}

#pragma endregion

#include "stdafx.h"
#include "AOIAlgo.h"


STDMETHODIMP CAOIAlgo::SetCurrentImage(LONGLONG* pImg)
{
	HObject* ptr = (HObject*)pImg;
	m_crtImg = *ptr;

	std::wostringstream woss;
	if (!m_crtImg.IsInitialized())

	{
		if (m_isWriteLog == 1)

		{
			woss << L"  OrigImage not exsited";
			m_loggerAlgo.QueueUpLogAndReport(woss.str());
			m_loggerAlgo.FlushLogCache();


		}
	}
	else

	{
		if (m_isWriteLog == 1)

		{
			woss << L"  The pattern begin";
			m_loggerAlgo.QueueUpLogAndReport(woss.str());
			m_loggerAlgo.FlushLogCache();


		}
	}
	return S_OK;
}

STDMETHODIMP CAOIAlgo::GetResultImg(LONGLONG* pRgn, LONG* bResultOk)
{
	HObject* pObj = (HObject*)pRgn;
	*pObj = m_ResultImg;
	LONG*bOk = bResultOk;
	*bOk = m_nResultOK;

	return S_OK;
}

STDMETHODIMP CAOIAlgo::GetMainMatrix2DHelp(BSTR* pHelpStr)
{
	CComBSTR strHelp("600$\
                      601;B$\
                      602;LP;0;255;2;21$\
                      603;LP;3;11;2;7$\
                      614;LP;0;10000;1;9600$\
                      615;LP;0;10000;1;5760");

	HRESULT hr = strHelp.CopyTo(pHelpStr);
	if (FAILED(hr))
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CAOIAlgo::GetMainMatrix2D(BYTE rgnID, VARIANT* ThVaule, VARIANT* MaskSize, VARIANT* MapWSize, VARIANT* MapHSize)
{
	// 重新计算仿摄变换矩阵
	if (m_nCalculateMainFlag == 1)

		m_HTMatrix.Clear();


	HTuple iThValue, iMaskSizeValue;
	HTuple iMapWSizeValue, iMapHSizeValue;
	HTuple dBgValue;
	int imageID = rgnID;
	iThValue = int(ThVaule->fltVal);
	iMaskSizeValue = int(MaskSize->fltVal);
	iMapWSizeValue = int(MapWSize->fltVal);
	iMapHSizeValue = int(MapHSize->fltVal);

	if (m_MainWilteImg.IsInitialized())

	{
		m_MainWilteImg.Clear();

	}
	if (m_ResultImg.IsInitialized())

	{
		m_ResultImg.Clear();

	}

	m_nResultOK = 2;
	try

	{
		CopyImage(m_crtImg, &m_MainWilteImg);
		if (m_nCalculateMainFlag == 1)

		{
			MainEstamateBackGround(m_MainWilteImg, 0, &dBgValue);
			m_dBGValue = dBgValue[0].D();


			if (dBgValue >= 50 && dBgValue <= 150)

			{
				MainMatrix2D(m_MainWilteImg, iThValue, iMaskSizeValue, iMapWSizeValue, iMapHSizeValue, &m_HTMatrix);

			}
			else

			{
				m_nResultOK = 1;
				CopyImage(m_crtImg, &m_ResultImg);


			}
		}
		MainNormalizeSize(m_MainWilteImg, &m_ResultImg, m_HTMatrix, iMapWSizeValue, iMapHSizeValue);

	}
	catch (const HException& except)

	{
		if (m_isWriteLog == 1)

		{
			m_loggerAlgo.QueueUpLogAndReport(L"MainMatrix: except!");
			m_loggerAlgo.FlushLogCache();

		}
		m_nResultOK = 1;
		m_algoSpy.QueueUpLogMsg(except, m_crtImg);
		return S_FALSE;

	}
	return S_OK;
}

STDMETHODIMP CAOIAlgo::GetSubMatrix2DHelp(BSTR* pHelpStr)
{
	CComBSTR strHelp("604$\
                      605;B$\
                      606;LP;10;255;1;60$\
                      607;FP;0.1;15;0.2;7$\
                      608;LP;0;1000;1;46$\
                      609;LP;0;1000;1;4$\
                      610;LP;0;999999;1;1400$\
                      611;LP;0;99999;1;650$\
                      612;LP;0;99999;1;400");

	HRESULT hr = strHelp.CopyTo(pHelpStr);
	if (FAILED(hr))
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CAOIAlgo::GetSubMatrix2D(BYTE RgnID, VARIANT* ThParam, VARIANT* ShParam, VARIANT* MaxDisParam, VARIANT* MaxDisParam2, VARIANT* LhParam, VARIANT* WhParam, VARIANT* DaParam)
{
	int imageID = RgnID;
	HTuple dThParam = double(ThParam->fltVal);
	HTuple iShParam = double(ShParam->fltVal);
	HTuple iMaxDisParam = int(MaxDisParam->fltVal);
	HTuple iMaxDisParam2 = int(MaxDisParam2->fltVal);
	HTuple iLhParam = int(LhParam->fltVal);
	HTuple iWhParam = int(WhParam->fltVal);
	HTuple iDaParam = int(DaParam->fltVal);

	if (m_SubWilteImg.IsInitialized())
	{
		m_SubWilteImg.Clear();
	}
	if (m_ResultImg.IsInitialized())
	{
		m_ResultImg.Clear();
	}

	m_nResultOK = 2;
	try
	{
		CopyImage(m_crtImg, &m_SubWilteImg);

		// 先将图像旋转至正常方位
		HObject RotatedImage;
		RotateImage(m_SubWilteImg, &RotatedImage, m_nRotateAngle, "bilinear");
		SubMatrix2D(RotatedImage, &m_ResultImg, dThParam, iShParam, iMaxDisParam, iMaxDisParam2, iLhParam, iWhParam, iDaParam);
	}
	catch (const HException& except)
	{
		if (m_isWriteLog == 1)

		{
			m_loggerAlgo.QueueUpLogAndReport(L"SubMatrix: except!");
			m_loggerAlgo.FlushLogCache();

		}
		m_nResultOK = 1;
		m_algoSpy.QueueUpLogMsg(except, m_crtImg);

		return S_FALSE;
	}

	// 对图像进行自动白平衡
	try
	{
		HTuple hRGain, hGGain, hBGain;
		Calc_WhiteBalance(m_ResultImg, &hRGain, &hGGain, &hBGain);

		HTuple hWidth, hHeight;
		GetImageSize(m_ResultImg, &hWidth, &hHeight);

		HObject htmpImage;
		GenImageConst(&htmpImage, "byte", hWidth, hHeight);
		HObject hR, hG, hB;
		Decompose3(m_ResultImg, &hR, &hG, &hB);
		AddImage(hR, htmpImage, &hR, hRGain, 0);
		AddImage(hG, htmpImage, &hG, hGGain, 0);
		AddImage(hB, htmpImage, &hB, hBGain, 0);
		Compose3(hR, hG, hB, &m_ResultImg);
	}
	catch (HException& e)
	{
		m_algoSpy.QueueUpLogMsg(e, m_crtImg);
	}

	return S_OK;
}

void CAOIAlgo::FindStraightEdge(HObject ho_Image, HTuple hv_Row, HTuple hv_Col, HTuple hv_Len1,
	HTuple hv_Len2, HTuple hv_Phi, HTuple hv_NumEdges, HTuple *hv_RowEdges,
	HTuple *hv_ColEdges, HTuple *hv_Row1, HTuple *hv_Col1, HTuple *hv_Row2,
	HTuple *hv_Col2)
{
	HObject  ho_Contour;
	HTuple  hv_Step, hv_StartRow, hv_StartCol, hv_L1;
	HTuple  hv_L2, hv_Width, hv_Height, hv_MeasureHandle, hv_MRow;
	HTuple  hv_MCol, hv_I, hv_RowEdge, hv_ColEdge, hv_Amp, hv_Dist;
	HTuple  hv_Nr, hv_Nc;

	hv_Step = (2.0*hv_Len2) / hv_NumEdges;
	hv_StartRow = hv_Row - ((hv_Len2 + (hv_Step / 2.0))*((hv_Phi + (HTuple(90).TupleRad())).TupleSin()));
	hv_StartCol = hv_Col + ((hv_Len2 + (hv_Step / 2.0))*((hv_Phi + (HTuple(90).TupleRad())).TupleCos()));
	hv_L1 = hv_Len1;
	hv_L2 = hv_Step / 2;

	GetImageSize(ho_Image, &hv_Width, &hv_Height);
	GenMeasureRectangle2(hv_StartRow, hv_StartCol, hv_Phi, hv_L1, hv_L2, hv_Width, hv_Height, "bilinear", &hv_MeasureHandle);

	hv_MRow = hv_StartRow;
	hv_MCol = hv_StartCol;
	(*hv_RowEdges) = HTuple();
	(*hv_ColEdges) = HTuple();
	{
		HTuple end_val13 = hv_NumEdges - 1;
		HTuple step_val13 = 1;
		for (hv_I = 0; hv_I.Continue(end_val13, step_val13); hv_I += step_val13)

		{
			hv_MRow = hv_MRow - (hv_Step*((hv_Phi - (HTuple(90).TupleRad())).TupleSin()));
			hv_MCol += hv_Step*((hv_Phi - (HTuple(90).TupleRad())).TupleCos());
			TranslateMeasure(hv_MeasureHandle, hv_MRow, hv_MCol);
			MeasurePos(ho_Image, hv_MeasureHandle, 1, 20, "positive", "first", &hv_RowEdge, &hv_ColEdge, &hv_Amp, &hv_Dist);

			if (0 != ((hv_RowEdge.TupleLength()) == 1))

			{
				(*hv_RowEdges) = (*hv_RowEdges).TupleConcat(hv_RowEdge);
				(*hv_ColEdges) = (*hv_ColEdges).TupleConcat(hv_ColEdge);
			}
		}
	}
	if (0 != (((*hv_RowEdges).TupleLength()) < 3))
		return;

	GenContourPolygonXld(&ho_Contour, (*hv_RowEdges), (*hv_ColEdges));
	FitLineContourXld(ho_Contour, "drop", -1, 0, 6, 2, &(*hv_Row1), &(*hv_Col1), &(*hv_Row2), &(*hv_Col2), &hv_Nr, &hv_Nc, &hv_Dist);


	return;
}

void CAOIAlgo::MainMatrix2D(HObject ho_ImageSrc, HTuple hv_ThParam, HTuple hv_MaskSizeParam,
	HTuple hv_w, HTuple hv_h, HTuple *hv_HomMat2D)
{
	// Local iconic variables
	HObject  ho_ImageGray, ho_Region, ho_RegionClosing;
	HObject  ho_RegionFillUp, ho_ConnectedRegions, ho_SelectedRegions;
	HObject  ho_LeftCross, ho_LeftLine, ho_TopCross, ho_TopLine;
	HObject  ho_RightCross, ho_RightLine, ho_BottomCross, ho_BottomLine;
	HObject  ho_TLCross, ho_TRCross, ho_BRCross, ho_BLCross;

	// Local control variables
	HTuple  hv_Mean, hv_Dev, hv_Ra, hv_Rb, hv_Phi;
	HTuple  hv_Area, hv_Row, hv_Col, hv_Row1, hv_Col1, hv_Phi1;
	HTuple  hv_Length1, hv_Length2, hv_LeftRow, hv_LeftCol;
	HTuple  hv_LeftLen1, hv_LeftLen2, hv_LeftPhi, hv_RowEdges;
	HTuple  hv_ColEdges, hv_LeftRow1, hv_LeftCol1, hv_LeftRow2;
	HTuple  hv_LeftCol2, hv_TopRow, hv_TopCol, hv_TopLen1, hv_TopLen2;
	HTuple  hv_TopPhi, hv_TopRow1, hv_TopCol1, hv_TopRow2, hv_TopCol2;
	HTuple  hv_RightRow, hv_RightCol, hv_RightLen1, hv_RightLen2;
	HTuple  hv_RightPhi, hv_RightRow1, hv_RightCol1, hv_RightRow2;
	HTuple  hv_RightCol2, hv_BottomRow, hv_BottomCol, hv_BottomLen1;
	HTuple  hv_BottomLen2, hv_BottomPhi, hv_BottomRow1, hv_BottomCol1;
	HTuple  hv_BottomRow2, hv_BottomCol2, hv_TLRow, hv_TLCol;
	HTuple  hv_IsParalle, hv_TRRow, hv_TRCol, hv_BRRow, hv_BRCol;
	HTuple  hv_BLRow, hv_BLCol, hv_Covariance;

	CopyImage(ho_ImageSrc, &ho_ImageGray);
	Intensity(ho_ImageGray, ho_ImageGray, &hv_Mean, &hv_Dev);
	Threshold(ho_ImageGray, &ho_Region, hv_Mean + (0.1*hv_Dev), 255);
	ClosingRectangle1(ho_Region, &ho_RegionClosing, hv_MaskSizeParam, hv_MaskSizeParam);

	FillUp(ho_RegionClosing, &ho_RegionFillUp);
	ShapeTrans(ho_RegionFillUp, &ho_RegionFillUp, "convex");
	Connection(ho_RegionFillUp, &ho_ConnectedRegions);
	SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 0);
	EllipticAxis(ho_SelectedRegions, &hv_Ra, &hv_Rb, &hv_Phi);
	AreaCenter(ho_SelectedRegions, &hv_Area, &hv_Row, &hv_Col);
	SmallestRectangle2(ho_SelectedRegions, &hv_Row1, &hv_Col1, &hv_Phi1, &hv_Length1, &hv_Length2);

	//********************拟合Left直线*************************
	hv_LeftRow = hv_Row - (hv_Length1*((hv_Phi + (HTuple(180).TupleRad())).TupleSin()));
	hv_LeftCol = hv_Col + (hv_Length1*((hv_Phi + (HTuple(180).TupleRad())).TupleCos()));
	hv_LeftLen1 = 15;

	hv_LeftLen2 = hv_Length2;
	hv_LeftPhi = hv_Phi;
	FindStraightEdge(ho_ImageGray, hv_LeftRow, hv_LeftCol, hv_LeftLen1, hv_LeftLen2, hv_LeftPhi,
		60, &hv_RowEdges, &hv_ColEdges, &hv_LeftRow1, &hv_LeftCol1, &hv_LeftRow2, &hv_LeftCol2);


	if (0 != ((hv_RowEdges.TupleLength())>0))
	{
		GenCrossContourXld(&ho_LeftCross, hv_RowEdges, hv_ColEdges, 6, 0);
		GenContourPolygonXld(&ho_LeftLine, hv_LeftRow1.TupleConcat(hv_LeftRow2), hv_LeftCol1.TupleConcat(hv_LeftCol2));
	}

	//******************拟合Top直线******************
	hv_TopRow = hv_Row - (hv_Length2*((hv_Phi + (HTuple(90).TupleRad())).TupleSin()));
	hv_TopCol = hv_Col + (hv_Length2*((hv_Phi + (HTuple(90).TupleRad())).TupleCos()));
	hv_TopLen1 = 15;

	hv_TopLen2 = hv_Length1;
	hv_TopPhi = hv_Phi - (HTuple(90).TupleRad());

	FindStraightEdge(ho_ImageGray, hv_TopRow, hv_TopCol, hv_TopLen1, hv_TopLen2, hv_TopPhi,
		60, &hv_RowEdges, &hv_ColEdges, &hv_TopRow1, &hv_TopCol1, &hv_TopRow2, &hv_TopCol2);

	if (0 != ((hv_RowEdges.TupleLength()) > 0))
	{
		GenCrossContourXld(&ho_TopCross, hv_RowEdges, hv_ColEdges, 6, 0);
		GenContourPolygonXld(&ho_TopLine, hv_TopRow1.TupleConcat(hv_TopRow2), hv_TopCol1.TupleConcat(hv_TopCol2));
	}

	//*********************拟合Right直线****************
	hv_RightRow = hv_Row - (hv_Length1*(hv_Phi.TupleSin()));
	hv_RightCol = hv_Col + (hv_Length1*(hv_Phi.TupleCos()));
	hv_RightLen1 = 15;

	hv_RightLen2 = hv_Length2;
	hv_RightPhi = hv_Phi + (HTuple(180).TupleRad());

	FindStraightEdge(ho_ImageGray, hv_RightRow, hv_RightCol, hv_RightLen1, hv_RightLen2, hv_RightPhi,
		60, &hv_RowEdges, &hv_ColEdges, &hv_RightRow1, &hv_RightCol1, &hv_RightRow2, &hv_RightCol2);

	if (0 != ((hv_RowEdges.TupleLength()) > 0))
	{
		GenCrossContourXld(&ho_RightCross, hv_RowEdges, hv_ColEdges, 6, 0);
		GenContourPolygonXld(&ho_RightLine, hv_RightRow1.TupleConcat(hv_RightRow2), hv_RightCol1.TupleConcat(hv_RightCol2));

	}

	//***********************拟合Bottom直线*********************
	hv_BottomRow = hv_Row - (hv_Length2*((hv_Phi - (HTuple(90).TupleRad())).TupleSin()));
	hv_BottomCol = hv_Col + (hv_Length2*((hv_Phi - (HTuple(90).TupleRad())).TupleCos()));
	hv_BottomLen1 = 15;
	hv_BottomLen2 = hv_Length1;
	hv_BottomPhi = hv_Phi + (HTuple(90).TupleRad());

	FindStraightEdge(ho_ImageGray, hv_BottomRow, hv_BottomCol, hv_BottomLen1, hv_BottomLen2, hv_BottomPhi,
		60, &hv_RowEdges, &hv_ColEdges, &hv_BottomRow1, &hv_BottomCol1, &hv_BottomRow2, &hv_BottomCol2);

	if (0 != ((hv_RowEdges.TupleLength()) > 0))
	{
		GenCrossContourXld(&ho_BottomCross, hv_RowEdges, hv_ColEdges, 6, 0);
		GenContourPolygonXld(&ho_BottomLine, hv_BottomRow1.TupleConcat(hv_BottomRow2), hv_BottomCol1.TupleConcat(hv_BottomCol2));
	}

	//TopLeft Corner
	IntersectionLines(hv_TopRow1, hv_TopCol1, hv_TopRow2, hv_TopCol2, hv_LeftRow1,
		hv_LeftCol1, hv_LeftRow2, hv_LeftCol2, &hv_TLRow, &hv_TLCol, &hv_IsParalle);
	GenCrossContourXld(&ho_TLCross, hv_TLRow, hv_TLCol, 6, 0);

	//TopRight Corner
	IntersectionLines(hv_TopRow1, hv_TopCol1, hv_TopRow2, hv_TopCol2, hv_RightRow1,
		hv_RightCol1, hv_RightRow2, hv_RightCol2, &hv_TRRow, &hv_TRCol, &hv_IsParalle);
	GenCrossContourXld(&ho_TRCross, hv_TRRow, hv_TRCol, 6, 0);

	//BottomRight Corner
	IntersectionLines(hv_BottomRow1, hv_BottomCol1, hv_BottomRow2, hv_BottomCol2, hv_RightRow1,
		hv_RightCol1, hv_RightRow2, hv_RightCol2, &hv_BRRow, &hv_BRCol, &hv_IsParalle);
	GenCrossContourXld(&ho_BRCross, hv_BRRow, hv_BRCol, 6, 0);

	//BottomLeft Corner
	IntersectionLines(hv_BottomRow1, hv_BottomCol1, hv_BottomRow2, hv_BottomCol2, hv_LeftRow1,
		hv_LeftCol1, hv_LeftRow2, hv_LeftCol2, &hv_BLRow, &hv_BLCol, &hv_IsParalle);
	GenCrossContourXld(&ho_BLCross, hv_BLRow, hv_BLCol, 6, 0);

	//lefttop - righttop - rightbottom - leftbottom
	VectorToProjHomMat2d(((((hv_TLRow + 0.5).TupleConcat(hv_TRRow + 0.5)).TupleConcat(hv_BRRow - 0.5)).TupleConcat(hv_BLRow - 0.5)) + 0.5,
		((((hv_TLCol + 0.5).TupleConcat(hv_TRCol - 0.5)).TupleConcat(hv_BRCol - 0.5)).TupleConcat(hv_BLCol + 0.5)) + 0.5,
		(((HTuple(0).Append(0)).TupleConcat(hv_h - 1)).TupleConcat(hv_h - 1)) + 0.5, (((HTuple(0).TupleConcat(hv_w - 1)).TupleConcat(hv_w - 1)).TupleConcat(0)) + 0.5,
		"normalized_dlt", HTuple(), HTuple(), HTuple(), HTuple(), HTuple(), HTuple(),
		&(*hv_HomMat2D), &hv_Covariance);

	return;
}

void CAOIAlgo::MainEstamateBackGround(HObject ho_ImageSrc, HTuple hv_Model, HTuple *hv_GrayValue)
{
	// Local iconic variables
	HObject  ho_ImageGray, ho_Region, ho_RegionClosing;
	HObject  ho_RegionFillUp, ho_ConnectedRegions, ho_SelectedRegions;
	HObject  ho_reduceImg, ho_cropImage, ho_PartImage, ho_PartCircle;

	// Local control variables
	HTuple  hv_MaskSizeParam, hv_Size, hv_Mean, hv_Dev;
	HTuple  hv_Width, hv_Height, hv_PartArea, hv_PartRol, hv_PartCol;
	HTuple  hv_TupleR, hv_Tem, hv_DValue, hv_Max, hv_Min, hv_IndexMax;
	HTuple  hv_TupleRemove, hv_IndexMin, hv_TupleResult, hv_tupleDeviation;
	HTuple  hv_tupleMean;

	// Local control variables
	hv_MaskSizeParam = 3;
	hv_Size = 3;
	CopyImage(ho_ImageSrc, &ho_ImageGray);
	Intensity(ho_ImageGray, ho_ImageGray, &hv_Mean, &hv_Dev);
	Threshold(ho_ImageGray, &ho_Region, hv_Mean + (0.1*hv_Dev), 255);
	ClosingRectangle1(ho_Region, &ho_RegionClosing, hv_MaskSizeParam, hv_MaskSizeParam);

	FillUp(ho_RegionClosing, &ho_RegionFillUp);
	ShapeTrans(ho_RegionFillUp, &ho_RegionFillUp, "rectangle1");
	Connection(ho_RegionFillUp, &ho_ConnectedRegions);
	SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 0);

	// Local control variables
	ReduceDomain(ho_ImageSrc, ho_SelectedRegions, &ho_reduceImg);
	CropDomain(ho_reduceImg, &ho_cropImage);
	GetImageSize(ho_cropImage, &hv_Width, &hv_Height);
	PartitionRectangle(ho_cropImage, &ho_PartImage, hv_Width / hv_Size, hv_Height / hv_Size);
	AreaCenter(ho_PartImage, &hv_PartArea, &hv_PartRol, &hv_PartCol);
	TupleGenConst(hv_Size*hv_Size, (hv_Height / hv_Size)*0.25, &hv_TupleR);
	GenCircle(&ho_PartCircle, hv_PartRol, hv_PartCol, hv_TupleR);

	// Local control variables
	hv_Tem = HTuple();
	Intensity(ho_PartCircle, ho_ImageSrc, &hv_Tem, &hv_DValue);
	TupleMax(hv_Tem, &hv_Max);
	TupleMin(hv_Tem, &hv_Min);
	TupleFind(hv_Tem, hv_Max, &hv_IndexMax);
	TupleRemove(hv_Tem, hv_IndexMax, &hv_TupleRemove);
	TupleFind(hv_TupleRemove, hv_Min, &hv_IndexMin);
	TupleRemove(hv_TupleRemove, hv_IndexMin, &hv_TupleResult);
	TupleDeviation(hv_TupleResult, &hv_tupleDeviation);
	TupleMean(hv_TupleResult, &hv_tupleMean);
	if (0 != (hv_Model == 0))
	{
		(*hv_GrayValue) = hv_tupleMean - (0.1*hv_tupleDeviation);
	}
	else
	{
		Intensity(ho_cropImage, ho_SelectedRegions, &(*hv_GrayValue), &hv_DValue);
	}
	return;
}

void CAOIAlgo::SubMatrix2D(HObject ho_ImageSource, HObject *ho_ImageTrans, HTuple hv_Thresh, HTuple hv_SmoothCont, HTuple hv_MaxLineDist1, HTuple hv_MaxLineDist2, HTuple hv_newWidth, HTuple hv_newHeight, HTuple hv_Delta)
{
	// Local iconic variables
	HObject  ho_GrayImage, ho_Region, ho_ConnectedRegions;
	HObject  ho_SelectedRegions, ho_Contours, ho_ContoursSplit;
	HObject  ho_RegressContours, ho_UnionContours, ho_SelectedContours;
	HObject  ho_FitContours, ho_ObjectSelected, ho_RegionLines;

	// Local control variables
	HTuple  hv_Selection, hv_Width, hv_Height, hv_UsedThreshold;
	HTuple  hv_Area, hv_Row, hv_Column, hv_Number, hv_Idx, hv_Length;
	HTuple  hv_Indices, hv_Index, hv_RowBegin, hv_ColBegin;
	HTuple  hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist;
	HTuple  hv_up, hv_down, hv_left, hv_right;
	HTuple  hv_corRow1, hv_corCol1, hv_IsOverlapping, hv_corRow2;
	HTuple  hv_corCol2, hv_corRow3, hv_corCol3, hv_corRow4;
	HTuple  hv_corCol4, hv_HomMat2D;
	HTuple  hv_Covariance;

	Rgb1ToGray(ho_ImageSource, &ho_GrayImage);
	GetImageSize(ho_GrayImage, &hv_Width, &hv_Height);

	//** 获取面板区域
	BinaryThreshold(ho_GrayImage, &ho_Region, "max_separability", "light", &hv_UsedThreshold);
	Threshold(ho_GrayImage, &ho_Region, hv_UsedThreshold / 2, 255);
	Connection(ho_Region, &ho_ConnectedRegions);
	SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 70);
	AreaCenter(ho_SelectedRegions, &hv_Area, &hv_Row, &hv_Column);

	//** 获取四条边轮廓
	GenContourRegionXld(ho_SelectedRegions, &ho_Contours, "border");
	SegmentContoursXld(ho_Contours, &ho_ContoursSplit, "lines", hv_SmoothCont, hv_MaxLineDist1, hv_MaxLineDist2);
	RegressContoursXld(ho_ContoursSplit, &ho_RegressContours, "no", 1);
	UnionCollinearContoursXld(ho_RegressContours, &ho_UnionContours, 1000, 20, 20, 0.2, "attr_keep");
	SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 400, 400000, -0.5, 0.5);
	CountObj(ho_SelectedContours, &hv_Number);
	hv_Idx = HTuple();
	if (0 != (hv_Number<4))
	{
		CopyImage(ho_ImageSource, ho_ImageTrans);
		return;
	}
	if (0 != (hv_Number>4))
	{
		LengthXld(ho_SelectedContours, &hv_Length);
		TupleSortIndex(hv_Length, &hv_Indices);
		hv_Idx.Clear();
		hv_Idx.Append(HTuple(hv_Indices[0]));
		hv_Idx.Append(HTuple(hv_Indices[1]));
		hv_Idx.Append(HTuple(hv_Indices[2]));
		hv_Idx.Append(HTuple(hv_Indices[3]));
	}
	if (0 != (hv_Number == 4))
	{
		hv_Idx.Clear();
		hv_Idx[0] = 0;
		hv_Idx[1] = 1;
		hv_Idx[2] = 2;
		hv_Idx[3] = 3;
	}
	GenEmptyObj(&ho_FitContours);
	{
		HTuple end_val35 = hv_Idx.TupleLength();
		HTuple step_val35 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val35, step_val35); hv_Index += step_val35)
		{
			SelectObj(ho_SelectedContours, &ho_ObjectSelected, hv_Index);
			ConcatObj(ho_FitContours, ho_ObjectSelected, &ho_FitContours);
		}
	}

	FitLineContourXld(ho_FitContours, "tukey", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin,
		&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
	GenRegionLine(&ho_RegionLines, hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd);
	hv_up = 0;
	hv_down = 0;
	hv_left = 0;
	hv_right = 0;
	{
		HTuple end_val47 = (hv_RowBegin.TupleLength()) - 1;
		HTuple step_val47 = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val47, step_val47); hv_Index += step_val47)
		{
			if (0 != (HTuple(HTuple(hv_RowBegin[hv_Index])<hv_Row).TupleAnd(HTuple(hv_RowEnd[hv_Index])<hv_Row)))
			{
				hv_up = hv_Index;
			}
			else if (0 != (HTuple(HTuple(hv_RowBegin[hv_Index])>hv_Row).TupleAnd(HTuple(hv_RowEnd[hv_Index])>hv_Row)))
			{
				hv_down = hv_Index;
			}
			else if (0 != (HTuple(HTuple(hv_ColBegin[hv_Index])<hv_Column).TupleAnd(HTuple(hv_ColEnd[hv_Index])<hv_Column)))
			{
				hv_left = hv_Index;
			}
			else if (0 != (HTuple(HTuple(hv_ColBegin[hv_Index])>hv_Column).TupleAnd(HTuple(hv_ColEnd[hv_Index])>hv_Column)))
			{
				hv_right = hv_Index;
			}
		}
	}

	//**lefttop
	IntersectionLines(HTuple(hv_RowBegin[hv_left]), HTuple(hv_ColBegin[hv_left]), HTuple(hv_RowEnd[hv_left]),
		HTuple(hv_ColEnd[hv_left]), HTuple(hv_RowBegin[hv_up]), HTuple(hv_ColBegin[hv_up]),
		HTuple(hv_RowEnd[hv_up]), HTuple(hv_ColEnd[hv_up]), &hv_corRow1, &hv_corCol1,
		&hv_IsOverlapping);
	IntersectionLines(HTuple(hv_RowBegin[hv_right]), HTuple(hv_ColBegin[hv_right]),
		HTuple(hv_RowEnd[hv_right]), HTuple(hv_ColEnd[hv_right]), HTuple(hv_RowBegin[hv_up]),
		HTuple(hv_ColBegin[hv_up]), HTuple(hv_RowEnd[hv_up]), HTuple(hv_ColEnd[hv_up]),
		&hv_corRow2, &hv_corCol2, &hv_IsOverlapping);
	IntersectionLines(HTuple(hv_RowBegin[hv_left]), HTuple(hv_ColBegin[hv_left]), HTuple(hv_RowEnd[hv_left]),
		HTuple(hv_ColEnd[hv_left]), HTuple(hv_RowBegin[hv_down]), HTuple(hv_ColBegin[hv_down]),
		HTuple(hv_RowEnd[hv_down]), HTuple(hv_ColEnd[hv_down]), &hv_corRow3, &hv_corCol3,
		&hv_IsOverlapping);
	IntersectionLines(HTuple(hv_RowBegin[hv_right]), HTuple(hv_ColBegin[hv_right]),
		HTuple(hv_RowEnd[hv_right]), HTuple(hv_ColEnd[hv_right]), HTuple(hv_RowBegin[hv_down]),
		HTuple(hv_ColBegin[hv_down]), HTuple(hv_RowEnd[hv_down]), HTuple(hv_ColEnd[hv_down]),
		&hv_corRow4, &hv_corCol4, &hv_IsOverlapping);

	VectorToProjHomMat2d(((hv_corRow1.TupleConcat(hv_corRow2)).TupleConcat(hv_corRow3)).TupleConcat(hv_corRow4),
		((hv_corCol1.TupleConcat(hv_corCol2)).TupleConcat(hv_corCol3)).TupleConcat(hv_corCol4),
		((HTuple(0).Append(0)).TupleConcat(hv_newHeight)).TupleConcat(hv_newHeight),
		((HTuple(0).TupleConcat(hv_newWidth)).TupleConcat(0)).TupleConcat(hv_newWidth),
		"normalized_dlt", HTuple(), HTuple(), HTuple(), HTuple(), HTuple(), HTuple(),
		&hv_HomMat2D, &hv_Covariance);

	ProjectiveTransImageSize(ho_ImageSource, ho_ImageTrans, hv_HomMat2D, "bilinear", hv_newWidth, hv_newHeight, "false");
}

void CAOIAlgo::MainNormalizeSize(HObject ho_ImageSrc, HObject *ho_ImageDst, HTuple hv_Hom2DMat,
	HTuple hv_Width, HTuple hv_Height)
{
	// Local iconic variables
	GenEmptyObj(&(*ho_ImageDst));
	ProjectiveTransImageSize(ho_ImageSrc, &(*ho_ImageDst), hv_Hom2DMat, "bilinear", hv_Width, hv_Height, "false");

	return;
}
void CAOIAlgo::SubNormalizeSize(HObject ho_ImageSrc, HObject *ho_ImageDst, HTuple hv_RotateScale)
{
	// Local iconic variables
	HObject  ho_Region, ho_RegionClosing, ho_RegionFillUp;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ImageReduce;
	HObject  ho_CropImg, ho_ImageZoom;
	HObject  ho_ImageRote;

	// Local control variables
	HTuple  hv_MaskSizeParam, hv_Mean, hv_Dev, hv_Row0;
	HTuple  hv_Col0, hv_Row1, hv_Col1, hv_Width, hv_Height;
	HTuple  hv_PLocationH, hv_PLocationW, hv_ScaleH, hv_ScaleW;

	hv_MaskSizeParam = 5;
	Intensity(ho_ImageSrc, ho_ImageSrc, &hv_Mean, &hv_Dev);
	Threshold(ho_ImageSrc, &ho_Region, hv_Mean + (0.1*hv_Dev), 255);
	ClosingRectangle1(ho_Region, &ho_RegionClosing, hv_MaskSizeParam, hv_MaskSizeParam);
	FillUp(ho_RegionClosing, &ho_RegionFillUp);
	Connection(ho_RegionFillUp, &ho_ConnectedRegions);
	SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 0);

	SmallestRectangle1(ho_SelectedRegions, &hv_Row0, &hv_Col0, &hv_Row1, &hv_Col1);
	Rectangle1Domain(ho_ImageSrc, &ho_ImageReduce, hv_Row0, hv_Col0, hv_Row1, hv_Col1);
	CropDomain(ho_ImageReduce, &ho_CropImg);

	RotateImage(ho_CropImg, &ho_ImageRote, hv_RotateScale, "bilinear");
	GetImageSize(ho_ImageRote, &hv_Width, &hv_Height);

	hv_PLocationH = 1200;
	hv_PLocationW = 2000;
	ZoomImageSize(ho_ImageRote, &(*ho_ImageDst), hv_PLocationW, hv_PLocationH, "bilinear");

	return;
}

STDMETHODIMP CAOIAlgo::SetCalculateMainMatrixFlag(BYTE nFlag)
{
	m_nCalculateMainFlag = nFlag;
	return S_OK;
}

STDMETHODIMP CAOIAlgo::SetSubMatrixRotateFlag(LONG nRotateAngle)
{
	m_nRotateAngle = nRotateAngle;
	return S_OK;
}

STDMETHODIMP CAOIAlgo::VAGetMainMatrix2DHelp(BSTR* pHelpStr)
{
	CComBSTR strHelp("616$\
                      617;B$\
                      618;LP;1;15;2;5$\
                      619;LP;0;255;2;20$\
                      620;LP;0;10000;1;9600$\
                      621;LP;0;10000;1;5760");

	HRESULT hr = strHelp.CopyTo(pHelpStr);
	if (FAILED(hr))
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CAOIAlgo::VAGetMainMatrix2D(BYTE rgnID, VARIANT* MaskSize, VARIANT* MeasureThs, VARIANT* MapWSize, VARIANT* MapHSize)
{
	// Add Code
	if (m_nCalculateMainFlag == 1)
	{
		Mat Matrix;
		m_MainProcess.GetMainMatrix2D(Matrix);
	}

	int iMaskSizeValue;
	int iMapWSizeValue;
	int iMapHSizeValue;
	float iMeasureThsValue;
	int imageID = rgnID;
	iMaskSizeValue = int(MaskSize->fltVal);
	iMeasureThsValue = float(MeasureThs->fltVal);
	iMapWSizeValue = int(MapWSize->fltVal);
	iMapHSizeValue = int(MapHSize->fltVal);

	m_nResultOK = 2;
	try
	{
		m_MainWImg = m_CtrImg.clone();
		if (m_nCalculateMainFlag == 1)

		{
			double dBgValue = m_MainProcess.EstimationBg(m_MainWImg);
			m_dBGValue = dBgValue;
			if (dBgValue >= 50 && dBgValue <= 130)

			{
				m_MainProcess.CalcPerspectiveMat(m_MainWImg, iMaskSizeValue, iMeasureThsValue, iMapWSizeValue, iMapHSizeValue);
				m_MainProcess.PerspectiveTransform(m_MainWImg, m_MainResultImg, iMapWSizeValue, iMapHSizeValue);

			}
			else

			{
				m_nResultOK = 1;


			}
		}
		m_MainProcess.PerspectiveTransform(m_MainWImg, m_MainResultImg, iMapWSizeValue, iMapHSizeValue);
		m_resultMat = m_MainResultImg.clone();
	}
	catch (const cv::Exception& except)
	{
		m_nResultOK = 1;
		if (m_isWriteLog == 1)

		{
			m_loggerAlgo.QueueUpLogAndReport(L"MainMatrix: Except!");
			m_loggerAlgo.FlushLogCache();

		}
		m_algoSpy.VQueueUpLogMsg(except, m_CtrImg);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CAOIAlgo::VAGetSubMatrix2DHelp(BSTR* pHelpStr)
{
	CComBSTR strHelp("622$\
                      623;B$\
                      624;LP;1;15;1;5$\
                      625;LP;3;1000;2;100$\
                      626;LP;1;10000;1;2000$\
                      627;LP;1;10000;1;1200");

	HRESULT hr = strHelp.CopyTo(pHelpStr);
	if (FAILED(hr))
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CAOIAlgo::VAGetSubMatrix2D(BYTE rgnID, VARIANT* MaskSize, VARIANT* LineMinLength, VARIANT* MapWSize, VARIANT* MapHSize)
{
	// TODO: 在此添加实现代码
	int imageID = rgnID;
	int iMaskSize = int(MaskSize->fltVal);
	int iLineMinLength = int(LineMinLength->fltVal);
	int iMapWSize = int(MapWSize->fltVal);
	int iMapHSize = int(MapHSize->fltVal);

	m_nResultOK = 2;
	try
	{
		if (!m_SubWImg.empty())

		{
			m_SubWImg.zeros(m_CtrImg.size(), CV_8UC1);

		}
		if (!m_SubResultImg.empty())

		{
			m_SubResultImg.zeros(m_CtrImg.size(), CV_8UC1);

		}

		Mat m_RoteImg;
		m_SubWImg = m_CtrImg.clone();
		m_SubProcess.RotationImg(m_SubWImg, m_RoteImg, m_nRotateAngle);
		m_SubProcess.CalcPerspectiveMat(m_RoteImg, iMaskSize, iLineMinLength, iMapWSize, iMapHSize);
		m_SubProcess.PerspectiveTransform(m_RoteImg, m_SubResultImg, iMapWSize, iMapHSize);
		m_resultMat = m_SubResultImg.clone();
	}
	catch (cv::Exception& except)
	{
		m_nResultOK = 1;
		if (m_isWriteLog == 1)

		{
			m_loggerAlgo.QueueUpLogAndReport(L"SubMatrix: except!");
			m_loggerAlgo.FlushLogCache();

		}
		m_algoSpy.VQueueUpLogMsg(except, m_CtrImg);
		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CAOIAlgo::VAGetResultImage(LONGLONG* pImg)
{
	Mat* mt = (Mat*)pImg;
	*mt = m_resultMat;
	return S_OK;
}


STDMETHODIMP CAOIAlgo::VASetInputImage(LONGLONG* pImg)
{
	Mat* ptr = (Mat*)pImg;
	m_CtrImg = *ptr;
	std::wostringstream woss;
	if (m_CtrImg.empty())
	{
		if (m_isWriteLog == 1)

		{
			woss << L"  OrigImage not exsited";
			m_loggerAlgo.QueueUpLogAndReport(woss.str());
			m_loggerAlgo.FlushLogCache();

		}
	}
	else
	{
		if (m_isWriteLog == 1)

		{
			woss << L"  The pattern begin";
			m_loggerAlgo.QueueUpLogAndReport(woss.str());
			m_loggerAlgo.FlushLogCache();

		}
	}
	return S_OK;
}


STDMETHODIMP CAOIAlgo::VACheckResultImage(LONG* nResultOK)
{
	*nResultOK = m_nResultOK;


	return S_OK;
}


STDMETHODIMP CAOIAlgo::GetBackGroundValue(DOUBLE* dBgValue)
{
	*dBgValue = m_dBGValue;


	return S_OK;
}

void CAOIAlgo::Calc_Sign(HTuple hv_Number, HTuple *hv_sign)
{
	if (hv_Number < 0)
	{
		(*hv_sign) = -1;
	}
	else if (hv_Number == 0)
	{
		(*hv_sign) = 0;
	}
	else if (hv_Number > 0)
	{
		(*hv_sign) = 1;
	}
}

void CAOIAlgo::Convert_Image_to_Tuple(HObject ho_Image, HTuple *hv_tupleImage)
{
	// Local control variables
	HTuple  hv_Rows, hv_Columns;

	GetRegionPoints(ho_Image, &hv_Rows, &hv_Columns);
	GetGrayval(ho_Image, hv_Rows, hv_Columns, hv_tupleImage);
}

void CAOIAlgo::Convert_Image_to_YCbCr(HTuple hv_Rorg, HTuple hv_Gorg, HTuple hv_Borg, HTuple *hv_Lu, HTuple *hv_Cb, HTuple *hv_Cr)
{
	// Local control variables
	HTuple  hv_Prod1, hv_Prod2, hv_Prod3, hv_Sum1, hv_Sum2;

	(*hv_Lu) = HTuple();
	(*hv_Cb) = HTuple();
	(*hv_Cr) = HTuple();

	//Lu := 0.299*Rorg + 0.587*Gorg + 0.114*Borg + 16
	TupleMult(hv_Rorg, 0.299, &hv_Prod1);
	TupleMult(hv_Gorg, 0.587, &hv_Prod2);
	TupleMult(hv_Borg, 0.114, &hv_Prod3);
	TupleAdd(hv_Prod1, hv_Prod2, &hv_Sum1);
	TupleAdd(hv_Sum1, hv_Prod3, &hv_Sum2);
	TupleAdd(hv_Sum2, 16, hv_Lu);

	//Cb := -0.169*Rorg - 0.331*Gorg + 0.500*Borg + 128
	TupleMult(hv_Rorg, -0.169, &hv_Prod1);
	TupleMult(hv_Gorg, -0.331, &hv_Prod2);
	TupleMult(hv_Borg, 0.500, &hv_Prod3);
	TupleAdd(hv_Prod1, hv_Prod2, &hv_Sum1);
	TupleAdd(hv_Sum1, hv_Prod3, &hv_Sum2);
	TupleAdd(hv_Sum2, 128, hv_Cb);

	//Cr := 0.500*Rorg - 0.418*Gorg - 0.082*Borg + 128
	TupleMult(hv_Rorg, 0.500, &hv_Prod1);
	TupleMult(hv_Gorg, -0.418, &hv_Prod2);
	TupleMult(hv_Borg, -0.082, &hv_Prod3);
	TupleAdd(hv_Prod1, hv_Prod2, &hv_Sum1);
	TupleAdd(hv_Sum1, hv_Prod3, &hv_Sum2);
	TupleAdd(hv_Sum2, 128, hv_Cr);
}

void CAOIAlgo::Calc_WhiteBalance(HObject ho_Image, HTuple *hv_RGain, HTuple *hv_GGain, HTuple *hv_BGain)
{
	// Local iconic variables
	HObject  ho_ImageConverted, ho_R, ho_G, ho_B;

	// Local control variables
	HTuple  hv_Channels, hv_Width, hv_Height, hv_Rorg;
	HTuple  hv_Gorg, hv_Borg, hv_Lu, hv_Cb, hv_Cr, hv_Mb, hv_Mr;
	HTuple  hv_Db, hv_Dr, hv_sign, hv_b1, hv_b2, hv_b1_ref;
	HTuple  hv_b2_ref, hv_Less1, hv_Less2, hv_Lu_Index, hv_Lu_Index2;
	HTuple  hv_Ciny, hv_iy, hv_nn, hv_Ciny2, hv_mn, hv_Lu_Index3;
	HTuple  hv_Lu_Index4, hv_tst_rshp, hv_RProd, hv_GProd, hv_BProd;
	HTuple  hv_SubR, hv_SumG, hv_SumB, hv_Rav, hv_Gav, hv_Bav;

	ConvertImageType(ho_Image, &ho_ImageConverted, "real");
	CountChannels(ho_ImageConverted, &hv_Channels);
	if (0 != (hv_Channels != 3))
	{
		return;
	}
	GetImageSize(ho_ImageConverted, &hv_Width, &hv_Height);
	Decompose3(ho_ImageConverted, &ho_R, &ho_G, &ho_B);

	//** 转换为向量
	Convert_Image_to_Tuple(ho_R, &hv_Rorg);
	Convert_Image_to_Tuple(ho_G, &hv_Gorg);
	Convert_Image_to_Tuple(ho_B, &hv_Borg);

	//** 转换到YCbCr空间
	Convert_Image_to_YCbCr(hv_Rorg, hv_Gorg, hv_Borg, &hv_Lu, &hv_Cb, &hv_Cr);

	//** Calc Mean-Valve
	TupleSum(hv_Cb, &hv_Mb);
	TupleSum(hv_Cr, &hv_Mr);
	hv_Mb = hv_Mb / (hv_Width*hv_Height);
	hv_Mr = hv_Mr / (hv_Width*hv_Height);

	//**Calc ave asb diff
	TupleSum((hv_Cb - hv_Mb).TupleAbs(), &hv_Db);
	hv_Db = hv_Db / (hv_Width*hv_Height);
	TupleSum((hv_Cr - hv_Mr).TupleAbs(), &hv_Dr);
	hv_Dr = hv_Dr / (hv_Width*hv_Height);

	//**白点检测
	Calc_Sign(hv_Mb - 128, &hv_sign);
	hv_b1 = (hv_Cb - (hv_Mb + (hv_Db*hv_sign))).TupleAbs();
	Calc_Sign(hv_Mr - 128, &hv_sign);
	hv_b2 = ((hv_Cr - ((1.5*hv_Mr) + (hv_Dr*hv_sign))) + 64).TupleAbs();
	hv_b1_ref = 1.5 * hv_Db;
	hv_b2_ref = 1.5 * hv_Dr;

	TupleLessElem(hv_b1, hv_b1_ref, &hv_Less1);
	TupleLessElem(hv_b2, hv_b2_ref, &hv_Less2);
	TupleAnd(hv_Less1, hv_Less2, &hv_Lu_Index);
	TupleFind(hv_Lu_Index, 1, &hv_Lu_Index);
	TupleLength(hv_Lu_Index, &hv_Lu_Index2);
	if (hv_Lu_Index == -1)
	{
		TupleOr(hv_Less1, hv_Less2, &hv_Lu_Index);
		TupleFind(hv_Lu_Index, 1, &hv_Lu_Index);
		TupleLength(hv_Lu_Index, &hv_Lu_Index2);
	}

	TupleSelect(hv_Lu, hv_Lu_Index, &hv_Ciny);
	TupleSort(hv_Ciny, &hv_iy);
	TupleInverse(hv_iy, &hv_iy);
	hv_nn = (hv_Lu_Index2 / 10).TupleRound();
	TupleSelectRange(hv_iy, 0, hv_nn - 1, &hv_Ciny2);
	TupleMin(hv_Ciny2, &hv_mn);

	TupleGreaterEqualElem(hv_Lu, hv_mn, &hv_Lu_Index3);
	TupleFind(hv_Lu_Index3, 1, &hv_Lu_Index3);
	TupleLength(hv_Lu_Index3, &hv_Lu_Index4);

	TupleGenConst(hv_Width*hv_Height, 0, &hv_tst_rshp);
	TupleReplace(hv_tst_rshp, hv_Lu_Index3, 1, &hv_tst_rshp);

	//** 白点调整
	TupleMult(hv_Rorg, hv_tst_rshp, &hv_RProd);
	TupleMult(hv_Gorg, hv_tst_rshp, &hv_GProd);
	TupleMult(hv_Borg, hv_tst_rshp, &hv_BProd);
	TupleSum(hv_RProd, &hv_SubR);
	TupleSum(hv_GProd, &hv_SumG);
	TupleSum(hv_BProd, &hv_SumB);
	hv_Rav = hv_SubR / hv_Lu_Index4;
	hv_Gav = hv_SumG / hv_Lu_Index4;
	hv_Bav = hv_SumB / hv_Lu_Index4;

	// 取得各通道增益,G通道一般不做处理,只对R/B通道进行计算
	(*hv_RGain) = hv_Gav / hv_Rav;
	(*hv_GGain) = 1;
	(*hv_BGain) = hv_Gav / hv_Bav;
}