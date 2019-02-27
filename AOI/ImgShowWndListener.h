#ifndef _H_IMGSHOWWNDLISTER_H
#define _H_IMGSHOWWNDLISTER_H
#include "opencv/cv.h"
#include "TypeDef.h"
class ImageShowWndListener
{
	public:
		virtual ~ImageShowWndListener(){}
		virtual void OnMouseMove(cv::Point2f pt) = 0;//����ƶ�ʱ����Ӧ�ĵ�ǰ�����ͼ������е�λ��
		virtual void OnSelectROIChange(eROI_TYPE eType, IMG_ROI_INFO imgROIInfo) = 0;//ѡ����С�����ı�,���ڻ��ROI��Ϣ
};
#endif 

