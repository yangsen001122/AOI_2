#pragma once
#include <afxwin.h>
#include <map>
#include <algorithm>
#include "Resource.h"

using namespace std;

enum eIMAGE_TYPE
{
	IMG_INVALID = -1,
	IMG_PNG_LOG_ON = 0,
	IMG_PNG_LOG_OUT,
	IMG_PNG_SYS_CONFIG,
	IMG_PNG_SYS_CONFIG_DENY,
	IMG_PNG_UAC,
	IMG_PNG_UAC_DENY,
	IMG_PNG_HELP,
	IMG_PNG_LANGUAGE,
	IMG_PNG_LANGUAGE_DENY,
	IMG_PNG_STOP,
	IMG_PNG_RESET,
	IMG_PNG_RUN,
	IMG_PNG_ERROR,
	IMG_PNG_SHUTDOWN,
	IMG_PNG_SHUTDOWN_DENY,
	IMG_PNG_CLOSE,
	IMG_PNG_CAMERA_SETTING,
	IMG_PNG_CAMERA_SETTING_SELECTED,
	IMG_PNG_LIGHT_SETTING,
	IMG_PNG_LIGHT_SETTING_SELECTED,
	IMG_PNG_PG_SETTING,
	IMG_PNG_PG_SETTING_SELECTED,
	IMG_PNG_PROJECT_SETTING,
	IMG_PNG_PROJECT_SETTING_SELECTED,
	IMG_PNG_SMALL_LOGO,
	IMG_PNG_SAVE,
	IMG_PNG_FIRST,
	IMG_PNG_FIRST_DENY,
	IMG_PNG_PREVIOUS,
	IMG_PNG_PREVIOUS_DENY,
	IMG_PNG_NEXT,
	IMG_PNG_NEXT_DENY,
	IMG_PNG_LAST,
	IMG_PNG_LAST_DENY,
	IMG_PNG_FOLDER,
	IMG_PNG_FOLDER_DISABLED,
	IMG_PNG_LOCK,
	IMG_PNG_UNLOCK,
	IMG_PNG_UNLOCK_DENY,
	IMG_PNG_CAMERA_ADD,
	IMG_PNG_DELETE,
	IMG_PNG_CAMERA_FIND_AGAIN,
	IMG_PNG_MOTION_CARD,
	IMG_PNG_ALGORITHM,
	IMG_PNG_ADD_LEFT,
	IMG_PNG_ADD_RIGHT,
	IMG_PNG_SAVE_SINGLEPIC,
	IMG_PNG_SAVE_SINGLEPIC_DENY,
	IMG_PNG_SAVE_MULTIPLEPIC,
	IMG_PNG_SAVE_MULTIPLEPIC_DENY,
	IMG_PNG_ONE_CIRCLE,
	IMG_PNG_ONE_CIRCLE_DENY,
	IMG_PNG_MULTIPLE_PROCESS,
	IMG_PNG_MULTIPLE_PROCESS_DENY,
	IMG_PNG_AUTO,
	IMG_PNG_SEMIAUTO,
	IMG_PNG_USERLEVEL,
	IMG_PNG_WARNING,
	IMG_PNG_ENQUIRE,
	IMG_PNG_LOG_VIEWER,
	IMG_PNG_LINE,
	IMG_PNG_CIRCLE,
	IMG_PNG_ELLAPSE_ROTAYE,
	IMG_PNG_RECTANGLE,
	IMG_PNG_RECTANGLE_ROTAYE,
	IMG_PNG_POLYGON,
	IMG_PNG_CLIP,
	IMG_PNG_CLEAR,
	IMG_PNG_GRAB_SCREEN
};

struct ui_decorator
{
public:
	ui_decorator()
	{
		HMODULE module = GetModuleHandle(0); 
		TCHAR pFileName[MAX_PATH]; 
		GetModuleFileName(module, pFileName, MAX_PATH); 
 
		CString csFullPath(pFileName); 
		int nPos = csFullPath.ReverseFind( _T('\\') ); 
		if(nPos <=0)
			return;

		CString path = csFullPath.Left(nPos); 
		m_imgs.clear();

		HRESULT hr = m_imgs[IMG_PNG_LOG_ON].Load(path + _T("\\ui_pictures\\log_on.png"));
		hr = m_imgs[IMG_PNG_LOG_OUT].Load(path + _T("\\ui_pictures\\log_out.png"));
		hr = m_imgs[IMG_PNG_SYS_CONFIG].Load(path + _T("\\ui_pictures\\system_setting.png"));
		hr = m_imgs[IMG_PNG_SYS_CONFIG_DENY].Load(path + _T("\\ui_pictures\\system_setting_deny.png"));
		hr = m_imgs[IMG_PNG_UAC].Load(path + _T("\\ui_pictures\\user_management.png"));
		hr = m_imgs[IMG_PNG_UAC_DENY].Load(path + _T("\\ui_pictures\\user_management_deny.png"));
		hr = m_imgs[IMG_PNG_HELP].Load(path + _T("\\ui_pictures\\help.png"));
		hr = m_imgs[IMG_PNG_LANGUAGE].Load(path + _T("\\ui_pictures\\language.png"));
		hr = m_imgs[IMG_PNG_LANGUAGE_DENY].Load(path + _T("\\ui_pictures\\language_deny.png"));
		hr = m_imgs[IMG_PNG_STOP].Load(path + _T("\\ui_pictures\\stop.png"));
		hr = m_imgs[IMG_PNG_RESET].Load(path + _T("\\ui_pictures\\reset.png"));
		hr = m_imgs[IMG_PNG_RUN].Load(path + _T("\\ui_pictures\\run.png"));
		hr = m_imgs[IMG_PNG_ERROR].Load(path + _T("\\ui_pictures\\error.png"));
		hr = m_imgs[IMG_PNG_SHUTDOWN].Load(path + _T("\\ui_pictures\\shutdown.png"));
		hr = m_imgs[IMG_PNG_SHUTDOWN_DENY].Load(path + _T("\\ui_pictures\\shutdown_deny.png"));

		hr = m_imgs[IMG_PNG_CLOSE].Load(path + _T("\\ui_pictures\\close.png"));
		hr = m_imgs[IMG_PNG_CAMERA_SETTING].Load(path + _T("\\ui_pictures\\camera_setting.png"));
		hr = m_imgs[IMG_PNG_CAMERA_SETTING_SELECTED].Load(path + _T("\\ui_pictures\\camera_setting_selected.png"));
		hr = m_imgs[IMG_PNG_LIGHT_SETTING].Load(path + _T("\\ui_pictures\\light_setting.png"));
		hr = m_imgs[IMG_PNG_LIGHT_SETTING_SELECTED].Load(path + _T("\\ui_pictures\\light_setting_selected.png"));
		hr = m_imgs[IMG_PNG_PG_SETTING].Load(path + _T("\\ui_pictures\\pg_setting.png"));
		hr = m_imgs[IMG_PNG_PG_SETTING_SELECTED].Load(path + _T("\\ui_pictures\\pg_setting_selected.png"));
		hr = m_imgs[IMG_PNG_PROJECT_SETTING].Load(path + _T("\\ui_pictures\\pattern_setting.png"));
		hr = m_imgs[IMG_PNG_PROJECT_SETTING_SELECTED].Load(path + _T("\\ui_pictures\\pattern_setting_selected.png"));
		hr = m_imgs[IMG_PNG_SMALL_LOGO].Load(path + _T("\\ui_pictures\\lead_logo.png"));
		hr = m_imgs[IMG_PNG_SAVE].Load(path + _T("\\ui_pictures\\save.png"));
		hr = m_imgs[IMG_PNG_SAVE_SINGLEPIC].Load(path + _T("\\ui_pictures\\savesingle.png"));
		hr = m_imgs[IMG_PNG_SAVE_SINGLEPIC_DENY].Load(path + _T("\\ui_pictures\\savesingle_deny.png"));
		hr = m_imgs[IMG_PNG_SAVE_MULTIPLEPIC].Load(path + _T("\\ui_pictures\\savemultiple.png"));
		hr = m_imgs[IMG_PNG_SAVE_MULTIPLEPIC_DENY].Load(path + _T("\\ui_pictures\\savemultiple_deny.png"));
		hr = m_imgs[IMG_PNG_ONE_CIRCLE].Load(path + _T("\\ui_pictures\\onecircle.png"));
		hr = m_imgs[IMG_PNG_MULTIPLE_PROCESS].Load(path + _T("\\ui_pictures\\processes.png"));
		hr = m_imgs[IMG_PNG_MULTIPLE_PROCESS_DENY].Load(path + _T("\\ui_pictures\\processes_deny.png"));
		hr = m_imgs[IMG_PNG_ONE_CIRCLE_DENY].Load(path + _T("\\ui_pictures\\onecircle_deny.png"));
		hr = m_imgs[IMG_PNG_AUTO].Load(path + _T("\\ui_pictures\\auto.png"));
		hr = m_imgs[IMG_PNG_SEMIAUTO].Load(path + _T("\\ui_pictures\\semiautomatic.png"));
		hr = m_imgs[IMG_PNG_FIRST].Load(path + _T("\\ui_pictures\\first.png"));
		hr = m_imgs[IMG_PNG_FIRST_DENY].Load(path + _T("\\ui_pictures\\first_deny.png"));
		hr = m_imgs[IMG_PNG_PREVIOUS].Load(path + _T("\\ui_pictures\\previous.png"));
		hr = m_imgs[IMG_PNG_PREVIOUS_DENY].Load(path + _T("\\ui_pictures\\previous_deny.png"));
		hr = m_imgs[IMG_PNG_NEXT].Load(path + _T("\\ui_pictures\\next.png"));
		hr = m_imgs[IMG_PNG_NEXT_DENY].Load(path + _T("\\ui_pictures\\next_deny.png"));
		hr = m_imgs[IMG_PNG_LAST].Load(path + _T("\\ui_pictures\\last.png"));
		hr = m_imgs[IMG_PNG_LAST_DENY].Load(path + _T("\\ui_pictures\\last_deny.png"));
		hr = m_imgs[IMG_PNG_FOLDER].Load(path + _T("\\ui_pictures\\folder.png"));
		hr = m_imgs[IMG_PNG_FOLDER_DISABLED].Load(path + _T("\\ui_pictures\\folder_disabled.png"));
		hr = m_imgs[IMG_PNG_LOCK].Load(path + _T("\\ui_pictures\\lock.png"));
		hr = m_imgs[IMG_PNG_UNLOCK].Load(path + _T("\\ui_pictures\\unlock.png"));
		hr = m_imgs[IMG_PNG_UNLOCK_DENY].Load(path + _T("\\ui_pictures\\unlock_deny.png"));
		hr = m_imgs[IMG_PNG_CAMERA_ADD].Load(path + _T("\\ui_pictures\\camera_add.png"));
		hr = m_imgs[IMG_PNG_DELETE].Load(path + _T("\\ui_pictures\\delete.png"));
		hr = m_imgs[IMG_PNG_CAMERA_FIND_AGAIN].Load(path + _T("\\ui_pictures\\camera_find_again.png"));
		hr = m_imgs[IMG_PNG_MOTION_CARD].Load(path + _T("\\ui_pictures\\motion_card.png"));
		hr = m_imgs[IMG_PNG_ALGORITHM].Load(path + _T("\\ui_pictures\\algorithm.png"));
		hr = m_imgs[IMG_PNG_ADD_LEFT].Load(path + _T("\\ui_pictures\\add_left.png"));
		hr = m_imgs[IMG_PNG_ADD_RIGHT].Load(path + _T("\\ui_pictures\\add_right.png"));
		hr = m_imgs[IMG_PNG_USERLEVEL].Load(path + _T("\\ui_pictures\\3peoples.png"));
		hr = m_imgs[IMG_PNG_WARNING].Load(path + _T("\\ui_pictures\\warning.png"));
		hr = m_imgs[IMG_PNG_ENQUIRE].Load(path + _T("\\ui_pictures\\enquire.png"));
		hr = m_imgs[IMG_PNG_LOG_VIEWER].Load(path + _T("\\ui_pictures\\log_viewer.png"));
		hr = m_imgs[IMG_PNG_LINE].Load(path + _T("\\ui_pictures\\line.png"));
		hr = m_imgs[IMG_PNG_CIRCLE].Load(path + _T("\\ui_pictures\\circle.png"));
		hr = m_imgs[IMG_PNG_ELLAPSE_ROTAYE].Load(path + _T("\\ui_pictures\\rotate_circle.png"));
		hr = m_imgs[IMG_PNG_RECTANGLE].Load(path + _T("\\ui_pictures\\rectangle.png"));
		hr = m_imgs[IMG_PNG_RECTANGLE_ROTAYE].Load(path + _T("\\ui_pictures\\rotate_rectangle.png"));
		hr = m_imgs[IMG_PNG_POLYGON].Load(path + _T("\\ui_pictures\\polygon.png"));
		hr = m_imgs[IMG_PNG_CLIP].Load(path + _T("\\ui_pictures\\clip.png"));
		hr = m_imgs[IMG_PNG_CLEAR].Load(path + _T("\\ui_pictures\\clear.png"));
		hr = m_imgs[IMG_PNG_GRAB_SCREEN].Load(path + _T("\\ui_pictures\\grab_screen.png"));
		HandleImageAlphaChannel();
	}
	~ui_decorator()
	{
		std::for_each(m_imgs.begin(),m_imgs.end(),[&](std::pair<eIMAGE_TYPE, ATL::CImage> pair)
		{
			pair.second.Destroy();
		});
		m_imgs.clear();
	}

	std::map<eIMAGE_TYPE, ATL::CImage> m_imgs;

private:
	void HandleImageAlphaChannel()
	{
		for(auto it = m_imgs.begin(); it != m_imgs.end(); ++it)
		{
			ATL::CImage& image = it->second;
			if(image.GetBPP() == 32)
			{
				for(int i = 0; i < image.GetWidth(); i++)
				{
					for(int j = 0; j < image.GetHeight(); j++)
					{
						byte* pByte = (byte*)image.GetPixelAddress(i, j);
						pByte[0] = pByte[0] * pByte[3]/255;
						pByte[1] = pByte[1] * pByte[3]/255;
						pByte[2] = pByte[2] * pByte[3]/255;
					}
				}
			}
		}
	}
	ui_decorator(const ui_decorator&) = delete;
	ui_decorator& operator=(const ui_decorator&) = delete;
};