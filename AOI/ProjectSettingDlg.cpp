#include "stdafx.h"
#include "AOI.h"
#include "ProjectSettingDlg.h"
#include "CopyProjectDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CProjectSettingDlg, CDialogEx)

CProjectSettingDlg::CProjectSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PROJECT_SETTING, pParent)
	, m_bOkStatus(true)
	, m_ptrPgImageArray(nullptr)
	, m_nCrtPatternIndex(-1)
	, m_nCrtAlgoIndex_R(-1)
	, m_nCrtAlgoIndex_L(-1)
	, m_bTurnOnLight(FALSE)
{
	m_smallFont.CreateFont(int(24 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_txtFont.CreateFont(int(36 * gSystemMgr.m_fRatioH), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_ptrAlgoObj = nullptr;
	m_bgImg.LoadBitmap(IDB_BITMAP_SYS_CONFIG_DLG_BK);
}

CProjectSettingDlg::~CProjectSettingDlg()
{
}

void CProjectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PG_PATTERN, m_listPgPattern);
	DDX_Control(pDX, IDC_LIST_ALGO_REPOSITORY, m_listAlgoRepository);
	DDX_Control(pDX, IDC_TREE_PROJECT_PATTERN, m_treePattern);
	DDX_Control(pDX, IDC_COMBO_PROJECT_NAME, m_comboProject);
	DDX_Check(pDX, IDC_CHECK_SIDE_LIGHT_ON, m_bTurnOnLight);
	DDX_Control(pDX, IDC_STATIC_PROJECT_PARAM_GROUP, m_stcGroup);
}

void CProjectSettingDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID)
{
	CWnd* pCtrl = (CWnd*)GetDlgItem(nCtrlID);
	if (pCtrl != nullptr)
	{
		pCtrl->SetFont(&m_smallFont);
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

void CProjectSettingDlg::CreateUiWidget()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcStatic, rcCtrl, rcBtn, rcList;

	int nLen = int(100 * gSystemMgr.m_fRatioH);
	int nHeight = int(30 * gSystemMgr.m_fRatioV);
	int nCtrlLen = m_nListWidth - int(1.5*nLen);
	//GroupBox
	m_rcGroup.top = rcClient.top + 5* LONG(gSystemMgr.m_fRatioV);
	m_rcGroup.bottom = m_rcGroup.top + LONG(nHeight * 4.5);
	m_rcGroup.left = m_nGapH;
	m_rcGroup.right = rcClient.right - 13*m_nGapH;
	CString strText;
	strText.LoadStringW(IDS_STRING_PROJECT_PARAM);
	m_stcGroup.MoveWindow(m_rcGroup);
	m_stcGroup.SetWindowTextW(strText);
	m_stcGroup.SetFont(&m_smallFont);
	SetWindowTheme(m_stcGroup.m_hWnd, L"", L"");
	m_bgBrushHelper.AddSubWindow(this,GetDlgItem(IDC_STATIC_PROJECT_PARAM_GROUP), m_bgImg);
	
	//Project Name
	rcStatic.left = rcClient.left + m_nGapH+10;
	rcStatic.top = m_rcGroup.top + 2*m_nGapV;
	rcStatic.right = rcStatic.left + nLen;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_PROJECT_NAME, rcStatic, IDS_STRING_PROJECT_NAME);

	rcCtrl.left = rcStatic.right;
	rcCtrl.top = rcStatic.top - 5;
	rcCtrl.right = rcCtrl.left + nCtrlLen;
	rcCtrl.bottom = rcCtrl.top + nHeight;
	InitializeCtrl(IDC_COMBO_PROJECT_NAME, rcCtrl);
	// Camera Height
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_MAIN_CAMERA_HEIGHT, rcStatic, IDS_STRING_MAIN_AXIS);
	rcCtrl.top = rcCtrl.bottom + m_nGapV;
	rcCtrl.bottom = rcCtrl.top + nHeight;
	InitializeCtrl(IDC_EDIT_MAIN_CAMERA_HEIGHT, rcCtrl);
	((CEdit*)GetDlgItem(IDC_EDIT_MAIN_CAMERA_HEIGHT))->SetWindowText(L"-1");
	
	// Product Name
	rcStatic.left = rcCtrl.right+2* m_nGapH;
	rcStatic.right = rcStatic.left + nLen;
	InitializeCtrl(IDC_STATIC_PRODUCT_NAME, rcStatic, IDS_STRING_PROJECT_PRODUCT_NAME);

	int nXlen = int(15 * gSystemMgr.m_fRatioH);
	int nEditLen = int((m_nListWidth - nLen - nXlen) / 2.5f);
	rcCtrl.left = rcStatic.right;
	rcCtrl.right = rcCtrl.left + nEditLen;
	CRect rcBgn = rcCtrl;
	InitializeCtrl(IDC_EDIT_PRODUCT_NAME, rcCtrl);
	
	//Station Name
	rcStatic.bottom = rcStatic.top - m_nGapV;
	rcStatic.top = rcStatic.bottom - nHeight;
	InitializeCtrl(IDC_STATIC_STATION_NAME, rcStatic, IDS_STRING_PROJECT_STATION_NAME);

	rcCtrl = rcBgn;
	rcCtrl.bottom = rcCtrl.top - m_nGapV;
	rcCtrl.top = rcCtrl.bottom - nHeight;
	InitializeCtrl(IDC_EDIT_STATION_NAME, rcCtrl);
	// Demension
	rcStatic = rcCtrl;
	rcStatic.top += 4;
	rcStatic.bottom += 4;
	rcStatic.left = rcCtrl.right+ 2*m_nGapH;
	rcStatic.right = rcStatic.left + nLen;
	InitializeCtrl(IDC_STATIC_DEMENSION, rcStatic, IDS_STRING_DIMENSION);

	nXlen = int(15 * gSystemMgr.m_fRatioH);
    nEditLen = int((m_nListWidth - nLen - nXlen) / 2.5f);
	rcCtrl.left = rcStatic.right;
	rcCtrl.right = rcCtrl.left + nEditLen;
    rcBgn  = rcCtrl;
	InitializeCtrl(IDC_EDIT_DEMENSION_WIDTH, rcCtrl);
	rcCtrl.left  = rcCtrl.right;
	rcCtrl.right = rcCtrl.left + nXlen;
	CRect rcTmp = rcCtrl;
	rcTmp.top += 4;
	rcTmp.bottom += 4;
	InitializeCtrl(IDC_STATIC_X_2, rcTmp);
	rcCtrl.left = rcCtrl.right;
	rcCtrl.right = rcCtrl.left + nEditLen;
	InitializeCtrl(IDC_EDIT_DEMENSION_HEIGHT, rcCtrl);
	//Resolution
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_RESOLUTION, rcStatic, IDS_STRING_RESOLUTION);

	rcCtrl = rcBgn;
	rcCtrl.top = rcCtrl.bottom + m_nGapV;
	rcCtrl.bottom = rcCtrl.top + nHeight;
	InitializeCtrl(IDC_EDIT_RESOLUTION_WIDTH, rcCtrl);
	rcCtrl.left = rcCtrl.right;
	rcCtrl.right = rcCtrl.left + nXlen;
	rcTmp = rcCtrl;
	rcTmp.top += 4;
	rcTmp.bottom += 4;
	InitializeCtrl(IDC_STATIC_X_1, rcTmp);
	rcCtrl.left = rcCtrl.right;
	rcCtrl.right = rcCtrl.left + nEditLen;
	InitializeCtrl(IDC_EDIT_RESOLUTION_HEIGHT, rcCtrl);


	
	rcBtn.left = rcClient.left + 2*m_nListWidth + 2*m_nBtnSz + 10*m_nGapH;
	int nBtnWidth = int((rcClient.right - rcBtn.left - 10) / 6.0f);
	rcBtn.right = rcBtn.left + nBtnWidth;
	rcBtn.top = m_rcGroup.top + 2*m_nGapV-4;
	rcBtn.bottom = rcBtn.top + nHeight-2;
	InitializeCtrl(IDC_BUTTON_PROJECT_CREATE, rcBtn, IDS_STRING_CREATE);

	rcBtn.left = rcBtn.right + 10;
	rcBtn.right = rcBtn.left + nBtnWidth;
	InitializeCtrl(IDC_BUTTON_PROJECT_MODIFY, rcBtn, IDS_STRING_MODIFY);

	rcBtn.top    = rcBtn.bottom + m_nGapV+4;
	rcBtn.bottom = rcBtn.top + nHeight-2;
	InitializeCtrl(IDC_BUTTON_PROJECT_DELETE, rcBtn, IDS_STRING_DELETE);

	rcBtn.right = rcBtn.left - 10;
	rcBtn.left = rcBtn.right - nBtnWidth;
	InitializeCtrl(IDC_BUTTON_PROJECT_COPY, rcBtn, IDS_STRING_COPY);
}

void CProjectSettingDlg::ListAllProjects()
{
	m_comboProject.ResetContent();

	m_comboProject.AddString(_T(""));
	std::map<int,STATION_CONFIG>* ptrDictStation = gConfigFileMgr.GetStaionConfigArray();
	if (ptrDictStation != nullptr && !ptrDictStation->empty())
	{
		STATION_CONFIG stationConfig = ptrDictStation->begin()->second;
		if (!stationConfig.m_dictProject.empty())
		{
			for (auto it = stationConfig.m_dictProject.begin(); it != stationConfig.m_dictProject.end(); it++)
			{
				wstring wstrProjectName = it->first;
				m_comboProject.AddString(wstrProjectName.c_str());
			}
		}
	}
	m_comboProject.SetCurSel(-1);

	SetButtonAccessibility(-1);
}

void CProjectSettingDlg::ReplaceCameraName()
{
	CString sCamera, sTmp;
	sTmp.LoadStringW(IDS_STRING_AUXILIARY_CAMERA);
	for (auto t = 0; t < m_crtProject.m_vPattern.size(); t++)
	{
		FRAME_PATTERN& framePattern = m_crtProject.m_vPattern[t];
		int nIndex = 0;
		auto tmpDictSnap = framePattern.m_dictCameraName_SAA;
		
		std::vector<tuple<wstring, std::vector<SNAP_ACTION>>> vCameraNameSAA;
		vCameraNameSAA.resize(tmpDictSnap.size());
		ReorderCameraMap(tmpDictSnap, vCameraNameSAA);

		framePattern.m_dictCameraName_SAA.clear();
		CString strName = L"";
		for (int i=0;i<vCameraNameSAA.size();i++)
		{
			if (nIndex == 0)
			{
				sCamera.LoadStringW(IDS_STRING_MAIN_CAMERA);
				nIndex++;
			}
			else
			{
				for (int j = 1; j < gConfigFileMgr.GetStaionConfigArray()->begin()->second.m_dictCamara.size(); j++)
				{
					strName.Format(_T("%d"), j);
					if (std::get<0>(vCameraNameSAA[i]).find(strName) != wstring::npos)
					{
						nIndex = i;
						break;
					}
				}
				sCamera.Format(sTmp, nIndex);
			}
			framePattern.m_dictCameraName_SAA[sCamera.GetString()] = std::get<1>(vCameraNameSAA[i]);
		}
	}
}

void CProjectSettingDlg::ReorderCameraMap(std::unordered_map<wstring, std::vector<SNAP_ACTION>> dictCameraNameIn, std::vector<tuple<wstring, std::vector<SNAP_ACTION>>>& vCameraNameOut)
{
	for (auto it : dictCameraNameIn)
	{
		wstring wstrBack=boost::lexical_cast<wstring>(it.first[it.first.length()-1]);
		if (!localUtils::utils::IsNumericString(wstrBack))
		{
			vCameraNameOut[0] = std::make_tuple(it.first,it.second);
		}
		else
		{
			vCameraNameOut[_ttoi(wstrBack.c_str())] = std::make_tuple(it.first, it.second);
		}
	}
}

void CProjectSettingDlg::DisplayCurrentProject()
{
	m_dictTreeItem_Pattern.clear();
	//Step 1:Display screen parameters
	SetDlgItemInt(IDC_EDIT_MAIN_CAMERA_HEIGHT, m_crtProject.m_nMainCameraHeight);
	SetDlgItemInt(IDC_EDIT_DEMENSION_WIDTH, m_crtProject.m_screenParam.m_nDimensionWidth);
	SetDlgItemInt(IDC_EDIT_DEMENSION_HEIGHT, m_crtProject.m_screenParam.m_nDimensionHeight);
	SetDlgItemInt(IDC_EDIT_RESOLUTION_WIDTH, m_crtProject.m_screenParam.m_nResolutionWidth_S);
	SetDlgItemInt(IDC_EDIT_RESOLUTION_HEIGHT, m_crtProject.m_screenParam.m_nResolutionHeight_S);
	SetDlgItemText(IDC_EDIT_STATION_NAME, m_crtProject.m_screenParam.m_strStationName.c_str());
	SetDlgItemText(IDC_EDIT_PRODUCT_NAME, m_crtProject.m_screenParam.m_strProductName.c_str());

	//Step 2: Display snap action groups
	std::map<int, PG_IMAGE>* ptrPg = gConfigFileMgr.GetPgSetting();
	if (ptrPg == nullptr || ptrPg->empty())
	{
		return;
	}
	HTREEITEM hRoot = NULL;
	HTREEITEM hSubItem = NULL;
	for (auto t = 0; t < m_crtProject.m_vPattern.size(); t++)
	{
		FRAME_PATTERN& crtPattern = m_crtProject.m_vPattern[t];
		if (ptrPg->find(crtPattern.m_nSeqIndex) != ptrPg->end())
		{
			wstring wstrName = (*ptrPg)[crtPattern.m_nSeqIndex].m_wstrName;
			HTREEITEM hInsertAfter = TVI_LAST;
			hRoot = m_treePattern.InsertItem(wstrName.c_str(), TVI_ROOT, hInsertAfter);
			m_treePattern.Expand(hRoot, TVE_EXPAND);
			int nTmpIndex = 0;
			CString sCamera,sTmp;
			CString strName = L"";
			sTmp.LoadStringW(IDS_STRING_AUXILIARY_CAMERA);
			for (auto it = crtPattern.m_dictCameraName_SAA.begin(); it != crtPattern.m_dictCameraName_SAA.end(); it++)
			{
				if (nTmpIndex == 0)
				{
					sCamera.LoadStringW(IDS_STRING_MAIN_CAMERA);
					nTmpIndex++;
				}
				else
				{
					for (int i = 1; i < gConfigFileMgr.GetStaionConfigArray()->begin()->second.m_dictCamara.size(); i++)
					{
						strName.Format(_T("%d"), i);
						if (it->first.find(strName) != wstring::npos)
						{
							nTmpIndex = i;
							break;
						}
					}
					sCamera.Format(sTmp, nTmpIndex);
				}
				hSubItem = m_treePattern.InsertItem(sCamera, hRoot);
				std::vector<SNAP_ACTION>& vSnapArray = it->second;
				for (auto t = 0; t < vSnapArray.size(); t++)
				{
					HTREEITEM hSnapItem = m_treePattern.InsertItem(_T("Snap#"), hSubItem);
					SNAP_ACTION& snap = vSnapArray[t];
					for (auto i = 0; i < snap.m_vAlgoFunc.size(); i++)
					{
						FUNC_INFO& funcInfo = snap.m_vAlgoFunc[i];
						if (funcInfo.m_wstrToolName == L"")
						{
							funcInfo.ParseHelpInfomation(m_ptrAlgoObj);
						}
						HTREEITEM hAlgoItem = m_treePattern.InsertItem(funcInfo.m_wstrToolName.c_str(), hSnapItem);
					}
				}
			}
			m_treePattern.Expand(hRoot, TVE_EXPAND);
			m_dictTreeItem_Pattern[hRoot] = crtPattern;
			m_treePattern.Select(hRoot, TVGN_CARET);
		}
	}

	m_treePattern.GetFocus();
	m_treePattern.UpdateData(FALSE);
}

void CProjectSettingDlg::CreateAllListControls()
{
	CRect rcClient, rcBtn;
	GetClientRect(&rcClient);

	m_rcList.left = rcClient.left + m_nGapH;
	m_rcList.top = m_rcGroup.bottom + 20*LONG(gSystemMgr.m_fRatioV);
	m_rcList.right = m_rcList.left + m_nListWidth;
	m_rcList.bottom = rcClient.bottom -m_nHeight;
	InitializeCtrl(IDC_LIST_PG_PATTERN, m_rcList);
	DWORD dwStyleEx = m_listPgPattern.GetExtendedStyle();
	dwStyleEx |= LVS_EX_FULLROWSELECT;
	dwStyleEx |= LVS_EX_GRIDLINES;
	m_listPgPattern.SetExtendedStyle(dwStyleEx);

	CString strIndex, strName, strColor;
	strIndex.LoadStringW(IDS_STRING_INDEX);
	strName.LoadStringW(IDS_STRING_NAME);
	strColor.LoadStringW(IDS_STRING_COLOR);
	int nWidth0 = 80;
	int nWidth1 = 120;
	int nWidth2 = m_nListWidth - nWidth0 - nWidth1 - 25;
	m_listPgPattern.InsertColumn(0, strIndex, LVCFMT_LEFT, nWidth0);
	m_listPgPattern.InsertColumn(1, strName, LVCFMT_LEFT, nWidth1);
	m_listPgPattern.InsertColumn(2, strColor, LVCFMT_LEFT, nWidth2);

	rcBtn.left = m_rcList.right + int(0.5f*m_nGapH);
	rcBtn.top = m_rcList.top + int((m_rcList.Height() - 5*m_nBtnSz) / 2.0f);
	rcBtn.right = rcBtn.left + m_nBtnSz;
	rcBtn.bottom = rcBtn.top + m_nBtnSz;

	m_btnAddPattern.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_ADD_RIGHT);
	m_btnAddPattern.SetLabelImage(IMG_PNG_ADD_RIGHT);
	m_btnAddPattern.EnableWindow(TRUE);
	m_btnAddPattern.Invalidate();
	
	rcBtn.top = rcBtn.bottom + 3 * m_nBtnSz;
	rcBtn.bottom = rcBtn.top + m_nBtnSz;
	m_btnDeletePattern.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_DELETE_PATTERN);
	m_btnDeletePattern.SetLabelImage(IMG_PNG_DELETE);
	m_btnDeletePattern.EnableWindow(TRUE);
	m_btnDeletePattern.Invalidate();
	
	m_rcList.left = rcBtn.right + int(0.5*m_nGapH);
	m_rcList.right = m_rcList.left + m_nListWidth;
	InitializeCtrl(IDC_TREE_PROJECT_PATTERN, m_rcList);

	CString strAlgoName;
	strAlgoName.LoadStringW(IDS_STRING_ALGO_NAME);
	rcBtn.left = m_rcList.right + int(0.5f*m_nGapH);
	rcBtn.right = rcBtn.left + m_nBtnSz;
	m_btnDeleteAlgo.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_DELETE_ALGO);
	m_btnDeleteAlgo.SetLabelImage(IMG_PNG_DELETE);
	m_btnDeleteAlgo.EnableWindow(TRUE);
	m_btnDeleteAlgo.Invalidate();

	rcBtn.bottom = rcBtn.top - 3 * m_nBtnSz;
	rcBtn.top = rcBtn.bottom - m_nBtnSz;
	m_btnAddAlgo.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_ADD_LEFT);
	m_btnAddAlgo.SetLabelImage(IMG_PNG_ADD_LEFT);
	m_btnAddAlgo.EnableWindow(TRUE);
	m_btnAddAlgo.Invalidate();

	m_rcList.left = rcBtn.right + int(0.5*m_nGapH);
	//m_rcList.right = m_rcList.left + m_nListWidth;
	m_rcList.right = m_rcGroup.right;
	InitializeCtrl(IDC_LIST_ALGO_REPOSITORY, m_rcList);
	m_rcAlgoList = m_rcList;
	dwStyleEx = m_listAlgoRepository.GetExtendedStyle();
	dwStyleEx |= LVS_EX_FULLROWSELECT;
	dwStyleEx |= LVS_EX_GRIDLINES;
    nWidth0 = 80;
	nWidth1 = m_nListWidth - nWidth0 - 5;
	m_listAlgoRepository.SetExtendedStyle(dwStyleEx);
	m_listAlgoRepository.InsertColumn(0, strIndex, LVCFMT_CENTER, nWidth0);
	m_listAlgoRepository.InsertColumn(1, strAlgoName, LVCFMT_LEFT, nWidth1);

	CRect rcMsg;
	rcMsg.left = m_rcGroup.left;
	rcMsg.top = m_rcList.bottom;
	rcMsg.right = m_rcGroup.right;
	rcMsg.bottom = rcClient.bottom;
	InitializeCtrl(IDC_STATIC_MESSAGE_PROJECT_UI, rcMsg);
}

void CProjectSettingDlg::CreateParamWidget()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int nLen = int(90 * gSystemMgr.m_fRatioH);
	int nHeight = int(30 * gSystemMgr.m_fRatioV);
	CRect rcStatic, rcEdit;
	// Upper filter && Exposure time
	rcStatic.left = m_rcAlgoList.right + m_nGapH;
	rcStatic.top = m_rcAlgoList.top;
	rcStatic.right = rcStatic.left + nLen;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_UPPER_FILTER, rcStatic, IDS_STRING_FILTER_WHEEL_UP);
	InitializeCtrl(IDC_STATIC_EXPOSURE, rcStatic, IDS_STRING_EXPOSURE_TIME);
	rcEdit.left = rcStatic.right + 2;
	rcEdit.top = rcStatic.top;
	rcEdit.right = rcClient.right;
	rcEdit.bottom = rcStatic.bottom;
	InitializeCtrl(IDC_EDIT_UPPER_FILTER, rcEdit);
	InitializeCtrl(IDC_EDIT_EXPOSURE, rcEdit);
	// Lower filter && Camera Gain
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_LOWER_FILTER, rcStatic, IDS_STRING_FILTER_WHEEL_DOWN);
	InitializeCtrl(IDC_STATIC_GAIN, rcStatic, IDS_STRING_CAMERA_GAIN);
	rcEdit.top = rcEdit.bottom + m_nGapV;
	rcEdit.bottom = rcEdit.top + nHeight;
	InitializeCtrl(IDC_EDIT_LOWER_FILTER, rcEdit);
	InitializeCtrl(IDC_EDIT_GAIN, rcEdit);
	//Empty && White Balance R
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	CRect rcCheck = rcStatic;
	rcCheck.right += 20;
	InitializeCtrl(IDC_CHECK_SIDE_LIGHT_ON, rcCheck, IDS_STRING_SIDE_LIGHT_ON);
	CWnd* pWnd = GetDlgItem(IDC_CHECK_SIDE_LIGHT_ON);
	m_bgBrushHelper.AddSubWindow(this, pWnd, m_bgImg);
	SetWindowTheme(pWnd->m_hWnd, L"", L"");

	InitializeCtrl(IDC_STATIC_WHITE_BALANCE_R, rcStatic, IDS_STRING_CAMERABALANCE_R);
	rcEdit.top = rcEdit.bottom + m_nGapV;
	rcEdit.bottom = rcEdit.top + nHeight;
	InitializeCtrl(IDC_EDIT_WHITE_BALANCE_R, rcEdit);

	//White Balance G
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_WHITE_BALANCE_G, rcStatic, IDS_STRING_CAMERABALANCE_G);
	rcEdit.top = rcEdit.bottom + m_nGapV;
	rcEdit.bottom = rcEdit.top + nHeight;
	InitializeCtrl(IDC_EDIT_WHITE_BALANCE_G, rcEdit);
	//White Balance B
	rcStatic.top = rcStatic.bottom + m_nGapV;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_WHITE_BALANCE_B, rcStatic, IDS_STRING_CAMERABALANCE_B);
	rcEdit.top = rcEdit.bottom + m_nGapV;
	rcEdit.bottom = rcEdit.top + nHeight;
	InitializeCtrl(IDC_EDIT_WHITE_BALANCE_B, rcEdit);
	// Save button.
	CRect rcBtn;
	rcBtn.right = rcEdit.right;
	rcBtn.left = rcEdit.right - m_nBtnSz;
	rcBtn.top = rcEdit.bottom + 2*m_nGapV;
	rcBtn.bottom = rcBtn.top + m_nBtnSz;
	m_btnSave.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcBtn, this, BTN_SAVE);
	m_btnSave.SetLabelImage(IMG_PNG_SAVE);
	m_btnSave.EnableWindow(TRUE);
	m_btnSave.ShowWindow(SW_HIDE);
	m_btnSave.Invalidate();
}

void CProjectSettingDlg::FillUpPatternRepository()
{
	m_ptrPgImageArray =  gConfigFileMgr.GetPgSetting();
	if (m_ptrPgImageArray->empty())
	{
		return;
	}
	CString sIndex;
	int nItemIndex = 0;
	for (auto it = m_ptrPgImageArray->begin(); it != m_ptrPgImageArray->end(); it++)
	{
		int nIndex = it->first;
		PG_IMAGE pgImg = it->second;
		sIndex.Format(_T("%d"), nIndex+1);
		m_listPgPattern.InsertItem(nItemIndex, sIndex);
		m_listPgPattern.SetItemText(nItemIndex, 1, pgImg.m_wstrName.c_str());
		nItemIndex++;
	}
}

void CProjectSettingDlg::FillUpAlgoRepository()
{
	HRESULT hr = S_FALSE;
	std::vector<std::tuple<long, wstring, wstring>> vFuncRepository;
	if (m_ptrAlgoObj != nullptr)
	{
		hr = localUtils::utils::GetIDispatchMethods(m_ptrAlgoObj, vFuncRepository);
	}
	if (SUCCEEDED(hr))
	{
		CString strIdx;
		int index = 0;
		std::wstring strHelp;
		std::for_each(vFuncRepository.begin(), vFuncRepository.end(), [&](std::tuple<long, wstring,wstring> item)
		{
			strIdx.Format(_T("   %d"), index + 1);
			m_listAlgoRepository.InsertItem(index, strIdx);
			wstring wstrHelp = std::get<2>(item);
			std::vector<wstring> vHelpStrItem = localUtils::utils::SplitString(wstrHelp, L"$");
			if (!vHelpStrItem.empty())
			{
				std::vector<wstring> vRcID = localUtils::utils::SplitString(vHelpStrItem[0], L";");
				if (!vRcID.empty())
				{
					UINT nRcID = boost::lexical_cast<UINT>(vRcID[0]);
					CString sName;
					sName.LoadStringW(nRcID);
					wstring wstrName = sName.GetString();
					std::vector<wstring> vStr = localUtils::utils::SplitString(wstrName, L"$");
					if (!vStr.empty())
					{
						m_listAlgoRepository.SetItemText(index, 1, vStr[0].c_str());
						index++;
						std::tuple<long, wstring, wstring, wstring> tmpTuple = std::make_tuple(std::get<0>(item), std::get<1>(item), vStr[0], wstrHelp);
						m_vFuncRepository.push_back(tmpTuple);
					}
				}
			}
		});
	}
}

void CProjectSettingDlg::DisplayMessage(CString sMsg, bool bOK)
{
	SetDlgItemTextW(IDC_STATIC_MESSAGE_PROJECT_UI, _T(""));
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_STATIC_MESSAGE_PROJECT_UI);
	pWnd->ShowWindow(SW_HIDE);
	m_bOkStatus = bOK;
	SetDlgItemTextW(IDC_STATIC_MESSAGE_PROJECT_UI, sMsg);
	pWnd->ShowWindow(SW_SHOW);
	SetTimer(TIMER_ID_FOR_PROJECT_UI, 5000,NULL);
}

int CProjectSettingDlg::GetChildNodeCount(const HTREEITEM & hTreeItem)
{
	if (hTreeItem == nullptr)
	{
		return 0;
	}
	HTREEITEM hChild = m_treePattern.GetChildItem(hTreeItem);
	int nCount = 0;
	if (hChild != nullptr)
	{
		nCount++;
	}
	while ((hChild = m_treePattern.GetNextSiblingItem(hChild)) != nullptr)
	{
		nCount++;
	}
	return nCount;
}

void CProjectSettingDlg::ShowWidget(int nStrID, BOOL bShow)
{
	CWnd* pWidget = GetDlgItem(nStrID);
	pWidget->ShowWindow(bShow);
}

void CProjectSettingDlg::ShowDeviceActionWidget(bool bShow)
{
	BOOL showFlag = (bShow) ? SW_SHOW : SW_HIDE;
	ShowWidget(IDC_STATIC_UPPER_FILTER, showFlag);
	ShowWidget(IDC_EDIT_UPPER_FILTER, showFlag);
	ShowWidget(IDC_STATIC_LOWER_FILTER, showFlag);
	ShowWidget(IDC_EDIT_LOWER_FILTER, showFlag);
	ShowWidget(IDC_CHECK_SIDE_LIGHT_ON, showFlag);
	m_btnSave.ShowWindow(showFlag);
}

void CProjectSettingDlg::ShowSnapActionWidget(bool bShow)
{
	BOOL showFlag = (bShow) ? SW_SHOW : SW_HIDE;
	ShowWidget(IDC_STATIC_EXPOSURE, showFlag);
	ShowWidget(IDC_EDIT_EXPOSURE, showFlag);
	ShowWidget(IDC_STATIC_GAIN, showFlag);
	ShowWidget(IDC_EDIT_GAIN, showFlag);
	ShowWidget(IDC_STATIC_WHITE_BALANCE_R, showFlag);
	ShowWidget(IDC_EDIT_WHITE_BALANCE_R, showFlag);
	ShowWidget(IDC_STATIC_WHITE_BALANCE_G, showFlag);
	ShowWidget(IDC_EDIT_WHITE_BALANCE_G, showFlag);
	ShowWidget(IDC_STATIC_WHITE_BALANCE_B, showFlag);
	ShowWidget(IDC_EDIT_WHITE_BALANCE_B, showFlag);
	m_btnSave.ShowWindow(showFlag);
}

bool CProjectSettingDlg::GetRootItem(const HTREEITEM & nodeItem, HTREEITEM & retRootItem)
{
	if (nodeItem == NULL)
	{
		return false;
	}
	HTREEITEM tmpItem = nodeItem;
	while ((retRootItem = m_treePattern.GetParentItem(tmpItem)) != NULL)
	{
		tmpItem = retRootItem;
	}
	retRootItem = tmpItem;
	return true;
}

bool CProjectSettingDlg::IsRootItem(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return false;
	}
	if (m_treePattern.GetParentItem(treeItem) == NULL)
	{
		return true;
	}
	return false;
}

SNAP_ACTION* CProjectSettingDlg::GetSnapActionReference(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return nullptr;
	}
	HTREEITEM hRoot = NULL;
	if (!GetRootItem(treeItem, hRoot) || (m_dictTreeItem_Pattern.find(hRoot) == m_dictTreeItem_Pattern.end()))
	{
		return nullptr;
	}
	CString sName = m_treePattern.GetItemText(treeItem);
	if (sName.Find(_T("Snap")) != -1)
	{
		FRAME_PATTERN& framePattern = m_dictTreeItem_Pattern[hRoot];
		HTREEITEM hParent = m_treePattern.GetParentItem(treeItem);
		if (hParent != NULL)
		{
			CString sCameraName = m_treePattern.GetItemText(hParent);
			wstring wstrCameraName = sCameraName.GetString();
			if (framePattern.m_dictCameraName_SAA.find(wstrCameraName) != framePattern.m_dictCameraName_SAA.end())
			{
				std::vector<SNAP_ACTION>& snapArray = framePattern.m_dictCameraName_SAA[wstrCameraName];
				if (!snapArray.empty())
				{
					int nIndex = GetItemIndexAmongSibling(treeItem);
					if (nIndex < snapArray.size())
					{
						return &snapArray[nIndex];
					}
				}
			}
		}
	}
	else
	{
		if (m_treePattern.ItemHasChildren(treeItem) == FALSE)
		{
			HTREEITEM hParent = m_treePattern.GetParentItem(treeItem);
			if (hParent != NULL)
			{
				return GetSnapActionReference(hParent);
			}
		}
	}

	return nullptr;
}

void CProjectSettingDlg::DeleteSnapAction(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return;
	}
	CString sName = m_treePattern.GetItemText(treeItem);
	if (sName.Find(_T("Snap")) == -1)
	{
		return;
	}
	HTREEITEM hRoot = NULL;
	if (GetRootItem(treeItem, hRoot)
		&& m_dictTreeItem_Pattern.find(hRoot) != m_dictTreeItem_Pattern.end())
	{
		FRAME_PATTERN& framePattern = m_dictTreeItem_Pattern[hRoot];
		HTREEITEM hParent = m_treePattern.GetParentItem(treeItem);
		if (hParent != NULL)
		{
			CString sCameraName = m_treePattern.GetItemText(hParent);
			wstring wstrCameraName = sCameraName.GetString();
			if (framePattern.m_dictCameraName_SAA.find(wstrCameraName) != framePattern.m_dictCameraName_SAA.end())
			{
				std::vector<SNAP_ACTION>& snapArray = framePattern.m_dictCameraName_SAA[wstrCameraName];
				if (!snapArray.empty())
				{
					int nIndex = GetItemIndexAmongSibling(treeItem);
					if (nIndex < snapArray.size())
					{
						//snapArray.erase(std::remove(snapArray.begin(), snapArray.end(), snapArray[nIndex]), snapArray.end());
						std::vector<SNAP_ACTION> tmpArray = snapArray;
						snapArray.clear();
						for (size_t t = 0; t < tmpArray.size(); t++)
						{
							if (t != nIndex)
							{
								snapArray.push_back(tmpArray[t]);
							}
						}
					}
				}
			}
		}
	}
}

void CProjectSettingDlg::DeleteBaslerCamera(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return;
	}
	HTREEITEM hRoot = NULL;
	if (GetRootItem(treeItem, hRoot) && m_dictTreeItem_Pattern.find(hRoot) != m_dictTreeItem_Pattern.end())
	{
		CString sCameraName = m_treePattern.GetItemText(treeItem);
		wstring wstrCameraName = sCameraName.GetString();
		FRAME_PATTERN& crtPattern = m_dictTreeItem_Pattern[hRoot];
		if (crtPattern.m_dictCameraName_SAA.find(wstrCameraName) != crtPattern.m_dictCameraName_SAA.end())
		{
			crtPattern.m_dictCameraName_SAA.erase(wstrCameraName);
		}
	}
}

void CProjectSettingDlg::DeleteAlgorithm(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return;
	}
	HTREEITEM hRoot = NULL;
	if (GetRootItem(treeItem, hRoot) && m_dictTreeItem_Pattern.find(hRoot) != m_dictTreeItem_Pattern.end())
	{
		FRAME_PATTERN& crtPattern = m_dictTreeItem_Pattern[hRoot];
		HTREEITEM hSnap = m_treePattern.GetParentItem(treeItem);
		if (hSnap != NULL)
		{
			int nIndex = GetItemIndexAmongSibling(hSnap);
			if (nIndex != -1)
			{
				HTREEITEM hCamera = m_treePattern.GetParentItem(hSnap);
				if (hCamera != NULL)
				{
					CString sCameraName = m_treePattern.GetItemText(hCamera);
					wstring wstrCameraName = sCameraName.GetString();
					if (crtPattern.m_dictCameraName_SAA.find(wstrCameraName) != crtPattern.m_dictCameraName_SAA.end())
					{
						SNAP_ACTION& snapAction = crtPattern.m_dictCameraName_SAA[wstrCameraName][nIndex];
						int nAlgoIndex = GetAlgoItemIndex(treeItem);
						if (nAlgoIndex != -1 && nAlgoIndex < snapAction.m_vAlgoFunc.size())
						{
							std::vector<FUNC_INFO> tmpArray = snapAction.m_vAlgoFunc;
							snapAction.m_vAlgoFunc.clear();
							for (size_t t = 0; t < tmpArray.size(); t++)
							{
								if (t != nAlgoIndex)
								{
									snapAction.m_vAlgoFunc.push_back(tmpArray[t]);
								}
							}
						}
					}
				}
			}
		}
	}
}

int CProjectSettingDlg::GetItemIndexAmongSibling(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return -1;
	}
	int nRetIndex = 0;
	HTREEITEM hSiblingItem = treeItem;
	while ((hSiblingItem = m_treePattern.GetPrevSiblingItem(hSiblingItem)) != NULL)
	{
		nRetIndex++;
	}
	return nRetIndex;
}

int CProjectSettingDlg::GetAlgoItemIndex(const HTREEITEM & treeItem)
{
	if (treeItem == NULL)
	{
		return -1;
	}
	int nRetIndex = 0;
	HTREEITEM hItem = treeItem;
	while ((hItem = m_treePattern.GetPrevSiblingItem(hItem)) != NULL)
	{
		nRetIndex++;
	}
	return nRetIndex;
}

void CProjectSettingDlg::ReadDeviceActionParameter(bool & bLightOn)
{
	UpdateData(TRUE);
	CString sUpperFilter, sLowerFilter;
	GetDlgItemTextW(IDC_EDIT_UPPER_FILTER, sUpperFilter);
	GetDlgItemTextW(IDC_EDIT_LOWER_FILTER, sLowerFilter);
	if (!localUtils::utils::IsNumericString(sUpperFilter) || !localUtils::utils::IsNumericString(sLowerFilter))
	{
		CString sMsg;
		sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
		DisplayMessage(sMsg, false);
		return;
	}
	bLightOn = ((m_bTurnOnLight == TRUE) ? true : false);
}

void CProjectSettingDlg::RestoreDeviceActionParameter(const HTREEITEM & treeItem)
{
	SetDlgItemTextW(IDC_EDIT_UPPER_FILTER, _T(""));
	SetDlgItemTextW(IDC_EDIT_LOWER_FILTER, _T(""));
	m_bTurnOnLight = FALSE;
	if (m_dictTreeItem_Pattern.find(treeItem) != m_dictTreeItem_Pattern.end())
	{
		FRAME_PATTERN& framePattern = m_dictTreeItem_Pattern[treeItem];
		CString sValue;
		m_bTurnOnLight = (framePattern.m_bSideLightOn ? TRUE : FALSE);
	}
	UpdateData(FALSE);
}

void CProjectSettingDlg::ReadSnapActionParameter(const HTREEITEM& treeItem)
{
	SNAP_ACTION* ptrTarget = GetSnapActionReference(treeItem);
	if (ptrTarget != nullptr)
	{
		CString sValue, sMsg;
		GetDlgItemTextW(IDC_EDIT_EXPOSURE, sValue);
		if (!localUtils::utils::IsNumericString(sValue))
		{
			sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
			DisplayMessage(sMsg, false);
			return;
		}
		ptrTarget->m_nExposureTime = (long)localUtils::utils::to_float(sValue);

		GetDlgItemTextW(IDC_EDIT_GAIN, sValue);
		if (!localUtils::utils::IsNumericString(sValue))
		{
			sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
			DisplayMessage(sMsg, false);
			return;
		}
		ptrTarget->m_nGain = (long)localUtils::utils::to_float(sValue);

		GetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_R, sValue);
		if (!localUtils::utils::IsNumericString(sValue))
		{
			sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
			DisplayMessage(sMsg, false);
			return;
		}
		ptrTarget->m_nWhiteBalanceR = (long)localUtils::utils::to_float(sValue);
		
		GetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_G, sValue);
		if (!localUtils::utils::IsNumericString(sValue))
		{
			sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
			DisplayMessage(sMsg, false);
			return;
		}
		ptrTarget->m_nWhiteBalanceG = (long)localUtils::utils::to_float(sValue);

		GetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_B, sValue);
		if (!localUtils::utils::IsNumericString(sValue))
		{
			sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
			DisplayMessage(sMsg, false);
			return;
		}
		ptrTarget->m_nWhiteBalanceB = (long)localUtils::utils::to_float(sValue);

		sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
		DisplayMessage(sMsg, true);
		return;
	}
}

void CProjectSettingDlg::RestoreSnapActionParameter(const HTREEITEM & treeItem)
{
	SNAP_ACTION* ptrTarget = GetSnapActionReference(treeItem);
	if (ptrTarget != nullptr)
	{
		CString sValue;
		sValue.Format(_T("%d"), ptrTarget->m_nExposureTime);
		SetDlgItemTextW(IDC_EDIT_EXPOSURE, sValue);
		sValue.Format(_T("%d"), ptrTarget->m_nGain);
		SetDlgItemTextW(IDC_EDIT_GAIN, sValue);
		sValue.Format(_T("%d"), ptrTarget->m_nWhiteBalanceR);
		SetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_R, sValue);
		sValue.Format(_T("%d"), ptrTarget->m_nWhiteBalanceG);
		SetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_G, sValue);
		sValue.Format(_T("%d"), ptrTarget->m_nWhiteBalanceB);
		SetDlgItemTextW(IDC_EDIT_WHITE_BALANCE_B, sValue);
	}
}

bool CProjectSettingDlg::GenerateProjectCommonParameters(PROJECT & crtProject)
{
	CString sProjectName, sValue;
	int nSel = m_comboProject.GetCurSel();
	if (nSel >= 0)
	{
		m_comboProject.GetLBText(nSel, sProjectName);
	}
	else
	{
		m_comboProject.GetWindowTextW(sProjectName);
	}
	wstring wstrName = sProjectName.GetString();
	CString sMsg;
	if (wstrName.empty())
	{
		sMsg.LoadStringW(IDS_STRING_PROJECT_NAME_INVALID);
		DisplayMessage(sMsg, false);
		return false;
	}
	crtProject.m_wstrProjectName = wstrName;
	GetDlgItemTextW(IDC_EDIT_MAIN_CAMERA_HEIGHT, sValue);
	if (!localUtils::utils::IsNumericString(sValue))
	{
		sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
		DisplayMessage(sMsg, false);
		return false;
	}
	crtProject.m_nMainCameraHeight = (int)localUtils::utils::to_float(sValue);
	CString sWidth, sHeight;
	GetDlgItemTextW(IDC_EDIT_DEMENSION_WIDTH, sWidth);
	GetDlgItemTextW(IDC_EDIT_DEMENSION_HEIGHT, sHeight);
	if (!localUtils::utils::IsNumericString(sWidth) || !localUtils::utils::IsNumericString(sHeight))
	{
		sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
		DisplayMessage(sMsg, false);
		return false;
	}
	crtProject.m_screenParam.m_nDimensionWidth = (long)localUtils::utils::to_float(sWidth);
	crtProject.m_screenParam.m_nDimensionHeight = (long)localUtils::utils::to_float(sHeight);

	GetDlgItemTextW(IDC_EDIT_RESOLUTION_WIDTH, sWidth);
	GetDlgItemTextW(IDC_EDIT_RESOLUTION_HEIGHT, sHeight);
	if (!localUtils::utils::IsNumericString(sWidth) || !localUtils::utils::IsNumericString(sHeight))
	{
		sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
		DisplayMessage(sMsg, false);
		return false;
	}
	crtProject.m_screenParam.m_nResolutionWidth_S = (long)localUtils::utils::to_float(sWidth);
	crtProject.m_screenParam.m_nResolutionHeight_S = (long)localUtils::utils::to_float(sHeight);

	CString sStation, sProduct;
	GetDlgItemTextW(IDC_EDIT_STATION_NAME, sStation);
	GetDlgItemTextW(IDC_EDIT_PRODUCT_NAME, sProduct);
	if (sStation == L"" || sProduct == L"")
	{
		sMsg.LoadStringW(IDS_STRING_INVALID_PARAMETER);
		DisplayMessage(sMsg, false);
		return false;
	}
	crtProject.m_screenParam.m_strStationName = sStation.GetString();
	crtProject.m_screenParam.m_strProductName = sProduct.GetString();
	return true;
}

bool CProjectSettingDlg::GenerateProjectPatternParameters(PROJECT & crtProject)
{
	CString sMsg;
	if (m_dictTreeItem_Pattern.empty())
	{
		sMsg.LoadStringW(IDS_STRING_WARNING_NO_PATTERN);
		DisplayMessage(sMsg, false);
		return false;
	}
	int nIndex = 0;
	std::map<int, FRAME_PATTERN> dictPattern;
	for (auto it = m_dictTreeItem_Pattern.begin(); it != m_dictTreeItem_Pattern.end(); it++)
	{
		FRAME_PATTERN& pattern = it->second;
		VerifyFramePattern(pattern);
		if (pattern.m_nIndex == -1)
		{
			pattern.m_nIndex = nIndex;
		}
		dictPattern[pattern.m_nIndex] = pattern;
		nIndex++;
	}

	for (auto it = dictPattern.begin(); it != dictPattern.end(); it++)
	{
		crtProject.m_vPattern.push_back(it->second);
	}
	return true;
}

void CProjectSettingDlg::VerifyFramePattern(FRAME_PATTERN & pattern)
{
	for (auto it = pattern.m_dictCameraName_SAA.begin(); it != pattern.m_dictCameraName_SAA.end(); it++)
	{
		std::vector<SNAP_ACTION>& vSnap = it->second;
		for (auto t = 0; t < vSnap.size(); t++)
		{
			SNAP_ACTION& snap = vSnap[t];
			int nRgnID = -1;
			for (auto i = 0; i < snap.m_vAlgoFunc.size(); i++)
			{
				FUNC_INFO& fi = snap.m_vAlgoFunc[i];
				if (!fi.m_bTestFunc)// Location Algorithm
				{
					nRgnID++;
				}
				fi.m_funcIndex = i;
				fi.m_nRgnId = nRgnID;
				if (!fi.m_vParam.empty())
				{
					fi.m_vParam[0].m_value = (float)nRgnID;
				}
				fi.m_vParamStr[1] = boost::lexical_cast<wstring>(nRgnID);
			}
		}
	}
}

void CProjectSettingDlg::UpdateStationConfig()
{
	::PostMessage(gSystemMgr.m_ptrMainUi->m_hWnd, WM_UPDATE_STATION_CONFIG, WPARAM(NULL), LPARAM(NULL));
}

BOOL CProjectSettingDlg::OnInitDialog()
{
	::CoInitialize(NULL);

	HRESULT hr = m_ptrAlgoObj.CoCreateInstance(__uuidof(AOIAlgo));

	UpdateData(FALSE);
	CRect rcClient;
	GetClientRect(&rcClient);

	m_nGapH = int(15 * gSystemMgr.m_fRatioH);
	m_nGapV = int(20 * gSystemMgr.m_fRatioV);
	m_nHeight = int(32 * gSystemMgr.m_fRatioV);
	m_nBtnSz = int(48 * gSystemMgr.m_fRatioV);
	m_nListWidth = int((rcClient.Width() - int(160 * gSystemMgr.m_fRatioH) - 2 * m_nBtnSz - 5 * m_nGapH) / 3.0f);

	CreateUiWidget();
	ListAllProjects();
	CreateAllListControls();
	CreateParamWidget();
	FillUpPatternRepository();
	FillUpAlgoRepository();
	return TRUE;
}

BOOL CProjectSettingDlg::PreTranslateMessage(MSG * pMsg)
{
	WPARAM param = pMsg->wParam;
	if (param == VK_ESCAPE || param == VK_RETURN || param == VK_SPACE)
		return TRUE;
	if (pMsg->message == WM_SYSKEYDOWN && param == VK_F4)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CProjectSettingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(BTN_ADD_RIGHT, &CProjectSettingDlg::OnStnClickedAddPattern)
	ON_STN_CLICKED(BTN_DELETE_PATTERN, &CProjectSettingDlg::OnStnClickedDeletePattern)
	ON_STN_CLICKED(BTN_ADD_LEFT, &CProjectSettingDlg::OnStnClickedAddAlgo)
	ON_STN_CLICKED(BTN_DELETE_ALGO, &CProjectSettingDlg::OnStnClickedDeleteAlgo)
	ON_STN_CLICKED(BTN_SAVE, &CProjectSettingDlg::OnStnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON_PROJECT_CREATE, &CProjectSettingDlg::OnBnClickedButtonProjectCreate)
	ON_BN_CLICKED(IDC_BUTTON_PROJECT_MODIFY, &CProjectSettingDlg::OnBnClickedButtonProjectModify)
	ON_BN_CLICKED(IDC_BUTTON_PROJECT_COPY, &CProjectSettingDlg::OnBnClickedButtonProjectCopy)
	ON_BN_CLICKED(IDC_BUTTON_PROJECT_DELETE, &CProjectSettingDlg::OnBnClickedButtonProjectDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PG_PATTERN, &CProjectSettingDlg::OnNMClickListPgPattern)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ALGO_REPOSITORY, &CProjectSettingDlg::OnNMClickListAlgoRepository)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PROJECT_PATTERN, &CProjectSettingDlg::OnNMClickTreeProjectPattern)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROJECT_PATTERN, &CProjectSettingDlg::OnTvnSelchangedTreeProjectPattern)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_PG_PATTERN, &CProjectSettingDlg::OnNMCustomdrawListPgPattern)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PROJECT_PATTERN, &CProjectSettingDlg::OnNMRClickTreeProjectPattern)
	ON_COMMAND_RANGE(IDR_ADD_CAMERA_1, IDR_ADD_CAMERA_4, &CProjectSettingDlg::OnAddCamera)
	ON_COMMAND(ID_ADD_SNAP,OnAddSnap)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_PROJECT_NAME, &CProjectSettingDlg::OnCbnSelchangeComboProjectName)
END_MESSAGE_MAP()

void CProjectSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ClientToScreen(&rcWindow);

	//CBitmap bakImg;
	BITMAP bitmap;
	m_bgImg.GetBitmap(&bitmap);
	CBitmap* pPngOld = dcMem.SelectObject(&m_bgImg);
	CRect rcParentWindow;
	GetParent()->GetWindowRect(&rcParentWindow);
	ClientToScreen(&rcParentWindow);

	dc.StretchBlt(0, 0, rect.Width(), rect.Height(),&dcMem,
		rcWindow.left - rcParentWindow.left, rcWindow.top - rcParentWindow.top, rect.Width(), rect.Height(), SRCCOPY);
}

void CProjectSettingDlg::ResetUiWidget()
{
	m_treePattern.DeleteAllItems();
	CWnd* pWnd = GetDlgItem(IDC_EDIT_MAIN_CAMERA_HEIGHT);
	pWnd->SetWindowTextW(_T("-1"));

	pWnd = GetDlgItem(IDC_EDIT_DEMENSION_WIDTH);
	pWnd->SetWindowTextW(_T(""));

	pWnd = GetDlgItem(IDC_EDIT_DEMENSION_HEIGHT);
	pWnd->SetWindowTextW(_T(""));

	pWnd = GetDlgItem(IDC_EDIT_RESOLUTION_WIDTH);
	pWnd->SetWindowTextW(_T(""));

	pWnd = GetDlgItem(IDC_EDIT_RESOLUTION_HEIGHT);
	pWnd->SetWindowTextW(_T(""));

	pWnd = GetDlgItem(IDC_EDIT_STATION_NAME);
	pWnd->SetWindowTextW(_T(""));

	pWnd = GetDlgItem(IDC_EDIT_PRODUCT_NAME);
	pWnd->SetWindowTextW(_T(""));

	ShowDeviceActionWidget(false);
	ShowSnapActionWidget(false);
}

void CProjectSettingDlg::SetButtonAccessibility(int nComboIndex)
{
	BOOL bNewProject = ((nComboIndex <= 0)? TRUE:FALSE);
	CWnd* pBtn = GetDlgItem(IDC_BUTTON_PROJECT_CREATE);
	pBtn->EnableWindow(bNewProject);

	pBtn = GetDlgItem(IDC_BUTTON_PROJECT_COPY);
	pBtn->EnableWindow(!bNewProject);

	pBtn = GetDlgItem(IDC_BUTTON_PROJECT_MODIFY);
	pBtn->EnableWindow(!bNewProject);

	pBtn = GetDlgItem(IDC_BUTTON_PROJECT_DELETE);
	pBtn->EnableWindow(!bNewProject);
}

HBRUSH CProjectSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_PROJECT_NAME ||
		ctrlID == IDC_STATIC_MAIN_CAMERA_HEIGHT ||
		ctrlID == IDC_STATIC_DEMENSION ||
		ctrlID == IDC_STATIC_X_1 ||
		ctrlID == IDC_STATIC_RESOLUTION ||
		ctrlID == IDC_STATIC_X_2 ||
		ctrlID == IDC_STATIC_MESSAGE_PROJECT_UI ||
		ctrlID == IDC_STATIC_UPPER_FILTER ||
		ctrlID == IDC_STATIC_LOWER_FILTER ||
		ctrlID == IDC_STATIC_EXPOSURE ||
		ctrlID == IDC_STATIC_GAIN ||
		ctrlID == IDC_STATIC_WHITE_BALANCE_R ||
		ctrlID == IDC_STATIC_WHITE_BALANCE_G ||
		ctrlID == IDC_STATIC_WHITE_BALANCE_B ||
		ctrlID == IDC_CHECK_SIDE_LIGHT_ON||
		ctrlID == IDC_STATIC_PROJECT_PARAM_GROUP||
		ctrlID == IDC_STATIC_STATION_NAME ||
		ctrlID == IDC_STATIC_PRODUCT_NAME)
	{
		pDC->SetBkMode(TRANSPARENT);
		if (ctrlID == IDC_STATIC_MESSAGE_PROJECT_UI)
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
			if (ctrlID == IDC_CHECK_SIDE_LIGHT_ON|| ctrlID==IDC_STATIC_PROJECT_PARAM_GROUP)
			{
				HBRUSH brush = m_bgBrushHelper.GetHBRUSH(pWnd->m_hWnd);
				if (brush)
				{
					return brush;
				}
			}
		}
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CProjectSettingDlg::OnBnClickedButtonProjectCreate()
{
	UpdateData(TRUE);

	PROJECT  crtProject;
	if (!GenerateProjectCommonParameters(crtProject))
	{
		return;
	}
	if (!GenerateProjectPatternParameters(crtProject))
	{
		return;
	}
	CString sMsg;
	if (!gConfigFileMgr.CreateNewProject(crtProject))
	{
		sMsg.LoadStringW(IDS_STRING_OPERATION_FAILED);
		DisplayMessage(sMsg, false);
	}
	else
	{
		sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
		DisplayMessage(sMsg, true);
		m_comboProject.AddString(crtProject.m_wstrProjectName.c_str());
		gSystemMgr.m_bProjectChanged = true;
		UpdateStationConfig();
	}
	UpdateData(FALSE);
}

void CProjectSettingDlg::OnBnClickedButtonProjectModify()
{
	UpdateData(TRUE);

	PROJECT  crtProject;
	if (!GenerateProjectCommonParameters(crtProject))
	{
		return;
	}
	if (!GenerateProjectPatternParameters(crtProject))
	{
		return;
	}
	CString sMsg;
	if (!gConfigFileMgr.ModifyProject(crtProject))
	{
		sMsg.LoadStringW(IDS_STRING_OPERATION_FAILED);
		DisplayMessage(sMsg, false);
	}
	else
	{
		sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
		DisplayMessage(sMsg, true);
		UpdateStationConfig();
	}
	UpdateData(FALSE);
	gSystemMgr.m_bProjectChanged = true;
}

void CProjectSettingDlg::OnBnClickedButtonProjectCopy()
{
	UpdateData(TRUE);
	CString sSrcName;
	int nSel = m_comboProject.GetCurSel();
	if (nSel >= 0)
	{
		m_comboProject.GetLBText(nSel, sSrcName);
	}
	else
	{
		m_comboProject.GetWindowTextW(sSrcName);
	}
	wstring wstrTargetName = L"";
	CCopyProjectDlg copyDlg(sSrcName,wstrTargetName);
	if (IDOK == copyDlg.DoModal())
	{
		PROJECT tmpProject = m_crtProject;
		tmpProject.m_wstrProjectName = wstrTargetName;
		CString sMsg;
		if (gConfigFileMgr.CreateNewProject(tmpProject))
		{
			sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
			DisplayMessage(sMsg, true);
			m_comboProject.AddString(tmpProject.m_wstrProjectName.c_str());
			gSystemMgr.m_bProjectChanged = true;
		}
		else
		{
			sMsg.LoadStringW(IDS_STRING_OPERATION_FAILED);
			DisplayMessage(sMsg, false);
		}
	}
	UpdateData(FALSE);
}

void CProjectSettingDlg::OnBnClickedButtonProjectDelete()
{
	UpdateData(TRUE);
	CString sProjectName, sMsg;
	m_comboProject.GetWindowTextW(sProjectName);
	wstring wstrName = sProjectName.GetString();

	wstring wstrCrtProjectName = gConfigFileMgr.GetCurrentProjectName();
	if (wstrCrtProjectName == wstrName)
	{
		sMsg.LoadStringW(IDS_STRING_WARNING_PROJECT_INUSE);
		DisplayMessage(sMsg,false);
		return;
	}
	
	CString tmpMsg;
	tmpMsg.LoadStringW(IDS_STRING_PROMPT_DELETE_PROJECT);
	sMsg.Format(tmpMsg, sProjectName);
	if (IDNO == AfxMessageBox(sMsg, MB_YESNO, NULL))
	{
		return;
	}
	
	if (gConfigFileMgr.DeleteProject(wstrName))
	{
		int nCrtSel = m_comboProject.GetCurSel();
		sMsg.LoadStringW(IDS_STRING_OPERATION_OK);
		DisplayMessage(sMsg, true);
		m_comboProject.DeleteString(nCrtSel);
		gSystemMgr.m_bProjectChanged = true;
		UpdateStationConfig();
	}
	else
	{
		sMsg.LoadStringW(IDS_STRING_OPERATION_FAILED);
		DisplayMessage(sMsg, false);
	}
	UpdateData(FALSE);
}

void CProjectSettingDlg::OnStnClickedAddPattern()
{
	if (m_nCrtPatternIndex == -1)
	{
		return;
	}
	if (m_ptrPgImageArray->find(m_nCrtPatternIndex) == m_ptrPgImageArray->end())
	{
		return;
	}
	PG_IMAGE pgImg = (*m_ptrPgImageArray)[m_nCrtPatternIndex];
	wstring wstrName = pgImg.m_wstrName;
	HTREEITEM hRoot,hSubItem;
	HTREEITEM hInsertAfter = TVI_LAST;
	hRoot = m_treePattern.InsertItem(wstrName.c_str(), TVI_ROOT, hInsertAfter);

	FRAME_PATTERN newPattern;
	newPattern.m_nSeqIndex = pgImg.m_seqIndex;
	newPattern.m_wstrName = wstrName;

	CString sMainCamera;
	sMainCamera.LoadStringW(IDS_STRING_MAIN_CAMERA);
	hSubItem = m_treePattern.InsertItem(sMainCamera,hRoot);
	m_treePattern.Expand(hRoot, TVE_EXPAND);

	wstring wstrMainCamera = sMainCamera.GetString();
	newPattern.m_dictCameraName_SAA[wstrMainCamera] = std::vector<SNAP_ACTION>();

	m_treePattern.Select(hSubItem, TVGN_CARET);
	m_crtSelectedItem = hSubItem;
	m_nCrtPatternIndex = -1;
	m_treePattern.GetFocus(); 
	m_treePattern.UpdateData(FALSE);

	m_dictTreeItem_Pattern[hRoot] = newPattern;
}

void CProjectSettingDlg::OnStnClickedDeletePattern()
{
	CString sMainCamera,sMsg;
	sMainCamera.LoadStringW(IDS_STRING_MAIN_CAMERA);
	if (m_crtSelectedItem != nullptr)
	{
		if (m_treePattern.GetParentItem(m_crtSelectedItem) == nullptr) // delete frame pattern
		{
			if (m_dictTreeItem_Pattern.find(m_crtSelectedItem) != m_dictTreeItem_Pattern.end())
			{
				m_dictTreeItem_Pattern.erase(m_crtSelectedItem);
			}
			m_treePattern.DeleteItem(m_crtSelectedItem);
			m_crtSelectedItem = m_treePattern.GetSelectedItem();
		}
		else
		{
			CString str = m_treePattern.GetItemText(m_crtSelectedItem);
			if (str == sMainCamera)
			{
				sMsg.LoadStringW(IDS_STRING_WARNING_CANNOT_DELETE_MAIN_CAMERA);
				DisplayMessage(sMsg, false);
				return;
			}
			else
			{
				HTREEITEM hParent = m_treePattern.GetParentItem(m_crtSelectedItem);
				if (hParent != NULL)
				{
					CString sName = m_treePattern.GetItemText(hParent);
					if (sName.Find(_T("Snap#")) != -1)
					{
						return;
					}
				}
				//Delete Basler Camera
				CString sItemName = m_treePattern.GetItemText(m_crtSelectedItem);
				if (sItemName.Find(_T("Snap")) != -1) // delete a snap 
				{
					DeleteSnapAction(m_crtSelectedItem);
				}
				else // delete a Basler camera
				{
					DeleteBaslerCamera(m_crtSelectedItem);
				}
				m_treePattern.DeleteItem(m_crtSelectedItem);
			}
		}
	}
}

void CProjectSettingDlg::OnStnClickedAddAlgo()
{
	if (m_crtSelectedItem == nullptr || m_nCrtAlgoIndex_R == -1)
	{
		return;
	}
	SNAP_ACTION* ptrSnap = GetSnapActionReference(m_crtSelectedItem);
	if (ptrSnap == nullptr)
	{
		return;
	}
	CString sNodeName = m_treePattern.GetItemText(m_crtSelectedItem);
	bool bInsertLast = ((sNodeName.Find(_T("Snap")) != -1) ? true : false);
	if (m_nCrtAlgoIndex_R < m_vFuncRepository.size())
	{
		std::tuple<long, wstring, wstring, wstring> tmpTuple = m_vFuncRepository[m_nCrtAlgoIndex_R];
		wstring wstrFuncName = std::get<2>(tmpTuple);
		if (bInsertLast)
		{
			m_treePattern.InsertItem(wstrFuncName.c_str(), m_crtSelectedItem, TVI_LAST);
		}
		else
		{
			HTREEITEM hParent = m_treePattern.GetParentItem(m_crtSelectedItem);
			m_treePattern.InsertItem(wstrFuncName.c_str(), hParent,m_crtSelectedItem);
		}
		
		m_treePattern.Expand(m_crtSelectedItem, TVE_EXPAND);
		//Add algorithm into FRAME_PATTERN
		FUNC_INFO crtFuncInfo;
		crtFuncInfo.m_dispId = std::get<0>(tmpTuple);
		crtFuncInfo.m_wstrFuncName = std::get<1>(tmpTuple);
		crtFuncInfo.m_wstrToolName = std::get<2>(tmpTuple);
		crtFuncInfo.m_wstrHelp = std::get<3>(tmpTuple);
		crtFuncInfo.GetTestFuncFlag();

		crtFuncInfo.ParseDefaultParameter(m_ptrAlgoObj);
		if (bInsertLast)
		{
			ptrSnap->m_vAlgoFunc.push_back(crtFuncInfo);
		}
		else
		{
			int nIndex = GetItemIndexAmongSibling(m_crtSelectedItem);
			auto itBgn = ptrSnap->m_vAlgoFunc.begin();
			ptrSnap->m_vAlgoFunc.insert(itBgn + nIndex + 1, crtFuncInfo);
		}
	}
}

void CProjectSettingDlg::OnStnClickedDeleteAlgo()
{
	if (m_crtSelectedItem != nullptr)
	{
		if (m_treePattern.GetParentItem(m_crtSelectedItem) == nullptr)
		{
			return;
		}
		CString sName = m_treePattern.GetItemText(m_crtSelectedItem);
		if (sName.Find(_T("Snap#")) != -1)
		{
			return;
		}
		CString sCamera;
		sCamera.LoadStringW(IDS_STRING_CAMERA);
		if (sName.Find(sCamera) != -1&& sName.GetLength()<5)
		{
			return;
		}

		if (m_treePattern.ItemHasChildren(m_crtSelectedItem))
		{
			return;
		}
		//delete the algorithm item from FRAME_PATTERN
		DeleteAlgorithm(m_crtSelectedItem);
		m_treePattern.DeleteItem(m_crtSelectedItem);
	}
}

void CProjectSettingDlg::OnStnClickedSave()
{
	HTREEITEM hRoot = NULL;
	if (IsRootItem(m_crtSelectedItem) 
		&& m_dictTreeItem_Pattern.find(m_crtSelectedItem) != m_dictTreeItem_Pattern.end()) // For saving device action parameters
	{
		FRAME_PATTERN& framePattern = m_dictTreeItem_Pattern[m_crtSelectedItem];
		ReadDeviceActionParameter(framePattern.m_bSideLightOn);
	}
	else
	{
		ReadSnapActionParameter(m_crtSelectedItem);
	}
}

void CProjectSettingDlg::OnNMClickListPgPattern(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int rowIndex = pNMItemActivate->iItem;
	if (rowIndex == -1)
	{
		*pResult = 0;
		return;
	}
	m_nCrtPatternIndex = rowIndex;
	*pResult = 0;
}

void CProjectSettingDlg::OnNMClickListAlgoRepository(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int rowIndex = pNMItemActivate->iItem;
	if (rowIndex == -1)
	{
		*pResult = 0;
		return;
	}
	m_nCrtAlgoIndex_R = rowIndex;
	*pResult = 0;
}

void CProjectSettingDlg::OnNMClickTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateData(TRUE);

	CPoint point;
	GetCursorPos(&point);
	m_treePattern.ScreenToClient(&point);

	UINT uFlags;
	HTREEITEM item = m_treePattern.GetSelectedItem();
	m_crtSelectedItem = m_treePattern.HitTest(point, &uFlags);
	if (m_crtSelectedItem != nullptr)
	{
		if (m_treePattern.GetParentItem(m_crtSelectedItem) == nullptr)
		{
			ShowSnapActionWidget(false);
			RestoreDeviceActionParameter(m_crtSelectedItem);
			//ShowDeviceActionWidget(true);
		}
		else
		{
			CString sItemTxt = m_treePattern.GetItemText(m_crtSelectedItem);
			if (sItemTxt.Find(_T("Snap#")) != -1)
			{
				ShowDeviceActionWidget(false);
				RestoreSnapActionParameter(m_crtSelectedItem);
				ShowSnapActionWidget(true);
			}
			else
			{
				ShowDeviceActionWidget(false);
				ShowSnapActionWidget(false);
			}
		}
	}
	*pResult = 0;
}

void CProjectSettingDlg::OnTvnSelchangedTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CProjectSettingDlg::OnNMCustomdrawListPgPattern(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = int(lplvcd->nmcd.dwItemSpec);

	if (lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	if (lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		lplvcd->clrText = RGB(0, 0, 0);
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}
	if (lplvcd->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM))
	{
		if (lplvcd->iSubItem == 2)
		{
			m_oldColor = lplvcd->clrTextBk;
			if (m_ptrPgImageArray->find(iRow) != m_ptrPgImageArray->end())
			{
				PG_IMAGE pgImg = (*m_ptrPgImageArray)[iRow];
				lplvcd->clrTextBk = pgImg.m_color;
				*pResult = CDRF_DODEFAULT;
				return;
			}
		}
	}
	*pResult = 0;
}

void CProjectSettingDlg::OnClose()
{
	m_ptrAlgoObj.Release();
	::CoUninitialize();
	CDialogEx::OnClose();
}

void CProjectSettingDlg::OnNMRClickTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint point;
	GetCursorPos(&point);
	m_treePattern.ScreenToClient(&point);
	UINT uFlags;
	HTREEITEM crtItem = m_treePattern.HitTest(point,&uFlags);
	if ((crtItem != nullptr) && (TVHT_ONITEM & uFlags))
	{
		if (crtItem != m_crtSelectedItem)
		{
			m_crtSelectedItem = crtItem;
			m_treePattern.Select(m_crtSelectedItem, TVGN_CARET);
		}
	}
	if (m_crtSelectedItem != nullptr)
	{
		if (m_treePattern.GetParentItem(m_crtSelectedItem) == nullptr)
		{
			DWORD dwPos = GetMessagePos();
			CPoint point(LOWORD(dwPos), HIWORD(dwPos));
			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_MENU_ADD_CAMERA));
			CMenu* popup = menu.GetSubMenu(0);
			ASSERT(popup != NULL);
			popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			popup->Detach();
		}
		else
		{
			CString str = m_treePattern.GetItemText(m_crtSelectedItem);
			CString sCamera;
			sCamera.LoadStringW(IDS_STRING_CAMERA);
			if (str.Find(sCamera) != -1)
			{
				DWORD dwPos = GetMessagePos();
				CPoint point(LOWORD(dwPos), HIWORD(dwPos));
				CMenu menu;
				VERIFY(menu.LoadMenu(IDR_MENU_ADD_SNAP));
				CMenu* popup = menu.GetSubMenu(0);
				ASSERT(popup != NULL);
				popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
				popup->Detach();
			}
		}
	}
	*pResult = 0;
}

void CProjectSettingDlg::OnAddCamera(UINT ID)
{
	CString sTmp, sCamera;
	sTmp.LoadStringW(IDS_STRING_AUXILIARY_CAMERA);
	sCamera.Format(sTmp, ID - IDR_ADD_CAMERA_1+1);
	wstring wstrCamera1 = sCamera.GetString();
	HTREEITEM tmpItem = m_treePattern.GetChildItem(m_crtSelectedItem);
	if (tmpItem != nullptr)
	{
		CString sItemName = m_treePattern.GetItemText(tmpItem);
		if (sItemName == sCamera)
		{
			return;
		}
		while ((tmpItem = m_treePattern.GetNextSiblingItem(tmpItem)) != nullptr)
		{
			sItemName = m_treePattern.GetItemText(tmpItem);
			if (sItemName == sCamera)
			{
				return;
			}
		}
	}
	HTREEITEM hRootItem = NULL;
	if (GetRootItem(m_crtSelectedItem, hRootItem)
		&& m_dictTreeItem_Pattern.find(hRootItem) != m_dictTreeItem_Pattern.end())
	{
		FRAME_PATTERN& pattern = m_dictTreeItem_Pattern[hRootItem];
		pattern.m_dictCameraName_SAA[wstrCamera1] = std::vector<SNAP_ACTION>();
	}
	HTREEITEM hMainCamera = m_treePattern.GetChildItem(m_crtSelectedItem);
	HTREEITEM hSubItem = NULL;
	if (hMainCamera != NULL&&(ID == IDR_ADD_CAMERA_1))
	{
		hSubItem = m_treePattern.InsertItem(sCamera, m_crtSelectedItem, hMainCamera);
	}
	else
	{
		hSubItem = m_treePattern.InsertItem(sCamera, m_crtSelectedItem, TVI_LAST);
	}
	m_treePattern.Select(hSubItem, TVGN_CARET);
	m_crtSelectedItem = hSubItem;
	ShowDeviceActionWidget(false);
	ShowSnapActionWidget(true);
	UpdateData(FALSE);
}

void CProjectSettingDlg::OnAddSnap()
{
	if (m_crtSelectedItem != nullptr)
	{
		CString sSnapIndex;
		sSnapIndex.LoadStringW(IDS_STRING_SNAP_INDEX);
		m_treePattern.InsertItem(sSnapIndex, m_crtSelectedItem, TVI_LAST);
		m_treePattern.Expand(m_crtSelectedItem, TVE_EXPAND);

		CString sCameraName = m_treePattern.GetItemText(m_crtSelectedItem);
		wstring wstrCameraName = sCameraName.GetString();
		HTREEITEM hRoot = NULL;
		if (GetRootItem(m_crtSelectedItem, hRoot) 
			&& m_dictTreeItem_Pattern.find(hRoot) != m_dictTreeItem_Pattern.end())
		{
			FRAME_PATTERN& framePattern = m_dictTreeItem_Pattern[hRoot];
			if (framePattern.m_dictCameraName_SAA.find(wstrCameraName) != framePattern.m_dictCameraName_SAA.end())
			{
				SNAP_ACTION snapAction;
				framePattern.m_dictCameraName_SAA[wstrCameraName].push_back(snapAction);
			}
		}
	}
}

void CProjectSettingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_FOR_PROJECT_UI)
	{
		KillTimer(nIDEvent);
		CWnd* pWnd = (CWnd*)GetDlgItem(IDC_STATIC_MESSAGE_PROJECT_UI);
		pWnd->ShowWindow(SW_HIDE);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CProjectSettingDlg::OnCbnSelchangeComboProjectName()
{
	UpdateData(TRUE);
	ResetUiWidget();
	int nSel = m_comboProject.GetCurSel();
	SetButtonAccessibility(nSel);
	if (nSel <= 0)
	{
		m_crtProject.m_wstrProjectName = L"";
		return;
	}
	CString sProjectName, sMsg;
	m_comboProject.GetLBText(nSel,sProjectName);
	wstring wstrName = sProjectName.GetString();

	std::map<int,STATION_CONFIG>* ptrDict =  gConfigFileMgr.GetStaionConfigArray();
	if (ptrDict != nullptr && !ptrDict->empty())
	{
		STATION_CONFIG& station = ptrDict->begin()->second;
		if (station.m_dictProject.find(wstrName) != station.m_dictProject.end())
		{
			if (wstrName != m_crtProject.m_wstrProjectName)
			{
				m_crtProject = station.m_dictProject[wstrName];
				ReplaceCameraName();
				DisplayCurrentProject();
			}
		}
	}
	UpdateData(FALSE);
}
