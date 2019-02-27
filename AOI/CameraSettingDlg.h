#pragma once
#include <unordered_map>
#include "LabelCtrl.h"
#include "TypeDef.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "WindowbackgroundBrushHelper.h"

using namespace std;


class CCameraBase;
class CCameraSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraSettingDlg)

public:
	CCameraSettingDlg(CWnd* pParent = NULL);
	virtual ~CCameraSettingDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAMERA_SETTING };
#endif


protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
private:
    std::map<wstring, CAMERA_SETTING> m_dictCameraSetting;
    CAMERA_SETTING  m_crtCameraSetting;
	CBitmap m_bgImg; // background image
	WindowBackgroundBrushHelper m_bgBrushHelper;
	bool      m_bOkStatus;
	CFont     m_txtFont;
	CFont     m_smallFont;
    CButton   m_btnLoad;
    BOOL      m_bMainCamera;
    CListCtrl m_listCameraList;
private:
	void InitializeCtrl(int nCtrlID, CRect& rcPos, int nStrID = -1, bool bSmallFont = false);
	void CreateUiWidget();
    void DisplayCameraData(CString sCameraName);
    void ResetUiWidget();
    void EnableUiWidget(int nCtrlID, bool bEnable);
    bool GetPendingSetting(CAMERA_SETTING& retCs);
	void DisplayPromptMessage(CString sMsg, bool bOk);
	void FillCameraList(); 	
    void DisplayOperationResult(bool bSuccess);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); 
	afx_msg void OnPaint();                         
    afx_msg void OnBnClickedButtonSetConfigPath();
    afx_msg void OnBnClickedButtonAddCamera();
    afx_msg void OnBnClickedButtonUpdateCamera();
    afx_msg void OnBnClickedButtonDeleteCamera();
    afx_msg void OnNMClickListCameraName(NMHDR *pNMHDR, LRESULT *pResult);
    
};
