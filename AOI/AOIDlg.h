#pragma once
#include "TypeDef.h"
#include "LabelCtrl.h"
#include "DemuraStation.h"
#include <boost/circular_buffer.hpp>
#include "LoadingDlg.h"
#include "LogViewerDlg.h"
#include "DispResultInfoDlg.h"
#include "VieworksCamera.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ImgShowWndListener.h"
#include <boost/bimap.hpp>

const int BTN_LOGIN					= 10000;
const int BTN_SYS_CONFIG			= 10001;
const int BTN_SYS_UAC				= 10002;
const int BTN_HELP					= 10003;
const int BTN_LANGUAGE				= 10004;
const int BTN_ONOFF					= 10005;
const int IDC_COMBO_PROJECT			= 10006;
const int IDB_DEBUG					= 10007;
const int IDB_SPC					= 10008;
const int BTN_SHUTDOWN				= 10009;
const int IDC_CURRENT_TIME			= 10010;
const int IDB_LOG_VIEWER            = 10011;
const int TIMER_ID_DATE_TIME		= 90000;
const int TIMER_ID_COUNT_PER_HOUR   = 90004;
const int BTN_SZ                    = 72;

class CAOIDlg : public CDialogEx, public ImageShowWndListener
{
public:
	CAOIDlg(CWnd* pParent = NULL);	
    ~CAOIDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MURASMOOTHER_DIALOG };
#endif
	CLogViewerDlg* m_ptrLogDlg;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	virtual void OnMouseMove(cv::Point2f pt);//鼠标移动时获取当前光标在图像中的位置
	virtual void OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo);//选择框大小发生改变,用于获得ROI信息
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	void LoadBackgroundImage(CPaintDC& dc);
	void CreateDateTimeWidget(CRect& rcScreen);
	void SetCurrentDatetime();
	void CreateTitleWidget(CRect& rcScreen);
	void CreateSystemControlButtons(CRect& rcScreen);
	void CreateStatsDataDisplayingWidget(CRect& rcScreen);
	void CreateControlWidget(CRect& rcScreen);
	void CreateImageWindows(CRect& rcScreen);
	void CreateSubImageWindow(CRect& rcPos, CImgShowWnd* &cImgWnd);
	void CreateResultWidget(CRect& rcScreen);
	void CreateStaion();
	void ShutDownStation();
	void SetSystemStatus();
	void ControlStationCameras(bool bRun);
	void ListAllProject();
	void GrantSuperAdmin();
	void ResetCountValue();
	void InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID = -1, bool bSmallFont = false);
	void DeleteAllImageWindow();
	void SegmentResultImageRegion(CImgShowWnd* cImgWnd,int nWidth,int nHeight);
	int  JudgeDefectRegionNumber(map<string, string> dictPos);
	void CloseExternalProgram(CString strProgram);
private:
	#pragma region UI
	CLabelCtrl m_dlgTitle;
	CLabelCtrl m_labelLogo;
	CLabelCtrl m_btnLogon;
	CLabelCtrl m_btnSysConfig;
	CLabelCtrl m_btnUAC; // User Access Control
	CLabelCtrl m_btnHelp;
	CLabelCtrl m_btnLanguage;
	COLORREF  m_fgColor;       // foreground color;
	COLORREF  m_bgColor;       // Background color;

	CLabelCtrl m_labelBackground1;
	CRect      m_rcBackGround1;  
	CLabelCtrl m_labelTotalTxt;
	CLabelCtrl m_labelTotalValue;
	CLabelCtrl m_labelOkCountTxt;
	CLabelCtrl m_lableOkCountValue; // NG count and NG ratio?
	CLabelCtrl m_labelCrtTimingTxt;
	CLabelCtrl m_lableCrtTimingValue; // NG count and NG ratio?
	CLabelCtrl m_lableAvgTimingTxt;
	CLabelCtrl m_lableAvgTimingValue;
	CRect      m_rcBackGround2;
	CLabelCtrl m_lableBackground2;

	CLabelCtrl m_btnOnOff;
	CComboBox  m_comboProject;
	CButton    m_btnDebug;
	CButton    m_btnSpc;
	CListCtrl  m_logList;//log list 
	CListCtrl  m_logDetectResult;//detect log list
	// Image windows
	CImgShowWnd* m_cImgWndMainLeft;
	CImgShowWnd* m_cImgWndMainRight;
	CImgShowWnd* m_cImgWndSub1;
	CImgShowWnd* m_cImgWndSub2;
	CImgShowWnd* m_cImgWndSub3; 
	CImgShowWnd* m_cImgWndSub4;
	CRect       m_rcWndMainLeft;
	CRect       m_rcWndMainRight;
	CRect       m_rcWndSub1;
	CRect       m_rcWndSub2;
	CRect       m_rcWndSub3;
	CRect       m_rcWndSub4;
	CRect       m_rcResultList;
	CRect       m_rcLogList;
	CLabelCtrl	m_btnShutdown;
	CLabelCtrl  m_labelBackground3;
	CFont       m_titleFont;
	int         m_nTxtFontSize;
	CFont       m_bigFont;
	CFont       m_txtFont;
	CFont       m_smallFont;
	CFont       m_logFont;
	int         m_nTitleHeight;

	float       m_fRatioH; // Horizontal zoom ratio
	float       m_fRatioV; // Vertical zoom ratio

	CString       m_sLogString;
	volatile int  m_nSysStatusFlag; //0:Ok, 1:Warning, 2:Error;
	volatile bool m_bStarted;
	volatile int  m_nBtnState;//current button state

	eEVENT_LEVEL m_eLevel ;
	eEVENT_LEVEL m_eResultLevel;
	CLoadingDlg  m_LoadingDlg;
	char*        m_strDataBase;
	char*        m_strDemuraTable;
	CString      m_strCopyLog;
	CString      m_strPatternIndex;
	int          m_nCrtPatternIndex;
	chronograph  m_timer;
	CLabelCtrl   m_labelResult;
	CLabelCtrl   m_labelScreenNameName;
	CLabelCtrl   m_labelScreenName;
	CLabelCtrl   m_labelScreenIDName;
	CLabelCtrl   m_labelScreenID;
	CEdit        m_edtScreenID;
	CLabelCtrl   m_labelDefectNumName;
	CLabelCtrl   m_labelDefectNum;
	CLabelCtrl   m_btnLogViewer;

	CDispResultInfoDlg* m_ptrDefectDlg;
	
#pragma endregion
#pragma region LOGIC
	std::map<int, CDemuraStation*>        m_dictStationIndex_Ptr; // station index <==> station class pointer
	std::map<int,wstring>                 m_dictCameraName;           
	circular_buffer<EVENT_PACKAGE>        m_eventBuffer;
	circular_buffer<EVENT_PACKAGE>        m_resultEventBuffer;
	std::vector<CImgShowWnd*>             m_vImgWindow;
	std::map<int, std::map<int, wstring>> m_dictPatternsDefectForDisp;
	
#pragma endregion


public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStnClickedOnOff();
	afx_msg void OnCbnSelchangeProject();
	afx_msg void OnBnClickedButtonDebug();
	afx_msg void OnBnClickedButtonSpc();
	afx_msg void OnStnClickedShutdown();
	afx_msg void OnStnClickedLogin();
	afx_msg void OnStnClickedSystemConfig();
	afx_msg void OnStnClickedUAC();
	afx_msg void OnStnClickedHelp();
	afx_msg void OnStnClickedLanguage();
	afx_msg void OnStnClickedLogViewer();
	afx_msg LRESULT OnUpdateStatistics(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveSystemState(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnReceiveSystemLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayResultInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayImageOnMainScreen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveVieworksCameraData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateStationConfig(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListResultInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickListBoxLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDbClickResultListBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickResultListBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCopylog();
};
