#pragma once
#include "stdafx.h"
class ICListCtrlExListener
{
public:
	virtual ~ICListCtrlExListener() {}
	virtual void OnLButtonDown(int nItem, int nSubItem) = 0;      //����ƶ�ʱ��Ӧ�ĵ�ǰ�����ͼ������е�λ��
	virtual void OnEndInplace(int nItem, int nSubItem, CString strVal, int nCtrlType) = 0;
};
