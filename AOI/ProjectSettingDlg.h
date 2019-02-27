#pragma once

#include "LabelCtrl.h"
#include "afxcmn.h"
#include <tuple>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "WindowbackgroundBrushHelper.h"
#include "afxwin.h"

using namespace std;

#import "AOIAlgo.dll" no_namespace

class CProjectSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProjectSettingDlg)

public:
	CProjectSettingDlg(CWnd* pParent = NULL);   
	virtual ~CProjectSettingDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROJECT_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	void ResetUiWidget();
	void SetButtonAccessibility(int nComboIndex);
private:
	CComPtr<IAOIAlgo>           m_ptrAlgoObj;
	CBitmap                      m_bgImg; // background image
	WindowBackgroundBrushHelper  m_bgBrushHelper;

	std::vector<std::tuple<long, wstring,wstring,wstring>> m_vFuncRepository;
	bool  m_bOkStatus;
	CFont m_txtFont;
	CFont m_smallFont;
	BOOL  m_bTurnOnLight;

	// UI design
	int m_nGapH;
	int m_nGapV;
	int m_nBtnSz;
	int m_nHeight;
	int m_nListWidth;

	CComboBox  m_comboProject;
	CRect      m_rcAlgoList;
	CLabelCtrl m_btnAddPattern; // right arrow
	CLabelCtrl m_btnDeletePattern;
	CLabelCtrl m_btnAddAlgo;
	CLabelCtrl m_btnDeleteAlgo;
	CLabelCtrl m_btnSave;

	CListCtrl  m_listPgPattern;
	COLORREF   m_oldColor;
	std::map<int, PG_IMAGE>* m_ptrPgImageArray;
	int        m_nCrtPatternIndex;
	int        m_nCrtAlgoIndex_R; // Right algorithm list table;
	int        m_nCrtAlgoIndex_L; // Left algorithm list table;

	CTreeCtrl  m_treePattern;
	HTREEITEM  m_crtSelectedItem;
	std::unordered_map<HTREEITEM, FRAME_PATTERN> m_dictTreeItem_Pattern;
	CListCtrl  m_listAlgoRepository;
	PROJECT    m_crtProject;
	CStatic    m_stcGroup;
	CRect      m_rcGroup;
	CRect      m_rcList;
private:
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1);
	void CreateUiWidget();
	void ListAllProjects();
	void ReplaceCameraName();
	void DisplayCurrentProject();
	void CreateAllListControls();
	void CreateParamWidget();
	void FillUpPatternRepository();
	void FillUpAlgoRepository();
	void DisplayMessage(CString sMsg, bool bOK = true);
	int  GetChildNodeCount(const HTREEITEM& hTreeItem);
	void ShowWidget(int nStrID,BOOL bShow);
	void ShowDeviceActionWidget(bool bShow);
	void ShowSnapActionWidget(bool bShow);
	bool GetRootItem(const HTREEITEM& nodeItem, HTREEITEM& retRootItem);
	bool IsRootItem(const HTREEITEM& treeItem);
	SNAP_ACTION* GetSnapActionReference(const HTREEITEM& treeItem);
	void DeleteSnapAction(const HTREEITEM& treeItem);
	void DeleteBaslerCamera(const HTREEITEM& treeItem);
	void DeleteAlgorithm(const HTREEITEM& treeItem);
	int  GetItemIndexAmongSibling(const HTREEITEM& treeItem);
	int  GetAlgoItemIndex(const HTREEITEM& treeItem);
	void ReadDeviceActionParameter(bool& bLightOn);
	void RestoreDeviceActionParameter(const HTREEITEM& treeItem);
	void ReadSnapActionParameter(const HTREEITEM& treeItem);
	void RestoreSnapActionParameter(const HTREEITEM& treeItem);
	bool GenerateProjectCommonParameters(PROJECT&  crtProject);
	bool GenerateProjectPatternParameters(PROJECT&  crtProject);
	void VerifyFramePattern(FRAME_PATTERN& pattern);
	void UpdateStationConfig();
	void ReorderCameraMap(std::unordered_map<wstring, std::vector<SNAP_ACTION>> dictCameraNameIn, std::vector<std::tuple<wstring, std::vector<SNAP_ACTION>>>& vCameraNameOut);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonProjectCreate();
	afx_msg void OnBnClickedButtonProjectModify();
	afx_msg void OnBnClickedButtonProjectCopy();
	afx_msg void OnBnClickedButtonProjectDelete();

	afx_msg void OnStnClickedAddPattern();
	afx_msg void OnStnClickedDeletePattern();
	afx_msg void OnStnClickedAddAlgo();
	afx_msg void OnStnClickedDeleteAlgo();
	afx_msg void OnStnClickedSave();
	afx_msg void OnNMClickListPgPattern(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListAlgoRepository(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListPgPattern(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnNMRClickTreeProjectPattern(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddCamera(UINT ID);
	afx_msg void OnAddCamera1();
	afx_msg void OnAddCamera2();
	afx_msg void OnAddCamera3();
	afx_msg void OnAddCamera4();
	afx_msg void OnAddSnap();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboProjectName();
};
