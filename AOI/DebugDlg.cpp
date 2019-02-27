#include "stdafx.h"
#include "AOI.h"
#include "DebugDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"
#include "DemuraStation.h"
#include "ImgShowWnd.h"
#include <WinUser.h>

extern ConfigFileManager gConfigFileMgr;
extern CSystemManager    gSystemMgr;

IMPLEMENT_DYNAMIC(CDebugDlg, CDialogEx)

CDebugDlg::CDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DEBUG, pParent),
	m_bStarted(false),
	m_ptrStation(nullptr),
	m_ptrProcessor(nullptr),
	m_ptrCrtFramePattern(nullptr),
	m_ptrCrtActionGroup(nullptr),
	m_ptrCrtFuncArray(nullptr),
	m_ptrCrtSelectedFunc(nullptr),
    m_ptrDictPatternForCrtCamera(nullptr),
	m_nCrtActionGpIndex(0),
	m_nCrtAlgoIndex(-1),
	m_nBtnSize(72),
	m_nGap(10),
	m_cImgWndDebug(NULL),
	m_nTitleLineHeight(92),
	m_nCameraIndex(0),
	m_nCrtMainCameraHeight(-1),
	m_nCrtPatternIndex(0),
	m_crtImgIndex(-1),
	m_nImgCount(0),
	m_bDebugMode(false),
	m_settingType(T_ALGO),
	m_bEnableOpenFolder(FALSE),
	m_bLocked(false),
	m_bMsgStatus(true),
	m_bCheckImgSave(FALSE),
	m_bIndependentGrab(FALSE),
	m_bIfSemiauto(false),
	m_bEnableMultipleProcess(false),
	m_bBatchMode(false),
	m_bShowResultImg(true),
	m_nPatternCtrlHeight(-1),
	m_nOneCircleID(0),
	m_nInternalImgIndex(0),
	m_nROINum(5),
	m_pWndListener(NULL),
	m_nIndieExpoTime(-1),
    m_nIndieGain(-1),
    m_nIndieDelayTime(-1),
	m_nIndieWhiteBalanceR(-1),
	m_nIndieWhiteBalanceG(-1),
	m_nIndieWhiteBalanceB(-1),
	m_nOldIndieExpoTime(-1),
    m_nOldIndieGain(-1),
	m_nOldIndieWhiteBalanceR(-1),
	m_nOldIndieWhiteBalanceG(-1),
	m_nOldIndieWhiteBalanceB(-1)
{
	m_smallFont.CreateFont(int(24 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_textFont.CreateFont(int(28 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_bigFont.CreateFont(int(40 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_titleFont.CreateFont(int(55 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_nBtnSize = int(gSystemMgr.m_fRatioH * BTN_SIZE);
	m_nGap = int(10.0f * gSystemMgr.m_fRatioH);
	m_nTitleLineHeight = int((10 + 10 + BTN_SIZE)*gSystemMgr.m_fRatioH);
	m_nWndWidth = int(6576 / 5.0f * gSystemMgr.m_fRatioH);
	m_nYPosLine0 = m_nTitleLineHeight + 2 * m_nGap + int(48 * gSystemMgr.m_fRatioH);
	m_nWndHeight = int(4384 / 5.0f * gSystemMgr.m_fRatioH);
	m_nYPosLine1 = m_nTitleLineHeight + 3 * m_nGap + m_nWndHeight + int(36 * gSystemMgr.m_fRatioH);
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	m_nXPosLine2 = int((m_nWndWidth + rcScreen.right - 15 * m_nGap) / 2.0f);
	m_nUnderLine = int(rcScreen.bottom - 5 * gSystemMgr.m_fRatioH);
	m_rcNavigateZone.left = m_nXPosLine2;
	m_rcNavigateZone.top = m_nTitleLineHeight;
	m_rcNavigateZone.right = rcScreen.right;
	m_rcNavigateZone.bottom = m_nYPosLine0 + 10;

	m_penTitle.CreatePen(PS_SOLID, 4, RGB(4, 40, 94));
	m_penXY.CreatePen(PS_SOLID, 1, RGB(0, 0, 100));

	m_fgColor = RGB(220, 220, 220);
	m_bgColor = RGB(4, 40, 94);

	m_rcLineZone.top = m_nTitleLineHeight - m_nGap + m_nBtnSize;//m_nTitleLineHeight+ m_nBtnSize;
	m_rcLineZone.bottom = m_rcNavigateZone.bottom;
	m_rcLineZone.left = m_rcNavigateZone.left;
	m_rcLineZone.right = m_rcNavigateZone.right;

	m_bgImg.LoadBitmap(IDB_BITMAP_DEBUG_DLG_BK);

	wstring wstrPatternCount = gConfigFileMgr.GetCommonSetting(L"PATTERN_LOOP_COUNT");
	int nPatternCount = boost::lexical_cast<int>(wstrPatternCount);
	m_cfParamUpdate.resize(nPatternCount, false);
	m_vImgSize.push_back(cv::Size2i(boost::lexical_cast<int>(gConfigFileMgr.GetCommonSetting(L"MAIN_CAMERA_WIDTH")), boost::lexical_cast<int>(gConfigFileMgr.GetCommonSetting(L"MAIN_CAMERA_HEIGHT"))));
	m_vImgSize.push_back(cv::Size2i(boost::lexical_cast<int>(gConfigFileMgr.GetCommonSetting(L"SUB_CAMERA_WIDTH")), boost::lexical_cast<int>(gConfigFileMgr.GetCommonSetting(L"SUB_CAMERA_HEIGHT"))));
}

CDebugDlg::~CDebugDlg()
{
	
}

void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALGO, m_listAlgo);
	DDX_Control(pDX, IDC_COMBO_PATTERN_ACTION, m_comboPatternAction);
	DDX_Radio(pDX, IDC_RADIO_CAMERA_1, m_nCameraIndex);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnApply);
	DDX_Control(pDX, IDC_STATIC_MESSAGE_PANEL, m_msgPanel);
	DDX_Control(pDX, IDC_STATIC_BTN_DECREASE, m_btnDecrease);
	DDX_Control(pDX, IDC_STATIC_BTN_INCREASE, m_btnIncrease);
	DDX_Check(pDX, IDC_CHECK_IMAGE_DEBUG, m_bEnableOpenFolder);
	DDX_Check(pDX, IDC_CHECK_SAVE_IMG, m_bCheckImgSave);
	DDX_Check(pDX, IDC_CHECK_INDEPENDENT_GRAB, m_bIndependentGrab);
	DDX_Control(pDX, IDC_STATIC_XY, m_StaticXY);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_edt_scale);
	DDX_Control(pDX, IDC_EDIT_SCALEINPUT, m_edt_sInput);
}

BOOL CDebugDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_MOUSEWHEEL|| pMsg->message == WM_MOUSEMOVE)
	{
		POINT nPos;
		GetCursorPos(&nPos);
		if (m_rcImgWnd.PtInRect(nPos))
		{
			m_cImgWndDebug->SetFocus();
		}
		else
		{
			//this->SetFocus();
		}	
	}
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if (pMsg->wParam == VK_RETURN && pMsg->message == WM_KEYDOWN)
		{

			CWnd* m_ptrCtrlEditCtrl = CWnd::GetFocus();
			if (m_ptrCtrlEditCtrl != nullptr)
			{
                int nCtrlID = m_ptrCtrlEditCtrl->GetDlgCtrlID();
                CString sValue, sOldValue;
				if (IDC_EDIT1_PARAMETER_CAMERA == nCtrlID)//ExposureTime
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nExposureTime);
					
					if (!localUtils::utils::IsNumericString(sValue))
					{
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_nIndieExpoTime = nValue;
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					if (sValue != sOldValue)
					{
						m_ptrCrtActionGroup->m_nExposureTime = nValue;
						m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nExposureTime = nValue;
						m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_EXPOSURE);
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_EXPOSURE);
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetExposureTime(nValue);
				}
				else if (IDC_EDIT2_PARAMETER_CAMERA == nCtrlID)//Gain
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nGain);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						//TODO:
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					m_nIndieGain = nValue;
					if (sValue != sOldValue)
					{
						m_ptrCrtActionGroup->m_nGain = nValue;
						m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nGain = nValue;
						m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_GAIN);
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_GAIN);
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetTapGain(nValue);
				}
				else if (IDC_EDIT_PARAM_BALANCER_CAMERA == nCtrlID)//White Balance R
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceR);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						//TODO:
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					m_nIndieWhiteBalanceR = nValue;
					if (sValue != sOldValue)
					{
						m_ptrCrtActionGroup->m_nWhiteBalanceR = nValue;
						m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceR = nValue;
						m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_R);
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_R);
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetWhiteBalance(nValue, m_ptrCrtActionGroup->m_nWhiteBalanceG, m_ptrCrtActionGroup->m_nWhiteBalanceB);
				}

				else if (IDC_EDIT_PARAM_BALANCEG_CAMERA == nCtrlID)//White Balance G
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceG);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						//TODO:
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					m_nIndieWhiteBalanceG = nValue;
					if (sValue != sOldValue)
					{
						m_ptrCrtActionGroup->m_nWhiteBalanceG = nValue;
						m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceG = nValue;
						m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_G);
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_G);
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetWhiteBalance(m_ptrCrtActionGroup->m_nWhiteBalanceR,nValue, m_ptrCrtActionGroup->m_nWhiteBalanceB);
				}

				else if (IDC_EDIT_PARAM_BALANCEB_CAMERA == nCtrlID)//White Balance B
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceB);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						//TODO:
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					m_nIndieWhiteBalanceB = nValue;
					if (sValue != sOldValue)
					{
						m_ptrCrtActionGroup->m_nWhiteBalanceB = nValue;
						m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceB = nValue;
						m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_B);
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_B);
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetWhiteBalance(m_ptrCrtActionGroup->m_nWhiteBalanceR, m_ptrCrtActionGroup->m_nWhiteBalanceG,nValue);
				}
				else if (IDC_EDIT3_PARAMETER_CAMERA == nCtrlID)//Delay
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), 0);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int nValue = _ttoi(sValue);
					m_nIndieDelayTime = nValue;
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					if (sValue != sOldValue)
					{
						m_cfParamUpdate[m_nCrtPatternIndex] = true;
					}
					m_ptrProcessor->m_ptrCamera->SetDelayTime(nValue);
				}
				else if (IDC_EDIT1_PARAMETER_LIGHT == nCtrlID)
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					AfxMessageBox(sValue);
				}
				else if (IDC_EDIT4_PARAMETER_MOTION == nCtrlID)//CameraHeight
				{
					m_ptrCtrlEditCtrl->GetWindowTextW(sValue);
					sOldValue.Format(_T("%d"), m_nCrtMainCameraHeight);
					if (!localUtils::utils::IsNumericString(sValue))
					{
						//TODO:
						m_ptrCrtEditCtrl->SetWindowText(sOldValue);
						return false;
					}
					int fValue = _ttoi(sValue);
					m_ptrCrtEditCtrl->SetWindowText(sValue);
					m_nCrtMainCameraHeight = fValue;
					if(sValue!= sOldValue)
					{
						m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, m_nCrtMainCameraHeight);
					}
					//DisplayAxisPosition(fValue);
				}
				if (m_settingType == T_ALGO && m_ptrCrtSelectedFunc != nullptr)
				{
					m_ptrProcessor->m_bExeAllAlgo = false;
					UpdateParamOfCurrentFunction();
					ProcessCurrentImage();
				}
				return TRUE;
			}
		}
	}
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_STATIC_BTN_INCREASE)->m_hWnd)
		{
			//TODO:
			SetTimer(TIMER_ID_FOR_INCREASE_BUTTON, 800, NULL);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_STATIC_BTN_DECREASE)->m_hWnd)
		{
			//TODO:
			SetTimer(TIMER_ID_FOR_DECREASE_BUTTON, 800, NULL);
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		KillTimer(TIMER_ID_FOR_INCREASE_BUTTON);
		KillTimer(TIMER_ID_FOR_DECREASE_BUTTON);
	}
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_SPACE)
	{
		return TRUE;
	}
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rcScreen.Width(), rcScreen.Height(), SWP_SHOWWINDOW);
	GetClientRect(&m_rcClient);

    m_ptrStation = gSystemMgr.GetStation(0);
    m_dictProjectPattern        = m_ptrStation->GetCurrentProjectPatternArray();
    m_dictCameraIndex_Name      = m_ptrStation->GetCameraIndexNameDict();
    m_diceCrtServoAxisSetting   = m_ptrStation->GetServoAxisSetting();
    //m_wstrCurtProjectName     = m_ptrStation->GetStationCrtProjectName();
	m_wstrCurtProjectName       = gSystemMgr.m_wstrCrtProjectName;
    m_nCrtMainCameraHeight      = m_ptrStation->GetStationCrtCameraHeight();
	CreateLogo(m_rcClient);
	CreateImageWindow(m_rcClient,m_cImgWndDebug);
	CreateAlgoList(m_rcClient);
    RetrieveInternalData(m_nCameraIndex);
	CreateSwitchButton(m_rcClient);
	CreateFunctionButton(m_rcClient);
	CreateSettingButtons(m_rcClient);
	CreatePatternIndexPanel(m_rcClient);
	CreateAutoTestingWidget();
	CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF);
	pCheck->ShowWindow(SW_HIDE);
	m_StaticXY.ShowWindow(false);
	if (NULL != m_ptrProcessor)
	{
		m_ptrProcessor->m_hDebugHandle = GetSafeHwnd();
	}
	m_pWndListener = this;
	return true;
}

BEGIN_MESSAGE_MAP(CDebugDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(BTN_OPEN_FOLDER, &CDebugDlg::OnStnClickedFolder)
	ON_STN_CLICKED(IDC_STATIC_SAVE_SINGLE_PIC, &CDebugDlg::OnStnClickedSaveSinglePicture)
	ON_STN_CLICKED(IDC_STATIC_SAVE_MULTIPLE_PIC, &CDebugDlg::OnStnClickedSaveMultiplePicture)
	ON_STN_CLICKED(IDS_STATIC_ONE_CIRCLE, &CDebugDlg::OnStnClickedOneCircle)
	ON_STN_CLICKED(IDS_STATIC_MULTIPLE_PROCESS, &CDebugDlg::OnStnClickedMultipleProcess)
	ON_STN_CLICKED(IDS_STATIC_AUTO_AEMIAUTO, &CDebugDlg::OnStnClickedAutoSemiauto)
	ON_STN_CLICKED(BTN_FIRST, &CDebugDlg::OnStnClickedFirst)
	ON_STN_CLICKED(BTN_PREVIOUS, &CDebugDlg::OnStnClickedPrevious)
	ON_STN_CLICKED(BTN_NEXT, &CDebugDlg::OnStnClickedNext)
	ON_STN_CLICKED(BTN_LAST, &CDebugDlg::OnStnClickedLast)
	ON_STN_CLICKED(BTN_LOCK, &CDebugDlg::OnStnClickedLock)
	ON_STN_CLICKED(BTN_ALGORITHM, &CDebugDlg::OnStnClickedAlgorithm)
	ON_STN_CLICKED(BTN_CAMERA, &CDebugDlg::OnStnClickedCamera)
	ON_STN_CLICKED(BTN_CLOSE, &CDebugDlg::OnStnClickedClose)
	ON_STN_CLICKED(BTN_RECTANGLE, &CDebugDlg::OnStnClickedDrawRectangle)

	ON_CBN_SELCHANGE(IDC_COMBO_PATTERN_ACTION, &CDebugDlg::OnCbnSelchangeComboPatternAction)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ALGO, &CDebugDlg::OnNMClickListAlgo)
	ON_EN_SETFOCUS(IDC_EDIT_PARAMETER, &CDebugDlg::OnEnSetfocusEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT1_PARAMETER_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT2_PARAMETER_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT3_PARAMETER_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT_PARAM_BALANCER_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT_PARAM_BALANCEG_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT_PARAM_BALANCEB_CAMERA, &CDebugDlg::OnEnSetfocusCameraEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT1_PARAMETER_LIGHT, &CDebugDlg::OnEnSetfocusLightEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT1_PARAMETER_MOTION, &CDebugDlg::OnEnSetfocusMotionEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT2_PARAMETER_MOTION, &CDebugDlg::OnEnSetfocusMotionEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT3_PARAMETER_MOTION, &CDebugDlg::OnEnSetfocusMotionEditParameter)
	ON_EN_SETFOCUS(IDC_EDIT4_PARAMETER_MOTION, &CDebugDlg::OnEnSetfocusMotionEditParameter)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_STATIC_BTN_INCREASE, &CDebugDlg::OnStnClickedStaticBtnIncrease)
	ON_BN_CLICKED(IDC_CHECK_IMAGE_DEBUG, &CDebugDlg::OnBnClickedCheckImageDebug)
	ON_STN_CLICKED(IDC_STATIC_BTN_DECREASE, &CDebugDlg::OnStnClickedStaticBtnDecrease)
	ON_STN_CLICKED(ID_PATTERN_WIDGET, &CDebugDlg::OnStnClickedPatternWidget)
	ON_MESSAGE(WM_SEND_MSG_LABELCTRL_RBTNDOWN,&CDebugDlg::OnStnRightClickedPatternWidget)
	ON_BN_CLICKED(IDC_CHECKBOX_ONOFF, &CDebugDlg::OnBnClickedCheckBoxOnOff)
	ON_BN_CLICKED(IDC_CHECK_INDEPENDENT_GRAB, &CDebugDlg::OnBnClickedCheckIndependentGrab)
	ON_BN_CLICKED(IDC_BUTTON_SOFTWARE_TRIGGER, &CDebugDlg::OnBnClickedButtonSoftwareTrigger)
	ON_BN_CLICKED(IDC_CHECK_SAVE_IMG, &CDebugDlg::OnBnClickedCheckSaveImg)
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALGO, &CDebugDlg::OnNMDblclkListAlgo)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CAMERA_1, IDC_RADIO_CAMERA_5, &CDebugDlg::OnBnClickedRadioCamera)
	ON_EN_SETFOCUS(IDC_EDIT_SCALEINPUT, &CDebugDlg::OnEnSetfocusEditScaleinput)
	ON_MESSAGE(WM_UPDATE_DISP_DEBUG, &CDebugDlg::OnDisplayImageOnDebugScreen)
	ON_MESSAGE(WM_ONE_MORE_IMAGE,&CDebugDlg::OnSendNextImage)
END_MESSAGE_MAP()

#pragma region Create UI
void CDebugDlg::DisplayBackgroundImage(CPaintDC & dc)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	BITMAP bitmap;
	m_bgImg.GetBitmap(&bitmap);
	CBitmap* pPngOld = dcMem.SelectObject(&m_bgImg);
	long nWidth = (rect.Width() < bitmap.bmWidth ? rect.Width() : bitmap.bmWidth);
	long nHeight = (rect.Height() < bitmap.bmHeight ? rect.Height() : bitmap.bmHeight);
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, nWidth, nHeight, SRCCOPY);
}

void CDebugDlg::CreateFunctionButton(CRect &m_rcClient)
{
	CRect rcBtn;
	rcBtn.left = m_rcClient.left + m_nGap;
	rcBtn.top = int((m_nTitleLineHeight - 36 * gSystemMgr.m_fRatioH) / 2.0f);
	rcBtn.right = rcBtn.left + int(150 * gSystemMgr.m_fRatioH);
	rcBtn.bottom = rcBtn.top + int(36 * gSystemMgr.m_fRatioH);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMAGE_DEBUG);
	pBtn->MoveWindow(&rcBtn);
	pBtn->SetFont(&m_textFont);
	pBtn->EnableWindow(false);
	CString str;
	str.LoadStringW(IDS_STRING_IMAGE_DEBUG);
	pBtn->SetWindowTextW(str);
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	//Folder
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.top = m_rcClient.top + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	rcBtn.bottom = rcBtn.top + m_nBtnSize;
	m_btnImageFolder.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_OPEN_FOLDER);
	m_btnImageFolder.SetLabelImage(IMG_PNG_FOLDER_DISABLED);
	m_btnImageFolder.BringWindowToTop();
	m_btnImageFolder.EnableWindow(FALSE);
	m_btnImageFolder.Invalidate();
	//First
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnFirst.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_FIRST);
	m_btnFirst.SetLabelImage(IMG_PNG_FIRST);
	m_btnFirst.BringWindowToTop();
	m_btnFirst.EnableWindow(TRUE);
	m_btnFirst.Invalidate();
	//Previous
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnPrevoius.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_PREVIOUS);
	m_btnPrevoius.SetLabelImage(IMG_PNG_PREVIOUS);
	m_btnPrevoius.BringWindowToTop();
	m_btnPrevoius.EnableWindow(TRUE);
	m_btnPrevoius.Invalidate();
	// Index Label
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + 2 * m_nBtnSize;
	m_lblIndex.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY, rcBtn, this, NULL);
	m_lblIndex.SetTxtFont(55*(int)(gSystemMgr.m_fRatioH), FW_BOLD, CENTER);
	m_lblIndex.SetColor(m_fgColor, m_bgColor);
	m_lblIndex.Invalidate();
	//Next
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnNext.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_NEXT);
	m_btnNext.SetLabelImage(IMG_PNG_NEXT);
	m_btnNext.BringWindowToTop();
	m_btnNext.EnableWindow(TRUE);
	m_btnNext.Invalidate();
	//Last
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnLast.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_LAST);
	m_btnLast.SetLabelImage(IMG_PNG_LAST);
	m_btnLast.BringWindowToTop();
	m_btnLast.EnableWindow(TRUE);
	m_btnLast.Invalidate();
	//Lock
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnLock.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_LOCK);
	m_btnLock.SetLabelImage(IMG_PNG_UNLOCK);
	m_btnLock.BringWindowToTop();
	m_btnLock.EnableWindow(TRUE);
	m_btnLock.Invalidate();
	//SaveSinglePicture
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnSaveSinglePic.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, IDC_STATIC_SAVE_SINGLE_PIC);
	m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC_DENY);
	m_btnSaveSinglePic.BringWindowToTop();
	m_btnSaveSinglePic.EnableWindow(TRUE);
	m_btnSaveSinglePic.Invalidate();

	//SaveMultiplePicture
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnSaveMultiplePic.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, IDC_STATIC_SAVE_MULTIPLE_PIC);
	m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC_DENY);
	m_btnSaveMultiplePic.BringWindowToTop();
	m_btnSaveMultiplePic.EnableWindow(TRUE);
	m_btnSaveMultiplePic.Invalidate();

	//Onecircle
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnOneCircle.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, IDS_STATIC_ONE_CIRCLE);
	m_btnOneCircle.SetLabelImage(IMG_PNG_ONE_CIRCLE_DENY);
	m_btnOneCircle.BringWindowToTop();
	m_btnOneCircle.EnableWindow(TRUE);
	m_btnOneCircle.Invalidate();

	//Multiple process
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnMultipleProcess.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, IDS_STATIC_MULTIPLE_PROCESS);
	m_btnMultipleProcess.SetLabelImage(IMG_PNG_MULTIPLE_PROCESS_DENY);
	m_btnMultipleProcess.BringWindowToTop();
	m_btnMultipleProcess.EnableWindow(TRUE);
	m_btnMultipleProcess.Invalidate();

	//Auto and Semiauto 
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + m_nBtnSize;
	m_btnAutoSemiauto.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, IDS_STATIC_AUTO_AEMIAUTO);
	m_btnAutoSemiauto.SetLabelImage(IMG_PNG_AUTO);
	m_btnAutoSemiauto.BringWindowToTop();
	m_btnAutoSemiauto.EnableWindow(TRUE);
	m_btnAutoSemiauto.Invalidate();

	//scale_edit(display)
	m_rcScale.left = rcBtn.right + m_nGap;
	m_rcScale.right = m_rcImgWnd.right - m_nGap*2 - m_nBtnSize/2;
	m_rcScale.top = rcBtn.top;
	m_rcScale.bottom = rcBtn.bottom;
	m_edt_scale.MoveWindow(m_rcScale);
	m_edt_scale.SetFont(&m_titleFont);
	m_edt_scale.SetWindowTextW(L"N/A");
	m_edt_scale.EnableWindow(FALSE);
	//scale_edit(edit)
	m_edt_sInput.MoveWindow(m_rcScale);
	m_edt_sInput.SetFont(&m_titleFont);
	m_edt_sInput.EnableWindow(FALSE);
	m_edt_sInput.ShowWindow(SW_HIDE);
	//percent
	CRect rcPercent;
	rcPercent.top = m_rcScale.top;
	rcPercent.bottom = m_rcScale.bottom;
	rcPercent.left = m_rcScale.right;
	rcPercent.right = m_rcImgWnd.right;
	m_labelScale.Create(L"%", WS_CHILD | WS_VISIBLE, rcPercent, this, NULL);
	m_labelScale.SetTxtFont(boost::numeric_cast<int>(55 * gSystemMgr.m_fRatioH), FW_BOLD, CENTER);
	m_labelScale.SetColor(m_fgColor, m_bgColor);
	m_labelScale.Invalidate();
	//shutdown
	rcBtn.right = m_rcClient.right - int(5* gSystemMgr.m_fRatioH);
	rcBtn.left = rcBtn.right - m_nBtnSize;
	rcBtn.bottom = m_rcScale.bottom ;
	rcBtn.top = m_rcScale.bottom - m_nBtnSize;
	m_btnClose.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_CLOSE);
	m_btnClose.SetLabelImage(IMG_PNG_SHUTDOWN);
	m_btnClose.BringWindowToTop();
	m_btnClose.EnableWindow(TRUE);
	m_btnClose.Invalidate();
	//------------------------------
	CRect rcToolBtn;
	int ImgBtnSize = m_rcImgWnd.left - m_nGap*2;
	rcToolBtn.left = m_rcClient.left + m_nGap;
	rcToolBtn.right = rcToolBtn.left + ImgBtnSize;
	rcToolBtn.top = m_rcImgWnd.top;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnLine.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_LINE);
	m_BtnLine.SetLabelImage(IMG_PNG_LINE);
	m_BtnLine.BringWindowToTop();
	m_BtnLine.EnableWindow(TRUE);
	m_BtnLine.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom+ m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnRectangle.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_RECTANGLE);
	m_BtnRectangle.SetLabelImage(IMG_PNG_RECTANGLE);
	m_BtnRectangle.BringWindowToTop();
	m_BtnRectangle.EnableWindow(TRUE);
	m_BtnRectangle.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnRectRotate.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_RECT_ROTATE);
	m_BtnRectRotate.SetLabelImage(IMG_PNG_RECTANGLE_ROTAYE);
	m_BtnRectRotate.BringWindowToTop();
	m_BtnRectRotate.EnableWindow(TRUE);
	m_BtnRectRotate.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnCircle.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_CIRCLE);
	m_BtnCircle.SetLabelImage(IMG_PNG_CIRCLE);
	m_BtnCircle.BringWindowToTop();
	m_BtnCircle.EnableWindow(TRUE);
	m_BtnCircle.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnEllipse.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_ELLIPSE);
	m_BtnEllipse.SetLabelImage(IMG_PNG_ELLAPSE_ROTAYE);
	m_BtnEllipse.BringWindowToTop();
	m_BtnEllipse.EnableWindow(TRUE);
	m_BtnEllipse.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnPolygon.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_POLYGON);
	m_BtnPolygon.SetLabelImage(IMG_PNG_POLYGON);
	m_BtnPolygon.BringWindowToTop();
	m_BtnPolygon.EnableWindow(TRUE);
	m_BtnPolygon.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnClip.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_CLIP);
	m_BtnClip.SetLabelImage(IMG_PNG_CLIP);
	m_BtnClip.BringWindowToTop();
	m_BtnClip.EnableWindow(TRUE);
	m_BtnClip.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnSaveImg.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_SAVE_BMP);
	m_BtnSaveImg.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC);
	m_BtnSaveImg.BringWindowToTop();
	m_BtnSaveImg.EnableWindow(TRUE);
	m_BtnSaveImg.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnSnip.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_SNAP_SHOT);
	m_BtnSnip.SetLabelImage(IMG_PNG_GRAB_SCREEN);
	m_BtnSnip.BringWindowToTop();
	m_BtnSnip.EnableWindow(TRUE);
	m_BtnSnip.Invalidate();

	rcToolBtn.top = rcToolBtn.bottom + m_nGap;
	rcToolBtn.bottom = rcToolBtn.top + ImgBtnSize;
	m_BtnClear.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcToolBtn, this, BTN_DELETE_ALL);
	m_BtnClear.SetLabelImage(IMG_PNG_CLEAR);
	m_BtnClear.BringWindowToTop();
	m_BtnClear.EnableWindow(TRUE);
	m_BtnClear.Invalidate();

	//------------------------------
}

void CDebugDlg::CreateLogo(CRect &m_rcClient)
{
	CRect rcBtn;
	rcBtn.right = m_rcClient.right - m_nGap- m_nBtnSize;
	rcBtn.left = m_rcClient.right - int(158 * gSystemMgr.m_fRatioH) - m_nBtnSize;
	rcBtn.top = m_rcClient.top + int(m_nGap / 2.0f);
	rcBtn.bottom = rcBtn.top + int(64 * gSystemMgr.m_fRatioH);
	m_lblLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcBtn, this, NULL);
	m_lblLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_lblLogo.BringWindowToTop();
	m_lblLogo.EnableWindow(TRUE);
	m_lblLogo.Invalidate();
}

void CDebugDlg::CreateImageWindow(CRect &m_rcClient,CImgShowWnd* &cImgWnd)
{
	m_rcImgWnd.left = m_rcClient.left + m_nGap+m_nBtnSize;
	m_rcImgWnd.top = m_nTitleLineHeight + int(10 * gSystemMgr.m_fRatioH);
	m_rcImgWnd.bottom = m_rcClient.bottom - int(36 * gSystemMgr.m_fRatioV + 3 * m_nGap);
	m_nWndHeight = m_rcImgWnd.bottom - m_rcImgWnd.top;
	m_nWndWidth = int(10000.0f/7096*m_nWndHeight);
	m_rcImgWnd.right = m_rcImgWnd.left + m_nWndWidth;
	m_rcImgWnd.bottom = m_rcImgWnd.top + m_nWndHeight;

	if (NULL== cImgWnd)
	{
		cImgWnd = new CImgShowWnd;
		if (FALSE == cImgWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, m_rcImgWnd, this, -1))
		{
			//TODO:log this event
			return;
		}
		cImgWnd->ShowWindow(SW_SHOW);
		cImgWnd->EnableDrag(true);
		cImgWnd->EnableZoom(true);
		cImgWnd->SetBkColor(RGB(100, 100, 100));
		cImgWnd->SetListener(this);
	}

}

void CDebugDlg::CreateSwitchButton(CRect &m_rcClient)
{
    assert(!m_dictCameraIndex_Name.empty());
	// Camera Index
    int nIndex = 0;
	int nWidth = int(160 * gSystemMgr.m_fRatioH);
	int nHeight = int(36 * gSystemMgr.m_fRatioV);
	CRect rcRadio;
	rcRadio.left = m_rcClient.left + m_nGap;
	rcRadio.right = rcRadio.left + nWidth + 170;
	rcRadio.bottom = m_rcClient.bottom - m_nGap;
	rcRadio.top = rcRadio.bottom - nHeight;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_CAMERA_1);
	pBtn->MoveWindow(rcRadio);
	pBtn->SetFont(&m_textFont);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
    assert(m_dictCameraIndex_Name.find(nIndex) != m_dictCameraIndex_Name.end());
    wstring wstrName = m_dictCameraIndex_Name[nIndex];
    pBtn->SetWindowTextW(wstrName.c_str());
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    if (m_ptrStation->GetPatternProcessor(wstrName) == nullptr)
    {
        pBtn->EnableWindow(FALSE);
    }

    nIndex++;
	rcRadio.left = rcRadio.right + 50;
	rcRadio.right = rcRadio.left + nWidth + 60;
	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CAMERA_2);
	pBtn->MoveWindow(rcRadio);
	pBtn->SetFont(&m_textFont);
    assert(m_dictCameraIndex_Name.find(nIndex) != m_dictCameraIndex_Name.end());
    wstrName = m_dictCameraIndex_Name[nIndex];
    pBtn->SetWindowTextW(wstrName.c_str());
    SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    if (m_ptrStation->GetPatternProcessor(wstrName) == nullptr)
    {
        pBtn->EnableWindow(FALSE);
    }

    nIndex++;
    rcRadio.left = rcRadio.right + 5;
    rcRadio.right = rcRadio.left + nWidth + 60;
	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CAMERA_3);
	pBtn->MoveWindow(rcRadio);
	pBtn->SetFont(&m_textFont);
    assert(m_dictCameraIndex_Name.find(nIndex) != m_dictCameraIndex_Name.end());
    wstrName = m_dictCameraIndex_Name[nIndex];
    pBtn->SetWindowTextW(wstrName.c_str());
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    if (m_ptrStation->GetPatternProcessor(wstrName) == nullptr)
    {
        pBtn->EnableWindow(FALSE);
    }

    nIndex++;
    rcRadio.left = rcRadio.right + 5;
    rcRadio.right = rcRadio.left + nWidth + 60;
	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CAMERA_4);
	pBtn->MoveWindow(rcRadio);
	pBtn->SetFont(&m_textFont);
    assert(m_dictCameraIndex_Name.find(nIndex) != m_dictCameraIndex_Name.end());
    wstrName = m_dictCameraIndex_Name[nIndex];
    pBtn->SetWindowTextW(wstrName.c_str());
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    if (m_ptrStation->GetPatternProcessor(wstrName) == nullptr)
    {
        pBtn->EnableWindow(FALSE);
    }

    nIndex++;
    rcRadio.left = rcRadio.right + 5;
    rcRadio.right = rcRadio.left + nWidth + 60;
	pBtn = (CButton*)GetDlgItem(IDC_RADIO_CAMERA_5);
	pBtn->MoveWindow(rcRadio);
	pBtn->SetFont(&m_textFont);
    assert(m_dictCameraIndex_Name.find(nIndex) != m_dictCameraIndex_Name.end());
    wstrName = m_dictCameraIndex_Name[nIndex];
    pBtn->SetWindowTextW(wstrName.c_str());
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    if (m_ptrStation->GetPatternProcessor(wstrName) == nullptr)
    {
        pBtn->EnableWindow(FALSE);
    }
	//Static XY
	rcXY = rcRadio;
	rcXY.left = rcRadio.right + 5;
	rcXY.right = m_rcImgWnd.right;
	m_StaticXY.MoveWindow(rcXY);
	m_StaticXY.SetFont(&m_textFont);
}

void CDebugDlg::CreateAlgoList(CRect &m_rcClient)
{
	CRect rcCombo;
	rcCombo.left = m_nXPosLine2 + m_nGap;
	rcCombo.top = (int)((m_rcClient.bottom - m_rcClient.top) / 2.0f)+50;
	rcCombo.right = m_rcClient.right - 5;
	rcCombo.bottom = rcCombo.top + int(36 * gSystemMgr.m_fRatioH);
	m_comboPatternAction.MoveWindow(&rcCombo);
	m_comboPatternAction.SetFont(&m_textFont);

	m_rcList.left = rcCombo.left;
	m_rcList.top = rcCombo.bottom + m_nGap;
	m_rcList.right = rcCombo.right;
	m_rcList.bottom = m_rcClient.bottom - m_nGap;
	m_listAlgo.MoveWindow(&m_rcList);
	m_listAlgo.SetFont(&m_smallFont);

	DWORD dwStyleEx = m_listAlgo.GetExtendedStyle();
	dwStyleEx |= LVS_EX_FULLROWSELECT;
	dwStyleEx |= LVS_EX_GRIDLINES;
	//dwStyleEx |= LVS_EX_CHECKBOXES;
	m_listAlgo.SetExtendedStyle(dwStyleEx);

	CString strIndex, strAlgoName;
	strIndex.LoadStringW(IDS_STRING_INDEX);
	strAlgoName.LoadStringW(IDS_STRING_ALGO_NAME);
	int nWidth0 = 60;
	int nWidth1 = m_rcList.Width() - nWidth0 - 10;

	m_listAlgo.InsertColumn(0, strIndex, LVCFMT_CENTER, nWidth0);
	m_listAlgo.InsertColumn(1, strAlgoName, LVCFMT_LEFT, nWidth1);
}

void CDebugDlg::CreatePatternIndexPanel(CRect &m_rcClient)
{
	auto nPatternCount = m_dictProjectPattern.size();
	if (nPatternCount == 0 || m_ptrDictPatternForCrtCamera == nullptr)
	{
		return;
	}
	int nMaxHeight = int(60 * gSystemMgr.m_fRatioV);
	int nHeight = int(1.0f*(m_rcClient.bottom - m_nTitleLineHeight - (nPatternCount + 1)*m_nGap) / nPatternCount);
	if (nHeight > nMaxHeight)
	{
		nHeight = nMaxHeight;
	}
	m_nPatternCtrlHeight = nHeight;
	int nWidth = m_nXPosLine2 - m_rcImgWnd.right - 5 * m_nGap;
	CRect rc;
	CString sIndex;
	rc.left = m_rcImgWnd.right + 3 * m_nGap;
	rc.right = m_nXPosLine2 - m_nGap;
	int t = 0;
	std::map<int, PG_IMAGE>* ptrDictSeqImg = gConfigFileMgr.GetPgSeqSetting();
	for (auto it = m_dictProjectPattern.begin(); it != m_dictProjectPattern.end(); it++)
	{
		FRAME_PATTERN& framePattern = it->second;
		rc.top = (t + 1)*m_nGap + t*nHeight + m_nTitleLineHeight;
		rc.bottom = rc.top + nHeight;
		CLabelCtrl* pWidget = new CLabelCtrl();
		pWidget->Create(_T(""), WS_CHILD | WS_VISIBLE | WS_BORDER |SS_CENTER | SS_NOTIFY, rc, this, ID_PATTERN_WIDGET);
		pWidget->m_nID = ID_PATTERN_WIDGET;
		pWidget->SetMainUiPtr(this);
		pWidget->SetTxtFont(int(nHeight / 2), FW_BOLD, CENTER);
		COLORREF bgColor = m_bgColor;
		if (ptrDictSeqImg != nullptr && ptrDictSeqImg->find(t) != ptrDictSeqImg->end())
		{
			bgColor = (*ptrDictSeqImg)[t].m_color;
		}
		pWidget->SetColor(m_fgColor, bgColor);
		pWidget->Invalidate();
		m_vPatternWidgetPtr.push_back(pWidget);
		m_vWidgetRect.push_back(rc);
		if (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end())
		{
			//sIndex.Format(_T("%d"), t + 1);
			//sIndex = (*ptrDictSeqImg)[t].m_wstrName.c_str();
			sIndex = (*ptrDictSeqImg)[t].m_wstrType.c_str();
			pWidget->SetWindowTextW(sIndex);
		}
		else
		{
			pWidget->SetWindowTextW(_T("X"));
		}
		t++;
	}

	auto szCount = m_vWidgetRect.size();
	if (m_nCrtPatternIndex >= 0 && m_nCrtPatternIndex < szCount)
	{
		m_rcCrtPatternWidget = m_vWidgetRect[m_nCrtPatternIndex];
		if (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end())
		{
			m_rcCrtPatternWidget.left -= m_nGap;
			m_rcCrtPatternWidget.right -= m_nGap;
		}
		m_vPatternWidgetPtr[m_nCrtPatternIndex]->MoveWindow(&m_rcCrtPatternWidget);
		CRect rcInvalidate = m_rcCrtPatternWidget;
		rcInvalidate.right += (3 * m_nGap + 5);
		InvalidateRect(rcInvalidate);
	}
}

void CDebugDlg::CreateSettingButtons(CRect &m_rcClient)
{
	int nBtnSz = int(48 * gSystemMgr.m_fRatioH);

	CRect rcBtn;
	int nHalfGap = int(m_nGap / 2.0f);
	//algorithm
	rcBtn.left = m_nXPosLine2 + m_nGap;
	rcBtn.top = m_nTitleLineHeight + m_nGap;
	rcBtn.right = rcBtn.left + nBtnSz;
	rcBtn.bottom = rcBtn.top + nBtnSz;
	m_btnAlgorithm.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_ALGORITHM);
	m_btnAlgorithm.SetLabelImage(IMG_PNG_ALGORITHM);
	m_btnAlgorithm.BringWindowToTop();
	m_btnAlgorithm.EnableWindow(TRUE);
	m_btnAlgorithm.Invalidate();
	m_rcAlgoBtn = rcBtn;
	m_rcCrtBtn = m_rcAlgoBtn;
	//camera
	rcBtn.left = rcBtn.right + m_nGap;
	rcBtn.right = rcBtn.left + nBtnSz;
	m_btnCamera.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_CAMERA);
	m_btnCamera.SetLabelImage(IMG_PNG_CAMERA_SETTING);
	m_btnCamera.BringWindowToTop();
	m_btnCamera.EnableWindow(TRUE);
	m_btnCamera.Invalidate();
	m_rcCameraBtn = rcBtn;

	m_rcParamBoard.left = m_rcNavigateZone.left;
	m_rcParamBoard.top = m_rcNavigateZone.bottom;
	m_rcParamBoard.right = m_rcClient.right - m_nGap;
	m_rcParamBoard.bottom = m_rcClient.bottom - int(48 * gSystemMgr.m_fRatioH) - 2 * m_nGap;
	//Apply button
	int nWidth = int(128 * gSystemMgr.m_fRatioH);
	rcBtn.left = m_rcParamBoard.left + int((m_rcParamBoard.Width() - nWidth) / 2.0f);
	rcBtn.right = rcBtn.left + nWidth;
	rcBtn.bottom = m_rcClient.bottom - m_nGap;
	rcBtn.top = rcBtn.bottom - int(48 * gSystemMgr.m_fRatioH);
	m_btnApply.SetFont(&m_bigFont);
	m_btnApply.MoveWindow(&rcBtn);
	CString str;
	str.LoadStringW(IDS_STRING_APPLY);
	m_btnApply.SetWindowTextW(str);
	m_btnApply.ShowWindow(HIDE_WINDOW);

	CRect rcPanel;
	rcPanel.left = m_rcParamBoard.left;
	rcPanel.top = m_rcClient.bottom - int(128 * gSystemMgr.m_fRatioH);
	rcPanel.right = m_rcClient.right - m_nGap;
	rcPanel.bottom = m_rcClient.bottom - int(48 * gSystemMgr.m_fRatioH);
	m_msgPanel.MoveWindow(&rcPanel);
	m_msgPanel.SetFont(&m_smallFont);
}

void CDebugDlg::RetrieveInternalData(int nCameraIndex)
{
    assert(m_dictCameraIndex_Name.find(nCameraIndex) != m_dictCameraIndex_Name.end());

	m_wstrCrtCameraName = m_dictCameraIndex_Name[nCameraIndex];
	CPatternProcessor* pOldProcessor = m_ptrProcessor;
	m_ptrProcessor = m_ptrStation->GetPatternProcessor(m_wstrCrtCameraName);
	if (m_ptrProcessor != nullptr && pOldProcessor != m_ptrProcessor)
	{
		if (pOldProcessor)
		{
			pOldProcessor->SetDebugFlag(false);
			pOldProcessor->m_hDebugHandle = NULL;
			if (m_bDebugMode)
			{
				pOldProcessor->ResetInnerData();
			}
		}
		m_ptrProcessor->SetDebugFlag(m_bDebugMode);
		m_ptrProcessor->m_hDebugHandle = GetSafeHwnd();
        m_ptrDictPatternForCrtCamera = m_ptrProcessor->GetCameraPatternArray();
        InitializeActionGroupWidget();
	}
}

void CDebugDlg::AddFunction2List()
{
	if (m_ptrCrtActionGroup->m_vAlgoFunc.empty())
	{
		return;
	}
	int nListIndex = 0;
	CString strIndex;
	m_ptrCrtFuncArray = &(m_ptrCrtActionGroup->m_vAlgoFunc);
	for (size_t t = 0; t < m_ptrCrtActionGroup->m_vAlgoFunc.size(); t++)
	{
		FUNC_INFO fi = m_ptrCrtActionGroup->m_vAlgoFunc[t];
		strIndex.Format(_T("   %d"), nListIndex + 1);
		m_listAlgo.InsertItem(nListIndex, strIndex);
		m_listAlgo.SetItemText(nListIndex, 1, fi.m_wstrToolName.c_str());
		nListIndex++;
	}
}

#pragma endregion

bool CDebugDlg::CheckImageIndex(int nIndex)
{
	if (nIndex < 0 || nIndex > m_nImgCount - 1)
	{
		return false;
	}
	if (nIndex < m_vImage.size())
	{
		Mat img = m_vImage[nIndex].m_img.clone();
		if(!img.data)
		{
			return false;
		}
	}
	return true;
}

void CDebugDlg::UpdateIndexPanel(int nIndex)
{
	CString str;
	if (nIndex < 0)
	{
		str = _T("N/A");
	}
	else
	{
		str.Format(_T("%d/%d"), ++nIndex, m_nImgCount);
	}
	m_lblIndex.SetWindowText(str);
	m_lblIndex.Invalidate();
}

void CDebugDlg::ProcessCurrentImage()
{
	if (m_nImgCount == 0 || m_crtImgIndex < 0 || m_crtImgIndex >= m_nImgCount)
		return;
	m_crtImage = m_vImage[m_crtImgIndex].m_img.clone();
	if (!m_crtImage.data)
		return;
	if (m_ptrProcessor->m_bInChecking)
	{
		return;
	}
	//int nCameraIndex = -1;
	//int nPatternIndex = -1;
	//int nActionGpIndex = -1;
	//bool bMarked = false;
	//localUtils::utils::ParseImgSymbol(m_vImage[m_crtImgIndex], nCameraIndex, nPatternIndex, nActionGpIndex, bMarked);
	//bool bLabeledImg = (((nCameraIndex == 0 || nCameraIndex == 1)) && (nPatternIndex >= 0)&&(nActionGpIndex == 0));
	//if (bLabeledImg && bMarked)
	//{
		//size_t nPatternNums = m_dictProjectPattern.size();
		//if (nPatternIndex > nPatternNums - 1)
		//{
		//	CString sMsg;
		//	sMsg.LoadStringW(IDS_STRING_IMAGE_LABEL_MISSED);
		//	AfxMessageBox(sMsg, MB_OK);
		//	return;
		//}
		//if ((m_nCameraIndex != nCameraIndex) || (m_nCrtActionGpIndex != nActionGpIndex) || (m_nCrtPatternIndex != nPatternIndex))
		//{
		//	OnBnClickedRadioStationByPicture(nCameraIndex);
		//	OnStnClickedPatternWidgetByPicture(nPatternIndex);
		//	OnCbnSelchangeComboPatternActionByPicture(nActionGpIndex);
		//}
	//}
	//else
	//{
	//	CString sMsg;
	//	sMsg.LoadStringW(IDS_STRING_IMAGE_LABEL_MISSED);
	//	AfxMessageBox(sMsg, MB_OK);
	//	return;
	//}

	//localUtils::utils::PrintDebugSymbol(m_crtImage);
	IMG_PACKAGE imgPac;
	imgPac.m_img = m_crtImage.clone();
	imgPac.m_bDebugFlag = true;
	imgPac.m_nPatternIndex = m_nCrtPatternIndex;
	if (m_nCameraIndex != 0)
	{
		imgPac.m_bMainCamera = false;
	}
	m_ptrProcessor->m_nCrtAlgoIndex = m_nCrtAlgoIndex;
	m_ptrProcessor->InsertDebugImage(imgPac.m_nPatternIndex, imgPac);
}

void CDebugDlg::DeleteAllSettingWidget()
{
	m_ptrCrtSelectedFunc = nullptr;
	m_ptrCrtEditCtrl = nullptr;
	std::for_each(m_vParamNameCtrl.begin(), m_vParamNameCtrl.end(), [&](CWnd* pWnd)
	{
		delete pWnd;
		pWnd = nullptr;
	});
	m_vParamNameCtrl.clear();
	std::for_each(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), [&](CWnd* pWnd)
	{
		delete pWnd;
		pWnd = nullptr;
	});
	m_vParamEditCtrl.clear();
	CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF);
	if (pCheck != nullptr)
	{
		pCheck->ShowWindow(SW_HIDE);
		pCheck = nullptr;
	}

	m_msgPanel.SetWindowTextW(_T(""));
	CRect rcPanel;
	m_msgPanel.GetWindowRect(&rcPanel);
	InvalidateRect(&rcPanel);
	ShowAutoTestingWidget(false);
	m_btnIncrease.ShowWindow(SW_HIDE);
	m_btnDecrease.ShowWindow(SW_HIDE);
}

void CDebugDlg::CreateAlgoParamItems()
{
	if (m_ptrCrtSelectedFunc == nullptr)
	{
		return;
	}
	CEdit* pEdit = nullptr;
	CStatic* pStatic = nullptr;

	std::vector<FUNC_PARAM> vFuncParam = m_ptrCrtSelectedFunc->m_vParam;
	CRect rcName, rcEdit;
	int nHeight = 25;
	int nNameWidth = int(200 * gSystemMgr.m_fRatioH);
	rcName.top = m_rcParamBoard.top;
	rcName.left = m_rcParamBoard.left + m_nGap;
	rcName.right = rcName.left + nNameWidth;

	rcEdit.top = rcName.top;
	rcEdit.left = rcName.right + m_nGap;
	rcEdit.right = rcEdit.left + int(90 * gSystemMgr.m_fRatioH);

	CString sParamHelp;
	for (int i = 0; i < vFuncParam.size(); i++)
	{
		FUNC_PARAM funcParam = vFuncParam[i];
		sParamHelp.LoadStringW(funcParam.m_descID);
		wstring wstrHelp = sParamHelp.GetString();

		rcName.top = m_rcParamBoard.top + (i + 1)*m_nGap/2 + i*nHeight;
		rcName.bottom = rcName.top + nHeight;
		rcEdit.top = rcName.top;
		rcEdit.bottom = rcName.bottom;

		pStatic = new CStatic;
		pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, 90000 + i);
		pStatic->SetFont(&m_smallFont);
		pStatic->SetWindowText(funcParam.m_wstrName.c_str());
		m_vParamNameCtrl.push_back(pStatic);

		pEdit = new CEdit;
		pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT_PARAMETER);
		pEdit->EnableWindow((m_bIfSemiauto&&i != 0));
		pEdit->SetFont(&m_smallFont);
		float fValue = funcParam.m_value;
		int   nValue = (int)fValue;
		CString strValue;
		if (funcParam.m_type == VT_R4)
		{
			strValue.Format(_T("%.2f"), fValue);
		}
		else
		{
			strValue.Format(_T("%d"), nValue);
		}
		pEdit->SetWindowText(strValue);
		m_vParamEditCtrl.push_back(pEdit);
		/*if (!m_bIfSemiauto)
		{
			pEdit->EnableWindow(false);
		}
		else
		{
			pEdit->EnableWindow(true);
		}*/
	}
	// Move Message Panel
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcMsg;
	rcMsg.left = rcName.left;
	rcMsg.top = rcName.bottom + m_nGap;
	rcMsg.right = m_rcClient.right - m_nGap;
	rcMsg.bottom = rcMsg.top + int(96 * gSystemMgr.m_fRatioH);
	m_msgPanel.MoveWindow(&rcMsg);
}

void CDebugDlg::CreateCameraParamItems()
{
	CEdit* pEdit = nullptr;
	CStatic* pStatic = nullptr;

	CRect rcName, rcEdit;
	int nHeight = 30;
	int nNameWidth = int(160 * gSystemMgr.m_fRatioH);
	rcName.top = m_rcParamBoard.top + m_nGap;
	rcName.left = m_rcParamBoard.left + m_nGap;
	rcName.right = rcName.left + nNameWidth;

	rcEdit.top = rcName.top;
	rcEdit.left = rcName.right + m_nGap;
	rcEdit.right = rcEdit.left + int(80 * gSystemMgr.m_fRatioH);
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC1_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	CString str;
	str.LoadStringW(IDS_STRING_EXPOSURE_TIME);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);
	//ExposureTime Editbox
	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT1_PARAMETER_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	CString strValue;
	strValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nExposureTime);
	m_nIndieExpoTime = m_ptrCrtActionGroup->m_nExposureTime;
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	if ((!m_bIfSemiauto) || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_CAMERA_GAIN);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);

	//m_nGain Editbox
	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT2_PARAMETER_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nGain);
	m_nIndieGain = m_ptrCrtActionGroup->m_nGain;
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	if (!m_bIfSemiauto || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	//m_nWhiteBalanceR
	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_CAMERABALANCE_R);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);

	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT_PARAM_BALANCER_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceR);
	m_nIndieWhiteBalanceR = m_ptrCrtActionGroup->m_nWhiteBalanceR;
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	if (!m_bIfSemiauto || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	//m_nWhiteBalanceG
	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_CAMERABALANCE_G);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);

	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT_PARAM_BALANCEG_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceG);
	m_nIndieWhiteBalanceG = m_ptrCrtActionGroup->m_nWhiteBalanceG;
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	if (!m_bIfSemiauto || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}
	//m_nWhiteBalanceB
	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_CAMERABALANCE_B);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);

	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT_PARAM_BALANCEB_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), m_ptrCrtActionGroup->m_nWhiteBalanceB);
	m_nIndieWhiteBalanceB = m_ptrCrtActionGroup->m_nWhiteBalanceB;
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);

	if (!m_bIfSemiauto || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;
	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_CAMERA);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_DELAY);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);

	//m_nDelay Editbox
	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT3_PARAMETER_CAMERA);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), 0);
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	// get iamge
	ShowAutoTestingWidget(true);
	if (!m_bIfSemiauto || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcName.bottom = rcName.top + nHeight;
	CWnd* pWidget = GetDlgItem(IDC_CHECK_INDEPENDENT_GRAB);
	pWidget->MoveWindow(rcName);
	if ((!m_bIfSemiauto) || m_bEnableOpenFolder || (!m_bStarted) || (m_ptrProcessor->m_bInChecking))
	{
		pWidget->EnableWindow(false);
	}
	else
	{
		pWidget->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcName.bottom = rcName.top + nHeight;
    pWidget = GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER);
	pWidget->MoveWindow(rcName);
	if ((!m_bIfSemiauto) || m_bEnableOpenFolder || (!m_bStarted) || (m_ptrProcessor->m_bInChecking)||!m_bIndependentGrab)
	{
		pWidget->EnableWindow(false);
	}
	else
	{
		pWidget->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcName.bottom = rcName.top + nHeight;
	pWidget = GetDlgItem(IDC_CHECK_SAVE_IMG);
	pWidget->MoveWindow(rcName);
	if ((!m_bIfSemiauto) || m_bEnableOpenFolder || (!m_bStarted) || (m_ptrProcessor->m_bInChecking))
	{
		pWidget->EnableWindow(false);
	}
	else
	{
		pWidget->EnableWindow(true);
	}

	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcMsg;
	rcMsg.left = rcName.left;
	rcMsg.top = rcName.bottom + m_nGap;
	rcMsg.right = m_rcClient.right - m_nGap;
	rcMsg.bottom = rcMsg.top + int(96 * gSystemMgr.m_fRatioH);
	m_msgPanel.MoveWindow(&rcMsg);
}

void CDebugDlg::CreateLightParamItems()
{
	CEdit* pEdit = nullptr;
	CStatic* pStatic = nullptr;

	CRect rcName, rcEdit;
	int nHeight = 30;
	int nNameWidth = int(160 * gSystemMgr.m_fRatioH);
	rcName.top = m_rcParamBoard.top + m_nGap;
	rcName.left = m_rcParamBoard.left + m_nGap;
	rcName.right = rcName.left + nNameWidth;

	rcEdit.top = rcName.top;
	rcEdit.left = rcName.right + m_nGap;
	rcEdit.right = rcEdit.left + int(80 * gSystemMgr.m_fRatioH);
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	CString str;
	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC2_PARAMETER_LIGHT);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_LIGHT_ONOFF);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);


	CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF);
	if (pCheck != nullptr)
	{
		pCheck->MoveWindow(rcEdit);
		pCheck->SetFont(&m_smallFont);
		pCheck->ShowWindow(SW_SHOW);
		m_bgBrushHelper.AddSubWindow(this, pCheck, m_bgImg);
	}
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcMsg;
	rcMsg.left = rcName.left;
	rcMsg.top = rcName.bottom + m_nGap;
	rcMsg.right = m_rcClient.right - m_nGap;
	rcMsg.bottom = rcMsg.top + int(96 * gSystemMgr.m_fRatioH);
	m_msgPanel.MoveWindow(&rcMsg);
}

void CDebugDlg::CreateMotionParamItems()
{
	CEdit* pEdit = nullptr;
	CStatic* pStatic = nullptr;

	CRect rcName, rcEdit;
	int nHeight = 30;
	int nNameWidth = int(160 * gSystemMgr.m_fRatioH);
	rcName.top = m_rcParamBoard.top + m_nGap;
	rcName.left = m_rcParamBoard.left + m_nGap;
	rcName.right = rcName.left + nNameWidth;

	rcEdit.top = rcName.top;
	rcEdit.left = rcName.right + m_nGap;
	rcEdit.right = rcEdit.left + int(80 * gSystemMgr.m_fRatioH);
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	CString str;
	CString strValue;
	pStatic = new CStatic;
	pStatic->Create(_T(""), WS_CHILD | WS_VISIBLE, rcName, this, IDC_STATIC4_PARAMETER_MOTION);
	pStatic->SetFont(&m_smallFont);
	str.LoadStringW(IDS_STRING_MAIN_AXIS);
	pStatic->SetWindowText(str);
	m_vParamNameCtrl.push_back(pStatic);
	//CameraHeight
	pEdit = new CEdit;
	pEdit->Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcEdit, this, IDC_EDIT4_PARAMETER_MOTION);
	pEdit->EnableWindow(true);
	pEdit->SetFont(&m_smallFont);
	strValue.Format(_T("%d"), m_nCrtMainCameraHeight);
	pEdit->SetWindowText(strValue);
	m_vParamEditCtrl.push_back(pEdit);
	if ((!m_bIfSemiauto) || m_bEnableOpenFolder)
	{
		pEdit->EnableWindow(false);
	}
	else
	{
		pEdit->EnableWindow(true);
	}

	rcName.top = rcName.bottom + m_nGap;
	rcEdit.top = rcName.top;
	rcName.bottom = rcName.top + nHeight;
	rcEdit.bottom = rcName.bottom;

	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcMsg;
	rcMsg.left = rcName.left;
	rcMsg.top = rcName.bottom + m_nGap;
	rcMsg.right = m_rcClient.right - m_nGap;
	rcMsg.bottom = rcMsg.top + int(96 * gSystemMgr.m_fRatioH);
	m_msgPanel.MoveWindow(&rcMsg);
}

#pragma region Message Handler
void CDebugDlg::OnPaint()
{
	CPaintDC dc(this);

	DisplayBackgroundImage(dc);

	CRect rcClient;
	GetClientRect(&rcClient);
	CPen* pOldPen = dc.SelectObject(&m_penTitle);

	CPoint p1, p2;
	p1.x = m_rcClient.left + 10;
	p1.y = m_nTitleLineHeight;
	p2 = p1;
	p2.x = m_rcClient.right - 10;

	dc.MoveTo(p1);
	dc.LineTo(p2);

	CPoint pl, pr;
	pl.x = m_rcImgWnd.right + m_nGap;
	pl.y = m_nTitleLineHeight + m_nGap;
	pr.x = pl.x;
	pr.y = m_rcClient.bottom - int(1.5*m_nGap);
	dc.MoveTo(pl.x, pl.y);
	dc.LineTo(pr.x, pr.y);

	CPoint ptBtmLeft, ptBtmRight;
	ptBtmLeft.x = m_rcClient.left + 10;
	ptBtmLeft.y = m_rcImgWnd.bottom + m_nGap;
	ptBtmRight.y = ptBtmLeft.y;
	ptBtmRight.x = pl.x - m_nGap;
	dc.MoveTo(ptBtmLeft);
	dc.LineTo(ptBtmRight);

	CPoint ptLeft, ptCenter, ptRight;
	ptCenter.y = m_rcCrtBtn.bottom;
	ptCenter.x = m_rcCrtBtn.left + int(m_rcCrtBtn.Width() / 2.0f);

	ptLeft.x = ptCenter.x - int(10 * gSystemMgr.m_fRatioH);
	ptLeft.y = m_nYPosLine0;

	ptRight.x = ptCenter.x + int(10 * gSystemMgr.m_fRatioH);
	ptRight.y = m_nYPosLine0;

	dc.MoveTo(m_nXPosLine2 + m_nGap, m_nYPosLine0);
	dc.LineTo(ptLeft);
	dc.MoveTo(ptLeft);
	dc.LineTo(ptCenter);
	dc.MoveTo(ptCenter);
	dc.LineTo(ptRight);
	dc.MoveTo(ptRight);
	dc.LineTo(p2.x, m_nYPosLine0);

	CPoint ptBgn, ptEnd, ptUp, ptDown;
	ptBgn.x = m_nXPosLine2;
	ptBgn.y = m_nTitleLineHeight + m_nGap;
	ptEnd.x = m_nXPosLine2;
	ptEnd.y = m_rcClient.bottom - int(1.5*m_nGap);
	int nHalf = int(m_rcCrtPatternWidget.Height() / 2.0f);
	ptCenter.x = m_rcCrtPatternWidget.right + 5;
	ptCenter.y = m_rcCrtPatternWidget.top + nHalf;
	ptUp.x = ptBgn.x;
	ptUp.y = m_rcCrtPatternWidget.top + int(m_nPatternCtrlHeight / 2.0f - (m_nXPosLine2 - m_rcCrtPatternWidget.right)) + 5;
	ptDown.x = ptBgn.x;
	ptDown.y = m_rcCrtPatternWidget.bottom - int(m_nPatternCtrlHeight / 2.0f - (m_nXPosLine2 - m_rcCrtPatternWidget.right)) - 5;
	dc.MoveTo(ptBgn);
	dc.LineTo(ptUp);
	dc.MoveTo(ptUp);
	dc.LineTo(ptCenter);
	dc.MoveTo(ptCenter);
	dc.LineTo(ptDown);
	dc.MoveTo(ptDown);
	dc.LineTo(ptEnd);
	CPoint ptDownLeft, ptDownRight;
	ptDownLeft.x = rcClient.left + m_nGap;
	ptDownLeft.y = m_nUnderLine;
	ptDownRight.x= rcClient.right -int(5 * gSystemMgr.m_fRatioH);
	ptDownRight.y = m_nUnderLine;
	dc.MoveTo(ptDownLeft);
	dc.LineTo(ptDownRight);
	dc.SelectObject(pOldPen);
}

HBRUSH CDebugDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	int ctrlID = pWnd->GetDlgCtrlID();

	if (ctrlID == IDC_RADIO_CAMERA_1 ||
		ctrlID == IDC_RADIO_CAMERA_2 ||
        ctrlID == IDC_RADIO_CAMERA_3 ||
        ctrlID == IDC_RADIO_CAMERA_4 ||
        ctrlID == IDC_RADIO_CAMERA_5 ||
		ctrlID == IDC_CHECK_IMAGE_DEBUG ||
		ctrlID == IDC_CHECK_INDEPENDENT_GRAB ||
		ctrlID == IDC_CHECK_SAVE_IMG ||
		ctrlID == IDC_CHECKBOX_ONOFF)
	{
		pDC->SetTextColor(RGB(250, 250, 250));
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH brush = m_bgBrushHelper.GetHBRUSH(pWnd->m_hWnd);
		if (brush)
		{
			return brush;
		}
	}
	if (ctrlID == IDC_STATIC_XY)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 250, 250));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	if (ctrlID == IDC_STATIC_MESSAGE_PANEL)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (m_bMsgStatus)
		{
			pDC->SetTextColor(RGB(250, 250, 250));
		}
		else
		{
			pDC->SetTextColor(RGB(250, 10, 10));
			m_bMsgStatus = true;
		}
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CDebugDlg::CloseDebugDlg()
{
	m_ptrProcessor->m_bExeAllAlgo = true;
	m_ptrProcessor->SetDebugFlag(false);
	m_ptrProcessor->SetBatchMode(false);
	m_ptrProcessor->m_hDebugHandle = NULL;
	if (m_bDebugMode)
	{
		m_ptrProcessor->ResetInnerData();
	}
	long long nPatternNums = m_dictProjectPattern.size();
	for (int i = 0; i < nPatternNums; i++)
	{
		if (m_cfParamUpdate[i] == true)
		{
			gConfigFileMgr.UpdatevActionGroup(0, (*m_ptrDictPatternForCrtCamera)[i].m_vActionGroup, m_wstrCurtProjectName, i, m_wstrCrtCameraName);
			m_cfParamUpdate[i] = false;
		}
		//gConfigFileMgr.UpdateDeviceAction(m_nStationIndex, m_dictPatternForCrtProject[i].m_bSideLightOn, m_wstrCurtProjectName, i);
	}
	gConfigFileMgr.UpdateCameraHeight(m_wstrCurtProjectName, m_nCrtMainCameraHeight);
	gSystemMgr.UpdateCrtProjectPatterns(m_dictProjectPattern);
	for_each(m_vPatternWidgetPtr.begin(), m_vPatternWidgetPtr.end(), [&](CLabelCtrl* ptr)
	{
		delete ptr;
		ptr = nullptr;
	});
	for (auto it : m_vParamNameCtrl)
	{
		delete it;
		it = nullptr;
	}
	for (auto it : m_vParamEditCtrl)
	{
		delete it;
		it = nullptr;
	}

	m_vPatternWidgetPtr.clear();
	CDialogEx::OnOK();
}

void CDebugDlg::OnStnClickedClose()
{
	m_cImgWndDebug->DestroyWindow();
	delete m_cImgWndDebug;
	m_cImgWndDebug = NULL;
	CloseDebugDlg();
}

void CDebugDlg::OnStnClickedSaveSinglePicture()
{
	if (((!m_bIfSemiauto) && m_bLocked) || m_bIfSemiauto || m_bEnableOpenFolder)
	{
		if (m_vImage.size() > 0)
		{
			CString strFilter = _T("Image Files (*.bmp)|*.bmp|BMP File (*.tif)|*.tif|PNG File (*.png)|*.png|JPG File (*.jpg)|*.jpg|All Files (*.*)|*.*||");
			CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
			if (dlg.DoModal() == IDOK)
			{
				CString str_totalPath = dlg.GetPathName();
				str_totalPath += ".bmp";
				USES_CONVERSION;
				char* Filename = T2A(str_totalPath);
				if(m_crtImage.data)
				{
					////localUtils::utils::PrintCrtSymbol(m_crtImage, m_nStationIndex, m_nCrtPatternIndex, m_nCrtActionGpIndex);
					imwrite(Filename, m_crtImage);
				}
			}
		}
	}
}

void CDebugDlg::OnStnClickedSaveMultiplePicture()
{
	if (((!m_bIfSemiauto) && m_bLocked) || m_bIfSemiauto || m_bEnableOpenFolder)
	{
		if (m_vImage.size() > 0)
		{
			CString strFilter = _T("Image Files (*.bmp)|*.bmp|BMP File (*.tif)|*.tif|PNG File (*.png)|*.png|JPG File (*.jpg)|*.jpg|All Files (*.*)|*.*||");
			CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
			if (dlg.DoModal() == IDOK)
			{
				CString sPath = dlg.GetPathName();
				for (int i = 0; i < m_vImage.size(); i++)
				{
					CString sTmpPath = sPath;
					CString sFileName;
					sFileName.Format(_T("_%d.bmp"), i);
					sTmpPath += sFileName;
					USES_CONVERSION;
					char* Filename = T2A(sTmpPath);
					if(m_vImage[i].m_img.data)
					{
						imwrite(Filename, m_vImage[i].m_img);
					}
				}
			}
		}
	}
}

void CDebugDlg::OnStnClickedOneCircle()
{
	SetTimer(TIMER_ID_ONE_CIRCLE, 5000, NULL);
	//gSystemMgr.CreatePatternImageRepository();
}

void CDebugDlg::OneCirlce()
{
	if (m_bIfSemiauto && m_bStarted)
	{
		if (m_nOneCircleID == 0)
		{
			//gSystemMgr.m_motionCard.TurnOnSideLight(true);
		}
		if (m_nOneCircleID == 1)
		{
			//gSystemMgr.m_motionCard.TurnOnSideLight(false);
		}
		gSystemMgr.SetCurrentPatternIndex(m_nOneCircleID);
		OnStnClickedPatternWidgetByPicture(m_nOneCircleID);
		OnCbnSelchangeComboPatternActionByPicture(0);
		m_nOneCircleID++;
		if (m_nOneCircleID >= 19)
		{
			KillTimer(TIMER_ID_ONE_CIRCLE);
			m_nOneCircleID = 0;
		}
	}
}

void CDebugDlg::OnStnClickedMultipleProcess()
{
	if (m_vImage.size() <= 0 || m_bBatchMode)
		return;
	if (m_bEnableMultipleProcess)
	{
		m_bBatchMode = true;
		m_ptrProcessor->SetBatchMode(m_bBatchMode);
		
		m_ptrProcessor->m_bExeAllAlgo = true;
		m_crtImgIndex = 0;

		if (CheckImageIndex(m_crtImgIndex))
		{
			UpdateIndexPanel(m_crtImgIndex);
			ProcessCurrentImage();
		}
	}
	else
	{
		AfxMessageBox(_T("Please make sure all the pictures are taken by the same Station and same Camera！"), MB_OK | MB_ICONERROR);
	}
}

void CDebugDlg::EnableAllWidget(bool bEnable)
{
	if (bEnable)//Enable all widget（semiauto state）
	{
		m_btnSaveSinglePic.BringWindowToTop();
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMAGE_DEBUG);
		pBtn->EnableWindow(true);
		m_btnLock.SetLabelImage(IMG_PNG_UNLOCK_DENY);
		m_btnLock.Invalidate();
	
		if (m_bStarted)
		{
			m_btnOneCircle.SetLabelImage(IMG_PNG_ONE_CIRCLE);
			m_btnOneCircle.Invalidate();
		}
		
		ShowLastWidget(m_settingType);
	}
	else//Disable all widget （auto state）
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_IMAGE_DEBUG);
		m_bEnableOpenFolder = false;
		UpdateData(false);
		m_btnImageFolder.SetLabelImage(IMG_PNG_FOLDER_DISABLED);
		m_btnImageFolder.BringWindowToTop();
		m_btnImageFolder.Invalidate();
		pBtn->EnableWindow(false);
		m_btnLock.SetLabelImage(IMG_PNG_UNLOCK);
		m_btnLock.Invalidate();
		m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC_DENY);
		m_btnSaveSinglePic.Invalidate();
		m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC_DENY);
		m_btnSaveMultiplePic.Invalidate();
		m_btnOneCircle.SetLabelImage(IMG_PNG_ONE_CIRCLE_DENY);
		m_btnOneCircle.Invalidate();
		m_btnMultipleProcess.SetLabelImage(IMG_PNG_MULTIPLE_PROCESS_DENY);
		m_btnMultipleProcess.Invalidate();
		ShowLastWidget(m_settingType);
	}
}

void CDebugDlg::OnStnClickedAutoSemiauto()
{
	if (m_bIfSemiauto)
	{
		m_bIfSemiauto = !m_bIfSemiauto;
		m_btnAutoSemiauto.SetLabelImage(IMG_PNG_AUTO);
		m_btnAutoSemiauto.BringWindowToTop();
		m_btnAutoSemiauto.EnableWindow(TRUE);
		m_btnAutoSemiauto.Invalidate();
		//TODO:
		EnableAllWidget(m_bIfSemiauto);
	}
	else
	{
		m_bIfSemiauto = !m_bIfSemiauto;
		m_btnAutoSemiauto.SetLabelImage(IMG_PNG_SEMIAUTO);
		m_btnAutoSemiauto.BringWindowToTop();
		m_btnAutoSemiauto.EnableWindow(TRUE);
		m_btnAutoSemiauto.Invalidate();
		EnableAllWidget(m_bIfSemiauto);
	}
}

void CDebugDlg::OnStnClickedFolder()
{
	if (m_bEnableOpenFolder)
	{
		TCHAR  moduleFileName[MAX_PATH];
		::GetModuleFileName(NULL, moduleFileName, MAX_PATH);
		CString imgFolder = moduleFileName;
		int index = imgFolder.ReverseFind('\\');
		imgFolder.Truncate(index + 1);
		imgFolder.Append(_T("pictures\\"));
		if (::GetFileAttributes(imgFolder) == INVALID_FILE_ATTRIBUTES)
		{
			::CreateDirectory(imgFolder, NULL);
		}
		CString strFilter = _T("Image Files (*.bmp)|*.bmp|BMP File (*.tif)|*.tif|PNG File (*.png)|*.png|JPG File (*.jpg)|*.jpg|All Files (*.*)|*.*||");
		CFileDialog dlgFile(TRUE, _T("bmp"), imgFolder, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, strFilter, NULL,0,TRUE);
		const unsigned long numberOfFileName = 256;
		const unsigned long fileNameMaxLen = 128;
		const unsigned long bufferSz = (numberOfFileName * fileNameMaxLen) + 1;
		std::vector<TCHAR> fileNameBuffer(bufferSz, 0);
		dlgFile.m_ofn.lpstrFile = &fileNameBuffer[0];
		dlgFile.m_ofn.nMaxFile = bufferSz;

		if (dlgFile.DoModal() == IDOK)
		{
			m_vImage.clear();
			m_crtImgIndex = -1;
			POSITION pos = dlgFile.GetStartPosition();
			CString imgFile;
			Mat     crtImg;
			m_nImgCount = 0;
			while (pos)
			{
				imgFile = dlgFile.GetNextPathName(pos);
				wstring wstrTmpFile = imgFile.GetString();
				string str = localUtils::utils::Unicode2Ansi(wstrTmpFile);
				crtImg = imread(str.c_str(), IMREAD_UNCHANGED);
				cv::Size2i sz = crtImg.size();
				if(sz==m_vImgSize[0]||sz== m_vImgSize[1])
				{
					bool bMainCamera = ((sz == m_vImgSize[0]) ? true : false);
					IMG_PACKAGE imgPac(crtImg.clone(),L"", bMainCamera);
					imgPac.m_bDebugFlag = true;
					m_vImage.push_back(imgPac);
				}
				else
				{
					gSystemLogger.QueueUpLogAndReport(L"not available image size !", LV_ERROR);
					gSystemLogger.FlushLogCache();
				}
			}
			m_nImgCount = (int)m_vImage.size();
			if (m_nImgCount <= 0)
				return;
			else
			{
				m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC);
				m_btnSaveSinglePic.Invalidate();
				m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC);
				m_btnSaveMultiplePic.Invalidate();
				m_btnMultipleProcess.SetLabelImage(IMG_PNG_MULTIPLE_PROCESS);
				m_btnMultipleProcess.Invalidate();
			}

			m_ptrProcessor->ResetInnerData();
			m_bDebugMode = true;
			m_crtImgIndex = 0;
			m_ptrProcessor->SetDebugFlag(m_bDebugMode);
			m_ptrProcessor->m_bExeAllAlgo = true;
			if (CheckImageIndex(m_crtImgIndex))
			{
				UpdateIndexPanel(m_crtImgIndex);
				ProcessCurrentImage();
			}
			m_bEnableMultipleProcess = true;
			std::set<int> setStationIndex;
			for (int i = 0; i < m_nImgCount; i++)
			{
				int nStationIndex = -1;
				int nPatternIndex = -1;
				int nActionGpIndex = -1;
				bool bMarked = false;
				////localUtils::utils::ParseImgSymbol(m_vImage[i], nStationIndex, nPatternIndex, nActionGpIndex, bMarked);
				bool bIsDebuged = (((nStationIndex == 0 || nStationIndex == 1)) && (nPatternIndex >= 0) && (nActionGpIndex == 0));
				if (bIsDebuged && nStationIndex != -1 && bMarked)
				{
					setStationIndex.insert(nStationIndex);
					if (setStationIndex.size() >= 2)
					{
						m_bEnableMultipleProcess = false;
						break;
					}
				}
			}
		}
	}
}

void CDebugDlg::OnStnClickedFirst()
{
	if (m_crtImgIndex == 0 || m_nImgCount == 0)
		return;

	m_ptrProcessor->m_bExeAllAlgo = true;
	m_crtImgIndex = 0;

	if (CheckImageIndex(m_crtImgIndex))
	{
		UpdateIndexPanel(m_crtImgIndex);
		ProcessCurrentImage();
	}
}

void CDebugDlg::OnStnClickedPrevious()
{
	if (m_nImgCount == 0 || m_crtImgIndex > m_nImgCount)
		return;

	m_ptrProcessor->m_bExeAllAlgo = true;
	m_crtImgIndex--;
	if (CheckImageIndex(m_crtImgIndex))
	{
		UpdateIndexPanel(m_crtImgIndex);
		ProcessCurrentImage();
	}
	else
		m_crtImgIndex++;
}

void CDebugDlg::OnStnClickedNext()
{
	if (m_nImgCount == 0 || m_crtImgIndex >= m_nImgCount - 1)
		return;

	m_ptrProcessor->m_bExeAllAlgo = true;
	m_crtImgIndex++;
	if (CheckImageIndex(m_crtImgIndex))
	{
		UpdateIndexPanel(m_crtImgIndex);
		ProcessCurrentImage();
	}
	else
		m_crtImgIndex--;
}

void CDebugDlg::OnStnClickedLast()
{
	if (m_nImgCount == 0 || m_crtImgIndex == m_nImgCount - 1)
		return;

	m_ptrProcessor->m_bExeAllAlgo = true;
	m_crtImgIndex = m_nImgCount - 1;
	if (CheckImageIndex(m_crtImgIndex))
	{
		UpdateIndexPanel(m_crtImgIndex);
		ProcessCurrentImage();
	}
}

void CDebugDlg::OnStnClickedLock()
{
	if (!m_bIfSemiauto)
	{
		if (m_bLocked)
		{
			m_bDebugMode = false;
			m_ptrProcessor->SetDebugFlag(m_bDebugMode);
			m_bLocked = !m_bLocked;
			m_btnLock.SetLabelImage(IMG_PNG_UNLOCK);
			m_btnLock.BringWindowToTop();
			m_btnLock.Invalidate();
			m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC_DENY);
			m_btnSaveSinglePic.Invalidate();
			m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC_DENY);
			m_btnSaveMultiplePic.Invalidate();
		}
		else
		{
			//TODO:
			std::vector<IMG_PACKAGE> vRetImgPac;
			m_ptrProcessor->GetImageFromCisternBuffer(vRetImgPac);
			m_vImage.clear();
			m_crtImgIndex = -1;
			for (auto it = vRetImgPac.begin(); it != vRetImgPac.end(); it++)
			{
				IMG_PACKAGE& imgPac = *it;
				m_vImage.push_back(imgPac);
			}
			////
			m_nImgCount = (int)m_vImage.size();
			if (m_nImgCount > 0)
			{
				m_ptrProcessor->ResetInnerData();
				m_bDebugMode = true;
				m_crtImgIndex = 0;
				if (CheckImageIndex(m_crtImgIndex))
				{
					UpdateIndexPanel(m_crtImgIndex);
					ProcessCurrentImage();
				}
			}

			m_bLocked = !m_bLocked;
			m_bDebugMode = true;
			m_ptrProcessor->SetDebugFlag(m_bDebugMode);
			m_btnLock.SetLabelImage(IMG_PNG_LOCK);
			m_btnLock.BringWindowToTop();
			m_btnLock.Invalidate();
			m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC);
			m_btnSaveSinglePic.Invalidate();
			m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC);
			m_btnSaveMultiplePic.Invalidate();
		}
	}
}

void CDebugDlg::OnStnClickedAlgorithm()
{
	//TODO:
	m_settingType = T_ALGO;
	m_rcCrtBtn = m_rcAlgoBtn;
	DeleteAllSettingWidget();
	//InvalidateRect(&m_rcNavigateZone);
	InvalidateRect(&m_rcLineZone);
}

void CDebugDlg::OnStnClickedCamera()
{
	m_settingType = T_CAMERA;
	m_rcCrtBtn = m_rcCameraBtn;
	DeleteAllSettingWidget();
	if (m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.find(m_wstrCrtCameraName) != m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.end())
	{
		CreateCameraParamItems();
	}
	m_btnApply.EnableWindow(false);
	InvalidateRect(&m_rcLineZone);
}

void CDebugDlg::OnStnClickedDrawRectangle()
{
	if (m_cImgWndDebug&&m_cImgWndDebug->GetSafeHwnd())
	{
		if (m_cImgWndDebug->CheckImageLoad()&& m_cImgWndDebug->GetROINumber()<=m_nROINum)
		{
			m_cImgWndDebug->AddSelectROI(ROI_RECTANGLE);
		}
	}
}

void CDebugDlg::ShowLastWidget(SETTING_TYPE tLastType)
{
	switch (tLastType)
	{
	case T_INVALID:
		break;
	case T_ALGO:
		OnStnClickedAlgorithm();
		break;
	case T_CAMERA:
		OnStnClickedCamera();
		break;
	case T_PROJECT:
		break;
	default:
		break;
	}
}

void CDebugDlg::CreateAutoTestingWidget()
{
	CWnd* pWidget = GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER);
	pWidget->SetFont(&m_smallFont);

	CString strName;
	strName.LoadStringW(IDS_STRING_SAVE_IMG);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_SAVE_IMG);
	pBtn->SetFont(&m_smallFont);
	pBtn->SetWindowTextW(strName);
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");

	strName.LoadStringW(IDS_STRING_INDEPENDENT_GRAB);
    pBtn = (CButton*)GetDlgItem(IDC_CHECK_INDEPENDENT_GRAB);
	pBtn->SetFont(&m_smallFont);
	pBtn->SetWindowTextW(strName);
	m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
	SetWindowTheme(pBtn->m_hWnd, L"", L"");
}

void CDebugDlg::ShowAutoTestingWidget(bool bShow)
{
	int nFlag = bShow ? SW_SHOW : SW_HIDE;
	CWnd* pWidget = GetDlgItem(IDC_CHECK_INDEPENDENT_GRAB);
	pWidget->ShowWindow(nFlag);

	pWidget = GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER);
	pWidget->ShowWindow(nFlag);

    pWidget = GetDlgItem(IDC_CHECK_SAVE_IMG);
	pWidget->ShowWindow(nFlag);
}

void CDebugDlg::OnBnClickedRadioStationByPicture(int nStationIndex)
{
	if (m_nCameraIndex == nStationIndex)
	{
		return;
	}
	m_nCameraIndex = nStationIndex;
	UpdateData(false);
	ResetInternalData();
	RetrieveInternalData(m_nCameraIndex);
}

void CDebugDlg::OnCbnSelchangeComboPatternActionByPicture(int nActionGpIndex)
{
	if (m_nCrtActionGpIndex == nActionGpIndex)
	{
		return;
	}
	m_comboPatternAction.SetCurSel(nActionGpIndex);
	UpdateData(false);
	m_nCrtActionGpIndex = nActionGpIndex;
	if (m_ptrProcessor)
	{
		m_ptrProcessor->SetCurrentActionGroupIndex(m_nCrtActionGpIndex);
	}
	m_ptrCrtActionGroup = &(*m_ptrCrtFramePattern).m_vActionGroup[nActionGpIndex];
	m_listAlgo.DeleteAllItems();
	DeleteAllSettingWidget();
	AddFunction2List();
	ShowLastWidget(m_settingType);
}

void CDebugDlg::OnCbnSelchangeComboPatternAction()
{
	int nOldActionIndex = m_nCrtActionGpIndex;
	UpdateData(TRUE);
	int nIndex = m_comboPatternAction.GetCurSel();
	if (nIndex == nOldActionIndex)
	{
		return;
	}
	m_nCrtActionGpIndex = nIndex;
	if (m_ptrProcessor)
	{
		m_ptrProcessor->SetCurrentActionGroupIndex(m_nCrtActionGpIndex);
	}
	m_ptrCrtActionGroup = &(*m_ptrCrtFramePattern).m_vActionGroup[nIndex];
	m_listAlgo.DeleteAllItems();
	DeleteAllSettingWidget();
	AddFunction2List();
	ShowLastWidget(m_settingType);
}

void CDebugDlg::OnNMClickListAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_settingType = T_ALGO;
	m_rcCrtBtn = m_rcAlgoBtn;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nCrtIndex = pNMItemActivate->iItem;
	if (nCrtIndex == -1)
	{
		DeleteAllSettingWidget();
		m_nCrtAlgoIndex = -1;
		m_ptrCrtSelectedFunc = nullptr;
		*pResult = 0;
		return;
	}

	m_nCrtAlgoIndex = nCrtIndex;
	DeleteAllSettingWidget();
	m_ptrCrtSelectedFunc = &(*m_ptrCrtFuncArray)[m_nCrtAlgoIndex];
	CreateAlgoParamItems();
	InvalidateRect(&m_rcNavigateZone);

	*pResult = 0;
}

void CDebugDlg::OnNMDblclkListAlgo(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_ptrProcessor->m_bExeAllAlgo = false;
	m_settingType = T_ALGO;
	m_rcCrtBtn = m_rcAlgoBtn;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nCrtIndex = pNMItemActivate->iItem;
	if (nCrtIndex == -1)
	{
		DeleteAllSettingWidget();
		m_nCrtAlgoIndex = -1;
		m_ptrCrtSelectedFunc = nullptr;
		*pResult = 0;
		return;
	}
	if (nCrtIndex != m_nCrtAlgoIndex)
	{
		m_nCrtAlgoIndex = nCrtIndex;
		DeleteAllSettingWidget();
		m_ptrCrtSelectedFunc = &(*m_ptrCrtFuncArray)[m_nCrtAlgoIndex];
		CreateAlgoParamItems();
		InvalidateRect(&m_rcNavigateZone);
	}
	if (m_bDebugMode)
	{
		ProcessCurrentImage();
	}

	*pResult = 0;
}

void CDebugDlg::OnEnSetfocusCameraEditParameter()
{
	size_t sz = m_vParamEditCtrl.size();
	if (sz <= 0)
	{
		return;
	}
	KillTimer(TIMER_ID_FOR_EDIT_CTRL);
	SetTimer(TIMER_ID_FOR_EDIT_CTRL, 30000, NULL);

	CRect rc;
	m_ptrCrtEditCtrl = CWnd::GetFocus();
	if (m_ptrCrtEditCtrl != nullptr)
	{
		m_ptrCrtEditCtrl->GetWindowRect(rc);
		int nBtnWidth = (m_rcParamBoard.right - rc.right - 3 * 5) / 2;
		if (nBtnWidth > 45)
		{
			nBtnWidth = 45;
		}	
		CRect rcBtn;
		rcBtn.left = rc.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		rcBtn.top = rc.top;
		rcBtn.bottom = rc.bottom;
		m_btnDecrease.MoveWindow(rcBtn);
		m_btnDecrease.ShowWindow(SW_SHOW);


		rcBtn.left = rcBtn.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		m_btnIncrease.MoveWindow(rcBtn);
		m_btnIncrease.ShowWindow(SW_SHOW);
	}
}

void CDebugDlg::OnEnSetfocusLightEditParameter()
{
	size_t sz = m_vParamEditCtrl.size();
	if (sz <= 0)
	{
		return;
	}
	KillTimer(TIMER_ID_FOR_EDIT_CTRL);
	SetTimer(TIMER_ID_FOR_EDIT_CTRL, 30000, NULL);

	CRect rc;
	m_ptrCrtEditCtrl = CWnd::GetFocus();
	if (m_ptrCrtEditCtrl != nullptr)
	{
		m_ptrCrtEditCtrl->GetWindowRect(rc);
		int nBtnWidth = (m_rcParamBoard.right - rc.right - 3 * 5) / 2;
		if (nBtnWidth > 45)
			nBtnWidth = 45;

		CRect rcBtn;
		rcBtn.left = rc.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		rcBtn.top = rc.top;
		rcBtn.bottom = rc.bottom;
		m_btnDecrease.MoveWindow(rcBtn);
		m_btnDecrease.ShowWindow(SW_SHOW);


		rcBtn.left = rcBtn.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		m_btnIncrease.MoveWindow(rcBtn);
		m_btnIncrease.ShowWindow(SW_SHOW);
	}
}

void CDebugDlg::OnEnSetfocusMotionEditParameter()
{
	size_t sz = m_vParamEditCtrl.size();
	if (sz <= 0)
	{
		return;
	}
	KillTimer(TIMER_ID_FOR_EDIT_CTRL);
	SetTimer(TIMER_ID_FOR_EDIT_CTRL, 30000, NULL);

	CRect rc;
	m_ptrCrtEditCtrl = CWnd::GetFocus();
	if (m_ptrCrtEditCtrl != nullptr)
	{
		m_ptrCrtEditCtrl->GetWindowRect(rc);
		int nBtnWidth = (m_rcParamBoard.right - rc.right - 3 * 5) / 2;
		if (nBtnWidth > 45)
			nBtnWidth = 45;

		CRect rcBtn;
		rcBtn.left = rc.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		rcBtn.top = rc.top;
		rcBtn.bottom = rc.bottom;
		m_btnDecrease.MoveWindow(rcBtn);
		m_btnDecrease.ShowWindow(SW_SHOW);


		rcBtn.left = rcBtn.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		m_btnIncrease.MoveWindow(rcBtn);
		m_btnIncrease.ShowWindow(SW_SHOW);
	}
}

void CDebugDlg::OnEnSetfocusEditParameter()
{
	size_t sz = m_vParamEditCtrl.size();
	if (sz <= 0)
	{
		return;
	}
	KillTimer(TIMER_ID_FOR_EDIT_CTRL);
	SetTimer(TIMER_ID_FOR_EDIT_CTRL, 30000, NULL);

	CRect rc;
	m_ptrCrtEditCtrl = CWnd::GetFocus();
	if (m_ptrCrtEditCtrl != nullptr)
	{
		m_ptrCrtEditCtrl->GetWindowRect(rc);
		int nBtnWidth = (m_rcParamBoard.right - rc.right - 3 * 4) / 2;
		if (nBtnWidth > 40)
			nBtnWidth = 40;
		CRect rcBtn;
		rcBtn.left = rc.right + m_nGap;
		rcBtn.right = rcBtn.left + nBtnWidth;
		rcBtn.top = rc.top;
		rcBtn.bottom = rc.bottom;
		m_btnDecrease.MoveWindow(rcBtn);
		m_btnDecrease.ShowWindow(SW_SHOW);

		rcBtn.left = rcBtn.right + m_nGap/2;
		rcBtn.right = rcBtn.left + nBtnWidth;
		m_btnIncrease.MoveWindow(rcBtn);
		m_btnIncrease.ShowWindow(SW_SHOW);

		// Display Parameter Help String 
		auto it = std::find(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), m_ptrCrtEditCtrl);
		if (it == m_vParamEditCtrl.end())
		{
			return;
		}
		m_msgPanel.SetWindowText(_T(""));

		int nDis = (int)std::distance(m_vParamEditCtrl.begin(), it);
		if (nDis >= 0 && nDis < (int)m_ptrCrtSelectedFunc->m_vParam.size())
		{
			wstring strHelp = m_ptrCrtSelectedFunc->m_vParam[nDis].m_wstrDescription;
			m_msgPanel.SetWindowText(strHelp.c_str());
		}
		CRect rcPanel;
		m_msgPanel.GetWindowRect(&rcPanel);
		InvalidateRect(&rcPanel);
	}
}

void CDebugDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_FOR_EDIT_CTRL)
	{
		KillTimer(TIMER_ID_FOR_EDIT_CTRL);
		m_msgPanel.SetWindowTextW(_T(""));
		CRect rcPanel;
		m_msgPanel.GetWindowRect(&rcPanel);
		InvalidateRect(&rcPanel);
		m_listAlgo.GetFocus();
	}
	if (nIDEvent == TIMER_ID_DEBUG_SCREEN)
	{
		KillTimer(TIMER_ID_DEBUG_SCREEN);
		m_msgPanel.SetWindowTextW(_T(""));
		CRect rcPanel;
		m_msgPanel.GetWindowRect(&rcPanel);
		InvalidateRect(&rcPanel);
	}
	if (nIDEvent == TIMER_ID_FOR_DECREASE_BUTTON)
	{
		OnStnClickedStaticBtnDecrease();
	}
	if (nIDEvent == TIMER_ID_FOR_INCREASE_BUTTON)
	{
		OnStnClickedStaticBtnIncrease();
	}
	if (nIDEvent == TIMER_ID_ONE_CIRCLE)
	{
		OneCirlce();
	}
	if (nIDEvent == TIMER_ID_FOR_DISPLAYA_AXIS_POS)
	{
		//DispAxisPosition();
	}
	if (nIDEvent == TIMER_ID_DEBUG_GRAB_CONTINUOUS)
	{
		KillTimer(TIMER_ID_DEBUG_GRAB_CONTINUOUS);
		DebugGetSingleImg();
		DWORD ret = WaitForSingleObject(gSystemMgr.m_hManualGrabOKFlag, 3000);
		if (WAIT_TIMEOUT != ret)
		{
			SetTimer(TIMER_ID_DEBUG_GRAB_CONTINUOUS, 200, NULL);
		}	
		else
		{
			gSystemLogger.QueueUpLogAndReport(L"camera capture time out!",LV_ERROR);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

#pragma endregion

void CDebugDlg::AdjustParameterValue(bool bIncrease)
{
	if (m_ptrCrtSelectedFunc == nullptr || m_vParamEditCtrl.size() <= 0)
	{
		return;
	}

	auto it = std::find(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), m_ptrCrtEditCtrl);
	if (it == m_vParamEditCtrl.end())
	{
		return;
	}
	m_btnApply.EnableWindow(TRUE);
	std::vector<FUNC_PARAM>& vParam = m_ptrCrtSelectedFunc->m_vParam;
	long long nIndex = std::distance(m_vParamEditCtrl.begin(), it);
	FUNC_PARAM& param = vParam[nIndex];
	CString sValue;
	m_ptrCrtEditCtrl->GetWindowTextW(sValue);
	wstring wstrValue = sValue.GetString();
	if (!localUtils::utils::IsNumericString(sValue))
	{
		//TODO:
		return;
	}
	float fValue = boost::lexical_cast<float>(wstrValue);
	if (bIncrease)
	{
		fValue += param.m_step;
	}
	else
	{
		fValue -= param.m_step;
	}
	if (fValue < param.m_min || fValue > param.m_max)
	{
		CString sMinMax, sMsg;
		sMsg.LoadStringW(IDS_STRING_PARAM_VALUE_OUT_RANGE);
		if (param.m_type == VT_R4)
		{
			sMinMax.Format(_T("%.1f~%.1f"), param.m_min, param.m_max);
		}
		else
		{
			sMinMax.Format(_T("%d~%d"), (int)param.m_min, (int)param.m_max);
		}
		sMsg.Replace(_T("#~#"), sMinMax);
		DisplayMessage(sMsg, false);
		return;
	}
	else
	{
		param.m_value = fValue;
		if (param.m_type == VT_R4)
		{
			if (param.m_step >= 0.5f)
			{
				sValue.Format(_T("%.1f"), fValue);
			}
			else
			{
				sValue.Format(_T("%.2f"), fValue);
			}
		}
		else
		{
			sValue.Format(_T("%d"), (int)fValue);
		}
		m_ptrCrtEditCtrl->SetWindowText(sValue);
		m_ptrCrtSelectedFunc->CreateComParamArray();
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
		m_ptrProcessor->m_bExeAllAlgo = false;
		ProcessCurrentImage();
	}
}

void CDebugDlg::AdjustCameraParameterValue(bool bIncrease)
{
	auto it = std::find(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), m_ptrCrtEditCtrl);
	if (it == m_vParamEditCtrl.end())
	{
		return;
	}
	long long nIndex = std::distance(m_vParamEditCtrl.begin(), it);
	CString sValue;
	m_ptrCrtEditCtrl->GetWindowTextW(sValue);
	wstring wstrValue = sValue.GetString();
	if (!localUtils::utils::IsNumericString(sValue))
	{
		//TODO:
		return;
	}
	int nValue = boost::lexical_cast<int>(wstrValue);
	if (bIncrease)
	{
		nValue += 1;
	}
	else
	{
		nValue -= 1;
	}
	sValue.Format(_T("%d"), nValue);
	m_ptrCrtEditCtrl->SetWindowText(sValue);
	if (nIndex == 0)//exposure time
	{
		m_ptrCrtActionGroup->m_nExposureTime = nValue;
		m_nIndieExpoTime = nValue;
		m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nExposureTime = nValue;
		m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern,PARA_EXPOSURE);
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_EXPOSURE);
		m_ptrProcessor->m_ptrCamera->SetExposureTime(nValue);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
	}
	else if (nIndex == 1)//gain
	{
		m_ptrCrtActionGroup->m_nGain = nValue;
		m_nIndieGain = nValue;
		m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nGain = nValue;
		m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_GAIN);
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_GAIN);
		m_ptrProcessor->m_ptrCamera->SetTapGain(nValue);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
	}
	//else if (nIndex == 2)//delay
	//{
		////TODO:
		//m_nIndieDelayTime = 0;
		//m_cfParamUpdate[m_nCrtPatternIndex] = true;
	//}
	else if (nIndex == 2)//white balance r
	{
		m_ptrCrtActionGroup->m_nWhiteBalanceR = nValue;
		m_nIndieWhiteBalanceR = nValue;
		m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceR = nValue;
		m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_R);
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_R);
		m_ptrProcessor->m_ptrCamera->SetWhiteBalance(nValue, m_ptrCrtActionGroup->m_nWhiteBalanceG, m_ptrCrtActionGroup->m_nWhiteBalanceB);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
	}
	else if (nIndex == 3)//white balance g
	{
		m_ptrCrtActionGroup->m_nWhiteBalanceG = nValue;
		m_nIndieWhiteBalanceG = nValue;
		m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceG = nValue;
		m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_G);
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_G);
		m_ptrProcessor->m_ptrCamera->SetWhiteBalance(m_ptrCrtActionGroup->m_nWhiteBalanceR, nValue, m_ptrCrtActionGroup->m_nWhiteBalanceB);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
	}
	else if (nIndex == 4)//white balance b
	{
		m_ptrCrtActionGroup->m_nWhiteBalanceB = nValue;
		m_nIndieWhiteBalanceB = nValue;
		m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA[m_wstrCrtCameraName][m_nCrtActionGpIndex].m_nWhiteBalanceB = nValue;
		m_ptrProcessor->UpdateDictIndexPatternDebug(m_nCameraIndex, m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_B);
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, PARA_WHITE_BALANCE_B);
		m_ptrProcessor->m_ptrCamera->SetWhiteBalance(m_ptrCrtActionGroup->m_nWhiteBalanceR, m_ptrCrtActionGroup->m_nWhiteBalanceG,nValue);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
	}
}

void CDebugDlg::AdjustLightParameterValue(bool bIncrease)
{
	auto it = std::find(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), m_ptrCrtEditCtrl);
	if (it == m_vParamEditCtrl.end())
	{
		return;
	}
	long long nIndex = std::distance(m_vParamEditCtrl.begin(), it);
	CString sValue;
	m_ptrCrtEditCtrl->GetWindowTextW(sValue);
	wstring wstrValue = sValue.GetString();
	if (!localUtils::utils::IsNumericString(sValue))
	{
		//TODO:
		return;
	}
	int fValue = boost::lexical_cast<int>(wstrValue);
	if (bIncrease)
	{
		fValue += 1;
	}
	else
	{
		fValue -= 1;
	}
	sValue.Format(_T("%d"), (int)fValue);
	m_ptrCrtEditCtrl->SetWindowText(sValue);
}

void CDebugDlg::AdjustMotionParameterValue(bool bIncrease)
{
	auto it = std::find(m_vParamEditCtrl.begin(), m_vParamEditCtrl.end(), m_ptrCrtEditCtrl);
	if (it == m_vParamEditCtrl.end())
	{
		return;
	}
	long long nIndex = std::distance(m_vParamEditCtrl.begin(), it);
	CString sValue;
	m_ptrCrtEditCtrl->GetWindowTextW(sValue);
	wstring wstrValue = sValue.GetString();
	if (!localUtils::utils::IsNumericString(sValue))
	{
		//TODO:
		return;
	}
	int fValue = boost::lexical_cast<int>(wstrValue);
	if (bIncrease)
	{
		fValue += 1;
	}
	else
	{
		fValue -= 1;
	}
	sValue.Format(_T("%d"), (int)fValue);
	m_ptrCrtEditCtrl->SetWindowText(sValue);

	if (nIndex == 0)//CameraHeight
	{
		m_nCrtMainCameraHeight = fValue;
		m_ptrStation->SetStationConfig(m_wstrCurtProjectName, m_nCrtPatternIndex, m_wstrCrtCameraName, m_nCrtActionGpIndex, m_dictProjectPattern, m_nCrtMainCameraHeight);
		m_cfParamUpdate[m_nCrtPatternIndex] = true;
		//gSystemMgr.m_motionCard.MoveAxis(fValue);
		DisplayAxisPosition(fValue);
	}
}

void CDebugDlg::DisplayAxisPosition(int nFinalPosition)
{
	SetTimer(TIMER_ID_FOR_DISPLAYA_AXIS_POS, 300, NULL);
}

void CDebugDlg::DisplayMessage(CString sMsg, bool bStatus)
{
	m_msgPanel.SetWindowText(sMsg);
	m_bMsgStatus = bStatus;
	CRect rcPanel;
	m_msgPanel.GetWindowRect(&rcPanel);
	InvalidateRect(&rcPanel);
	SetTimer(TIMER_ID_DEBUG_SCREEN, 5000, NULL);
}

void CDebugDlg::ApplyNewAlgoParam()
{
	if (m_ptrProcessor && m_ptrCrtSelectedFunc)
	{
		m_ptrProcessor->ApplyNewParam(m_nCrtPatternIndex, m_nCrtActionGpIndex, m_ptrCrtSelectedFunc->m_funcIndex);
	}
}

void CDebugDlg::ResetInternalData()
{
	if (!m_bEnableOpenFolder)
	{
		m_vImage.clear();
		m_nImgCount = 0;
		m_crtImgIndex = -1;
		UpdateIndexPanel(m_crtImgIndex);
	}
	m_settingType = T_ALGO;
	m_rcCrtBtn = m_rcAlgoBtn;
	m_ptrProcessor->SetDebugFlag(m_bDebugMode);
	m_nCrtActionGpIndex = 0;
	InvalidateRect(&m_rcNavigateZone);

	m_btnDecrease.ShowWindow(SW_HIDE);
	m_btnIncrease.ShowWindow(SW_HIDE);

	m_bCheckImgSave = FALSE;
	
	ShowAutoTestingWidget(false);

	UpdateData(FALSE);
}

void CDebugDlg::UpdateParamOfCurrentFunction()
{
	auto sz = m_vParamEditCtrl.size();
	if (m_ptrCrtSelectedFunc == nullptr || sz <= 0 || m_ptrCrtSelectedFunc->m_vParam.size() != sz)
	{
		return;
	}
	CString sValue;
	std::vector<FUNC_PARAM> vOldParam = m_ptrCrtSelectedFunc->m_vParam;
	for (auto t = 0; t < sz; t++)
	{
		auto ptrEdit = m_vParamEditCtrl[t];
		FUNC_PARAM oldParam = vOldParam[t];
		ptrEdit->GetWindowTextW(sValue);
		float fValue = 0.0f;
		if (!localUtils::utils::IsNumericString(sValue))
		{
			//TODO: Message box here
			continue;
		}
		fValue = localUtils::utils::to_float(sValue);
		if (oldParam.m_value != fValue)
		{
			if (fValue < oldParam.m_min || fValue > oldParam.m_max)
			{
				CString sMinMax, sMsg;
				sMsg.LoadStringW(IDS_STRING_PARAM_VALUE_OUT_RANGE);
				if (oldParam.m_type == VT_R4)
				{
					sMinMax.Format(_T("%.1f~%.1f"), oldParam.m_min, oldParam.m_max);
				}
				else
				{
					sMinMax.Format(_T("%d~%d"), (int)oldParam.m_min, (int)oldParam.m_max);
				}
				sMsg.Replace(_T("#~#"), sMinMax);
				DisplayMessage(sMsg, false);
				//Restore to original value;
				if (oldParam.m_type == VT_R4)
				{
					sValue.Format(_T("%.1f"), oldParam.m_value);
				}
				else
				{
					sValue.Format(_T("%d"), (int)oldParam.m_value);
				}
				ptrEdit->SetWindowTextW(sValue);
				return;
			}
			m_ptrCrtSelectedFunc->m_vParam[t].m_value = fValue;
			m_cfParamUpdate[m_nCrtPatternIndex] = true;
			//m_bAnyPendingParam = true;
			m_btnApply.EnableWindow(TRUE);
		}
	}

	if (m_ptrProcessor && m_ptrCrtSelectedFunc)
	{
		m_ptrProcessor->ApplyNewParam(m_nCrtPatternIndex, m_nCrtActionGpIndex, m_ptrCrtSelectedFunc->m_funcIndex);

	}
}

void CDebugDlg::InitializeActionGroupWidget()
{
	if (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end())
	{
		m_ptrCrtFramePattern = &(*m_ptrDictPatternForCrtCamera)[m_nCrtPatternIndex];
		if (m_ptrCrtFramePattern->m_vActionGroup.empty())
		{
			//TODO:log this internal error;
			return;
		}
	}
	else
	{
		return;
	}
	//Shoot Combo box;
	m_comboPatternAction.ResetContent();
	for (auto t = 0; t < m_ptrCrtFramePattern->m_vActionGroup.size(); t++)
	{
		CString str;
		str.Format(_T("Shoot#%d"), t + 1);
		m_comboPatternAction.InsertString(t, str);
	}
	m_comboPatternAction.SetCurSel(0);
	m_nCrtActionGpIndex = 0;
	if (m_ptrProcessor)
	{
		m_ptrProcessor->SetCurrentActionGroupIndex(m_nCrtActionGpIndex);
	}
	m_ptrCrtActionGroup = &(*m_ptrCrtFramePattern).m_vActionGroup[0];
	m_listAlgo.DeleteAllItems();
	AddFunction2List();
}

void CDebugDlg::OnStnClickedStaticBtnIncrease()
{
	if (m_settingType == T_ALGO)
	{
		AdjustParameterValue(true);
	}
	else if (m_settingType == T_CAMERA)
	{
		AdjustCameraParameterValue(true);
	}
	else if (m_settingType == T_LIGHT)
	{
		AdjustLightParameterValue(true);
	}
	else if (m_settingType == T_MOTION_CARD)
	{
		AdjustMotionParameterValue(true);
	}
}

void CDebugDlg::OnStnClickedStaticBtnDecrease()
{
	if (m_settingType == T_ALGO)
	{
		AdjustParameterValue(false);
	}
	else if (m_settingType == T_CAMERA)
	{
		AdjustCameraParameterValue(false);
	}
	else if (m_settingType == T_LIGHT)
	{
		AdjustLightParameterValue(false);
	}
	else if (m_settingType == T_MOTION_CARD)
	{
		AdjustMotionParameterValue(false);
	}
}

void CDebugDlg::OnBnClickedCheckImageDebug()
{
	UpdateData(TRUE);
	if (m_bEnableOpenFolder)
	{
		m_btnImageFolder.SetLabelImage(IMG_PNG_FOLDER);
		m_btnOneCircle.SetLabelImage(IMG_PNG_ONE_CIRCLE_DENY);
		m_btnOneCircle.Invalidate();
		ShowLastWidget(m_settingType);
	}
	else
	{
		m_btnImageFolder.SetLabelImage(IMG_PNG_FOLDER_DISABLED);
		m_btnSaveSinglePic.SetLabelImage(IMG_PNG_SAVE_SINGLEPIC_DENY);
		m_btnSaveSinglePic.Invalidate();
		m_btnSaveMultiplePic.SetLabelImage(IMG_PNG_SAVE_MULTIPLEPIC_DENY);
		m_btnSaveMultiplePic.Invalidate();
		m_btnMultipleProcess.SetLabelImage(IMG_PNG_MULTIPLE_PROCESS_DENY);
		m_btnMultipleProcess.Invalidate();
		if (m_bStarted)
		{
			m_btnOneCircle.SetLabelImage(IMG_PNG_ONE_CIRCLE);
			m_btnOneCircle.Invalidate();
		}
		ShowLastWidget(m_settingType);
	}
	m_btnImageFolder.BringWindowToTop();
	m_btnImageFolder.EnableWindow(m_bEnableOpenFolder);
	if (m_ptrProcessor)
	{
		bool bDebug = (m_bEnableOpenFolder == TRUE) ? true : false;
		m_ptrProcessor->SetDebugFlag(bDebug);
	}
	CRect rc;
	m_btnImageFolder.GetWindowRect(&rc);
	InvalidateRect(&rc);
}

LRESULT CDebugDlg::OnStnRightClickedPatternWidget(WPARAM wParam, LPARAM lParam)
{
	int nID = (int)lParam;
	if (nID == ID_PATTERN_WIDGET)
	{
		if (!m_bIfSemiauto)
		{
			return 0;
		}
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		int nOldPatternIndex = m_nCrtPatternIndex;
		for (auto t = 0; t < m_vWidgetRect.size(); t++)
		{
			if (m_vWidgetRect[t].PtInRect(pt))
			{
				m_nCrtPatternIndex = t;
				break;
			}
		}
		// Step 1: restore the old-selected widget;
		if (nOldPatternIndex < m_vWidgetRect.size())
		{
			CRect rcWidget = m_vWidgetRect[nOldPatternIndex];
			m_vPatternWidgetPtr[nOldPatternIndex]->MoveWindow(&rcWidget);
			rcWidget.left += m_nGap;
			rcWidget.right += (2 * m_nGap + 6);
			InvalidateRect(&rcWidget);
		}
		bool bValidPattern = (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end());
		// Step 2: Highlight the new-selected widget;
		if (m_nCrtPatternIndex >= 0 && m_nCrtPatternIndex < m_vWidgetRect.size())
		{
			m_rcCrtPatternWidget = m_vWidgetRect[m_nCrtPatternIndex];
			if (bValidPattern)
			{
				m_rcCrtPatternWidget.left -= m_nGap;
				m_rcCrtPatternWidget.right -= m_nGap;
			}
			m_vPatternWidgetPtr[m_nCrtPatternIndex]->MoveWindow(&m_rcCrtPatternWidget);
			CRect rcInvalidate = m_rcCrtPatternWidget;
			rcInvalidate.right += (3 * m_nGap + 5);
			InvalidateRect(rcInvalidate);
			if (bValidPattern)
			{
				if (m_crtImgIndex >= 0 && m_crtImgIndex < m_vImage.size() && m_vImage[m_crtImgIndex].m_img.data)
				{
					//localUtils::utils::PrintCrtSymbol(m_vImage[m_crtImgIndex], m_nCameraIndex, m_nCrtPatternIndex, m_nCrtActionGpIndex);
				}
				if (m_crtImgIndex >= 0 && m_crtImgIndex < m_vImage.size() && m_vImage[m_crtImgIndex].m_img.data)
				{
					//localUtils::utils::PrintCrtSymbol(m_vImage[m_crtImgIndex], m_nCameraIndex, m_nCrtPatternIndex, m_nCrtActionGpIndex);
				}
			}
		}
	}
	return 0;
}

void CDebugDlg::OnStnClickedPatternWidget()
{
	if (!m_bIfSemiauto)
	{
		return;
	}
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	int nOldPatternIndex = m_nCrtPatternIndex;
	for (auto t = 0; t < m_vWidgetRect.size(); t++)
	{
		if (m_vWidgetRect[t].PtInRect(pt))
		{
			m_nCrtPatternIndex = t;
			break;
		}
	}
	if (m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.find(m_wstrCrtCameraName) == m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.end())
	{
		DeleteAllSettingWidget();
	}
	m_ptrProcessor->m_bExeAllAlgo = true;
	if (nOldPatternIndex != m_nCrtPatternIndex)
	{
		// Step 1: restore the old-selected widget;
		if (nOldPatternIndex < m_vWidgetRect.size())
		{
			CRect rcWidget = m_vWidgetRect[nOldPatternIndex];
			m_vPatternWidgetPtr[nOldPatternIndex]->MoveWindow(&rcWidget);
			rcWidget.left += m_nGap;
			rcWidget.right += (2 * m_nGap + 6);
			InvalidateRect(&rcWidget);
		}
		bool bValidPattern = (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end());
		// Step 2: Highlight the new-selected widget;
		if (m_nCrtPatternIndex >= 0 && m_nCrtPatternIndex < m_vWidgetRect.size())
		{
			m_rcCrtPatternWidget = m_vWidgetRect[m_nCrtPatternIndex];
			if (bValidPattern)
			{
				m_rcCrtPatternWidget.left -= m_nGap;
				m_rcCrtPatternWidget.right -= m_nGap;
			}
			m_vPatternWidgetPtr[m_nCrtPatternIndex]->MoveWindow(&m_rcCrtPatternWidget);
			CRect rcInvalidate = m_rcCrtPatternWidget;
			rcInvalidate.right += (3 * m_nGap + 5);
			InvalidateRect(rcInvalidate);
		}
		// Step 3: Update the Shoot ComboBox and algorithm list table.
		if (bValidPattern)
		{
			InitializeActionGroupWidget();
			ShowLastWidget(m_settingType);
			CAMERA_FRAME_PATTERN crtPattern = (*m_ptrDictPatternForCrtCamera)[m_nCrtPatternIndex];
			if (m_crtImgIndex >= 0 && m_crtImgIndex < m_vImage.size() && m_vImage[m_crtImgIndex].m_img.data)
			{
				int nStationIndex = -1;
				int nPatternIndex = -1;
				int nActionGpIndex = -1;
				bool bMarked = false;
				//localUtils::utils::ParseImgSymbol(m_vImage[m_crtImgIndex], nStationIndex, nPatternIndex, nActionGpIndex, bMarked);
				bool bLabeledImg = (((nStationIndex == 0 || nStationIndex == 1)) && (nPatternIndex >= 0) && (nActionGpIndex == 0));
				if (NeedProcessTargetPicture())
				{
					m_crtImgIndex = m_nCrtPatternIndex;
					UpdateIndexPanel(m_crtImgIndex);
				}
				ProcessCurrentImage();
			}
		}
		else
		{
			m_listAlgo.DeleteAllItems();
		}
	}
	else
	{
		if (m_nCrtPatternIndex >= 0 && m_crtImgIndex >= 0 && m_crtImgIndex < m_nImgCount)
		{
			if (m_vImage[m_crtImgIndex].m_img.data)
			{
				int nStationIndex = -1;
				int nPatternIndex = -1;
				int nActionGpIndex = -1;
				bool bMarked = false;
				//localUtils::utils::ParseImgSymbol(m_vImage[m_crtImgIndex], nStationIndex, nPatternIndex, nActionGpIndex, bMarked);
				bool bLabeledImg = (((nStationIndex == 0 || nStationIndex == 1)) && (nPatternIndex >= 0));
				if (NeedProcessTargetPicture())
				{
					m_crtImgIndex = m_nCrtPatternIndex;
					UpdateIndexPanel(m_crtImgIndex);
				}
				ProcessCurrentImage();
			}
		}
	}
}

bool CDebugDlg::NeedProcessTargetPicture()
{
	if (m_vImage.size() > 19)
	{
		return false;
	}
	for (int i = 0; i < m_vImage.size(); i++)
	{
		int nStationIndex = -1;
		int nPatternIndex = -1;
		int nActionGpIndex = -1;
		bool bMarked = false;
		//localUtils::utils::ParseImgSymbol(m_vImage[i], nStationIndex, nPatternIndex, nActionGpIndex, bMarked);
		bool bLabeledImg = (((nStationIndex == 0 || nStationIndex == 1)) && (nPatternIndex >= 0) && (nActionGpIndex == 0));
		if (!(bLabeledImg && bMarked))
		{
			return false;
		}
		if (nPatternIndex != i)
		{
			return false;
		}
	}
	return true;
}

void CDebugDlg::OnStnClickedPatternWidgetByPicture(int nPatternIndex)
{
	int nOldPatternIndex = m_nCrtPatternIndex;
	m_nCrtPatternIndex = nPatternIndex;
	if (nOldPatternIndex != m_nCrtPatternIndex)
	{
		// Step 1: restore the old-selected widget;
		if (nOldPatternIndex < m_vWidgetRect.size())
		{
			CRect rcWidget = m_vWidgetRect[nOldPatternIndex];
			m_vPatternWidgetPtr[nOldPatternIndex]->MoveWindow(&rcWidget);
			rcWidget.left += m_nGap;
			rcWidget.right += (2 * m_nGap + 6);
			InvalidateRect(&rcWidget);
		}
		bool bValidPattern = (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex) != m_ptrDictPatternForCrtCamera->end());
		// Step 2: Highlight the new-selected widget;
		if (m_nCrtPatternIndex >= 0 && m_nCrtPatternIndex < m_vWidgetRect.size())
		{
			m_rcCrtPatternWidget = m_vWidgetRect[m_nCrtPatternIndex];
			if (bValidPattern)
			{
				m_rcCrtPatternWidget.left -= m_nGap;
				m_rcCrtPatternWidget.right -= m_nGap;
			}
			m_vPatternWidgetPtr[m_nCrtPatternIndex]->MoveWindow(&m_rcCrtPatternWidget);
			CRect rcInvalidate = m_rcCrtPatternWidget;
			rcInvalidate.right += (3 * m_nGap + 5);
			InvalidateRect(rcInvalidate);
		}
		// Step 3: Update the Shoot ComboBox and algorithm list table.
		if (bValidPattern)
		{
			InitializeActionGroupWidget();
			ShowLastWidget(m_settingType);
		}
		else
		{
			m_nCrtActionGpIndex = -1;
			m_ptrCrtFramePattern = nullptr;
			m_comboPatternAction.ResetContent();
			m_listAlgo.DeleteAllItems();
		}
	}
}

void CDebugDlg::OnBnClickedCheckBoxOnOff()
{
	int nCheckState = ((CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF))->GetCheck();
	if (nCheckState == 0)
	{
		((CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF))->SetWindowTextW(L"Off");
	}
	else if (nCheckState == 1)
	{
		((CButton*)GetDlgItem(IDC_CHECKBOX_ONOFF))->SetWindowTextW(L"On");
	}

}

void CDebugDlg::OnEnChangedEditbox()
{

}

void CDebugDlg::OnBnClickedCheckIndependentGrab()
{
	UpdateData(TRUE);
	if (m_bIndependentGrab)
	{
		((CButton*)GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER))->EnableWindow(TRUE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER))->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}

void CDebugDlg::OnBnClickedButtonSoftwareTrigger()
{
	CString strTxt = L"";
	((CButton*)GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER))->GetWindowText(strTxt);
	if (strTxt == L"软触发取图")
	{
		ResetEvent(gSystemMgr.m_hManualGrabOKFlag);
		((CButton*)GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER))->SetWindowText(L"停止取图");
		int nDelayTime=0;
		if (0 == m_nCameraIndex)
		{
			nDelayTime = 1000;
		}
		else
		{
			nDelayTime = 300;
		}
		SetTimer(TIMER_ID_DEBUG_GRAB_CONTINUOUS, nDelayTime, NULL);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BUTTON_SOFTWARE_TRIGGER))->SetWindowText(L"软触发取图");
		KillTimer(TIMER_ID_DEBUG_GRAB_CONTINUOUS);
		ResetEvent(gSystemMgr.m_hManualGrabOKFlag);
	}
}

void CDebugDlg::OnBnClickedCheckSaveImg()
{
	UpdateData(TRUE);
}

void CDebugDlg::SetInitialState()
{
	m_btnAutoSemiauto.SetLabelImage(IMG_PNG_AUTO);
	m_btnAutoSemiauto.BringWindowToTop();
	m_btnAutoSemiauto.EnableWindow(TRUE);
	m_btnAutoSemiauto.Invalidate();
	EnableAllWidget(false);
	m_bIfSemiauto = false;
}

void CDebugDlg::SetDlgShowListener(ImageShowWndListener * pListener)
{
	//if (NULL != pListener)
	//{
	//	m_pWndListener = pListener;
	//}
}
void CDebugDlg::DeleteDlgShowListener()
{
	//TODO:
}
BOOL CDebugDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CDebugDlg::OnEnSetfocusEditScaleinput()
{
	m_edt_sInput.EnableWindow(TRUE);
	m_edt_sInput.ShowWindow(SW_SHOW);
}

LRESULT CDebugDlg::OnSendNextImage(WPARAM wParam, LPARAM lParam)
{
	OnStnClickedNext();
	if (m_crtImgIndex == m_nImgCount - 1)
	{
		m_bBatchMode = false;
		m_ptrProcessor->SetBatchMode(m_bBatchMode);
	}
	return 0;
}

void CDebugDlg::OnBnClickedRadioCamera(UINT nID)
{
	SETTING_TYPE m_tLastType = m_settingType;
    int nOldIndex = m_nCameraIndex;
   
	for (int i = 0; i < m_cfParamUpdate.size(); i++)
	{
		if (m_cfParamUpdate[i] == true)
		{
			gConfigFileMgr.UpdatevActionGroup(0, (*m_ptrDictPatternForCrtCamera)[i].m_vActionGroup, m_wstrCurtProjectName, i, m_wstrCrtCameraName);
		}
	}
	
    m_nCameraIndex = nID - IDC_RADIO_CAMERA_1;
	DeleteAllSettingWidget();
	ResetInternalData();
    RetrieveInternalData(m_nCameraIndex);
	m_ptrProcessor->m_nCameraIndex= m_nCameraIndex;
	if (m_ptrDictPatternForCrtCamera->find(m_nCrtPatternIndex)== m_ptrDictPatternForCrtCamera->end())
	{
		m_listAlgo.DeleteAllItems();
	}
	ShowLastWidget(m_tLastType);
	
	if (m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.find(m_wstrCrtCameraName) == m_dictProjectPattern[m_nCrtPatternIndex].m_dictCameraName_SAA.end())
	{
		DeleteAllSettingWidget();
	}
    UpdateData(FALSE);
}

void CDebugDlg::OnMouseMove(cv::Point2f pt)
{
	//TODO
	//CString strTmp = _T("");
	//strTmp.Format(_T("%s : Row = %.3f,Col = %.3f"), L"Debug图像窗口", pt.y, pt.x);
	//m_edt_scale.SetWindowText(strTmp);
}

void CDebugDlg::OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo)
{
	if (NULL != m_pWndListener)
	{
		double dX, dY, dL1, dL2;
		if (eType == ROI_RECTANGLE)
		{
			dX = imgROIInfo.m_arrROIInfo[0];
			dY = imgROIInfo.m_arrROIInfo[1];
			dL1= imgROIInfo.m_arrROIInfo[2];
			dL2= imgROIInfo.m_arrROIInfo[3];
		}
		//TODO:用于更新ROI位置信息
	}
}

LRESULT CDebugDlg::OnDisplayImageOnDebugScreen(WPARAM wParam, LPARAM lParam)
{
	if (wParam == WINDOW_NDEBUG_RESULT&&lParam!=NULL)//not debug mode,result image
	{
		CString strCameraName = (BSTR)lParam;
		SysFreeString((BSTR)lParam);
		if (gSystemMgr.m_dictResultImg.find(strCameraName.GetString()) != gSystemMgr.m_dictResultImg.end())
		{
			Mat mt = gSystemMgr.m_dictResultImg[strCameraName.GetString()].m_image.clone();
			bool bNumeric = localUtils::utils::IsNumericString(strCameraName.Right(1));
			if (mt.data &&
				((bNumeric&& m_ptrProcessor->m_nCameraIndex == _ttoi(strCameraName.Right(1))) ||
				((!bNumeric&& m_ptrProcessor->m_nCameraIndex == 0))))
			{
				IplImage cResultImg(mt);
				m_cImgWndDebug->ClearImage();
				m_cImgWndDebug->LoadImageDataReset(&cResultImg, IMAGE_RGB8);
				return 0;
			}
		}
		
	}
	else if (wParam == WINDOW_DEBUG_RESULT&&0<=lParam&&lParam<(int)gSystemMgr.m_crtProject->m_vPattern.size())//debug mode,result image
	{
		Mat mt;
		gSystemMgr.m_debugResultImgBuffer.pop_back(&mt);
		IplImage cResultImg(mt);
		m_cImgWndDebug->ClearImage();
		m_cImgWndDebug->LoadImageDataReset(&cResultImg, IMAGE_RGB8);
		int nPatternIndex = (int)(lParam);
		wstring wstrPatternName = MapPatternName(nPatternIndex);
		m_cImgWndDebug->ShowText(wstrPatternName.c_str(), 100, 100, L"Green");
		return 0;
	}
	else if(wParam == INDEPENDENT_GRAB&&0<= lParam&&lParam <=4) //main capmera,independent grab on debug screen
	{
		Mat mt = gSystemMgr.m_dictImageSource[m_dictCameraIndex_Name[int(lParam)]].m_image.clone();
		if (mt.data)
		{
			IplImage cpCrtImg(mt);
			m_cImgWndDebug->ClearImage();
			m_cImgWndDebug->LoadImageDataReset(&cpCrtImg,IMAGE_GRAY8);
			if (m_bCheckImgSave)
			{
				//OnSaveIndependentImg();
			}
			return 0;
		}	
	}
	return -1;
}

void CDebugDlg::OnSaveIndependentImg()
{
	if (m_bCheckImgSave)
	{
		//if (m_bIfSemiauto && !m_bEnableOpenFolder&&m_bStarted && !m_bDebugMode && (!m_ptrProcessor->m_bInChecking))
		{
			wstring wstrCameraName;
			std::unordered_map<wstring, std::shared_ptr<CCameraBase>>* dictStationCameraPtr = m_ptrStation->GetStationCameraPtr();
			wstrCameraName = m_dictCameraIndex_Name[m_nCameraIndex];
			Mat tempMat = gSystemMgr.m_dictImageSource[wstrCameraName].m_image.clone();
			if (tempMat.data)
			{
				CString strFilter = _T("Image Files (*.bmp)|*.bmp|BMP File (*.tif)|*.tif|PNG File (*.png)|*.png|JPG File (*.jpg)|*.jpg|All Files (*.*)|*.*||");
				CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter,NULL,0);
				if (dlg.DoModal() == IDOK)
				{
					CString str_totalPath = dlg.GetPathName();
					str_totalPath += ".bmp";
					USES_CONVERSION;
					char* Filename = T2A(str_totalPath);
					imwrite(Filename, tempMat);
				}
			}
		}
	}
}

void CDebugDlg::DebugGetSingleImg()
{
	if (FALSE == m_bIndependentGrab)
	{
		m_ptrProcessor->GrabOneImage(m_nCrtPatternIndex);//根据m_nCrtPatternIndex取图进处理流程,不允许存图
	}
	else
	{
		//独立取图，不进处理流程
		std::unordered_map<wstring, std::shared_ptr<CCameraBase>>* dictStationCameraPtr = m_ptrStation->GetStationCameraPtr();
		auto ptrCamera = (*dictStationCameraPtr)[m_dictCameraIndex_Name[m_nCameraIndex]];
		if ((NULL != ptrCamera && ptrCamera->IsCameraOk()))
		{
		  if (m_nOldIndieExpoTime != m_nIndieExpoTime)
		  {
			  ptrCamera->SetExposureTime(m_nIndieExpoTime);
			  m_nOldIndieExpoTime = m_nIndieExpoTime;
		  }
		  if (m_nOldIndieGain != m_nIndieGain)
		  {
			  ptrCamera->SetTapGain(m_nIndieGain);
			  m_nOldIndieGain = m_nIndieGain;
		  }
		  if (m_nOldIndieWhiteBalanceR != m_nIndieWhiteBalanceR ||
			  m_nOldIndieWhiteBalanceG != m_nIndieWhiteBalanceG ||
			  m_nOldIndieWhiteBalanceB != m_nIndieWhiteBalanceB
			  )
		  {
			  ptrCamera->SetWhiteBalance(m_nIndieWhiteBalanceR, m_nIndieWhiteBalanceG, m_nIndieWhiteBalanceB);
			  m_nOldIndieWhiteBalanceR = m_nIndieWhiteBalanceR;
			  m_nOldIndieWhiteBalanceG = m_nIndieWhiteBalanceG;
			  m_nOldIndieWhiteBalanceB = m_nIndieWhiteBalanceB;
		  }
		  //ptrCamera->SetDelayTime(m_nIndieDelayTime);
		  ptrCamera->SendTriggerSignal(true);
		}
	}
}

wstring CDebugDlg::MapPatternName(int nPatternIndex)
{
	std::map<int, PG_IMAGE>* ptrDictImg = gConfigFileMgr.GetPgSetting();
	if (ptrDictImg->find(nPatternIndex) != ptrDictImg->end())
	{
		return ((*ptrDictImg)[nPatternIndex]).m_wstrType;
	}
	else
		return  L"";
}
