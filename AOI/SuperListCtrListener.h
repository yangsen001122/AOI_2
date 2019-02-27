#pragma once
#include "stdafx.h"
class ICListCtrlExListener
{
public:
	virtual ~ICListCtrlExListener() {}
	virtual void OnLButtonDown(int nItem, int nSubItem) = 0;      //鼠标移动时对应的当前鼠标在图像矩阵中的位置
	virtual void OnEndInplace(int nItem, int nSubItem, CString strVal, int nCtrlType) = 0;
};
