#include "stdafx.h"
#include "AOI.h"
#include "CameraSettingDlg.h"
#include "afxdialogex.h"
#include "ConfigFileManager.h"
#include "Resource.h"
#include "utils.h"
#include <map>

using namespace localUtils;

extern ConfigFileManager gConfigFileMgr;

IMPLEMENT_DYNAMIC(CCameraSettingDlg, CDialogEx)

CCameraSettingDlg::CCameraSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CAMERA_SETTING, pParent)
	, m_bOkStatus(true)
    , m_bMainCamera(FALSE)
{
	m_smallFont.CreateFont(int(24 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_txtFont.CreateFont(int(32 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
}

CCameraSettingDlg::~CCameraSettingDlg()
{
}

void CCameraSettingDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
{
	CWnd* pCtrl = (CWnd*)GetDlgItem(nCtrlID);
	if (pCtrl != nullptr)
	{
		if (bSmallFont)
		{
			pCtrl->SetFont(&m_smallFont);
		}
		else
		{
			pCtrl->SetFont(&m_txtFont);
		}
		pCtrl->MoveWindow(&rcPos);
		if (nStrID != -1)
		{
			CString str;
			str.LoadStringW(nStrID);
			pCtrl->SetWindowTextW(str);
		}
		pCtrl->Invalidate();
	}
}

void CCameraSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CAMERA_NAME, m_listCameraList);
    DDX_Control(pDX, IDC_BUTTON_SET_CONFIG_PATH, m_btnLoad);
    DDX_Check(pDX, IDC_CHECK_MAIN_CAMERA, m_bMainCamera);
}

void CCameraSettingDlg::CreateUiWidget()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcStatic, rcCtrl, rcBtn,rcCheck;
	int nGapH = int(20 * gSystemMgr.m_fRatioH);
	int nGapV = int(25 * gSystemMgr.m_fRatioV);
	int nLen = int(130 * gSystemMgr.m_fRatioH);
	int nHeight = int(36 * gSystemMgr.m_fRatioV);
	int nCtrlLen = int(280 * gSystemMgr.m_fRatioH);
	//------------------------------------------
	rcStatic.left = rcClient.left + nGapH;
	rcStatic.top = rcClient.top + 2 * nGapV;
	rcStatic.right = rcStatic.left + nLen;
	rcStatic.bottom = rcStatic.top + nHeight;
    InitializeCtrl(IDC_STATIC_CAMERA_NAME, rcStatic, IDS_STRING_CAMERA_NAME);

	rcCtrl.left = rcStatic.right;
	rcCtrl.top = rcStatic.top - 5;
	rcCtrl.right = rcCtrl.left + nCtrlLen;
	rcCtrl.bottom = rcCtrl.top + nHeight; 
	InitializeCtrl(IDC_EDIT_CAMERA_NAME, rcCtrl);

    //check box
    rcCheck.left = rcCtrl.right + nGapH;
    rcCheck.top = rcCtrl.top;
    rcCheck.right = rcCheck.left + nGapH * 6;
    rcCheck.bottom = rcCheck.top + nHeight;
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_MAIN_CAMERA);
    pBtn->MoveWindow(&rcCheck);
    pBtn->SetFont(&m_txtFont);
    CString str;
    str.LoadStringW(IDS_STRING_MAIN_CAMERA);
    pBtn->SetWindowTextW(str);
    m_bgBrushHelper.AddSubWindow(this, pBtn, m_bgImg);
    SetWindowTheme(pBtn->m_hWnd, L"", L"");

    // Resolution
    rcStatic.top = rcStatic.bottom + nGapV;
    rcStatic.bottom = rcStatic.top + nHeight;
    InitializeCtrl(IDC_STATIC_RESOLUTION, rcStatic, IDS_STRING_RESOLUTION);

    rcCtrl.left = rcStatic.right;
    rcCtrl.top = rcStatic.top - 5;
    rcCtrl.right = rcCtrl.left + nCtrlLen;
    rcCtrl.bottom = rcCtrl.top + nHeight;
    CRect rcResolution;
    rcResolution.left = rcCtrl.left;
    rcResolution.top = rcCtrl.top;
    rcResolution.right = rcResolution.left + int((rcCtrl.Width() - 30) / 2.0f);
    rcResolution.bottom = rcCtrl.bottom;
    InitializeCtrl(IDC_EDIT_CAMERA_WIDTH, rcResolution);

    rcResolution.left = rcResolution.right;
    rcResolution.right = rcResolution.left + 30;
    InitializeCtrl(IDC_STATIC_X, rcResolution);

    rcResolution.left = rcResolution.right;
    rcResolution.right = rcResolution.left + int((rcCtrl.Width() - 30) / 2.0f);;
    InitializeCtrl(IDC_EDIT_CAMERA_HEIGHT, rcResolution);

    // Type Combobox
    rcStatic.top = rcStatic.bottom + nGapV;
    rcStatic.bottom = rcStatic.top + nHeight;
    InitializeCtrl(IDC_STATIC_CAMERA_TYPE, rcStatic, IDS_STRING_CAMERA_TYPE);
    rcCtrl.left = rcStatic.right;
    rcCtrl.top = rcStatic.top - 5;
    rcCtrl.right = rcCtrl.left + nCtrlLen;
    rcCtrl.bottom = rcCtrl.top + nHeight;
    InitializeCtrl(IDC_COMBO_CAMERA_TYPE, rcCtrl);

    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_TYPE);
    pCombo->InsertString(0, _T("GigE"));
    pCombo->InsertString(1, _T("Camera Link"));
    pCombo->SetCurSel(-1);

    // Serial Port
    rcStatic.top = rcStatic.bottom + nGapV;
    rcStatic.bottom = rcStatic.top + nHeight;
    InitializeCtrl(IDC_STATIC_CAMERA_PORT, rcStatic, IDS_STRING_SERIAL_PORT);
    rcCtrl.left = rcStatic.right;
    rcCtrl.top = rcStatic.top - 5;
    rcCtrl.right = rcCtrl.left + nCtrlLen;
    rcCtrl.bottom = rcCtrl.top + nHeight;
    InitializeCtrl(IDC_EDIT_CAMERA_PORT, rcCtrl);

	// Configuration File Path
	rcStatic.top = rcStatic.bottom + nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_CONFIG_PATH, rcStatic, IDS_STRING_CONFIG_FILE);

	rcCtrl.left = rcStatic.right;
	rcCtrl.top = rcStatic.top - 5;
	rcCtrl.right = rcCtrl.left + int(2.5f * nCtrlLen);
	rcCtrl.bottom = rcCtrl.top + nHeight;
	InitializeCtrl(IDC_EDIT_CONFIG_PATH, rcCtrl);

    rcBtn.left = rcCtrl.right + nGapH;
    rcBtn.top = rcCtrl.top;
    rcBtn.right = rcBtn.left + rcCheck.Width() - nGapH;
    rcBtn.bottom = rcCtrl.bottom;
    InitializeCtrl(IDC_BUTTON_SET_CONFIG_PATH,rcBtn);

    // Camera List
    CRect rcList;
	rcList.left  = rcStatic.left;
	rcList.top   = rcBtn.bottom + 2 * nGapV;
	rcList.right = rcBtn.right;
	rcList.bottom = rcList.top + 6 * nHeight;
    InitializeCtrl(IDC_LIST_CAMERA_NAME,rcList);

    CString strIndex;
    CString strCameraName;
    CString sResolution;
    CString sType;
    CString sPort;
    CString sConfigFile;
    strIndex.LoadStringW(IDS_STRING_INDEX);
    strCameraName.LoadStringW(IDS_STRING_CAMERA_NAME);
    sResolution.LoadStringW(IDS_STRING_RESOLUTION);
    sType.LoadStringW(IDS_STRING_CAMERA_TYPE);
    sPort.LoadStringW(IDS_STRING_SERIAL_PORT);
    sConfigFile.LoadStringW(IDS_STRING_CONFIG_FILE);
    m_listCameraList.SetFont(&m_smallFont);

    DWORD dwStyleEx = m_listCameraList.GetExtendedStyle();
    dwStyleEx |= LVS_EX_FULLROWSELECT;
    dwStyleEx |= LVS_EX_GRIDLINES;
	m_listCameraList.SetExtendedStyle(dwStyleEx);
	m_listCameraList.InsertColumn(0, strIndex, LVCFMT_LEFT,60);
	m_listCameraList.InsertColumn(1, strCameraName, LVCFMT_LEFT,140);
    m_listCameraList.InsertColumn(2, sResolution, LVCFMT_LEFT, 140);
    m_listCameraList.InsertColumn(3, sType, LVCFMT_LEFT, 100);
    m_listCameraList.InsertColumn(4, sPort, LVCFMT_LEFT, 80);
    m_listCameraList.InsertColumn(5, sConfigFile, LVCFMT_LEFT, 390);

    //-----------------------------------------
    CRect rcMsg;
    rcMsg.left   = rcList.left;
    rcMsg.right  = rcList.right;
    rcMsg.top    = rcList.bottom + 5 * nHeight;
    rcMsg.bottom = rcClient.bottom - nGapV;
    InitializeCtrl(IDC_STATIC_ERROR, rcMsg, -1, true);

    // Buttons
    CRect rcFuncBtn;
    rcFuncBtn.right = rcList.right;
    rcFuncBtn.top = rcCheck.top;
    rcFuncBtn.left = rcFuncBtn.right - rcCheck.Width();
    rcFuncBtn.bottom = rcFuncBtn.top + nHeight;
    InitializeCtrl(IDC_BUTTON_ADD_CAMERA,rcFuncBtn, IDS_STRING_CREATE);
    EnableUiWidget(IDC_BUTTON_ADD_CAMERA, true);

    rcFuncBtn.top = rcFuncBtn.bottom + nGapV;
    rcFuncBtn.bottom = rcFuncBtn.top + nHeight;
    InitializeCtrl(IDC_BUTTON_UPDATE_CAMERA, rcFuncBtn, IDS_STRING_MODIFY);
    EnableUiWidget(IDC_BUTTON_UPDATE_CAMERA, false);

    rcFuncBtn.top = rcFuncBtn.bottom + nGapV;
    rcFuncBtn.bottom = rcFuncBtn.top + nHeight;
    InitializeCtrl(IDC_BUTTON_DELETE_CAMERA, rcFuncBtn, IDS_STRING_DELETE);
    EnableUiWidget(IDC_BUTTON_DELETE_CAMERA, false);
	//UpdateData(FALSE);
}

void CCameraSettingDlg::DisplayCameraData(CString sCameraName)
{
    wstring wstrCameraName = sCameraName.GetString();
    if (m_dictCameraSetting.find(wstrCameraName) == m_dictCameraSetting.end())
    {
        ResetUiWidget();
        return;
    }
    m_crtCameraSetting = m_dictCameraSetting[wstrCameraName];
    m_bMainCamera = m_crtCameraSetting.m_bMainCamera;
    SetDlgItemTextW(IDC_EDIT_CAMERA_NAME, m_crtCameraSetting.m_wstrCameraName.c_str());
    SetDlgItemInt(IDC_EDIT_CAMERA_WIDTH, m_crtCameraSetting.m_nWidth);
    SetDlgItemInt(IDC_EDIT_CAMERA_HEIGHT, m_crtCameraSetting.m_nHeight);
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_TYPE);
    pCombo->SetCurSel(m_crtCameraSetting.m_type);

    if (m_crtCameraSetting.m_nSerialPort < 0)
    {
        SetDlgItemTextW(IDC_EDIT_CAMERA_PORT, _T("N/A"));
    }
    else
    {
        SetDlgItemInt(IDC_EDIT_CAMERA_PORT, m_crtCameraSetting.m_nSerialPort);
    }

    if (m_crtCameraSetting.m_wstrConfigFile == L"")
    {
        SetDlgItemTextW(IDC_EDIT_CONFIG_PATH,_T("N/A"));
    }
    else
    {
        SetDlgItemTextW(IDC_EDIT_CONFIG_PATH, m_crtCameraSetting.m_wstrConfigFile.c_str());
    }
    EnableUiWidget(IDC_BUTTON_ADD_CAMERA, false);
    EnableUiWidget(IDC_BUTTON_UPDATE_CAMERA, true);
    EnableUiWidget(IDC_BUTTON_DELETE_CAMERA, true);
}

void CCameraSettingDlg::ResetUiWidget()
{
    m_bMainCamera = FALSE;
    SetDlgItemTextW(IDC_EDIT_CAMERA_NAME, _T(""));
    SetDlgItemTextW(IDC_EDIT_CAMERA_WIDTH, _T(""));
    SetDlgItemTextW(IDC_EDIT_CAMERA_HEIGHT, _T(""));
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_TYPE);
    pCombo->SetCurSel(-1);
    SetDlgItemTextW(IDC_EDIT_CAMERA_PORT, _T(""));
    SetDlgItemTextW(IDC_EDIT_CONFIG_PATH, _T(""));

    EnableUiWidget(IDC_BUTTON_ADD_CAMERA, true);
    EnableUiWidget(IDC_BUTTON_UPDATE_CAMERA, false);
    EnableUiWidget(IDC_BUTTON_DELETE_CAMERA, false);
}

void CCameraSettingDlg::EnableUiWidget(int nCtrlID, bool bEnable)
{
    CWnd* pWnd = GetDlgItem(nCtrlID);
    if (pWnd)
    {
        pWnd->EnableWindow(bEnable);
    }
}

bool CCameraSettingDlg::GetPendingSetting(CAMERA_SETTING& retCs)
{
    UpdateData(TRUE);

    CString sName,sMsg;
    GetDlgItemTextW(IDC_EDIT_CAMERA_NAME,sName);
    if (sName.GetLength() <= 0)
    {
        sMsg.LoadStringW(IDS_STRING_MSG_CAMERA_NAME_EMPTY);
        DisplayPromptMessage(sMsg, false);
        return false;
    }
    retCs.m_bMainCamera = (bool)m_bMainCamera;
    retCs.m_nWidth = GetDlgItemInt(IDC_EDIT_CAMERA_WIDTH);
    retCs.m_nHeight = GetDlgItemInt(IDC_EDIT_CAMERA_HEIGHT);
    if (retCs.m_nWidth <= 0 || retCs.m_nHeight <= 0)
    {
        sMsg.LoadStringW(IDS_STRING_MSG_INVALID_RESOLUTUIN);
        DisplayPromptMessage(sMsg, false);
        return false;
    }
    
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_TYPE);
    retCs.m_type = pCombo->GetCurSel();
    if (retCs.m_type < 0)
    {
        sMsg.LoadStringW(IDS_STRING_MSG_SELECT_CAMERA_TYPE);
        DisplayPromptMessage(sMsg, false);
        return false;
    }

    CString sPort = _T("");
    GetDlgItemTextW(IDC_EDIT_CAMERA_PORT, sPort);
    if (sPort != _T("") && sPort != _T("N/A") && localUtils::utils::IsNumericString(sPort,true))
    {
        retCs.m_nSerialPort = GetDlgItemInt(IDC_EDIT_CAMERA_PORT);
    }

    CString sConfigFile = _T("");
    GetDlgItemTextW(IDC_EDIT_CONFIG_PATH, sConfigFile);
    if (sConfigFile != _T("") && sConfigFile != _T("N/A"))
    {
        retCs.m_wstrConfigFile = sConfigFile.GetString();
    }
    return true;
}

BOOL CCameraSettingDlg::OnInitDialog()
{
    UpdateData(FALSE);
	CreateUiWidget();
	FillCameraList();
	return TRUE;
}

BOOL CCameraSettingDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CCameraSettingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BUTTON_SET_CONFIG_PATH, &CCameraSettingDlg::OnBnClickedButtonSetConfigPath)
    ON_BN_CLICKED(IDC_BUTTON_ADD_CAMERA, &CCameraSettingDlg::OnBnClickedButtonAddCamera)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE_CAMERA, &CCameraSettingDlg::OnBnClickedButtonUpdateCamera)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_CAMERA, &CCameraSettingDlg::OnBnClickedButtonDeleteCamera)
    ON_NOTIFY(NM_CLICK, IDC_LIST_CAMERA_NAME, &CCameraSettingDlg::OnNMClickListCameraName)
END_MESSAGE_MAP()

void CCameraSettingDlg::OnPaint()
{
	CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ClientToScreen(&rcWindow);
	CBitmap bakImg;
	if (bakImg.LoadBitmap(IDB_BITMAP_SYS_CONFIG_DLG_BK))
	{
		BITMAP bitmap;
		bakImg.GetBitmap(&bitmap);
		CBitmap* pPngOld = dcMem.SelectObject(&bakImg);
		CRect rcParentWindow;
		GetParent()->GetWindowRect(&rcParentWindow);
		ClientToScreen(&rcParentWindow);
		dc.StretchBlt(0, 0, rect.Width(), rect.Height(),
		&dcMem,
		rcWindow.left - rcParentWindow.left, rcWindow.top - rcParentWindow.top, rect.Width(), rect.Height(), SRCCOPY);
	}
}

HBRUSH CCameraSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_CAMERA_MANAGEMENT ||
        ctrlID == IDC_STATIC_CAMERA_NAME ||
		ctrlID == IDC_STATIC_RESOLUTION ||
		ctrlID == IDC_STATIC_X ||
		ctrlID == IDC_STATIC_CAMERA_TYPE ||
        ctrlID == IDC_STATIC_CAMERA_PORT ||
		ctrlID == IDC_STATIC_CAMERA_LIST ||
		ctrlID == IDC_STATIC_ERROR ||
        ctrlID == IDC_STATIC_CONFIG_PATH ||
		ctrlID == IDC_CHECK_MAIN_CAMERA)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (ctrlID == IDC_STATIC_ERROR)
		{
			if (m_bOkStatus)
			{
				pDC->SetTextColor(RGB(10, 240, 10));
			}
			else
			{
				pDC->SetTextColor(RGB(250, 10, 10));
			}
		}
		else
		{
			pDC->SetTextColor(RGB(250, 250, 250));
		}
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CCameraSettingDlg::FillCameraList()
{
    std::map<int, STATION_CONFIG>*  ptrCameraStationConfig = gConfigFileMgr.GetStaionConfigArray();
    if (ptrCameraStationConfig->empty())
    {
        return;
    }
	std::map<int, STATION_CONFIG>::iterator it = ptrCameraStationConfig->begin();
    m_dictCameraSetting = it->second.m_dictCamara;

	int nRow = 0;
    CString sIndex;
    CString sResolution;
    CString sType;
    CString sPort;
    CString sConfigFile;
	m_listCameraList.DeleteAllItems();
	for (auto it = m_dictCameraSetting.begin();it != m_dictCameraSetting.end(); it++)
	{
        CAMERA_SETTING cs = it->second;
        sIndex.Format(_T("%d"), nRow + 1);
        m_listCameraList.InsertItem(nRow, sIndex);
        m_listCameraList.SetItemText(nRow, 1, cs.m_wstrCameraName.c_str());

        sResolution.Format(_T("%d X %d"), cs.m_nWidth, cs.m_nHeight);
        m_listCameraList.SetItemText(nRow, 2, sResolution);

        sType = (cs.m_type == 0) ? _T("GigE") : _T("Camera Link");
        m_listCameraList.SetItemText(nRow, 3, sType);

        if (cs.m_type == 0) // GigE
        {
            sPort = _T(""); 
            sConfigFile = _T("");
        }
        else
        {
            sPort.Format(_T("%d"),cs.m_nSerialPort);
            sConfigFile = cs.m_wstrConfigFile.c_str();
        }
        m_listCameraList.SetItemText(nRow, 4, sPort);
        m_listCameraList.SetItemText(nRow, 5, sConfigFile);
        nRow++;
	}
}

void CCameraSettingDlg::DisplayOperationResult(bool bSuccess)
{
    CString sMsg;
    if (bSuccess)
    {
        sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
    }
    else
    {
        sMsg.LoadStringW(IDS_STRING_OPERATION_FAILED);
    }
    DisplayPromptMessage(sMsg, bSuccess);
}

void CCameraSettingDlg::DisplayPromptMessage(CString sMsg, bool bOk)
{
	SetDlgItemTextW(IDC_STATIC_ERROR, _T(""));
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_STATIC_ERROR);
	pWnd->ShowWindow(SW_HIDE);
	m_bOkStatus = bOk;
	SetDlgItemTextW(IDC_STATIC_ERROR, sMsg);
	pWnd->ShowWindow(SW_SHOW);
}

void CCameraSettingDlg::OnBnClickedButtonSetConfigPath()
{
    //TODO:
}

void CCameraSettingDlg::OnBnClickedButtonAddCamera()
{
    CAMERA_SETTING pendingCs;
    bool bRet = GetPendingSetting(pendingCs);
    if (!bRet)
    {
        return;
    }
    if (m_dictCameraSetting.find(pendingCs.m_wstrCameraName) != m_dictCameraSetting.end())
    {
        //TODO:
        return;
    }
    m_bOkStatus = gConfigFileMgr.AddNewCamera(pendingCs);
    if (m_bOkStatus)
    {
        m_dictCameraSetting[pendingCs.m_wstrCameraName] = pendingCs;
        m_crtCameraSetting = pendingCs;
    }
    DisplayOperationResult(m_bOkStatus);
}

void CCameraSettingDlg::OnBnClickedButtonUpdateCamera()
{
    CAMERA_SETTING pendingCs;
    bool bRet = GetPendingSetting(pendingCs);
    if (!bRet)
    {
        return;
    }
    if (pendingCs == m_crtCameraSetting)
    {
        return;
    }
    if (m_dictCameraSetting.find(pendingCs.m_wstrCameraName) == m_dictCameraSetting.end())
    {
        wstring wstrOldName = m_crtCameraSetting.m_wstrCameraName;
        wstring wstrNewName = pendingCs.m_wstrCameraName;

        m_bOkStatus = gConfigFileMgr.ReplaceCamera(wstrOldName, pendingCs);
        if (m_bOkStatus)
        {
            m_dictCameraSetting.erase(wstrOldName);
            m_dictCameraSetting[wstrNewName] = pendingCs;
            m_crtCameraSetting = pendingCs;
        }
        DisplayOperationResult(m_bOkStatus);
    }
    else
    {
        m_bOkStatus = gConfigFileMgr.UpdateCamera(pendingCs);
        if (m_bOkStatus)
        {
            m_dictCameraSetting[pendingCs.m_wstrCameraName] = pendingCs;
            m_crtCameraSetting = pendingCs;
        }
        DisplayOperationResult(m_bOkStatus);
    }
}

void CCameraSettingDlg::OnBnClickedButtonDeleteCamera()
{
    CAMERA_SETTING pendingCs;
    bool bRet = GetPendingSetting(pendingCs);
    if (!bRet)
    {
        return;
    }
    if (m_dictCameraSetting.find(pendingCs.m_wstrCameraName) != m_dictCameraSetting.end())
    {
        m_bOkStatus = gConfigFileMgr.DeleteCamera(pendingCs.m_wstrCameraName);
        if (m_bOkStatus)
        {
            m_dictCameraSetting.erase(pendingCs.m_wstrCameraName);
        }
        m_crtCameraSetting.Reset();
        DisplayOperationResult(m_bOkStatus);
    }
}

void CCameraSettingDlg::OnNMClickListCameraName(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    int nRowIndex = pNMItemActivate->iItem;
    CString sCameraName = m_listCameraList.GetItemText(nRowIndex, 1);
    DisplayCameraData(sCameraName);
    UpdateData(FALSE);
    *pResult = 0;
}
