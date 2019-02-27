#pragma once

#include "LabelCtrl.h" 
#include "CameraSettingDlg.h"
#include "ProjectSettingDlg.h"
#include "ConfigFileCpyDlg.h"

const int SYS_DLG_TIMER_ID = 543210;

class CSystemConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemConfigDlg)

public:
	CSystemConfigDlg(CWnd* pParent = NULL);   
	virtual ~CSystemConfigDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SYS_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CLabelCtrl m_btnClose;
	CLabelCtrl m_btnCamera;
	CRect      m_rcCameraBtn;

	CLabelCtrl m_btnLight;
	CRect      m_rcLightBtn;

	CLabelCtrl m_btnProject;
	CRect      m_rcProjectBtn;

	CLabelCtrl m_btnMotion;
	CRect      m_rcMotionBtn;

	CLabelCtrl m_btnConfigFile;
	CRect      m_rcConfigFile;

	CRect      m_rcCurrentBtn;
	CLabelCtrl m_labelLogo;

	CFont      m_smallFont;
	CFont      m_txtFont;
	int        m_nBtnSize;
	CPen       m_penFrame;
	CPen       m_penTitle;

	CRect              m_rcChild;
	SETTING_TYPE       m_crtSettingType;
	CCameraSettingDlg  m_cameraSettingDlg;
	CProjectSettingDlg m_projectSettingDlg;
	ConfigFileCpyDlg   m_configFileCpyDlg;
private:
	void CreateTabButton(CRect& rcClient);
	void CreateLogo(CRect& rcClient);
	void CreateCloseButton(CRect& rcClient);
	void SetDlgVisibility(SETTING_TYPE oldType,SETTING_TYPE newType);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedClose();
	afx_msg void OnStnClickedCamera();
	afx_msg void OnStnClickedLight();
	afx_msg void OnStnClickedPgDevice();
	afx_msg void OnStnClickedProject();
	afx_msg void OnStnClickedMotion();
	afx_msg void OnPaint();
	afx_msg void OnStnClickedConfigFileSet();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
