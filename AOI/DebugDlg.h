#pragma once
#include <vector>
#include <assert.h>
#include <unordered_map>
#include "LabelCtrl.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "WindowbackgroundBrushHelper.h"
#include "ImgShowWndListener.h"
using namespace std;

class CDemuraStation;
class CPatternProcessor;


const int ID_PATTERN_WIDGET = 30001;

class CDebugDlg : public CDialogEx,public ImageShowWndListener
{
	DECLARE_DYNAMIC(CDebugDlg)

public:
	CDebugDlg(CWnd* pParent = NULL);
	virtual ~CDebugDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEBUG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnMouseMove(cv::Point2f pt);//鼠标移动时获取当前光标在图像中的位置
	virtual void OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo) ;//选择框大小发生改变,用于获得ROI信息
	DECLARE_MESSAGE_MAP()

private:
	void DisplayBackgroundImage(CPaintDC& dc);
	void CreateFunctionButton(CRect& rcClient);
	void CreateLogo(CRect& rcClient);
	void CreateImageWindow(CRect& rcClient,CImgShowWnd* &cImgWnd);
	void CreateSwitchButton(CRect& rcClient);
	void CreateAlgoList(CRect& rcClient);
	void CreatePatternIndexPanel(CRect& rcClient);
	void CreateSettingButtons(CRect& rcClient);
	void RetrieveInternalData(int nCameraIndex);
	void AddFunction2List();
	bool CheckImageIndex(int nIndex);
	void UpdateIndexPanel(int nIndex);
	void ProcessCurrentImage();
	void DeleteAllSettingWidget();
	void CreateAlgoParamItems();
	void AdjustParameterValue(bool bIncrease);
	void AdjustCameraParameterValue(bool bIncrease);
	void AdjustLightParameterValue(bool bIncrease);
	void AdjustMotionParameterValue(bool bIncrease);
	void DisplayMessage(CString sMsg, bool bStatus);
	void DisplayAxisPosition(int nFinalPosition);
	void ApplyNewAlgoParam();
	void ResetInternalData();
	void UpdateParamOfCurrentFunction();
	void InitializeActionGroupWidget();
	void CreateCameraParamItems();
	void CreateLightParamItems();
	void CreateMotionParamItems();
	void ShowLastWidget(SETTING_TYPE tLastType);
	void CreateAutoTestingWidget();
	void ShowAutoTestingWidget(bool bShow);
	void OnStnClickedPatternWidgetByPicture(int nPatternIndex);
	void OnBnClickedRadioStationByPicture(int nStationIndex);
	void OnCbnSelchangeComboPatternActionByPicture(int nActionGpIndex);
	void EnableAllWidget(bool bEnable);
	void OneCirlce();
	void CloseDebugDlg();
	bool NeedProcessTargetPicture();
	void SetInitialState();
	void SetDlgShowListener(ImageShowWndListener* pListener);
	void DeleteDlgShowListener();
	void OnSaveIndependentImg();
	void DebugGetSingleImg();
	wstring  MapPatternName(int nPatternIndex);
private:
	CBitmap                                 m_bgImg; // background image
	WindowBackgroundBrushHelper             m_bgBrushHelper;
	CDemuraStation*							m_ptrStation;
	CPatternProcessor*						m_ptrProcessor;
	int										m_nPatternCount;

    std::map<int, wstring>                  m_dictCameraIndex_Name;
	std::vector<SERVO_AXIS_SETTING>         m_diceCrtServoAxisSetting;

	std::map<int, FRAME_PATTERN>            m_dictProjectPattern;
	std::map<int, CAMERA_FRAME_PATTERN>*    m_ptrDictPatternForCrtCamera;

	CRect m_rcClient;
	// Control Logic
	int										m_nOneCircleID;
	int										m_nCameraIndex;
	wstring									m_wstrCurtProjectName;
	wstring									m_wstrCrtCameraName;
	int										m_nCrtPatternIndex;
	int										m_nCrtActionGpIndex;
	int										m_nCrtAlgoIndex;
	int										m_nCrtMainCameraHeight;
	SETTING_TYPE							m_settingType;
	CAMERA_FRAME_PATTERN*					m_ptrCrtFramePattern;
	SNAP_ACTION*							m_ptrCrtActionGroup;
	std::vector<FUNC_INFO>*					m_ptrCrtFuncArray;
	FUNC_INFO*								m_ptrCrtSelectedFunc;
	int										m_nBtnSize;
	int										m_nGap;
	int										m_nTitleLineHeight;
	int										m_nYPosLine0;
	int										m_nYPosLine1;
	int										m_nXPosLine2;
	int                                     m_nUnderLine;
	int										m_nWndWidth;
	int										m_nWndHeight;
	CImgShowWnd*                            m_cImgWndDebug;
	CRect									m_rcImgWnd;
	CListCtrl								m_listAlgo;
	CRect									m_rcList;
	CComboBox								m_comboPatternAction;
	CLabelCtrl								m_btnSaveSinglePic;
	CLabelCtrl								m_btnSaveMultiplePic;
	CLabelCtrl								m_btnOneCircle;
	CLabelCtrl								m_btnMultipleProcess;
	CLabelCtrl								m_btnAutoSemiauto;
	bool									m_bIfSemiauto;
	CLabelCtrl								m_btnClose;
	CLabelCtrl                              m_lblLogo;
	//Select Debug
	BOOL									m_bEnableOpenFolder;
	bool									m_bLocked;
	bool									m_bEnableMultipleProcess;
	bool									m_bBatchMode;
	bool									m_bShowResultImg;
	CLabelCtrl								m_btnImageFolder;
	CLabelCtrl								m_btnFirst;
	CLabelCtrl								m_btnPrevoius;

	COLORREF								m_fgColor;
	COLORREF								m_bgColor;

	CLabelCtrl								m_lblIndex;
	CLabelCtrl								m_btnNext;
	CLabelCtrl								m_btnLast;
	CLabelCtrl								m_btnDispMode; // Displaying modde
	CLabelCtrl								m_btnLock;

	CLabelCtrl								m_labelScale;
	CLabelCtrl								m_btnAlgorithm;
	CRect									m_rcAlgoBtn;
	CLabelCtrl								m_btnCamera;
	CRect									m_rcCameraBtn;
	CRect									m_rcPgBtn;

	CRect									m_rcNavigateZone;
	CRect									m_rcLineZone;
	CRect									m_rcParamBoard;
	CRect									m_rcCrtBtn;
	CRect									rcXY;
	CRect									m_rcScale;
	CFont									m_textFont;
	CFont									m_smallFont;
	CFont									m_bigFont;
	CFont									m_titleFont;
	CPen									m_penTitle;
	CPen									m_penXY;
    std::vector<IMG_PACKAGE>			    m_vImage;                       
    Mat										m_crtImage;
	int										m_crtImgIndex;
	int										m_nInternalImgIndex;
	int										m_nImgCount;
	bool									m_bDebugMode;
	//
	std::vector<CWnd*>						m_vParamNameCtrl;
	std::vector<CWnd*>						m_vParamEditCtrl;
	CWnd*									m_ptrCrtEditCtrl;
	CButton									m_btnDecrease;
	CButton									m_btnIncrease;
	CButton									m_btnApply;
	bool									m_bMsgStatus; // true:prompt message;false:warning/error message
	CStatic									m_msgPanel;
	std::vector<CLabelCtrl*>				m_vPatternWidgetPtr;
	std::vector<CRect>						m_vWidgetRect;
	CRect									m_rcCrtPatternWidget;
	// For camera get image
	BOOL									m_bCheckImgSave;
	BOOL                                    m_bIndependentGrab;
	int										m_nPatternCtrlHeight;
	boost::circular_buffer<bool>			m_cfParamUpdate;//判断当前Pttern是否有参数改动
	CLabelCtrl                              m_BtnLine;
	CLabelCtrl                              m_BtnRectangle;
	CLabelCtrl                              m_BtnRectRotate;
	CLabelCtrl                              m_BtnCircle;
	CLabelCtrl                              m_BtnEllipse;
	CLabelCtrl                              m_BtnPolygon;
	CLabelCtrl                              m_BtnClip;
	CLabelCtrl                              m_BtnClear;
	CLabelCtrl                              m_BtnSaveImg;
	CLabelCtrl                              m_BtnSnip;
	int                                     m_nROINum;
	ImageShowWndListener*                   m_pWndListener;
	int                                     m_nIndieExpoTime;
	int                                     m_nIndieGain;
	int                                     m_nIndieDelayTime;
	int                                     m_nIndieWhiteBalanceR;
	int                                     m_nIndieWhiteBalanceG;
	int                                     m_nIndieWhiteBalanceB;
	int                                     m_nOldIndieWhiteBalanceR;
	int                                     m_nOldIndieWhiteBalanceG;
	int                                     m_nOldIndieWhiteBalanceB;
	int                                     m_nOldIndieExpoTime;
	int                                     m_nOldIndieGain;
	vector<cv::Size2i>                      m_vImgSize;
public:
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedClose();

	afx_msg void OnStnClickedFolder();
	afx_msg void OnStnClickedSaveSinglePicture();
	afx_msg void OnStnClickedSaveMultiplePicture();
	afx_msg void OnStnClickedOneCircle();
	afx_msg void OnStnClickedMultipleProcess();
	afx_msg void OnStnClickedAutoSemiauto();
	afx_msg void OnStnClickedFirst();
	afx_msg void OnStnClickedPrevious();
	afx_msg void OnStnClickedNext();
	afx_msg void OnStnClickedLast();
	afx_msg void OnStnClickedLock();
	afx_msg void OnStnClickedAlgorithm();
	afx_msg void OnStnClickedCamera();
	afx_msg void OnStnClickedDrawRectangle();

	afx_msg void OnCbnSelchangeComboPatternAction();
	afx_msg void OnNMClickListAlgo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnSetfocusEditParameter();
	afx_msg void OnEnSetfocusCameraEditParameter();
	afx_msg void OnEnSetfocusLightEditParameter();
	afx_msg void OnEnSetfocusMotionEditParameter();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedStaticBtnDecrease();
	afx_msg void OnStnClickedStaticBtnIncrease();
	afx_msg void OnBnClickedCheckImageDebug();
	afx_msg void OnStnClickedPatternWidget();
	afx_msg void OnBnClickedCheckBoxOnOff();
	afx_msg void OnEnChangedEditbox();
	afx_msg void OnBnClickedCheckIndependentGrab();
	afx_msg void OnBnClickedButtonSoftwareTrigger();
	afx_msg void OnBnClickedCheckSaveImg();
    void OnBnClickedRadioCamera(UINT nID);
	CStatic      m_StaticXY;
	CEdit        m_edt_scale;
	CEdit        m_edt_sInput;
	bool         m_bStarted;
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMDblclkListAlgo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnSetfocusEditScaleinput();
	afx_msg LRESULT OnSendNextImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStnRightClickedPatternWidget(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayImageOnDebugScreen(WPARAM wParam, LPARAM lParam);
	
};
