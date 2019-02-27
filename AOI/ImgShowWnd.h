#pragma once
#include"CvvImage.h"
#include <vector>
#include <string>
#include "utils.h"
#include "TypeDef.h"

class CROIBase;
class ImageShowWndListener;

class CImgShowWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgShowWnd)
public:
	CImgShowWnd();
	virtual ~CImgShowWnd();

	//回调
	void SetListener(ImageShowWndListener*pListener);
	void DeleteListener(ImageShowWndListener*pListener);
	void OnROIChange(eROI_TYPE eType, cv::Rect2f rect);
	//接口
	void GetImage(cv::Mat&mat);
	void GetROIAll(std::vector<IMG_ROI_INFO>&vROIAllInfo);
	void AddSelectROI(eROI_TYPE eType);

	void DeleteSelectROI();
	void DeleteAllROI();
	void DeleteAllLine();
	void DeleteAllText();
	void DeleteAllCross();
	void DeleteAllCircle();
	void DeleteAllEllipse();
	void DeleteAllRectangle();
	void DeleteAllPolygions();
	void DeleteAllPoint();
	void EnableROISelect(bool bEnableROISel);
	void SetROIMouseOnColor(COLORREF color);
	void EnableZoom(bool bZoom);
	void EnableDrag(bool bDrag);
	bool LoadImageData(IplImage *pImage);
	bool LoadImageDataReset(IplImage *pImage, eCAMERA_IMAGE_TYPE eType);
	bool LoadImage(CString strFile);
	bool SaveImage(CString strFile);
	bool ClearImage();
	bool CheckImageLoad();
	void SetBkColor(COLORREF color);
	COLORREF GetBkColor();
	void ShowText(CString strText, float x, float y, wstring txtColorName);
	void ShowLine(float fStartX, float fStartY, float fEndX, float fEndY, wstring lineColorName, int lineWide);
	void ShowCross(CString strText, float x, float y, float dRadius, wstring lineColorName, int lineWide);
	void ShowCircle(float fCenterX, float fCenterY, float dRadius, wstring lineColorName, int lineWide);
	void ShowEllipse(float fCenterX, float fCenterY, double dAngle, double dRadius1, double dRadius2, wstring lineColorName, int lineWide);
	void ShowRectangle(cv::Point2f point1, cv::Point2f point2, wstring lineColorName, int lineWide);
	void ShowRectangle(float x, float y, float length1, float length2, wstring lineColorName, int lineWide);
	void ShowPolygons(vector<cv::Point2f> dictPos, wstring lineColorName, int lineWide);
	void ShowPoint(cv::Point2f point, wstring lineColorName, int lineWide);
	int  GetROINumber();
	cv::Point2f GetWndCenterImgPos();
	double GetScale();
	void UnLockCriticalShow();
	bool SaveBitmapToFile(const CBitmap& bitmap, CString fileName);
	void GetSaveBitmap(CBitmap& bitmap);
	
	void SnapShot(wstring strSnapName);
	void DrawROI();
	void CropROI();
	void SelectROI();
	void EnableROIDrag(bool bROIDrag);
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC*pDC);
	virtual void OnDrawROI(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);//未透传给ROI！！！

	bool CalculateZoomPointInImg(const CPoint&point);
	void ResetData();
	void CalculateScale(double& dScale);
	void GetLocalXYScale(const cv::Point2f&point,int& x,int& y);
	void CheckSelectROI(const CPoint& point);
	void GetROIRectangle(IMG_ROI_INFO& imgROIInfo, CROIBase*pROI);
protected:
	CBitmap          m_compatibleBitMap;
	CCriticalSection m_criticalShowData;
	CvvImage         m_drawImage;
	COLORREF         m_bkColor;
	cv::Point2d      m_imgZoomPoint;
	cv::Point2d      m_wndCenterPoint;
	CPoint           m_wndZoomPoint;
	CPoint           m_lBtnDownPoint;
	CPoint           m_checkClickPoint;

	std::vector<CROIBase*> m_dictSelectROI;
	std::vector<IMG_TEXT> m_dictImgText;
	std::vector<IMG_LINE> m_dictImgLine;
	std::vector<IMG_CIRCLE> m_dictImgCircle;
	std::vector<IMG_RECTANGLE> m_dictImgRectangle;
	std::vector<ImageShowWndListener*> m_dictListener;
	std::map<wstring, COLORREF>m_dictDrawColor;
	double           m_dScale;
	bool             m_blBtnDown;
	bool             m_bTrack;
	bool             m_bDrag;
	bool             m_bZoom;
	bool             m_bROIDrag;
	chronograph      m_timer;
};
extern CImgShowWnd   gImgShowWnd;

