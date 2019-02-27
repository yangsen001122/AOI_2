#ifndef _H_IMGSHOWWNDLISTER_H
#define _H_IMGSHOWWNDLISTER_H
#include "opencv/cv.h"
#include "TypeDef.h"
class ImageShowWndListener
{
	public:
		virtual ~ImageShowWndListener(){}
		virtual void OnMouseMove(cv::Point2f pt) = 0;//鼠标移动时，对应的当前鼠标在图像矩阵中的位置
		virtual void OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo) = 0;//选择框大小发生改变,用于获得ROI信息
};
#endif 

