#include "stdafx.h"
#include "AOI.h"
#include "SystemUserManagementDlg.h"
#include "afxdialogex.h"
#include "TypeDef.h"
#include "MessageDlg.h"

extern CSystemManager    gSystemMgr;
extern ConfigFileManager gConfigFileMgr;


IMPLEMENT_DYNAMIC(CSystemUserManagementDlg, CDialogEx)

CSystemUserManagementDlg::CSystemUserManagementDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SYS_USER_MANAGEMENT, pParent) 
	, m_bHitItem(false)
	, m_nConsume(0)
{
	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	float fRatio = rcScreen.Width() / 1920.0f;

	m_smallFont.CreateFont(int(26 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));

	m_txtFont.CreateFont(int(36 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_titleFont.CreateFont(int(46 * fRatio), 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	m_penTitle.CreatePen(PS_SOLID, 3, RGB(4, 40, 94));
	const char* indexnamestr = "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, username char(5), key char(6), level char(5)";
}

CSystemUserManagementDlg::~CSystemUserManagementDlg()
{
}

void CSystemUserManagementDlg::InitializeCtrl(int nCtrlID, CRect & rcPos, int nStrID, bool bSmallFont)
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

void CSystemUserManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER_MGMT_LIST, m_listUserMgmtList);
	DDX_Control(pDX, IDC_EDIT_USER_MGMT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_USER_MGMT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_COMBO_USER_MGMT_LEVEL, m_cboxLevel);

}

BOOL CSystemUserManagementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CDialogEx::SetBackgroundImage(IDB_BITMAP_SYS_CONFIG_DLG_BK);

	CRect rcScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	float fRatioH = rcScreen.Width() / 1920.0f;
	float fRatioV = rcScreen.Height() / 1080.0f;
	int nWidth = int(SYS_USER_MANAGERMENT_UI_WIDTH * fRatioH);
	int nHeight = int(SYS_USER_MANAGERMENT_UI_HEIGHT * fRatioV);

	CPoint lt;
	lt.x = int((rcScreen.Width() - nWidth) / 2.0f) + int(fRatioH * 105);
	lt.y = int((rcScreen.Height() - nHeight) / 2.0f) + int(fRatioV * 45);
	if (lt.y < int(100 * fRatioV))
	{
		lt.y = int(100 * fRatioV);
	}
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, lt.x, lt.y, nWidth, nHeight, SWP_SHOWWINDOW);
	
	CreateUiWidget();
	UpdateList();
	m_editPassword.SetPasswordChar('*');
	return TRUE;  
				  
}

void CSystemUserManagementDlg::UpdateList()
{
	m_editUser.SetWindowText(_T(""));
	m_editPassword.SetWindowText(_T(""));
	CString UserManagement;
	QueryResult = gSystemMgr.m_sysDatabase.Query(SQL_QUERY_USER_MGMT_DATA);
	int Row = gSystemMgr.m_sysDatabase.Row(SQL_QUERY_USER_MGMT_DATA);
	for (int i = 0; !QueryResult.eof(); i++)
	{
		m_QueryResult[i].username = QueryResult.fieldValue("username");
		m_QueryResult[i].key = "******";
		m_QueryResult[i].level = QueryResult.fieldValue("level");
		wstring wstrName = localUtils::utils::Utf8ToUnicode(m_QueryResult[i].username);
		wstring wstrKey = localUtils::utils::Utf8ToUnicode(m_QueryResult[i].key);
		wstring wstrLevel = localUtils::utils::Utf8ToUnicode(m_QueryResult[i].level);
		UserManagement.Format(_T("%d"), i + 1);
		m_listUserMgmtList.InsertItem(i, UserManagement);
		m_listUserMgmtList.SetItemText(i, 1, wstrName.c_str());
		m_listUserMgmtList.SetItemText(i, 2, wstrKey.c_str());
		m_listUserMgmtList.SetItemText(i, 3, wstrLevel.c_str());
		QueryResult.NextRow();
	}

    int	m_ItemCount = m_listUserMgmtList.GetItemCount();
	if (Row < m_ItemCount)
	{
		int deleterow = 0;
		deleterow = m_ItemCount - Row;
		for (int i = deleterow; i >= 0; i--)
		{
			m_listUserMgmtList.DeleteItem(Row + i);
		}
	}
	m_listUserMgmtList.SetRedraw(TRUE);
	m_listUserMgmtList.Invalidate();
	m_listUserMgmtList.UpdateWindow();
}

BEGIN_MESSAGE_MAP(CSystemUserManagementDlg, CDialogEx)
	ON_STN_CLICKED(BTN_CLOSE, &CSystemUserManagementDlg::OnStnClickedClose)
	ON_STN_CLICKED(BTN_ADD_USERINFO, &CSystemUserManagementDlg::OnBnClickedButtonUserMgmtCreate)
	ON_STN_CLICKED(BTN_DELETE_USERINFO, &CSystemUserManagementDlg::OnBnClickedButtonUserMgmtDelete)
	ON_STN_CLICKED(BTN_UPDATE_USERINFO, &CSystemUserManagementDlg::OnBnClickedButtonUserMgmtUpdate)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_LIST_USER_MGMT_LIST, &CSystemUserManagementDlg::OnNMClickListUserMgmtList)
END_MESSAGE_MAP()

void CSystemUserManagementDlg::CreateUiWidget()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcLogo,rcExit,rcTitle,rcStatic,rcList,rcEdit,rcButton,rcCreate,rcDelete,rcUpdate,rcUser,rcKey,rcLevel;
	int nGapH = int(30 * gSystemMgr.m_fRatioH);
	int nGapV = int(25 * gSystemMgr.m_fRatioV);
	int nLen = int(160 * gSystemMgr.m_fRatioH);
	int nHeight = int(40 * gSystemMgr.m_fRatioV);
	int nCtrlLen = int(320 * gSystemMgr.m_fRatioH);//combo length
	int nCtrllistLen = int(670 * gSystemMgr.m_fRatioH);

	//------------------------------------------LOGO
	rcLogo.right = rcClient.right - 5;
	rcLogo.left = rcClient.right - int(158 * gSystemMgr.m_fRatioH);
	rcLogo.top = rcClient.top + 5;
	rcLogo.bottom = rcLogo.top + int(64 * gSystemMgr.m_fRatioH);
	m_labelLogo.Create(_T(""), WS_CHILD | WS_VISIBLE, rcLogo, this, NULL);
	m_labelLogo.SetLabelImage(IMG_PNG_SMALL_LOGO);
	m_labelLogo.BringWindowToTop();
	m_labelLogo.EnableWindow(TRUE);
	m_labelLogo.Invalidate();
	//----------------------------------------------Picture Exit
	rcExit.right = rcClient.right - 5;
	rcExit.left = rcClient.right - int(32 * gSystemMgr.m_fRatioH);
	rcExit.bottom = rcClient.bottom - 5;
	rcExit.top = rcExit.bottom - int(32 * gSystemMgr.m_fRatioV);
	m_btnClose.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcExit, this, BTN_CLOSE);
	m_btnClose.SetLabelImage(IMG_PNG_CLOSE);
	m_btnClose.BringWindowToTop();
	m_btnClose.EnableWindow(TRUE);
	m_btnClose.Invalidate();

	//------------------------------------Static
	rcStatic.left = rcClient.left + int(112 * gSystemMgr.m_fRatioH);;
	rcStatic.top = rcClient.top + int(130 * gSystemMgr.m_fRatioV);
	rcStatic.right = rcStatic.left + nLen;
	rcStatic.bottom = rcStatic.top + nHeight;
	InitializeCtrl(IDC_STATIC_USER_MGMT_LIST, rcStatic, IDS_STRING_USER_MGMT_LIST);
	//------------------------------------ListControl
	rcList.left = rcClient.left + int(112 * gSystemMgr.m_fRatioH);
	rcList.right = rcList.left + nCtrllistLen;
	rcList.top = rcClient.top + int(180 * gSystemMgr.m_fRatioV);
	rcList.bottom = rcList.top + 6 * nHeight;
	InitializeCtrl(IDC_LIST_USER_MGMT_LIST, rcList, IDS_STRING_USER_MGMT_LIST_INUSE, true);
	CString IndexTitle, UsernameTitle, PasswordTitle, LevelTitle;
	IndexTitle.Format(IDS_STRING_USER_MGMT_INDEX_TITLE);
	UsernameTitle.Format(IDS_STRING_USER_MGMT_USER_NAME_TITLE);
	PasswordTitle.Format(IDS_STRING_USER_MGMT_PASSWORD_TITLE);
	LevelTitle.Format(IDS_STRING_USER_MGMT_LEVEL_TITLE);
	m_listUserMgmtList.SetExtendedStyle(m_listUserMgmtList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listUserMgmtList.InsertColumn(0, IndexTitle, LVCFMT_LEFT, rcList.Width() / 7, 0);
	m_listUserMgmtList.InsertColumn(1, UsernameTitle, LVCFMT_LEFT, rcList.Width() * 2 / 7, 1);
	m_listUserMgmtList.InsertColumn(2, PasswordTitle, LVCFMT_LEFT, rcList.Width() * 2 / 7, 2);
	m_listUserMgmtList.InsertColumn(3, LevelTitle, LVCFMT_LEFT, rcList.Width() * 2 / 7, 3);


	//------------------------------------------USERLOGO
	rcUser.left = rcList.left;
	rcUser.right = rcUser.left + nHeight;
	rcUser.top = rcList.bottom + int(20 * gSystemMgr.m_fRatioV);
	rcUser.bottom = rcUser.top + nHeight;
	m_labelUser.Create(_T(""), WS_CHILD | WS_VISIBLE, rcUser, this, NULL);
	m_labelUser.SetLabelImage(IMG_PNG_UAC);
	m_labelUser.BringWindowToTop();
	m_labelUser.EnableWindow(TRUE);
	m_labelUser.Invalidate();
	
	rcEdit.left = rcUser.right+ int(10 * gSystemMgr.m_fRatioH);
	rcEdit.top = rcUser.top;
	rcEdit.right = rcEdit.left + nLen;
	rcEdit.bottom = rcEdit.top + nHeight;
	InitializeCtrl(IDC_EDIT_USER_MGMT_USER, rcEdit);//¿Ø¼þ

	//------------------------------------------KEYLOGO
	rcKey.left = rcEdit.right + int(20 * gSystemMgr.m_fRatioH);
	rcKey.right = rcKey.left + nHeight;
	rcKey.top = rcUser.top;
	rcKey.bottom = rcKey.top+ nHeight;
	m_labelKey.Create(_T(""), WS_CHILD | WS_VISIBLE, rcKey, this, NULL);
	m_labelKey.SetLabelImage(IMG_PNG_LOG_OUT);
	m_labelKey.BringWindowToTop();
	m_labelKey.EnableWindow(TRUE);
	m_labelKey.Invalidate();


	rcEdit.left = rcKey.right + int(10 * gSystemMgr.m_fRatioH);
	rcEdit.right = rcEdit.left + nLen;
	InitializeCtrl(IDC_EDIT_USER_MGMT_PASSWORD, rcEdit);


	//------------------------------------------LEVELLOGO
	rcLevel.left = rcEdit.right + int(20 * gSystemMgr.m_fRatioH);
	rcLevel.right = rcLevel.left + nHeight;
	rcLevel.top = rcUser.top;
	rcLevel.bottom = rcLevel.top + nHeight;
	m_labelLevel.Create(_T(""), WS_CHILD | WS_VISIBLE, rcLevel, this, NULL);
	m_labelLevel.SetLabelImage(IMG_PNG_USERLEVEL);
	m_labelLevel.BringWindowToTop();
	m_labelLevel.EnableWindow(TRUE);
	m_labelLevel.Invalidate();

	rcEdit.left = rcLevel.right + int(10 * gSystemMgr.m_fRatioH);
	rcEdit.right = rcEdit.left + nLen;
	InitializeCtrl(IDC_COMBO_USER_MGMT_LEVEL, rcEdit);
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_USER_MGMT_LEVEL);
	pCombo->SendMessage(CB_SETITEMHEIGHT, -1, 33);
	pCombo->SendMessage(CB_SETITEMHEIGHT, 0, 33);
	CString strLevel;
	strLevel.LoadStringW(IDS_STRING_USER_MGMT_LEVEL_SPAD);
	pCombo->AddString(strLevel);
	strLevel.LoadStringW(IDS_STRING_USER_MGMT_LEVEL_AD);
	pCombo->AddString(strLevel);
	strLevel.LoadStringW(IDS_STRING_USER_MGMT_LEVEL_PRO);
	pCombo->AddString(strLevel);
	pCombo->SetCurSel(0);

	//----------------------------------------------Picture Create
	rcCreate.left = rcList.right + int(15 * gSystemMgr.m_fRatioV);
	rcCreate.right = rcCreate.left + nHeight;
	rcCreate.top = rcList.top;
	rcCreate.bottom = rcCreate.top+ nHeight;
	m_btnCreate.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcCreate, this, BTN_ADD_USERINFO);
	m_btnCreate.SetLabelImage(IMG_PNG_ADD_LEFT);
	m_btnCreate.BringWindowToTop();
	m_btnCreate.EnableWindow(TRUE);
	m_btnCreate.Invalidate();

	//----------------------------------------------Picture Delete
	rcDelete.left = rcList.right + int(15 * gSystemMgr.m_fRatioV);
	rcDelete.right = rcDelete.left+ nHeight;
	rcDelete.top = rcCreate.bottom + nGapV;
	rcDelete.bottom = rcDelete.top + nHeight;
	m_btnDelete.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcDelete, this, BTN_DELETE_USERINFO);
	m_btnDelete.SetLabelImage(IMG_PNG_DELETE);
	m_btnDelete.BringWindowToTop();
	m_btnDelete.EnableWindow(TRUE);
	m_btnDelete.Invalidate();

	//----------------------------------------------Picture Update
	rcUpdate.left = rcList.right + int(15 * gSystemMgr.m_fRatioV);
	rcUpdate.right = rcUpdate.left + nHeight;
	rcUpdate.top = rcDelete.bottom + nGapV;
	rcUpdate.bottom = rcUpdate.top + nHeight;
	m_btnUpdate.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rcUpdate, this, BTN_UPDATE_USERINFO);
	m_btnUpdate.SetLabelImage(IMG_PNG_CAMERA_FIND_AGAIN);
	m_btnUpdate.BringWindowToTop();
	m_btnUpdate.EnableWindow(TRUE);
	m_btnUpdate.Invalidate();
}

void CSystemUserManagementDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rcClient, rcTitle;
	GetClientRect(&rcClient);
	CPen* pOldPen = dc.SelectObject(&m_penTitle);
	rcTitle.left = rcClient.left + int(150 * gSystemMgr.m_fRatioH);
	rcTitle.top = rcClient.top + int(25 * gSystemMgr.m_fRatioV);
	rcTitle.right = rcTitle.left + int(700 * gSystemMgr.m_fRatioH);
	rcTitle.bottom = rcTitle.top + int(65 * gSystemMgr.m_fRatioV);
	dc.MoveTo(rcClient.left, rcTitle.bottom);
	dc.LineTo(rcClient.right, rcTitle.bottom);
}

HBRUSH CSystemUserManagementDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int ctrlID = pWnd->GetDlgCtrlID();
	if (ctrlID == IDC_STATIC_USER_MGMT_LIST||
		ctrlID == IDC_STATIC_USER_MGMT_USER||
		ctrlID == IDC_STATIC_USER_MGMT_PASSWORD||
		ctrlID == IDC_STATIC_USER_MGMT_LEVEL)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 250, 250));
		return HBRUSH(GetStockObject(NULL_BRUSH));
	}
	return hbr;
}

void CSystemUserManagementDlg::OnStnClickedClose()
{
	CDialogEx::OnOK();
}


void CSystemUserManagementDlg::AddData()
{
	gSystemMgr.m_sysDatabase.saveData("UAC",
		m_StoreData[m_nConsume].username,
		m_StoreData[m_nConsume].key,
		m_StoreData[m_nConsume].level);
	m_nConsume = (m_nConsume + 1) % BUFFERSIZE;
}


void CSystemUserManagementDlg::OnBnClickedButtonUserMgmtCreate()
{
	CString strUser, strPassword, strLevel;
	m_editUser.GetWindowText(strUser);
	m_editPassword.GetWindowText(strPassword);
	m_cboxLevel.GetWindowTextW(strLevel);
	bool bNameExist = false;
	string user = localUtils::utils::Unicode2Utf8(strUser.GetString());
	string key = localUtils::utils::Unicode2Utf8(strPassword.GetString());
	string level= localUtils::utils::Unicode2Utf8(strLevel.GetString());
	m_StoreData[m_nConsume].username = user.c_str();
	m_StoreData[m_nConsume].key = key.c_str();
	m_StoreData[m_nConsume].level = level.c_str();
	if (strUser.IsEmpty() || strPassword.IsEmpty())
	{
		gSystemMgr.ShowMsg(IDS_STRING_USERINFO_EMPTY);
		bNameExist = true;
	}
	if (strLevel == "engineer")
	{
		gSystemMgr.ShowMsg(IDS_STRING_SUPERADMIN);
		return;
	}
if (gSystemMgr.m_sysDatabase.QueryRow(m_StoreData[m_nConsume].username)>0)
	{
	  gSystemMgr.ShowMsg(IDS_STRING_USERINFO_EXIST);
		bNameExist = true;
	}
    int m_ItemCount = m_listUserMgmtList.GetItemCount();
	if (!bNameExist)
	{
			if (m_ItemCount < MAXROWNUM)
			{
				AddData();
				m_listUserMgmtList.DeleteAllItems();
				UpdateList();
			}
			else
			{
				gSystemMgr.ShowMsg(IDS_STRING_UERINFO_FULL);
			}

	}
}

void CSystemUserManagementDlg::OnBnClickedButtonUserMgmtUpdate()
{
	if (!m_bHitItem)
	{
		gSystemMgr.ShowMsg(IDS_STRING_USERINFO_SELECTED);
		return;
	}
	m_bHitItem = false;
	CString newstrUsername,newstrPassword,newstrLevel;
	m_editUser.GetWindowText(newstrUsername);
	m_editPassword.GetWindowText(newstrPassword);
	m_cboxLevel.GetWindowTextW(newstrLevel);
	string userupdate = localUtils::utils::Unicode2Utf8(newstrUsername.GetString());
	string keyupdate = localUtils::utils::Unicode2Utf8(newstrPassword.GetString());
	string levelupdate = localUtils::utils::Unicode2Utf8(newstrLevel.GetString());

	if (newstrUsername.IsEmpty() || newstrPassword.IsEmpty())
		gSystemMgr.ShowMsg(IDS_STRING_USERINFO_EMPTY);
	else
	{
		string strlevel = selectedUser.level;
		if (strlevel !="engineer")
		{
			if ((strcmp(userupdate.c_str(), selectedUser.username) != 0) && (gSystemMgr.m_sysDatabase.QueryRow(userupdate.c_str()) > 0))
			{
				gSystemMgr.ShowMsg(IDS_STRING_USERINFO_EXIST);
			}
			else
			{
				if (newstrLevel != "engineer")
				{
					if (!strcmp(keyupdate.c_str(), selectedUser.key))
					{
						gSystemMgr.m_sysDatabase.updateUserData(userupdate.c_str(), levelupdate.c_str(), selectedUser.id);
					}
					else
					{
						gSystemMgr.m_sysDatabase.updateUserData(userupdate.c_str(), keyupdate.c_str(), levelupdate.c_str(), selectedUser.id);
					}
				}
				else
					gSystemMgr.ShowMsg(IDS_STRING_SUPERADMIN);
				
			}
		}
		else
		{
			if (newstrLevel != "engineer")
			{
				gSystemMgr.ShowMsg(IDS_STRING_SUPERADMIN);
				return;
			}
				gSystemMgr.m_sysDatabase.updateUserData(userupdate.c_str(), keyupdate.c_str(), levelupdate.c_str(), selectedUser.id);

		}

	}
	UpdateList();
}

void CSystemUserManagementDlg::OnBnClickedButtonUserMgmtDelete()
{
	if (m_bHitItem)
	{
		m_bHitItem = false;
		string strlevel = selectedUser.level;
		if (strlevel == "engineer")
		{
			gSystemMgr.ShowMsg(IDS_STRING_SUPERADMIN);
			return;
		}
		if (!gSystemMgr.ShowMsg(IDS_STRING_UERINFO_DELETEORNOT, MESSAGE_CHOOSE))
			return;
		gSystemMgr.m_MsgResult = false;
		QUERY_USER_RESULT m_QueryResult;
		m_listUserMgmtList.SetRedraw(FALSE);
		for (int i = m_listUserMgmtList.GetItemCount() - 1; i >= 0; i--)
		{
			if (m_listUserMgmtList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				m_listUserMgmtList.DeleteItem(i);
				m_QueryResult = gSystemMgr.m_sysDatabase.GetResult(i);
				break;
			}
		}
		gSystemMgr.m_sysDatabase.DeleteRow(m_QueryResult.id);
		UpdateList();
	}
}

void CSystemUserManagementDlg::OnNMClickListUserMgmtList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	if (-1 != pNMListView->iItem)
	{
		CString    m_strUserName, m_strPassWord;
		m_strUserName = m_listUserMgmtList.GetItemText(pNMListView->iItem, 1);
		string selectuser = localUtils::utils::Unicode2Utf8(m_strUserName.GetString());
		const char* username= selectuser.c_str();
		selectedUser = gSystemMgr.m_sysDatabase.GetPassWord(username);
		m_strPassWord = selectedUser.key;
		m_strUserLevel = selectedUser.level;
		m_editUser.SetWindowText(m_strUserName);
		m_editPassword.SetWindowText(m_strPassWord);
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_USER_MGMT_LEVEL);
		if (m_strUserLevel == "engineer")
		{
			pCombo->SetCurSel(0);
		}
		else if (m_strUserLevel == "operator")
		{
			pCombo->SetCurSel(1);
		}
		else
		{
			pCombo->SetCurSel(2);
		}
		m_bHitItem = true;
	}
	else
	{
		m_editUser.SetWindowText(_T(""));
		m_editPassword.SetWindowText(_T(""));
		m_bHitItem = false;
	}
	*pResult = 0;
}


