#pragma once
#include "afxwin.h"
class CEditEx :public CEdit
{
	DECLARE_DYNAMIC(CEditEx)
public:
	CEditEx();
	~CEditEx();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

