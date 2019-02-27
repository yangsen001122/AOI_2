#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include<io.h>
#include <set>
#include <vector>
#include <iostream>
#include <iomanip>
#include <tchar.h>
#include <atlcomcli.h>
#include <atlconv.h>
#include <atltypes.h>
#include "opencv.hpp"
#include "utils.h"
#include <boost\lexical_cast.hpp>


using namespace std;
using namespace cv;
using namespace boost;


#define WM_UPDATE_NG_IMG_WND		    WM_USER + 1
#define WM_RECEIVE_SPC_DATA			    WM_USER + 2
#define MSG_MODEL_IMG_READY			    WM_USER + 3
#define WM_SEND_STATE_TO_MAIN_UI        WM_USER + 4
#define WM_SEND_LOG_MSG_TO_MAIN_UI      WM_USER + 5
#define WM_UPDATE_STATISTICS            WM_USER + 6
#define WM_COMM_RXCHAR_DETECTED         WM_USER + 8
#define WM_UPDATE_DISP_DEBUG            WM_USER + 9
#define WM_SEND_MSG_LABELCTRL_RBTNDOWN  WM_USER + 10
#define WM_ONE_MORE_IMAGE               WM_USER + 11
#define	WM_UPDATE_FILIMAPPING			WM_USER + 12
#define WM_UPDATE_DISP_MAIN             WM_USER + 13
#define WM_UPDATE_RESULT_INFO           WM_USER + 14
#define WM_UPDATE_STATION_CONFIG        WM_USER + 15

#define INITIAL_STATE			101       //代表等待初始化过程中
#define READY_STATE				102       //代表准备好了，home过程中
#define ALARM_STATE				103       //报警过程中
#define STOP_STATE				104       //停止状态
#define RUN_STATE				105       //正常运行状态
#define MANUAL_STATE			106       //手动模式
#define WARNING_STATE			107       //警告模式
#define SUSPEND_STATE           108       //暂停状态
#define WRONG_RESULT            110       //测试结果失败
#define OTP1                    111        //屏幕序号
#define OTP2                    112
#define WAIT1                   113
#define WAIT2                   114
#define GRAB1                   115
#define GRAB2                   116
#define DEMURA1                 117
#define DEMURA2                 118
#define SCREEN_FAIL             119
#define SCREEN_OK               120
#define MESSAGE_CONFIRM         121
#define MESSAGE_CHOOSE          122

#define INDEPENDENT_GRAB         123

#define LOG_LEVEL_INFO          0         //Info级别
#define LOG_LEVEL_WARN          1         //Warn级别
#define LOG_LEVEL_ERROR         2         //Error级别

#define PGSCREEN_NO_ELECTRIC_CURRENT             0         //PG上的屏幕不存在电流
#define PGSCREEN_EXIST_ELECTRIC_CURRENT          1         //PG上的屏幕存在电流
#define PGSCREEN_READ_ELECTRIC_CURRENT_ERROR     2         //PG上的屏幕电流读取错误

#define BIT_RANGE_16   65535
#define BIT_RANGE_8    255
#define ROI_MINWIDTH   20
#define ROI_MINHEIGHT  20
#define DBL_MAX 1.7976931348623158e+308
const long long MAX_INT64_VALUE = 9223372036854775807;

const int GIGE = 0;
const int CAMERA_LINK = 1;

// UI layout
const long BK_IMG_WIDTH = 1920;
const long BK_IMG_HEIGHT = 1080;
const long BK_IMG_TITLE_HEIGHT = 96;
const long BK_IMG_LOGO_BGN = 50;
const long BK_IMG_LOGO_END = 245;
const long BTN_SIZE = 64;

// System Configuration UI layout
const long SYS_CONFIG_UI_WIDTH = 1280;
const long SYS_CONFIG_UI_HEIGHT = 800;

// System Login UI layout
const long SYS_LOGIN_UI_WIDTH = 768;
const long SYS_LOGIN_UI_HEIGHT = 432;

//System Message UI layout
const long SYS_MESSAGE_UI_WIDTH = 512;
const long SYS_MESSAGE_UI_HEIGHT = 320;

// System UAC UI layout
const long SYS_USER_MANAGERMENT_UI_WIDTH = 960;
const long SYS_USER_MANAGERMENT_UI_HEIGHT = 600;

//System Query UI layout
const long SYS_QUERY_UI_WIDTH = 1480;
const long SYS_QUERY_UI_HEIGHT = 700;
//todo:
const int  IO_ON = 1;
const int  IO_OFF = 0;
const int  OLED_READY = 1;
const int  SHUTDOWN_FLAG = 0;

const int STATUS_OK        = 0;
const int STATUS_WARNING   = 1;
const int STATUS_ERROR     = 2;
const int RED_PROGRESS_BAR = -100;
const int RESTART_DEMURA_PROGRESS_BAR = 80;

const int BTN_CLOSE = 20000;
const int BTN_CAMERA = 20001;
const int BTN_LIGHT = 20002;
const int BTN_PG_DEVICE = 20003;
const int BTN_PROJECT = 20004;
const int BTN_OPEN_FOLDER = 20005;
const int BTN_FIRST = 20006;
const int BTN_PREVIOUS = 20007;
const int BTN_NEXT = 20008;
const int BTN_LAST = 20009;
const int BTN_DISP_MODE = 20010;
const int BTN_LOCK = 20011;
const int BTN_ALGORITHM = 20012;
const int BTN_MOTION_CARD = 20013;
const int BTN_FOLDER = 20014;
const int BTN_ADD_RIGHT = 20015;
const int BTN_DELETE_PATTERN = 20016;
const int BTN_ADD_LEFT = 20017;
const int BTN_DELETE_ALGO = 20018;
const int BTN_SAVE = 20019;
const int BTN_ADD_USERINFO = 20020;
const int BTN_DELETE_USERINFO = 20021;
const int BTN_UPDATE_USERINFO = 20022;
const int BTN_CONFIG_FILE_SET = 20023;
const int BTN_LINE = 20024;
const int BTN_RECTANGLE = 20025;
const int BTN_RECT_ROTATE = 20026;
const int BTN_CIRCLE = 20027;
const int BTN_ELLIPSE = 20028;
const int BTN_POLYGON = 20029;
const int BTN_CLIP = 20030;
const int BTN_SAVE_BMP = 20031;
const int BTN_SNAP_SHOT= 20032;
const int BTN_DELETE_ALL = 20033;

const int TIMER_ID_DEBUG_SCREEN = 30000;
const int IDC_EDIT_PARAMETER = 70028;
const int IDC_STATIC1_PARAMETER_CAMERA = 70030;
const int IDC_EDIT1_PARAMETER_CAMERA = 70031;
const int IDC_STATIC2_PARAMETER_CAMERA = 70032;
const int IDC_EDIT2_PARAMETER_CAMERA = 70033;
const int IDC_EDIT3_PARAMETER_CAMERA = 70034;
const int IDC_EDIT_PARAM_BALANCER_CAMERA = 70035;
const int IDC_EDIT_PARAM_BALANCEG_CAMERA = 70036;
const int IDC_EDIT_PARAM_BALANCEB_CAMERA = 70037;

const int IDC_STATIC1_PARAMETER_LIGHT = 70050;
const int IDC_EDIT1_PARAMETER_LIGHT = 70051;
const int IDC_STATIC2_PARAMETER_LIGHT = 70053;
const int IDC_STATIC1_PARAMETER_MOTION = 70070;
const int IDC_EDIT1_PARAMETER_MOTION = 70071;
const int IDC_STATIC2_PARAMETER_MOTION = 70072;
const int IDC_EDIT2_PARAMETER_MOTION = 70073;
const int IDC_STATIC3_PARAMETER_MOTION = 70074;
const int IDC_EDIT3_PARAMETER_MOTION = 70075;
const int IDC_STATIC4_PARAMETER_MOTION = 70076;
const int IDC_EDIT4_PARAMETER_MOTION = 70077;
const int IDC_STATIC5_PARAMETER_MOTION = 70078;
const int TIMER_ID_FOR_EDIT_CTRL = 90002;
const int TIMER_ID_FOR_INCREASE_BUTTON = 90003;
const int TIMER_ID_FOR_DECREASE_BUTTON = 90004;
const int TIMER_ID_FOR_PROJECT_UI = 90006;
const int TIMER_ID_FOR_DISPLAYA_AXIS_POS = 90007;
const int TIMER_ID_FOR_PROGRESS_BAR = 90008;
const int TIMER_ID_ONE_CIRCLE = 90009;
const int TIMER_ID_CLOSE_DEBUGDLG = 90010;
const int TIMER_ID_FOR_PROJCET_CHANGE_PROCESS_BAR = 90011;
const int TIMER_ID_DEBUG_GRAB_CONTINUOUS = 90012;

const int IDC_STATIC_SAVE_SINGLE_PIC = 70100;
const int IDC_STATIC_SAVE_MULTIPLE_PIC = 70101;
const int IDS_STATIC_ONE_CIRCLE = 70102;
const int IDS_STATIC_AUTO_AEMIAUTO = 70103;
const int IDS_STATIC_MULTIPLE_PROCESS = 70104;



enum eCHANNEL_STATE
{
	CH_ON = 0,
	CH_DEBUG,
	CH_OFF,
	CH_ERROR
};

enum eEVENT_LEVEL
{
	LV_INVALID = -1,
	LV_EVENT = 0,
	LV_WARNING,
	LV_ERROR,
	LV_OK
};

enum eEVENT_SOURCE
{
	SRC_INVALID = -1,
	LOG_IN = 0,
	LOG_OUT,
	CAMERA,
	PG,
	SERVO,
	SYSTEAM,
	DETECT_RESULT
};

enum ePG_PICTURE_ID
{
	//PIC_ZERO,
	PIC_WHITE,
	PIC_REG_ERROR,
	PIC_ERASE_FAILED,
	PIC_CANNOT_FIND_FLASH,
	PIC_REG_NG,
	PIC_FLASH_NG,
	PIC_OTP_OK,
	PIC_OK
};
enum eCAMERAPARAM_TYPE
{
	PARA_EXPOSURE=0,
	PARA_GAIN,
	PARA_DELAY,
	PARA_WHITE_BALANCE_R,
	PARA_WHITE_BALANCE_G,
	PARA_WHITE_BALANCE_B
};
enum eCAMERA_IMAGE_TYPE
{
	IMAGE_INVALID=0,
	IMAGE_GRAY8,
	IMAGE_RGB8,
	IMAGE_GRAY16
};
enum eWINDOW_ID
{
	WINDOW_INVALID = -1,
	WINDOW_MAIN,
    WINDOW_SUB1,
	WINDOW_SUB2,
	WINDOW_SUB3,
	WINDOW_SUB4,
	WINDOW_MAIN_RESULT,
	WINDOW_DEBUG_RESULT,
	WINDOW_NDEBUG_RESULT,
	WINDOW_MAIN_GRAY_VALUE,
	WINDOW_MAIN_CLEAR,
	WINDOW_MAIN_FINAL_DISP,
	WINDOW_MAIN_SELECTED_ONE_DEFECT
};

enum eROI_TYPE
{
	ROI_FULL=-1,
	ROI_LINE=0,
	ROI_RECTANGLE,
	ROI_RECT_ROTATE,
	ROI_CIRCLE,
	ROI_ELLIPSE,
	ROI_POLYGON,
};
enum eROI_RECT_DRAG_TYPE
{
	ROI_RECT_DRAG_NULL=-1,
	ROI_RECT_DRAG_LEFT = 0,
	ROI_RECT_DRAG_RIGHT,
	ROI_RECT_DRAG_TOP,
	ROI_RECT_DRAG_BOTTOM,
	ROI_RECT_DRAG_INNER
};

enum eSUPER_LIST_CTRL_TYPE
{
	SUPER_LIST_CTRL_VALID=-1,
	SUPER_LIST_CTRL_EDIT=0,			
	SUPER_LIST_CTRL_LIST_BOX,
	SUPER_LIST_CTRL_BUTTON
};
struct EVENT_PACKAGE
{
	int           m_nStationIndex; // the station which fires this event.
	eEVENT_LEVEL  m_level;
	eEVENT_SOURCE m_source;
	wstring       m_wstrCameraName;   // if event source is camera, its name
	int           m_nAxisIndex;       // if event source is servo, which axis
	wstring       m_wstrTimestamp;    //
	wstring       m_wstrDescription;  // Event description

	EVENT_PACKAGE()
		:m_nStationIndex(-1),
		m_level(LV_INVALID),
		m_source(SRC_INVALID),
		m_wstrCameraName(L""),
		m_nAxisIndex(-1),
		m_wstrTimestamp(L""),
		m_wstrDescription(L"")
	{
		
	}
	EVENT_PACKAGE(const EVENT_PACKAGE& other)
	{
		m_nStationIndex  = other.m_nStationIndex;
		m_level          = other.m_level;
		m_source         = other.m_source;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nAxisIndex     = other.m_nAxisIndex;
		m_wstrTimestamp  = other.m_wstrTimestamp;
		m_wstrDescription = other.m_wstrDescription;
	}
	EVENT_PACKAGE(EVENT_PACKAGE&& other)
	{
		m_nStationIndex = other.m_nStationIndex;
		m_level = other.m_level;
		m_source = other.m_source;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_wstrTimestamp = other.m_wstrTimestamp;
		m_wstrDescription = other.m_wstrDescription;
	}
	EVENT_PACKAGE& operator=(const EVENT_PACKAGE& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nStationIndex = other.m_nStationIndex;
		m_level = other.m_level;
		m_source = other.m_source;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_wstrTimestamp = other.m_wstrTimestamp;
		m_wstrDescription = other.m_wstrDescription;
		
		return *this;
	}

	EVENT_PACKAGE& operator=(EVENT_PACKAGE&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nStationIndex = other.m_nStationIndex;
		m_level = other.m_level;
		m_source = other.m_source;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_wstrTimestamp = other.m_wstrTimestamp;
		m_wstrDescription = other.m_wstrDescription;

		return *this;
	}
};

enum eUAC_LEVEL
{
	OPERATOR = 0,
	ADMIN,
	SADMIN
};

enum eWARNING_LEVEL
{
	WL_INVALID = -1,
	WL_WARNING = 0,
	WL_ERROR = 1
};
struct LOGVIEWER
{
	LOGVIEWER() :m_logLevel(LV_EVENT), m_logSource(SYSTEAM)
	{

	}
	LOGVIEWER(eEVENT_LEVEL nLevel, eEVENT_SOURCE nType)
		:m_logLevel(nLevel),
		m_logSource(nType)
	{

	}
	LOGVIEWER(const LOGVIEWER& other)
	{
		m_logLevel = other.m_logLevel;
		m_logSource = other.m_logSource;
	}
	LOGVIEWER(LOGVIEWER&& other)
	{
		m_logLevel = other.m_logLevel;
		m_logSource = other.m_logSource;
	}
	LOGVIEWER& operator=(const LOGVIEWER& other)
	{
		if (&other == this)
			return *this;
		m_logLevel = other.m_logLevel;
		m_logSource = other.m_logSource;
		return *this;
	}
	LOGVIEWER& operator=(LOGVIEWER&& other)
	{
		if (&other == this)
			return *this;
		m_logLevel = other.m_logLevel;
		m_logSource = other.m_logSource;
		return *this;
	}

	eEVENT_LEVEL m_logLevel;
	eEVENT_SOURCE m_logSource;
};

enum INFOTYPE
{
	CAPTURERESULT,
	PROCESSRESULT,
};


struct  FILEMAPPING_COMM_INFO
{
	// Motion->Vision
	int  m_nTotalCapNums;					// 拍照总数
	int  m_nCurCapIndex;					// 当前拍照索引
	boost::array<char, 100> m_Codestr;		// 二维码信息
	boost::array<char, 100> m_ChScreenID;	// 点灯颜色信息

											// Vision->Motion
	INFOTYPE m_Type;						// 信息类型
	BOOL  m_bCapResult;						// 拍照结果
	BOOL  m_bDetectResult;					// 检测结果

	FILEMAPPING_COMM_INFO()
	{
		m_nTotalCapNums = 0;
		m_nCurCapIndex = 0;
		memset(&m_Codestr, 0, sizeof(m_Codestr));
		memset(&m_ChScreenID, 0, sizeof(m_ChScreenID));

		m_Type = CAPTURERESULT;
		m_bCapResult = false;
		m_bDetectResult = false;
	}

	FILEMAPPING_COMM_INFO(INFOTYPE type, bool capResult, bool detResult)
		:m_Type(type),
		m_bCapResult(capResult),
		m_bDetectResult(detResult)
	{
		memset(&m_Codestr, 0, sizeof(m_Codestr));
		memset(&m_ChScreenID, 0, sizeof(m_ChScreenID));
	}
	FILEMAPPING_COMM_INFO(const FILEMAPPING_COMM_INFO& other)
	{
		m_nTotalCapNums = other.m_nTotalCapNums;
		m_nCurCapIndex = other.m_nCurCapIndex;
		m_Type = other.m_Type;
		m_bCapResult = other.m_bCapResult;
		m_bDetectResult = other.m_bDetectResult;
		m_Codestr = other.m_Codestr;
		m_ChScreenID = other.m_ChScreenID;
	}
	FILEMAPPING_COMM_INFO(FILEMAPPING_COMM_INFO&& other)
	{
		m_nTotalCapNums = other.m_nTotalCapNums;
		m_nCurCapIndex = other.m_nCurCapIndex;
		m_Type = other.m_Type;
		m_bCapResult = other.m_bCapResult;
		m_bDetectResult = other.m_bDetectResult;
		m_Codestr = other.m_Codestr;
		m_ChScreenID = other.m_ChScreenID;
	}
	FILEMAPPING_COMM_INFO& operator=(const FILEMAPPING_COMM_INFO& other)
	{
		if (&other == this)
			return *this;
		m_nTotalCapNums = other.m_nTotalCapNums;
		m_nCurCapIndex = other.m_nCurCapIndex;
		m_Type = other.m_Type;
		m_bCapResult = other.m_bCapResult;
		m_bDetectResult = other.m_bDetectResult;
		m_Codestr = other.m_Codestr;
		m_ChScreenID = other.m_ChScreenID;
		return *this;
	}
	FILEMAPPING_COMM_INFO& operator=(FILEMAPPING_COMM_INFO&& other)
	{
		if (&other == this)
			return *this;
		m_nTotalCapNums = other.m_nTotalCapNums;
		m_nCurCapIndex = other.m_nCurCapIndex;
		m_Type = other.m_Type;
		m_bCapResult = other.m_bCapResult;
		m_bDetectResult = other.m_bDetectResult;
		m_Codestr = other.m_Codestr;
		m_ChScreenID = other.m_ChScreenID;
		return *this;
	}
};

struct STATIONINFO
{
	string m_strCode;
	HANDLE m_hProcessFinish;
	STATIONINFO()
	{
		m_strCode = "";
		m_hProcessFinish = NULL;
	}

	STATIONINFO(const STATIONINFO& other)
	{
		m_strCode = other.m_strCode;
		m_hProcessFinish = other.m_hProcessFinish;
	}
	STATIONINFO(STATIONINFO&& other)
	{
		m_strCode = other.m_strCode;
		m_hProcessFinish = other.m_hProcessFinish;
	}
	STATIONINFO& operator=(const STATIONINFO& other)
	{
		if (&other == this)
			return *this;
		m_strCode = other.m_strCode;
		m_hProcessFinish = other.m_hProcessFinish;
		return *this;
	}
	STATIONINFO& operator=(STATIONINFO&& other)
	{
		if (&other == this)
			return *this;
		m_strCode = other.m_strCode;
		m_hProcessFinish = other.m_hProcessFinish;
		return *this;
	}
};

struct WAITINFO
{
	string m_strCode;
	string m_strTask;

	WAITINFO()
	{
		m_strCode = "";
		m_strTask = "";
	}

	WAITINFO(string strCode,string strTask)
	{
		m_strCode = strCode;
		m_strTask = strTask;
	}

	WAITINFO(const WAITINFO& other)
	{
		m_strCode = other.m_strCode;
		m_strTask = other.m_strTask;
	}
	WAITINFO(WAITINFO&& other)
	{
		m_strCode = other.m_strCode;
		m_strTask = other.m_strTask;
	}
	WAITINFO& operator=(const WAITINFO& other)
	{
		if (&other == this)
			return *this;
		m_strCode = other.m_strCode;
		m_strTask = other.m_strTask;
		return *this;
	}
	WAITINFO& operator=(WAITINFO&& other)
	{
		if (&other == this)
			return *this;
		m_strCode = other.m_strCode;
		m_strTask = other.m_strTask;
		return *this;
	}
};

enum URLPAGENAME
{
	PAGE_D01,
	PAGE_D02,
	PAGE_D03,
	PAGE_D04,
	PAGE_D05,
	PAGE_D06,
};

enum HTTPINFOTYPE
{
	/*
	specifiedInfo = {"result":[{"ProdID":"产品ID","ProdName":"产品名称","ProdMemo","产品说明"},{}, ...]}
	*/
	AOI_CLIENT_REC_D01,
	/*
	commInfo ={"ProdID":"产品类别ID"}
	*/
	AOI_CLIENT_SEND_D02,
	/*
	specifiedInfo = {"result":[{"RuleID":"规则ID","RuleName":"规则名称","RuleMemo":"规则说明","Frames":["画面编号1","画面编号2",……]},{}, ...]}
	*/
	AOI_CLIENT_REC_D02,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号"}
	*/
	AOI_CLIENT_SEND_D03,
	/*
	commInfo ={"Status":"OK/NG"}
	defectsInfo = {"Defects":[{"DefectID":"缺陷代码","Confidence":"置信度","Camera":"相机编号","Frame":"画面编号","Photo":"照片编号","PicPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"},"ScrPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"}},{},...]}
	*/
	AOI_CLIENT_REC_D03,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号","Status":"OK/NG"}
	defectsInfo = {"Defects":[{"DefectID":"缺陷代码","Confidence":"置信度","Camera":"相机编号","Frame":"画面编号","Photo":"照片编号","PicPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"},"ScrPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"}},{},...]}
	*/
	TEST_CLIENT_SEND_D04,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号","Status":"OK/NG"}
	defectsInfo = {"Defects":[{"DefectID":"缺陷代码","Confidence":"置信度","Camera":"相机编号","Frame":"画面编号","Photo":"照片编号","PicPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"},"ScrPos":{"X1":"左上X","Y1":"左上Y1","Width":"宽","Height":"高"}},{},...]}
	*/
	AOI_SERVE_REC_D04,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号","ProdID":"产品类别ID","RuleID":"质检规则ID","Frame":"画面编号","TranType":"normal/finish/again","Pics":[{"Camera":"相机编号","Photo":"照片编号","Filename":"照片文件名"},{},...]}
	*/
	AOI_CLIENT_SEND_D05,
	AOI_CLIENT_REC_D05,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号","Frame":"画面编号"}
	*/
	TEST_CLIENT_SEND_D06,
	/*
	commInfo ={"AOIID":"AOI编号","TaskID":"任务编号","MaterielID":"物料编号","Frame":"画面编号"}
	*/
	AOI_SERVE_REC_D06,
};

struct DEFECTINFO
{
	map<string, string> defCommInfo;
	map<string, map<string, string>> defSpeInfo;

	DEFECTINFO()
	{
		defCommInfo.clear();
		defSpeInfo.clear();
	}

	DEFECTINFO(const DEFECTINFO& other)
	{
		defCommInfo = other.defCommInfo;
		defSpeInfo = other.defSpeInfo;
	}
	DEFECTINFO(DEFECTINFO&& other)
	{
		defCommInfo = other.defCommInfo;
		defSpeInfo = other.defSpeInfo;
	}
	DEFECTINFO& operator=(const DEFECTINFO& other)
	{
		if (&other == this)
			return *this;
		defCommInfo = other.defCommInfo;
		defSpeInfo = other.defSpeInfo;
		return *this;
	}
	DEFECTINFO& operator=(DEFECTINFO&& other)
	{
		if (&other == this)
			return *this;
		defCommInfo = other.defCommInfo;
		defSpeInfo = other.defSpeInfo;
		return *this;
	}
};

struct BASEINFO
{
	HTTPINFOTYPE m_type;
	map<string, string> m_commInfo;
	map<string, vector<map<string, string>>> m_specifiedInfo;
	map<string, vector<DEFECTINFO>> m_defectsInfo;

	BASEINFO()
	{
		m_type = AOI_CLIENT_SEND_D02;
		m_commInfo.clear();
		m_specifiedInfo.clear();
		m_defectsInfo.clear();
	}

	BASEINFO(const BASEINFO& other)
	{
		m_type = other.m_type;
		m_commInfo = other.m_commInfo;
		m_specifiedInfo = other.m_specifiedInfo;
		m_defectsInfo = other.m_defectsInfo;
	}
	BASEINFO(BASEINFO&& other)
	{
		m_type = other.m_type;
		m_commInfo = other.m_commInfo;
		m_specifiedInfo = other.m_specifiedInfo;
		m_defectsInfo = other.m_defectsInfo;
	}
	BASEINFO& operator=(const BASEINFO& other)
	{
		if (&other == this)
			return *this;
		m_type = other.m_type;
		m_commInfo = other.m_commInfo;
		m_specifiedInfo = other.m_specifiedInfo;
		m_defectsInfo = other.m_defectsInfo;
		return *this;
	}
	BASEINFO& operator=(BASEINFO&& other)
	{
		if (&other == this)
			return *this;
		m_type = other.m_type;
		m_commInfo = other.m_commInfo;
		m_specifiedInfo = other.m_specifiedInfo;
		m_defectsInfo = other.m_defectsInfo;
		return *this;
	}

	void clear()
	{
		m_commInfo.clear();
		m_specifiedInfo.clear();
		m_defectsInfo.clear();
	}
};

struct UAC
{
	UAC() :m_strUsername(L""), m_uacLevel(OPERATOR)
	{
	}
	UAC(wstring strUsername, eUAC_LEVEL nLevel)
		:m_strUsername(strUsername),
		m_uacLevel(nLevel)
	{
	}
	UAC(const UAC& other)
	{
		m_strUsername = other.m_strUsername;
		m_strPassword = other.m_strPassword;
		m_uacLevel = other.m_uacLevel;
	}
	UAC(UAC&& other)
	{
		m_strUsername = other.m_strUsername;
		m_strPassword = other.m_strPassword;
		m_uacLevel = other.m_uacLevel;
	}
	UAC& operator=(const UAC& other)
	{
		if (&other == this)
			return *this;
		m_strUsername = other.m_strUsername;
		m_strPassword = other.m_strPassword;
		m_uacLevel = other.m_uacLevel;

		return *this;
	}
	UAC& operator=(UAC&& other)
	{
		if (&other == this)
			return *this;
		m_strUsername = other.m_strUsername;
		m_strPassword = other.m_strPassword;
		m_uacLevel = other.m_uacLevel;

		return *this;
	}
	wstring    m_strUsername;
	wstring    m_strPassword;
	eUAC_LEVEL m_uacLevel;
};

enum SETTING_TYPE
{
	T_INVALID = -1,
	T_ALGO,
	T_CAMERA,
	T_MOTION_CARD,
	T_LIGHT,
	T_PG,
	T_PROJECT,
	T_CONFIGFILE
};

struct IMG_PACKAGE // Image Package
{
	Mat       m_img;
	wstring   m_strCameraName;
	bool      m_bMainCamera;
	long long m_nIndex;
	int       m_nPatternIndex;
	bool      m_bDebugFlag;
	bool      m_bExitFlag;
	
	IMG_PACKAGE() :m_nIndex(-1), m_nPatternIndex(-1), m_bDebugFlag(false), m_bExitFlag(false)
	{

	}
	IMG_PACKAGE(Mat img, wstring strCameraName, bool bMainCamera,long long nIndex = -1, int nPatternIndex = -1, bool bDebugFlag = false, bool bExitFlag = false) :
		m_nIndex(nIndex), m_nPatternIndex(nPatternIndex), m_bDebugFlag(bDebugFlag), m_bExitFlag(bExitFlag)
	{
		m_img = img.clone();
		m_strCameraName = strCameraName;
		m_bMainCamera = bMainCamera;
	}
	IMG_PACKAGE(const IMG_PACKAGE& other)
	{
		m_img = other.m_img.clone();
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_nIndex = other.m_nIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		m_bDebugFlag = other.m_bDebugFlag;
		m_bExitFlag = other.m_bExitFlag;
	}

	IMG_PACKAGE(IMG_PACKAGE&& other)
	{
		m_img = other.m_img.clone();
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_nIndex = other.m_nIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		m_bDebugFlag = other.m_bDebugFlag;
		m_bExitFlag = other.m_bExitFlag;
	}

	IMG_PACKAGE& operator=(const IMG_PACKAGE& other)
	{
		if (&other == this)
			return *this;
		m_img = other.m_img.clone();
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_nIndex = other.m_nIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		m_bDebugFlag = other.m_bDebugFlag;
		m_bExitFlag = other.m_bExitFlag;
		return *this;
	}

	IMG_PACKAGE& operator=(IMG_PACKAGE&& other)
	{
		if (&other == this)
			return *this;
		m_img = other.m_img.clone();
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_nIndex = other.m_nIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		m_bDebugFlag = other.m_bDebugFlag;
		m_bExitFlag = other.m_bExitFlag;
		return *this;
	}
};
struct CAMERA_IMG_SOURCE
{
	wstring   m_strCameraName;
	bool      m_bMainCamera;
	Mat       m_image;
	eCAMERA_IMAGE_TYPE  m_eType;
	CAMERA_IMG_SOURCE() :m_strCameraName(L""), m_eType(IMAGE_RGB8)
	{

	}
	CAMERA_IMG_SOURCE(wstring strCameraName, bool bMainCamera, Mat image, eCAMERA_IMAGE_TYPE  eType = IMAGE_RGB8) :
		m_eType(eType)
	{
		m_strCameraName = strCameraName;
		m_bMainCamera = bMainCamera;
		m_image = image.clone();
	}
	CAMERA_IMG_SOURCE(const CAMERA_IMG_SOURCE& other)
	{
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_image = other.m_image.clone();
		m_eType = other.m_eType;
	}

	CAMERA_IMG_SOURCE(CAMERA_IMG_SOURCE&& other)
	{
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_image = other.m_image.clone();
		m_eType = other.m_eType;
	}

	CAMERA_IMG_SOURCE& operator=(const CAMERA_IMG_SOURCE& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_image = other.m_image.clone();
		m_eType = other.m_eType;
		return *this;
	}

	CAMERA_IMG_SOURCE& operator=(CAMERA_IMG_SOURCE&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strCameraName = other.m_strCameraName;
		m_bMainCamera = other.m_bMainCamera;
		m_image = other.m_image.clone();
		m_eType = other.m_eType;
		return *this;
	}
};
// parameters of the OLED screen.
struct SCREEN_PARAM
{
	long m_nDimensionWidth;
	long m_nDimensionHeight;
	long m_nResolutionWidth_S;
	long m_nResolutionHeight_S;
	wstring m_strStationName;
	wstring	m_strProductName;

	SCREEN_PARAM()
	:m_nDimensionWidth(-1),
	 m_nDimensionHeight(-1),
	 m_nResolutionWidth_S(1920),
	 m_nResolutionHeight_S(1080),
	 m_strStationName(L""),
	 m_strProductName(L"")
	{

	}

	SCREEN_PARAM(const SCREEN_PARAM& other)
	{
		m_nDimensionWidth   = other.m_nDimensionWidth;
		m_nDimensionHeight  = other.m_nDimensionHeight;
		m_nResolutionWidth_S  = other.m_nResolutionWidth_S;
		m_nResolutionHeight_S = other.m_nResolutionHeight_S;
		m_strStationName = other.m_strStationName;
		m_strProductName = other.m_strProductName;
	}
	SCREEN_PARAM(SCREEN_PARAM&& other)
	{
		m_nDimensionWidth = other.m_nDimensionWidth;
		m_nDimensionHeight = other.m_nDimensionHeight;
		m_nResolutionWidth_S = other.m_nResolutionWidth_S;
		m_nResolutionHeight_S = other.m_nResolutionHeight_S;
		m_strStationName = other.m_strStationName;
		m_strProductName = other.m_strProductName;
	}
	SCREEN_PARAM& operator=(const SCREEN_PARAM& other)
	{
		if (&other == this)
			return *this;
		m_nDimensionWidth = other.m_nDimensionWidth;
		m_nDimensionHeight = other.m_nDimensionHeight;
		m_nResolutionWidth_S = other.m_nResolutionWidth_S;
		m_nResolutionHeight_S = other.m_nResolutionHeight_S;
		m_strStationName = other.m_strStationName;
		m_strProductName = other.m_strProductName;
		return *this;
	}
	SCREEN_PARAM& operator=(SCREEN_PARAM&& other)
	{
		if (&other == this)
			return *this;
		m_nDimensionWidth = other.m_nDimensionWidth;
		m_nDimensionHeight = other.m_nDimensionHeight;
		m_nResolutionWidth_S = other.m_nResolutionWidth_S;
		m_nResolutionHeight_S = other.m_nResolutionHeight_S;
		m_strStationName = other.m_strStationName;
		m_strProductName = other.m_strProductName;
		return *this;
	}
};

// Parameters which need to be sent to PG before switching project
// These parameters are related to OLED screen.
struct PG_IMAGE
{
	int      m_nIndex;
	int		 m_seqIndex;
	wstring  m_wstrName;
	COLORREF m_color;
	wstring  m_wstrType;
	//TODO:
	PG_IMAGE() :m_nIndex(-1),m_wstrName(L""), m_wstrType(L"")
	{
		m_color = RGB(255, 255, 255);
	}
	PG_IMAGE(const PG_IMAGE& other)
	{
		m_nIndex   = other.m_nIndex;
		m_wstrName = other.m_wstrName;
		m_color    = other.m_color;
		m_seqIndex = other.m_seqIndex;
		m_wstrType = other.m_wstrType;
	}
	PG_IMAGE(PG_IMAGE&& other)
	{
		m_nIndex   = other.m_nIndex;
		m_wstrName = other.m_wstrName;
		m_color    = other.m_color;
	    m_seqIndex = other.m_seqIndex;
		m_wstrType = other.m_wstrType;
	}
	PG_IMAGE& operator=(const PG_IMAGE& other)
	{
		if (&other == this)
			return *this;
		m_nIndex   = other.m_nIndex;
		m_wstrName = other.m_wstrName;
		m_color    = other.m_color;
		m_seqIndex = other.m_seqIndex;
		m_wstrType = other.m_wstrType;
		return *this;
	}
	PG_IMAGE& operator=(PG_IMAGE&& other)
	{
		if (&other == this)
			return *this;
		m_nIndex = other.m_nIndex;
		m_wstrName = other.m_wstrName;
		m_color = other.m_color;
		m_seqIndex = other.m_seqIndex;
		m_wstrType = other.m_wstrType;
		return *this;
	}
};

//Parameters which need to be sent to PG before pattern switching 
struct PG_RUNTIME_PARAM
{
	PG_RUNTIME_PARAM()
	{
		//TODO:
	}
	PG_RUNTIME_PARAM(const PG_RUNTIME_PARAM& other)
	{
		//TODO
	}
	PG_RUNTIME_PARAM(PG_RUNTIME_PARAM&& other)
	{
		//TODO
	}
	PG_RUNTIME_PARAM& operator=(const PG_RUNTIME_PARAM& other)
	{
		if (&other == this)
			return *this;

		//TODO

		return *this;
	}
	PG_RUNTIME_PARAM& operator=(PG_RUNTIME_PARAM&& other)
	{
		if (&other == this)
			return *this;

		//TODO

		return *this;
	}
};

struct THRESHOLD
{
	THRESHOLD()
	{
		m_helpId = -1;
		m_lcl = -99999.0f;
		m_ucl = 99999.0f;
		m_min = 0.0f;
		m_max = 99999.0f;
		m_stepLen = 1.0f;
		m_specV = 0.0f;
		m_type = VT_EMPTY;
		m_compType = COMP_INVALID;
	}
	THRESHOLD(const THRESHOLD& other)
	{
		m_helpId = other.m_helpId;
		m_lcl = other.m_lcl;
		m_specV = other.m_specV;
		m_ucl = other.m_ucl;
		m_min = other.m_min;
		m_max = other.m_max;
		m_stepLen = other.m_stepLen;
		m_type = other.m_type;
		m_compType = other.m_compType;
	}
	THRESHOLD(THRESHOLD&& other)
	{
		m_helpId = other.m_helpId;
		m_lcl = other.m_lcl;
		m_specV = other.m_specV;
		m_ucl = other.m_ucl;
		m_min = other.m_min;
		m_max = other.m_max;
		m_stepLen = other.m_stepLen;
		m_type = other.m_type;
		m_compType = other.m_compType;
	}

	THRESHOLD& operator=(const THRESHOLD& other)
	{
		if (&other == this)
			return *this;
		m_helpId = other.m_helpId;
		m_lcl = other.m_lcl;
		m_specV = other.m_specV;
		m_ucl = other.m_ucl;
		m_min = other.m_min;
		m_max = other.m_max;
		m_stepLen = other.m_stepLen;
		m_type = other.m_type;
		m_compType = other.m_compType;
		return *this;
	}

	THRESHOLD& operator=(THRESHOLD&& other)
	{
		if (&other == this)
			return *this;
		m_helpId = other.m_helpId;
		m_lcl = other.m_lcl;
		m_specV = other.m_specV;
		m_ucl = other.m_ucl;
		m_min = other.m_min;
		m_max = other.m_max;
		m_stepLen = other.m_stepLen;
		m_type = other.m_type;
		m_compType = other.m_compType;
		return *this;
	}
	bool IsEqual(const THRESHOLD& other)
	{
		return (m_helpId == other.m_helpId &&
			m_lcl == other.m_lcl &&
			m_ucl == other.m_ucl &&
			m_min == other.m_min &&
			m_max == other.m_max &&
			m_stepLen == other.m_stepLen &&
			m_specV == other.m_specV &&
			m_type == other.m_type);
	}
	long          m_helpId;
	float		  m_lcl;
	float		  m_ucl;
	float         m_min;
	float         m_max;
	float         m_stepLen;
	float         m_specV;
	VARENUM		  m_type;
	COMPARE_TYPE  m_compType;
};

struct FUNC_PARAM // These information will be displayed on screen.
{
	VARENUM m_type;
	long    m_descID;
	wstring m_wstrName;
	wstring m_wstrDescription;
	float   m_value;
	float   m_min;
	float   m_max;
	float   m_step;

	FUNC_PARAM()
	{
		m_type = VT_EMPTY;
		m_wstrName = L"";
		m_wstrDescription = L"";
		m_descID = 0;
		m_value = 0.0f;
		m_min = 0.0f;
		m_max = 0.0f;
		m_step = 0.0f;
	}
	FUNC_PARAM(const FUNC_PARAM& other)
	{
		m_type = other.m_type;
		m_descID = other.m_descID;
		m_wstrName = other.m_wstrName;
		m_wstrDescription = other.m_wstrDescription;
		m_value = other.m_value;
		m_min = other.m_min;
		m_max = other.m_max;
		m_step = other.m_step;
	}
	FUNC_PARAM(FUNC_PARAM&& other)
	{
		m_type = other.m_type;
		m_descID = other.m_descID;
		m_wstrName = other.m_wstrName;
		m_wstrDescription = other.m_wstrDescription;
		m_value = other.m_value;
		m_min = other.m_min;
		m_max = other.m_max;
		m_step = other.m_step;
	}
	FUNC_PARAM& operator=(const FUNC_PARAM& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_type = other.m_type;
		m_descID = other.m_descID;
		m_wstrName = other.m_wstrName;
		m_wstrDescription = other.m_wstrDescription;
		m_value = other.m_value;
		m_min = other.m_min;
		m_max = other.m_max;
		m_step = other.m_step;
		return *this;
	}
	FUNC_PARAM& operator=(FUNC_PARAM&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_type = other.m_type;
		m_descID = other.m_descID;
		m_wstrName = other.m_wstrName;
		m_wstrDescription = other.m_wstrDescription;
		m_value = other.m_value;
		m_min = other.m_min;
		m_max = other.m_max;
		m_step = other.m_step;
		return *this;
	}
	bool IsEqual(const FUNC_PARAM& other)
	{
		return (m_type == other.m_type) && \
			(m_descID == other.m_descID) && \
			(m_wstrName == other.m_wstrName) && \
			(m_wstrDescription == other.m_wstrDescription) && \
			(m_value == other.m_value) && \
			(m_min == other.m_min) && \
			(m_max == other.m_max) && \
			(m_step == other.m_step);
	}
};

struct FUNC_INFO
{
	DISPID                        m_dispId;
	int                           m_funcIndex;
	int                           m_nRgnId;
	double						  m_dProcessTime;       
	std::wstring                  m_wstrFuncName;
	std::wstring                  m_wstrHelp;           // Help string of this function;
	long                          m_nToolDescId;         //Function description sting resource ID.
	std::wstring                  m_wstrToolName;       //Hard-coded name from the help string, CANNOT change it
	std::wstring                  m_wstrToolDesc;       //Hard-coded help information from the help string.
	bool                          m_bTestFunc;          // Y: Normal Function with a return value; N: Location Function without return value;
	bool                          m_bPass;              // Current Testing result;
	THRESHOLD                     m_threshold;
	FUNC_INFO*                    m_ptrRoiFunc;         // If this function is a testing function, m_ptrRoiFunc is refer to its ROI function.
	std::vector<wstring>          m_vParamStr;          // all input parametetrs for an interface method;
	std::vector<FUNC_PARAM>       m_vParam;
	std::vector<CComVariant>      m_vComParam;          // used to call COM functions

	FUNC_INFO()
	{
		m_dispId = -1;
		m_funcIndex = -1;
		m_nRgnId = -1;
		m_dProcessTime = 0;
		m_wstrFuncName = L"";
		m_wstrHelp = L"";
		m_nToolDescId = -1;
		m_wstrToolName = L"";
		m_wstrToolDesc = L"";
		m_bTestFunc = true;
		m_bPass = true;
		m_ptrRoiFunc = nullptr;
		m_vParamStr.clear();
		m_vParam.clear();
		m_vComParam.clear();
	}

	FUNC_INFO(const FUNC_INFO& other)
	{
		m_dispId = other.m_dispId;
		m_funcIndex = other.m_funcIndex;
		m_nRgnId = other.m_nRgnId;
		m_dProcessTime = other.m_dProcessTime;
		m_wstrFuncName = other.m_wstrFuncName;
		m_wstrHelp = other.m_wstrHelp;
		m_nToolDescId = other.m_nToolDescId;
		m_wstrToolName = other.m_wstrToolName;
		m_wstrToolDesc = other.m_wstrToolDesc;
		m_bTestFunc = other.m_bTestFunc;
		m_bPass = other.m_bPass;
		m_ptrRoiFunc = other.m_ptrRoiFunc;
		m_vParamStr.clear();
		m_vParamStr = other.m_vParamStr;
		m_vParam.clear();
		m_vParam = other.m_vParam;
		CopyComParamArray(other.m_vComParam);
	}

	FUNC_INFO(FUNC_INFO&& other)
	{
		m_dispId = other.m_dispId;
		m_funcIndex = other.m_funcIndex;
		m_nRgnId = other.m_nRgnId;
		m_dProcessTime = other.m_dProcessTime;
		m_wstrFuncName = other.m_wstrFuncName;
		m_wstrHelp = other.m_wstrHelp;
		m_nToolDescId = other.m_nToolDescId;
		m_wstrToolName = other.m_wstrToolName;
		m_wstrToolDesc = other.m_wstrToolDesc;
		m_bTestFunc = other.m_bTestFunc;
		m_bPass = other.m_bPass;
		m_ptrRoiFunc = other.m_ptrRoiFunc;
		m_vParamStr.clear();
		m_vParamStr = other.m_vParamStr;
		m_vParam.clear();
		m_vParam = other.m_vParam;
		CopyComParamArray(other.m_vComParam);
	}

	FUNC_INFO& operator=(const FUNC_INFO& other)
	{
		if (&other == this)
			return *this;
		m_dispId = other.m_dispId;
		m_funcIndex = other.m_funcIndex;
		m_nRgnId = other.m_nRgnId;
		m_dProcessTime = other.m_dProcessTime;
		m_wstrFuncName = other.m_wstrFuncName;
		m_wstrHelp = other.m_wstrHelp;
		m_nToolDescId = other.m_nToolDescId;
		m_wstrToolName = other.m_wstrToolName;
		m_wstrToolDesc = other.m_wstrToolDesc;
		m_bTestFunc = other.m_bTestFunc;
		m_bPass = other.m_bPass;
		m_ptrRoiFunc = other.m_ptrRoiFunc;
		m_vParamStr.clear();
		m_vParamStr = other.m_vParamStr;
		m_vParam.clear();
		m_vParam = other.m_vParam;
		CopyComParamArray(other.m_vComParam);

		return *this;
	}
	FUNC_INFO& operator=(FUNC_INFO&& other)
	{
		if (&other == this)
			return *this;
		m_dispId = other.m_dispId;
		m_funcIndex = other.m_funcIndex;
		m_nRgnId = other.m_nRgnId;
		m_dProcessTime = other.m_dProcessTime;
		m_wstrFuncName = other.m_wstrFuncName;
		m_wstrHelp = other.m_wstrHelp;
		m_nToolDescId = other.m_nToolDescId;
		m_wstrToolName = other.m_wstrToolName;
		m_wstrToolDesc = other.m_wstrToolDesc;
		m_bTestFunc = other.m_bTestFunc;
		m_bPass = other.m_bPass;
		m_ptrRoiFunc = other.m_ptrRoiFunc;
		m_vParamStr.clear();
		m_vParamStr = other.m_vParamStr;
		m_vParam.clear();
		m_vParam = other.m_vParam;
		CopyComParamArray(other.m_vComParam);
		return *this;
	}

	bool ParseSelf(IDispatch * pDisp)
	{
		if (pDisp == nullptr)
			return false;
		m_dispId = localUtils::utils::GetInterfaceDispID(pDisp, m_wstrFuncName);
		HRESULT hr = localUtils::utils::GetInterfaceHelpString(pDisp, m_wstrFuncName, m_wstrHelp);
		if (!localUtils::utils::CheckResult(hr))
		{
			//TODO: log this event;
			return false;
		}
		int index = -1;
		CString sHelp;
		std::vector<wstring> vHelpStrItem = localUtils::utils::SplitString(m_wstrHelp, L"$");
		for (auto t = 0; t < vHelpStrItem.size(); t++)
		{
			wstring wstrItem = vHelpStrItem[t];
			std::vector<wstring> vDetailItem = localUtils::utils::SplitString(wstrItem, L";");
			if (vDetailItem.size() == 1 || (vDetailItem.size() == 2 && vDetailItem[1] == L"L")) // tool name and its description
			{
				m_nToolDescId = boost::lexical_cast<long>(vDetailItem[0]);
				CString sResource;
				sResource.LoadStringW(m_nToolDescId);
				wstring wstrRet = sResource.GetString();
				std::vector<wstring> vElement = localUtils::utils::SplitString(wstrRet, L"$");
				if (vElement.size() != 2)
				{
					//TODO:log this event;
					return false;
				}
				m_wstrToolName = vElement[0];
				m_wstrToolDesc = vElement[1];
			}
			if (vDetailItem.size() == 2) // Parameter ---- Region ID
			{
				if (vDetailItem[1] == L"L")
				{
					m_bTestFunc = false;
				}
				else
				{
					index = 1;
					FUNC_PARAM param;
					param.m_descID = localUtils::utils::to_long(vDetailItem[0]);
					sHelp.LoadStringW(param.m_descID);
					wstring wstrHelp = sHelp.GetString();
					std::vector<wstring> vParamItem = localUtils::utils::SplitString(wstrHelp, L"$");
					if (vParamItem.size() == 1)
					{
						param.m_wstrName = vParamItem[0];
					}
					if (vParamItem.size() == 2)
					{
						param.m_wstrName = vParamItem[0];
						param.m_wstrDescription = vParamItem[1];
					}
					param.m_type = localUtils::utils::GetType(vDetailItem[1]);
					param.m_value = localUtils::utils::ConvertStr2Value(m_vParamStr[index]);
					m_nRgnId = (int)param.m_value;
					m_vParam.push_back(param);
					index++;
				}
			}
			if (vDetailItem.size() == 6) // other parameter
			{
				FUNC_PARAM param;
				param.m_descID = boost::lexical_cast<long>(vDetailItem[0]);
				sHelp.LoadStringW(param.m_descID);
				wstring wstrHelp = sHelp.GetString();
				std::vector<wstring> vParamItem = localUtils::utils::SplitString(wstrHelp, L"$");
				if (vParamItem.size() == 1)
				{
					param.m_wstrName = vParamItem[0];
				}
				if (vParamItem.size() == 2)
				{
					param.m_wstrName = vParamItem[0];
					param.m_wstrDescription = vParamItem[1];
				}
				param.m_type = localUtils::utils::GetType(vDetailItem[1]);
				param.m_value = localUtils::utils::ConvertStr2Value(m_vParamStr[index]);
				param.m_min = boost::lexical_cast<float>(vDetailItem[2]);
				param.m_max = boost::lexical_cast<float>(vDetailItem[3]);
				param.m_step = boost::lexical_cast<float>(vDetailItem[4]);
				m_vParam.push_back(param);
				index++;
			}
		}
		if (!CreateComParamArray())
		{
			return false;
		}
		return true;
	}
	bool ParseDefaultParameter(IDispatch * pDisp)
	{
		if (pDisp == nullptr)
			return false;
		CString sHelp;
		std::vector<wstring> vHelpStrItem = localUtils::utils::SplitString(m_wstrHelp, L"$");
		for (auto t = 0; t < vHelpStrItem.size(); t++)
		{
			wstring wstrHelpItem = vHelpStrItem[t];
			std::vector<wstring> vDetailItem = localUtils::utils::SplitString(wstrHelpItem, L";");
			if (vDetailItem.size() == 1 || (vDetailItem.size() == 2 && vDetailItem[1] == L"L")) // tool name and its description
			{
				m_nToolDescId = boost::lexical_cast<long>(vDetailItem[0]);
				CString sResource;
				sResource.LoadStringW(m_nToolDescId);
				wstring wstrRet = sResource.GetString();
				std::vector<wstring> vElement = localUtils::utils::SplitString(wstrRet, L"$");
				if (vElement.size() != 2)
				{
					//TODO:log this event;
					return false;
				}
				m_wstrToolName = vElement[0];
				m_wstrToolDesc = vElement[1];
				m_vParamStr.push_back(L"N/A");
			}
			if (vDetailItem.size() == 2) // Parameter ---- Region ID
			{
				if (vDetailItem[1] == L"L")
				{
					m_bTestFunc = false;
				}
				else
				{
					FUNC_PARAM param;
					param.m_descID = localUtils::utils::to_long(vDetailItem[0]);
					sHelp.LoadStringW(param.m_descID);
					wstring wstrHelp = sHelp.GetString();
					std::vector<wstring> vParamItem = localUtils::utils::SplitString(wstrHelp, L"$");
					if (vParamItem.size() == 1)
					{
						param.m_wstrName = vParamItem[0];
					}
					if (vParamItem.size() == 2)
					{
						param.m_wstrName = vParamItem[0];
						param.m_wstrDescription = vParamItem[1];
					}
					param.m_type = localUtils::utils::GetType(vDetailItem[1]);
					param.m_value = -1;
					m_nRgnId = -1;
					m_vParam.push_back(param);
					m_vParamStr.push_back(L"-1");
				}
			}
			if (vDetailItem.size() == 6) // other parameter
			{
				FUNC_PARAM param;
				param.m_descID = boost::lexical_cast<long>(vDetailItem[0]);
				sHelp.LoadStringW(param.m_descID);
				wstring wstrHelp = sHelp.GetString();
				std::vector<wstring> vParamItem = localUtils::utils::SplitString(wstrHelp, L"$");
				if (vParamItem.size() == 1)
				{
					param.m_wstrName = vParamItem[0];
				}
				if (vParamItem.size() == 2)
				{
					param.m_wstrName = vParamItem[0];
					param.m_wstrDescription = vParamItem[1];
				}
				param.m_type = localUtils::utils::GetType(vDetailItem[1]);
				param.m_value = localUtils::utils::ConvertStr2Value(vDetailItem[5]);
				param.m_min = boost::lexical_cast<float>(vDetailItem[2]);
				param.m_max = boost::lexical_cast<float>(vDetailItem[3]);
				param.m_step = boost::lexical_cast<float>(vDetailItem[4]);
				m_vParam.push_back(param);
				m_vParamStr.push_back(vDetailItem[5]);
			}
		}
		return true;
	}
	bool ParseHelpInfomation(IDispatch * pDisp)
	{
		if (pDisp == nullptr)
			return false;
		m_dispId = localUtils::utils::GetInterfaceDispID(pDisp, m_wstrFuncName);
		HRESULT hr = localUtils::utils::GetInterfaceHelpString(pDisp, m_wstrFuncName, m_wstrHelp);
		if (!localUtils::utils::CheckResult(hr))
		{
			//TODO: log this event;
			return false;
		}
		CString sHelp;
		std::vector<wstring> vHelpStrItem = localUtils::utils::SplitString(m_wstrHelp, L"$");
		for (auto t = 0; t < vHelpStrItem.size(); t++)
		{
			wstring wstrHelpItem = vHelpStrItem[t];
			std::vector<wstring> vDetailItem = localUtils::utils::SplitString(wstrHelpItem, L";");
			if (vDetailItem.size() == 1 || (vDetailItem.size() == 2 && vDetailItem[1] == L"L")) // tool name and its description
			{
				m_nToolDescId = boost::lexical_cast<long>(vDetailItem[0]);
				CString sResource;
				sResource.LoadStringW(m_nToolDescId);
				wstring wstrRet = sResource.GetString();
				std::vector<wstring> vElement = localUtils::utils::SplitString(wstrRet, L"$");
				if (vElement.size() != 2)
				{
					//TODO:log this event;
					return false;
				}
				m_wstrToolName = vElement[0];
				m_wstrToolDesc = vElement[1];
			}
		}
		return true;
	}
	void CopyComParamArray(const std::vector<ATL::CComVariant>& vComParam)
	{
		size_t sz = vComParam.size();
		if (sz <= 0)
			return;
		m_vComParam.clear();
		m_vComParam = std::vector<ATL::CComVariant>(sz);
		for (size_t t = 0; t < sz; t++)
		{
			m_vComParam[t].Copy(&vComParam[t]);
		}
	}
	void GetTestFuncFlag()
	{
		if (m_wstrHelp.find(L"L") != wstring::npos)
		{
			m_bTestFunc = false;
		}
	}
	bool IsLocationFunction()
	{
		if (m_wstrHelp.empty())
		{
			return false;
		}
		if (m_wstrHelp.find(L"L") == wstring::npos)
		{
			return true;
		}
		return false;
	}
	bool CreateComParamArray()
	{
		size_t sz = m_vParam.size();
		if (sz <= 0)
			return false;

		m_vComParam.clear();
		for (size_t t = sz; t > 0; t--)
		{
			ATL::CComVariant var(m_vParam[t - 1].m_value);
			m_vComParam.push_back(var);
		}
		return true;
	}
	void Reset()
	{
		m_dispId = -1;
		m_funcIndex = -1;
		m_wstrFuncName = L"";
		m_wstrToolName = L"";
		m_wstrHelp = L"";
		m_nToolDescId = -1;
		m_bTestFunc = false;
		m_bPass = true;
		m_ptrRoiFunc = nullptr;
		m_vParamStr.clear();
		m_vComParam.clear();
	}
	string PackageDefaultParam()
	{
		string strRet("");
		auto sz = m_vParamStr.size();
		for (size_t t = 0; t < sz; t++)
		{
			string strItem = localUtils::utils::Unicode2Utf8(m_vParamStr[t]);
			strRet.append(strItem);
			if (t != sz - 1)
			{
				strRet.append(",");
			}
		}
		return strRet;
	}
};

struct SNAP_ACTION
{
	int m_nFocalPos;     // Adjust the focal length;
	int m_nExposureTime; // Adjust the exposure time;
	int m_nGain;
	int m_nWhiteBalanceR;
	int m_nWhiteBalanceG;
	int m_nWhiteBalanceB;
	double m_dTotalProcessTime;
	std::vector<FUNC_INFO> m_vAlgoFunc; // Call these algorithm functions one by one

	SNAP_ACTION() :m_nFocalPos(0), m_nExposureTime(100), m_nGain(0), m_nWhiteBalanceR(1000),
		m_nWhiteBalanceG(1000), m_nWhiteBalanceB(1000),m_dTotalProcessTime(0)
	{
		m_vAlgoFunc.clear();
	}
	SNAP_ACTION(const SNAP_ACTION& other)
	{
		m_nFocalPos = other.m_nFocalPos;
		m_nExposureTime = other.m_nExposureTime;
		m_nGain = other.m_nGain;
		m_nWhiteBalanceR = other.m_nWhiteBalanceR;
		m_nWhiteBalanceG = other.m_nWhiteBalanceG;
		m_nWhiteBalanceB = other.m_nWhiteBalanceB;
		m_dTotalProcessTime = other.m_dTotalProcessTime;
		m_vAlgoFunc.clear();
		m_vAlgoFunc = other.m_vAlgoFunc;
	}
	SNAP_ACTION(SNAP_ACTION&& other)
	{
		m_nFocalPos = other.m_nFocalPos;
		m_nExposureTime = other.m_nExposureTime;
		m_nGain = other.m_nGain;
		m_nWhiteBalanceR = other.m_nWhiteBalanceR;
		m_nWhiteBalanceG = other.m_nWhiteBalanceG;
		m_nWhiteBalanceB = other.m_nWhiteBalanceB;
		m_dTotalProcessTime = other.m_dTotalProcessTime;
		m_vAlgoFunc.clear();
		m_vAlgoFunc = other.m_vAlgoFunc;
	}
	SNAP_ACTION& operator=(const SNAP_ACTION& other)
	{
		if (&other == this)
			return *this;
		m_nFocalPos = other.m_nFocalPos;
		m_nExposureTime = other.m_nExposureTime;
		m_nGain = other.m_nGain;
		m_nWhiteBalanceR = other.m_nWhiteBalanceR;
		m_nWhiteBalanceG = other.m_nWhiteBalanceG;
		m_nWhiteBalanceB = other.m_nWhiteBalanceB;
		m_dTotalProcessTime = other.m_dTotalProcessTime;
		m_vAlgoFunc.clear();
		m_vAlgoFunc = other.m_vAlgoFunc;
		return *this;
	}
	SNAP_ACTION& operator=(SNAP_ACTION&& other)
	{
		if (&other == this)
			return *this;
		m_nFocalPos = other.m_nFocalPos;
		m_nExposureTime = other.m_nExposureTime;
		m_nGain = other.m_nGain;
		m_nWhiteBalanceR = other.m_nWhiteBalanceR;
		m_nWhiteBalanceG = other.m_nWhiteBalanceG;
		m_nWhiteBalanceB = other.m_nWhiteBalanceB;
		m_dTotalProcessTime = other.m_dTotalProcessTime;
		m_vAlgoFunc.clear();
		m_vAlgoFunc = other.m_vAlgoFunc;
		return *this;
	}
	bool ParseSelf(IDispatch * pDisp)
	{
		for (auto t = 0; t < m_vAlgoFunc.size(); t++)
		{
			if (!m_vAlgoFunc[t].ParseSelf(pDisp))
				return false;
		}
		return true;
	}
	bool IsFocalExposureGainEqual(const SNAP_ACTION& other)
	{
		return (m_nFocalPos == other.m_nFocalPos) &&
			(m_nExposureTime == other.m_nExposureTime) &&
			(m_nGain == other.m_nGain)&&
			(m_nWhiteBalanceR = other.m_nWhiteBalanceR) &&
			(m_nWhiteBalanceG = other.m_nWhiteBalanceG) &&
			(m_nWhiteBalanceB = other.m_nWhiteBalanceB);
	}
};

struct PENDING_PARAM
{
	int m_nPatternIndex;
	long m_nPatternColor;
	long m_nPatternGrayLevel;
	int m_nCameraWidth_R;
	int m_nCameraHeight_R;
	std::vector<SNAP_ACTION>* m_ptrActionGp;

	PENDING_PARAM() :
		m_nPatternIndex(-1),
		m_nPatternColor(-1),
		m_nPatternGrayLevel(-1),
		m_nCameraWidth_R(0),
		m_nCameraHeight_R(0)
	{
		m_ptrActionGp = nullptr;
	}

	PENDING_PARAM(const PENDING_PARAM& other)
	{
		m_nPatternIndex = other.m_nPatternIndex;
		m_nPatternColor = other.m_nPatternColor;
		m_nPatternGrayLevel = other.m_nPatternGrayLevel;
		m_nCameraWidth_R = other.m_nCameraWidth_R;
		m_nCameraHeight_R = other.m_nCameraHeight_R;
		m_ptrActionGp = other.m_ptrActionGp;
	}

	PENDING_PARAM(PENDING_PARAM&& other)
	{
		m_nPatternIndex = other.m_nPatternIndex;
		m_nPatternColor = other.m_nPatternColor;
		m_nPatternGrayLevel = other.m_nPatternGrayLevel;
		m_nCameraWidth_R = other.m_nCameraWidth_R;
		m_nCameraHeight_R = other.m_nCameraHeight_R;
		m_ptrActionGp = other.m_ptrActionGp;
	}

	PENDING_PARAM& operator=(const PENDING_PARAM& other)
	{
		if (&other == this)
			return *this;

		m_nPatternIndex = other.m_nPatternIndex;
		m_nPatternColor = other.m_nPatternColor;
		m_nPatternGrayLevel = other.m_nPatternGrayLevel;
		m_nCameraWidth_R = other.m_nCameraWidth_R;
		m_nCameraHeight_R = other.m_nCameraHeight_R;
		m_ptrActionGp = other.m_ptrActionGp;

		return *this;
	}

	PENDING_PARAM& operator=(PENDING_PARAM&& other)
	{
		if (&other == this)
			return *this;

		m_nPatternIndex = other.m_nPatternIndex;
		m_nPatternColor = other.m_nPatternColor;
		m_nPatternGrayLevel = other.m_nPatternGrayLevel;
		m_nCameraWidth_R = other.m_nCameraWidth_R;
		m_nCameraHeight_R = other.m_nCameraHeight_R;
		m_ptrActionGp = other.m_ptrActionGp;

		return *this;
	}
};

struct CAMERA_FRAME_PATTERN
{
	int m_nIndex;
	int m_nSeqIndex;
	wstring m_wstrName;
	bool m_bSideLightOn;
	//Real resolution;
	int      m_nWidth_R;
	int      m_nHeight_R;
	std::vector<SNAP_ACTION> m_vActionGroup;

	CAMERA_FRAME_PATTERN() :m_nIndex(-1), m_nSeqIndex(-1), m_wstrName(L""), m_bSideLightOn(false), m_nWidth_R(0), m_nHeight_R(0)
	{
		m_vActionGroup.clear();
	}
	CAMERA_FRAME_PATTERN(const CAMERA_FRAME_PATTERN& other)
	{
		m_nIndex       = other.m_nIndex;
		m_nSeqIndex    = other.m_nSeqIndex;
		m_wstrName     = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_vActionGroup = other.m_vActionGroup;
	}
	CAMERA_FRAME_PATTERN(CAMERA_FRAME_PATTERN&& other)
	{
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_vActionGroup = other.m_vActionGroup;
	}
	CAMERA_FRAME_PATTERN& operator=(const CAMERA_FRAME_PATTERN& other)
	{
		if (&other == this)
			return *this;
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_vActionGroup = other.m_vActionGroup;
		return *this;
	}
	CAMERA_FRAME_PATTERN& operator=(CAMERA_FRAME_PATTERN&& other)
	{
		if (&other == this)
			return *this;
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_vActionGroup = other.m_vActionGroup;
		return *this;
	}
};

struct FRAME_PATTERN
{
	int      m_nIndex;
	int      m_nSeqIndex;
	wstring  m_wstrName; // customized name;
	bool     m_bSideLightOn;
	//Real resolution;
	int      m_nWidth_R;
	int      m_nHeight_R; 
	std::unordered_map<wstring, std::vector<SNAP_ACTION>> m_dictCameraName_SAA; // Camera Name <====> Snap Action Array
	
	FRAME_PATTERN() :m_nIndex(-1), m_nSeqIndex(-1),m_wstrName(L""), m_bSideLightOn(false), m_nWidth_R(0), m_nHeight_R(0)
	{
		m_dictCameraName_SAA.clear();
	}

	FRAME_PATTERN(const FRAME_PATTERN& other)
	{
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_dictCameraName_SAA.clear();
		m_dictCameraName_SAA = other.m_dictCameraName_SAA;
	}

	FRAME_PATTERN(FRAME_PATTERN&& other)
	{
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_dictCameraName_SAA.clear();
		m_dictCameraName_SAA = other.m_dictCameraName_SAA;
	}
	FRAME_PATTERN& operator=(const FRAME_PATTERN& other)
	{
		if (&other == this)
			return *this;
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_dictCameraName_SAA.clear();
		m_dictCameraName_SAA = other.m_dictCameraName_SAA;
		return *this;
	}
	FRAME_PATTERN& operator=(FRAME_PATTERN&& other)
	{
		if (&other == this)
			return *this;
		m_nIndex = other.m_nIndex;
		m_nSeqIndex = other.m_nSeqIndex;
		m_wstrName = other.m_wstrName;
		m_bSideLightOn = other.m_bSideLightOn;
		m_nWidth_R = other.m_nWidth_R;
		m_nHeight_R = other.m_nHeight_R;
		m_dictCameraName_SAA.clear();
		m_dictCameraName_SAA = other.m_dictCameraName_SAA;
		return *this;
	}

	 bool GetCameraFramePattern(wstring wstrCameraName, CAMERA_FRAME_PATTERN& ret)
	{
		ret.m_nIndex = m_nIndex;
		ret.m_nSeqIndex = m_nSeqIndex;
		ret.m_wstrName = m_wstrName;
		ret.m_bSideLightOn = m_bSideLightOn;//滤光轮运动参数
		ret.m_nWidth_R = m_nWidth_R;
		ret.m_nHeight_R = m_nHeight_R;
		if (m_dictCameraName_SAA.find(wstrCameraName) != m_dictCameraName_SAA.end())//主相机名字
		{
			ret.m_vActionGroup = m_dictCameraName_SAA[wstrCameraName];
            return true;
		}
		return false;
	}
};

struct PROJECT
{
	wstring                    m_wstrProjectName;
	int                        m_nMainCameraHeight;
	SCREEN_PARAM               m_screenParam;
	std::vector<FRAME_PATTERN> m_vPattern;
	PROJECT() : m_wstrProjectName(L""), m_nMainCameraHeight(-1)
	{
		m_vPattern.clear();
	}

	PROJECT(const PROJECT& other)
	{
		m_wstrProjectName = other.m_wstrProjectName;
		m_nMainCameraHeight = other.m_nMainCameraHeight;
		m_screenParam = other.m_screenParam;
		m_vPattern.clear();
		m_vPattern = other.m_vPattern;
	}

	PROJECT(PROJECT&& other)
	{
		m_wstrProjectName = other.m_wstrProjectName;
		m_nMainCameraHeight = other.m_nMainCameraHeight;
		m_screenParam = other.m_screenParam;
		m_vPattern.clear();
		m_vPattern = other.m_vPattern;
	}

	PROJECT& operator=(const PROJECT& other)
	{
		if (&other == this)
			return *this;
		m_wstrProjectName = other.m_wstrProjectName;
		m_nMainCameraHeight = other.m_nMainCameraHeight;
		m_screenParam = other.m_screenParam;
		m_vPattern.clear();
		m_vPattern = other.m_vPattern;
		return *this;
	}

	PROJECT& operator=(PROJECT&& other)
	{
		if (&other == this)
			return *this;
		m_wstrProjectName = other.m_wstrProjectName;
		m_nMainCameraHeight = other.m_nMainCameraHeight;
		m_screenParam = other.m_screenParam;
		m_vPattern.clear();
		m_vPattern = other.m_vPattern;
		return *this;
	}
};

struct CAMERA_SETTING
{
	wstring m_wstrCameraName;
	int     m_nIndex;
	bool    m_bMainCamera;
	int     m_nSerialPort;
	int     m_type;
	long    m_nWidth;
	long    m_nHeight;
	int     m_nBuffSize;
	int     m_nBaudrate;
	int     m_nDatabits;
	int     m_nStopsbits;
	wstring m_wstrConfigFile; // Only for Vieworks/Xtium CL card
							  //TODO: put other parameters here
	CAMERA_SETTING()
		:m_wstrCameraName(L""),
		m_nIndex(-1),
		m_bMainCamera(false),
		m_nSerialPort(-1),
		m_type(0),
		m_nWidth(0),
		m_nHeight(0),
		m_nBuffSize(0),
		m_nBaudrate(0),
		m_nDatabits(0),
		m_nStopsbits(0),
		m_wstrConfigFile(L"")
	{
		//TODO:
	}
	CAMERA_SETTING(const CAMERA_SETTING& other)
	{
		m_wstrCameraName = other.m_wstrCameraName;
		m_nIndex = other.m_nIndex;
		m_bMainCamera = other.m_bMainCamera;
		m_nSerialPort = other.m_nSerialPort;
		m_type = other.m_type;
		m_nWidth = other.m_nWidth;
		m_nHeight = other.m_nHeight;
		m_nBuffSize = other.m_nBuffSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		m_wstrConfigFile = other.m_wstrConfigFile;
	}
	CAMERA_SETTING(CAMERA_SETTING&& other)
	{
		m_wstrCameraName = other.m_wstrCameraName;
		m_nIndex = other.m_nIndex;
		m_bMainCamera = other.m_bMainCamera;
		m_nSerialPort = other.m_nSerialPort;
		m_type = other.m_type;
		m_nWidth = other.m_nWidth;
		m_nHeight = other.m_nHeight;
		m_nBuffSize = other.m_nBuffSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		m_wstrConfigFile = other.m_wstrConfigFile;
	}

	CAMERA_SETTING& operator=(const CAMERA_SETTING& other)
	{
		if (&other == this)
			return *this;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nIndex = other.m_nIndex;
		m_bMainCamera = other.m_bMainCamera;
		m_nSerialPort = other.m_nSerialPort;
		m_type = other.m_type;
		m_nWidth = other.m_nWidth;
		m_nHeight = other.m_nHeight;
		m_nBuffSize = other.m_nBuffSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		m_wstrConfigFile = other.m_wstrConfigFile;
		return *this;
	}
	CAMERA_SETTING& operator=(CAMERA_SETTING&& other)
	{
		if (&other == this)
			return *this;
		m_wstrCameraName = other.m_wstrCameraName;
		m_nIndex = other.m_nIndex;
		m_bMainCamera = other.m_bMainCamera;
		m_nSerialPort = other.m_nSerialPort;
		m_type = other.m_type;
		m_nWidth = other.m_nWidth;
		m_nHeight = other.m_nHeight;
		m_nBuffSize = other.m_nBuffSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		m_wstrConfigFile = other.m_wstrConfigFile;
		return *this;
	}
	void Reset()
	{
		m_nIndex = -1;
		m_wstrCameraName = L"";
		m_bMainCamera = false;
		m_nSerialPort = -1;
		m_nWidth = 0;
		m_nHeight = 0;
		m_type = -1;
		m_nBuffSize = 0;
		m_nBaudrate = 0;
		m_nDatabits = 0;
		m_nStopsbits = 0;
		m_wstrConfigFile = L"";	
	}

	bool operator==(const CAMERA_SETTING& other)
	{
		if (m_wstrCameraName != other.m_wstrCameraName)
		{
			return false;
		}
		if (m_bMainCamera != other.m_bMainCamera)
		{
			return false;
		}
		if (m_nSerialPort != other.m_nSerialPort)
		{
			return false;
		}
		if (m_nWidth != other.m_nWidth)
		{
			return false;
		}
		if (m_nHeight != other.m_nHeight)
		{
			return false;
		}
		if (m_type != other.m_type)
		{
			return false;
		}
		if (m_wstrConfigFile != other.m_wstrConfigFile)
		{
			return false;
		}
		if (m_nBuffSize != other.m_nBuffSize)
		{
			return false;
		}
		if (m_nBaudrate != other.m_nBaudrate)
		{
			return false;
		}
		if (m_nDatabits != other.m_nDatabits)
		{
			return false;
		}
		if (m_nStopsbits != other.m_nStopsbits)
		{
			return false;
		}
		return true;
	}
};

struct AXIS_GENERAL_SETTING
{
	wstring m_wstrAxisName;
	int     m_nAxisIndex;
	int     m_nVmMove;
	int     m_nAccMove;
	int     m_nDecMove;
	int     m_nDecStop;
	int     m_nDirectHome;
	int     m_nVmHome;
	int     m_nAccHome;
	int     m_nDecHome;
	int     m_nVStopHome;
	AXIS_GENERAL_SETTING() :m_wstrAxisName(L""), m_nAxisIndex(-1), m_nVmMove(-1), m_nAccMove(-1),
		m_nDecMove(-1), m_nDecStop(-1), m_nDirectHome(-1), m_nVmHome(-1),
		m_nAccHome(-1), m_nDecHome(-1), m_nVStopHome(-1)
	{

	}
	AXIS_GENERAL_SETTING(const AXIS_GENERAL_SETTING& other)
	{
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex  = other.m_nAxisIndex;
		m_nVmMove = other.m_nVmMove;
		m_nAccMove = other.m_nAccMove;
		m_nDecMove = other.m_nDecMove;
		m_nDecStop = other.m_nDecStop;
		m_nDirectHome = other.m_nDirectHome;
		m_nVmHome = other.m_nVmHome;
		m_nAccHome = other.m_nAccHome;
		m_nDecHome = other.m_nDecHome;
		m_nVStopHome = other.m_nVStopHome;
	}
	AXIS_GENERAL_SETTING(AXIS_GENERAL_SETTING&& other)
	{
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nVmMove = other.m_nVmMove;
		m_nAccMove = other.m_nAccMove;
		m_nDecMove = other.m_nDecMove;
		m_nDecStop = other.m_nDecStop;
		m_nDirectHome = other.m_nDirectHome;
		m_nVmHome = other.m_nVmHome;
		m_nAccHome = other.m_nAccHome;
		m_nDecHome = other.m_nDecHome;
		m_nVStopHome = other.m_nVStopHome;
	}
	AXIS_GENERAL_SETTING& operator=(const AXIS_GENERAL_SETTING& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nVmMove = other.m_nVmMove;
		m_nAccMove = other.m_nAccMove;
		m_nDecMove = other.m_nDecMove;
		m_nDecStop = other.m_nDecStop;
		m_nDirectHome = other.m_nDirectHome;
		m_nVmHome = other.m_nVmHome;
		m_nAccHome = other.m_nAccHome;
		m_nDecHome = other.m_nDecHome;
		m_nVStopHome = other.m_nVStopHome;
		return *this;
	}
	AXIS_GENERAL_SETTING& operator=(AXIS_GENERAL_SETTING&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nVmMove = other.m_nVmMove;
		m_nAccMove = other.m_nAccMove;
		m_nDecMove = other.m_nDecMove;
		m_nDecStop = other.m_nDecStop;
		m_nDirectHome = other.m_nDirectHome;
		m_nVmHome = other.m_nVmHome;
		m_nAccHome = other.m_nAccHome;
		m_nDecHome = other.m_nDecHome;
		m_nVStopHome = other.m_nVStopHome;
		return *this;
	}
};

struct CODE_SCANER_SETTING
{
	int     m_nPortNum;
	int     m_nBufferSize;
	int     m_nBaudrate;
	int     m_nDatabits;
	int     m_nStopsbits;
	CODE_SCANER_SETTING() :m_nPortNum(-1), m_nBufferSize(-1), m_nBaudrate(-1),
		m_nDatabits(-1), m_nStopsbits(-1)
	{

	}
	CODE_SCANER_SETTING(const CODE_SCANER_SETTING& other)
	{
		m_nPortNum = other.m_nPortNum;
		m_nBufferSize = other.m_nBufferSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
	}
	CODE_SCANER_SETTING(CODE_SCANER_SETTING&& other)
	{
		m_nPortNum = other.m_nPortNum;
		m_nBufferSize = other.m_nBufferSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
	}
	CODE_SCANER_SETTING& operator=(const CODE_SCANER_SETTING& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nPortNum = other.m_nPortNum;
		m_nBufferSize = other.m_nBufferSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		return *this;
	}
	CODE_SCANER_SETTING& operator=(CODE_SCANER_SETTING&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nPortNum = other.m_nPortNum;
		m_nBufferSize = other.m_nBufferSize;
		m_nBaudrate = other.m_nBaudrate;
		m_nDatabits = other.m_nDatabits;
		m_nStopsbits = other.m_nStopsbits;
		return *this;
	}
};

struct USER_MANAGEMENT_GENERAL_SETTING
{
	wstring m_wstrUsername;
	wstring m_wstrPassword;
	wstring m_wstrLevel;
	USER_MANAGEMENT_GENERAL_SETTING() :m_wstrUsername(L""),
		m_wstrPassword(L""), m_wstrLevel(L"")
	{

	}
	USER_MANAGEMENT_GENERAL_SETTING(const USER_MANAGEMENT_GENERAL_SETTING& other)
	{
		m_wstrUsername = other.m_wstrUsername;
		m_wstrPassword = other.m_wstrPassword;
		m_wstrLevel = other.m_wstrLevel;
	}
	USER_MANAGEMENT_GENERAL_SETTING(USER_MANAGEMENT_GENERAL_SETTING&& other)
	{
		m_wstrUsername = other.m_wstrUsername;
		m_wstrPassword = other.m_wstrPassword;
		m_wstrLevel = other.m_wstrLevel;
	}
	USER_MANAGEMENT_GENERAL_SETTING& operator=(const USER_MANAGEMENT_GENERAL_SETTING& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrUsername = other.m_wstrUsername;
		m_wstrPassword = other.m_wstrPassword;
		m_wstrLevel = other.m_wstrLevel;
		return *this;
	}
	USER_MANAGEMENT_GENERAL_SETTING& operator=(USER_MANAGEMENT_GENERAL_SETTING&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrUsername = other.m_wstrUsername;
		m_wstrPassword = other.m_wstrPassword;
		m_wstrLevel = other.m_wstrLevel;
		return *this;
	}
};

struct SERVO_AXIS_SETTING
{
	wstring m_wstrAxisName;
	int     m_nAxisIndex;
	int    m_nAccSpeed; // Acceleration Speed
	int    m_nDecSpeed; // Deceleration Speed
	int    m_nMaxSpeed; // Maximum Speed    

	SERVO_AXIS_SETTING() :m_wstrAxisName(L""), m_nAxisIndex(-1), m_nAccSpeed(0), m_nDecSpeed(0), m_nMaxSpeed(0)
	{

	}
	SERVO_AXIS_SETTING(const SERVO_AXIS_SETTING& other)
	{
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nAccSpeed = other.m_nAccSpeed;
		m_nDecSpeed = other.m_nDecSpeed;
		m_nMaxSpeed = other.m_nMaxSpeed;
	}
	SERVO_AXIS_SETTING(SERVO_AXIS_SETTING&& other)
	{
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nAccSpeed = other.m_nAccSpeed;
		m_nDecSpeed = other.m_nDecSpeed;
		m_nMaxSpeed = other.m_nMaxSpeed;
	}
	SERVO_AXIS_SETTING& operator=(const SERVO_AXIS_SETTING& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nAccSpeed = other.m_nAccSpeed;
		m_nDecSpeed = other.m_nDecSpeed;
		m_nMaxSpeed = other.m_nMaxSpeed;
		return *this;
	}
	SERVO_AXIS_SETTING& operator=(SERVO_AXIS_SETTING&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrAxisName = other.m_wstrAxisName;
		m_nAxisIndex = other.m_nAxisIndex;
		m_nAccSpeed = other.m_nAccSpeed;
		m_nDecSpeed = other.m_nDecSpeed;
		m_nMaxSpeed = other.m_nMaxSpeed;
		return *this;
	}
};

struct STATION_CONFIG
{
	int            m_nStationIndex;
	wstring        m_wstrCrtProjectName;
	std::vector<SERVO_AXIS_SETTING>             m_vServoAxisSetting; // There must be three servo axes for each station.
	std::map<wstring, CAMERA_SETTING>           m_dictCamara;
	std::map<wstring, PROJECT>                  m_dictProject;

	STATION_CONFIG() :m_nStationIndex(-1), m_wstrCrtProjectName(L"")
	{

	}
	STATION_CONFIG(const STATION_CONFIG& other)
	{
		m_nStationIndex = other.m_nStationIndex;
		m_wstrCrtProjectName = other.m_wstrCrtProjectName;
		m_vServoAxisSetting.clear();
		m_vServoAxisSetting = other.m_vServoAxisSetting;
		m_dictCamara.clear();
		m_dictCamara = other.m_dictCamara;
		m_dictProject.clear();
		m_dictProject = other.m_dictProject;
	}
	STATION_CONFIG(STATION_CONFIG&& other)
	{
		m_nStationIndex = other.m_nStationIndex;
		m_wstrCrtProjectName = other.m_wstrCrtProjectName;
		m_vServoAxisSetting.clear();
		m_vServoAxisSetting = other.m_vServoAxisSetting;
		m_dictCamara.clear();
		m_dictCamara = other.m_dictCamara;
		m_dictProject.clear();
		m_dictProject = other.m_dictProject;
	}

	STATION_CONFIG& operator=(const STATION_CONFIG& other)
	{
		if (&other == this)
			return *this;

		m_nStationIndex = other.m_nStationIndex;
		m_wstrCrtProjectName = other.m_wstrCrtProjectName;
		m_vServoAxisSetting.clear();
		m_vServoAxisSetting = other.m_vServoAxisSetting;
		m_dictCamara.clear();
		m_dictCamara = other.m_dictCamara;
		m_dictProject.clear();
		m_dictProject = other.m_dictProject;
		return *this;
	}
	STATION_CONFIG& operator=(STATION_CONFIG&& other)
	{
		if (&other == this)
			return *this;
		m_nStationIndex = other.m_nStationIndex;
		m_wstrCrtProjectName = other.m_wstrCrtProjectName;
		m_vServoAxisSetting.clear();
		m_vServoAxisSetting = other.m_vServoAxisSetting;
		m_dictCamara.clear();
		m_dictCamara = other.m_dictCamara;
		m_dictProject.clear();
		m_dictProject = other.m_dictProject;
		return *this;
	}
};

struct PG_PROXY_SETTING
{
	int m_nIndex;
	int m_nStationIdex;
	wstring m_wstrIpAddress;
	PG_PROXY_SETTING() :m_nIndex(-1), m_nStationIdex(-1), m_wstrIpAddress(L"")
	{

	}
	PG_PROXY_SETTING(int nIndex, int nStaionIndex, wstring wstrIp) :
		m_nIndex(nIndex), m_nStationIdex(nStaionIndex), m_wstrIpAddress(wstrIp)
	{

	}

	PG_PROXY_SETTING(const PG_PROXY_SETTING& other)
	{
		m_nIndex = other.m_nIndex;
		m_nStationIdex = other.m_nStationIdex;
		m_wstrIpAddress = other.m_wstrIpAddress;
	}
	PG_PROXY_SETTING(PG_PROXY_SETTING&& other)
	{
		m_nIndex = other.m_nIndex;
		m_nStationIdex = other.m_nStationIdex;
		m_wstrIpAddress = other.m_wstrIpAddress;
	}
	PG_PROXY_SETTING& operator=(const PG_PROXY_SETTING& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nIndex = other.m_nIndex;
		m_nStationIdex = other.m_nStationIdex;
		m_wstrIpAddress = other.m_wstrIpAddress;
		return *this;
	}
	PG_PROXY_SETTING& operator=(PG_PROXY_SETTING&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nIndex = other.m_nIndex;
		m_nStationIdex = other.m_nStationIdex;
		m_wstrIpAddress = other.m_wstrIpAddress;
		return *this;
	}
};

struct PENDING_FLASH_DATA
{
	string    m_strFlashData;
	PENDING_FLASH_DATA() :m_strFlashData("")
	{

	}
	PENDING_FLASH_DATA(string strData): m_strFlashData(strData)
	{

	}

	PENDING_FLASH_DATA(const PENDING_FLASH_DATA& other)
	{
		m_strFlashData = other.m_strFlashData;
	}
	PENDING_FLASH_DATA(PENDING_FLASH_DATA&& other)
	{
		m_strFlashData = other.m_strFlashData;
	}

	PENDING_FLASH_DATA& operator=(const PENDING_FLASH_DATA& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strFlashData = other.m_strFlashData;
		return *this;
	}
	PENDING_FLASH_DATA& operator=(PENDING_FLASH_DATA&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strFlashData = other.m_strFlashData;
		return *this;
	}
};

struct PENDING_RAW_IMAGE_DATA
{
	wstring    m_wstrGrayLevel;
	Mat        m_rawImage;
	int        m_nIndex;
	wstring    m_wstrCameraName;
	bool       m_bSave;
	PENDING_RAW_IMAGE_DATA() :m_wstrGrayLevel(L""), m_nIndex(-1), m_bSave(true)
	{

	}
	PENDING_RAW_IMAGE_DATA(wstring wstrGrayLevel, Mat rawImage,int nIndex, wstring wstrCameraName) :m_wstrGrayLevel(wstrGrayLevel)
	{
		m_rawImage = rawImage.clone();
		m_nIndex = nIndex;
		m_wstrCameraName = wstrCameraName;
		m_bSave = true;
	}

	PENDING_RAW_IMAGE_DATA(const PENDING_RAW_IMAGE_DATA& other)
	{
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_rawImage = other.m_rawImage.clone();
		m_nIndex = other.m_nIndex;
		m_wstrCameraName = other.m_wstrCameraName;
		m_bSave = other.m_bSave;
	}
	PENDING_RAW_IMAGE_DATA(PENDING_RAW_IMAGE_DATA&& other)
	{
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_rawImage = other.m_rawImage.clone();
		m_nIndex = other.m_nIndex;
		m_wstrCameraName = other.m_wstrCameraName;
		m_bSave = other.m_bSave;
	}

	PENDING_RAW_IMAGE_DATA& operator=(const PENDING_RAW_IMAGE_DATA& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_rawImage = other.m_rawImage.clone();
		m_nIndex = other.m_nIndex;
		m_wstrCameraName = other.m_wstrCameraName;
		m_bSave = other.m_bSave;
		return *this;
	}
	PENDING_RAW_IMAGE_DATA& operator=(PENDING_RAW_IMAGE_DATA&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_rawImage = other.m_rawImage.clone();
		m_nIndex = other.m_nIndex;
		m_wstrCameraName = other.m_wstrCameraName;
		m_bSave = other.m_bSave;
		return *this;
	}
};

struct PENDING_CSV_DATA
{
	wstring   m_wstrGrayLevel;
	double*   m_ptrData;
	int       m_nIndex;
	PENDING_CSV_DATA() :m_wstrGrayLevel(L""), m_ptrData(nullptr),m_nIndex(-1)
	{
	}
	PENDING_CSV_DATA(wstring wstrGrayLevel, double* ptrData,int nIndex) :m_wstrGrayLevel(wstrGrayLevel), m_ptrData(ptrData),m_nIndex(nIndex)
	{

	}

	PENDING_CSV_DATA(const PENDING_CSV_DATA& other)
	{
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_ptrData = other.m_ptrData;
		m_nIndex = other.m_nIndex;
	}
	PENDING_CSV_DATA(PENDING_CSV_DATA&& other)
	{
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_ptrData = other.m_ptrData;
		m_nIndex = other.m_nIndex;
	}

	PENDING_CSV_DATA& operator=(const PENDING_CSV_DATA& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_ptrData = other.m_ptrData;
		m_nIndex = other.m_nIndex;
		return *this;
	}
	PENDING_CSV_DATA& operator=(PENDING_CSV_DATA&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_wstrGrayLevel = other.m_wstrGrayLevel;
		m_ptrData = other.m_ptrData;
		m_nIndex = other.m_nIndex;
		return *this;
	}
};


struct PENDING_SWITCH_PATTERN
{
	int       m_nSeqIndex;
	int       m_nPatternIndex;
	PENDING_SWITCH_PATTERN():m_nSeqIndex(-1),m_nPatternIndex(-1)
	{
	}
	PENDING_SWITCH_PATTERN(int nSeq,int nIndex):m_nSeqIndex(nSeq),m_nPatternIndex(nIndex)
	{

	}
	PENDING_SWITCH_PATTERN(const PENDING_SWITCH_PATTERN& other)
	{
		m_nSeqIndex = other.m_nSeqIndex;
		m_nPatternIndex = other.m_nPatternIndex;
	}
	PENDING_SWITCH_PATTERN(PENDING_SWITCH_PATTERN&& other)
	{
		m_nSeqIndex = other.m_nSeqIndex;
		m_nPatternIndex = other.m_nPatternIndex;
	}

	PENDING_SWITCH_PATTERN& operator=(const PENDING_SWITCH_PATTERN& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nSeqIndex = other.m_nSeqIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		return *this;
	}
	PENDING_SWITCH_PATTERN& operator=(PENDING_SWITCH_PATTERN&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_nSeqIndex = other.m_nSeqIndex;
		m_nPatternIndex = other.m_nPatternIndex;
		return *this;
	}
};

typedef union define_ui//to receive return value of CRC16RTU()
{
	int ui;
	unsigned char uc[2];
}define_ui;

const std::vector<unsigned char> SELECT_MODULE = {\
0X42,  0X01,  0X00,  0X82,  0X00,  0X40,  0X00,  0X01,\
0X00 , 0X01 , 0X00 , 0X27 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X10 , 0X74 , 0X73 , 0X65 , 0X74,\
0X00 , 0X01 , 0X00 , 0X11 , 0X00 , 0X00 , 0X00 , 0X31,\
0X00 , 0X01 , 0X00 , 0X12 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X13 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X14 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X15 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X16 , 0X00 , 0X00 , 0X00 , 0X00,\
0X00 , 0X01 , 0X00 , 0X17 , 0X00 , 0X00 , 0X00 , 0X00};
const std::vector<unsigned char> SELECT_MODULE_REPLY = { 0X00,0X00,0X00,0X83,0X00,0X08,0X00,0X01,0X00,0X00,0X00,0X01,0X00,0X00,0X00,0X00};

const std::vector<unsigned char> SPI_FLASH_EN = { 0X42,0X01,0X00,0X86,0X00,0X18,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X05,0X00,0X0a,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0X98,0X01,0Xcf,0X00,0X00 };
const std::vector<unsigned char> RAD_DEMURA_EN = { 0X42,0X01,0X00,0X86,0X00,0X28,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X0d,0X00,0X1a,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0X72,0X01,0X11,0X00,0Xb4,0X01,0X31,0X00,0Xb7,0X01,0X42,0X00,0Xaa,0X01,0X03,0X00,0X09,0X01,0X13,0X00,0X00 };
const std::vector<unsigned char> SCREEN_ON = { 0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X20,0X00,0X00,0X80,0X01};
const std::vector<unsigned char> SCREEN_OFF   = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X20,0X00,0X00,0X80,0X00};
const std::vector<unsigned char> ON_OFF_REPLY = {0X00,0X00,0X00,0X83,0X00,0X08,0X00,0X01,0X00,0X00,0X00,0X01,0X00,0X00,0X00,0X00};
const std::vector<unsigned char> SET_RGB = { 0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X31,0X00,0X00,0X00,0X00 };//后三位分别表示B，G，R
const std::vector<unsigned char> EREASE_FLASH = {\
0X42,0X01,0X00,0X86,0X00,0X20,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
0X00,0X09,0X00,0X12,0X01,0X80,0X00,0Xfe,\
0X01,0X20,0X00,0Xf2,0X01,0X04,0X00,0Xb1,\
0X01,0X00,0X00,0Xf3,0X01,0X01,0X00,0X00 };//最后2位为crc校验

//Switch pattern;
const std::vector<unsigned char> SELECT_PATTERN = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X21,0X00,0X00,0X00,0X00 };
const std::vector<unsigned char> SELECT_PATTERN_REPLY = {0X00,0X00,0X00,0X83,0X00,0X08,0X00,0X01,0X00,0X00,0X00,0X01,0X00,0X00,0X00,0X00};

// Write Register
const string WRITE_REGISTER = "0X42,0X01,0X00,0X86,Length(2Byte),0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0x00,0x0x,0x00,0x0y,TYPE_WR,CH,DATA,crc_l,crc_h";
const string WRITE_REGISTER_REPLY = "0X42,0X01,0X00,0X86,0X00,0X0C,0X00,0X01,0X00 , 0X02 , 0X00 , 0X00 ,0X32 , 0X10 , 0X20 , 0X93 , 0X00 , FLAG , crc_l , crc_h";

// Read Register
const string READ_REGISTER = "0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00 , 0X80 , 0X00 , 0X00 ,\
0X32 , 0X10 , 0X20 , 0X94 , 0x00 , 0x03 , 0x00 , 0x06 , 00 , TYPE_RD , CH , HighAddrByte , LowAddrByte , DataByteNum_N , crc_l , crc_h";
const string READ_REGISTER_REPLY = "0X42,0X01,0X00,0X86,Length(2Byte),0X00,0X01,0X00 , 0X80 , 0X00 , 0X00 ,0X32 , 0X10 , 0X20 , 0X94 , DATA , crc_l , crc_h";

//const std::vector<unsigned char> READ_VOL_CUR = {0X42,0X01,0X00,0X86,0X00,0X0C,0X00,0X01,0X00,0X20,0X00,0X00,0X32,0X03,0X20,0X39,0X00,0x50,crc_l,crc_h};
////Read voltage and current
//0X42 , 0X01 , 0X00 , 0X86 , 0X00 , 0X0C , 0X00 , 0X01 ,
//0X00 , 0X20 , 0X00 , 0X00 ,
//0X32 , 0X03 , 0X20 , 0X39 , 0X00 , 0x50 , crc_l , crc_h
//Feedback
//0X42 , 0X01 , 0X00 , 0X86 , 0X00 , 0XAA , 0X00 , 0X01 ,
//0X00 , 0X04 , 0X00 , 0X00 ,
//0X32 , 0X03 , 0X00 , 0XA0 , DATA , crc_l , crc_h
//DATA : 数据排列见附件


const std::vector<unsigned char> ENABLE_WRITE_DDR  = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X00,0X00,0X01,0X00,0X46,0X00,0X00,0X00,0X01};
const std::vector<unsigned char> DISABLE_WRITE_DDR = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X46,0X00,0X00,0X00,0X00};

const std::vector<unsigned char> READ_FPGA  = {0X42,0X01,0X00, 0X80,0X00,0X04,0X12,0X0a,0X00,0X01,0X00,0X00};
const std::vector<unsigned char> WRITE_FPGA = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X00,0X00, 0X01,0X00,0X27,0X00,0X00,0X00,0X02};
// write 2 into 0x00010027 42 01 00 82 00 08 00 00 00 01 00 27 00 00 00 02

//const std::vector<unsigned char> EREASE_FLASH = {0X42,0X01,0X00,0X86,0X00,0X20,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X09,0X00,0X12,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0Xf2,0X01,0X04,0X00,0Xb1,0X01,0X00,0X00,0Xf3,0X01,0X01,0X00,0X00};//最后2位为crc校验

const std::vector<unsigned char> GET_FPGA_CACHE_SZ = { 0X42,0X01,0X00,0X80,0X00,0X04,0X12,0X0a,0X00,0X01,0X00,0X2a };
const string PACKAGE_HEADER    = "00 00 00 00 03 ## ## ##";              // 00 00 00 00 03 ## ## ## +  data 
const string LAST_PACKAGE_FLAG = "00 00 00 00 02 ## ## ## 00 00 00 01";

const std::vector<unsigned char> DATA_TRANSFER_OVER = { 0X00,0X00,0X00,0X00,0X02,0X00,0X00,0X00,0X00,0X00,0X00,0X01 };
const std::vector<unsigned char>  CHANGE_TO_LOW_SPEED = { 0X42,0X01,0X00,0X86,0X00,0X10,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X96,0X00,0X01,0X00,0X02,0Xc0,0X00,0X00,0X00 };
const std::vector<unsigned char>  CHANGE_TO_HIGH_SPEED = { 0x42,0X01,0X00,0X86,0X00,0X10,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X96,0X00,0X01,0X00,0X02,0Xc0,0X01,0X00,0X00 };
//const std::vector<unsigned char>  IC_READ_E8 = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,0Xe8,0X01,0X00,0X00 };
const std::vector<unsigned char>  START_SEND_CMD = {0X00,0X00,0X00,0X00,0X01,0X00,0X00,0X01,0X00,0X00,0X00,0X01,0Xc0,0X00,0X00,0X00,0X00,0X00,0X00,0X00 };
const std::vector<unsigned char>  IC_WRITE_CHECK_SUM_WRITE = {\
0X42,0X01,0X00,0X86,0X00,0X34,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
0X00,0X13,0X00,0X26,0X00,0X80,0X00,0Xfe,\
0X01,0X20,0X00,0Xf2,0X01,0X04,0X00,0Xab,\
0X01,0X00,0X00,0Xac,0X01,0X00,0X00,0Xad,\
0X01,0X00,0X00,0Xae,0X01,0X1a,0X00,0Xaf,\
0X01,0Xb3,0X00,0Xb0,0X01,0X62,0X00,0Xf2,\
0X01,0X01,0X00,0X00 };
const std::vector<unsigned char> READ_ENABLE_FLASH_CTRL = { 0X42,0X01,0X00,0X86,0X00,0X18,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X05,0X00,0X0a,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0Xf3,0X01,0X02,0X00,0X00 };
const std::vector<unsigned char> IC_READ_EA = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,0Xea,0X01,0X00,0X00 };
const std::vector<unsigned char> IC_READ_EB = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,0Xeb,0X01,0X00,0X00 };
const std::vector<unsigned char> IC_READ_EC = {	0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,0Xec,0X01,0X00,0X00 };
const std::vector<unsigned char>  IC_WRITE_RAM_EN = {\
0X42,0X01,0X00,0X86,0X00,0X44,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
0X00,0X1b,0X00,0X36,0X01,0X80,0X00,0Xfe,\
0X01,0X40,0X00,0X0e,0X01,0X80,0X00,0Xfe,\
0X01,0X20,0X00,0Xb1,0X01,0X00,0X00,0Xfe,\
0X01,0Xd0,0X00,0X55,0X01,0X30,0X00,0X07,\
0X01,0X01,0X00,0Xfe,0X01,0X40,0X00,0X0e,\
0X01,0X50,0X00,0Xfe,0X01,0Xd0,0X00,0X42,\
0X01,0X81,0X00,0X27,0X01,0X55,0X00,0Xfe,\
0X01,0X00,0X00,0X00 };//最后2个字节为crc校验低位，crc校验高位
const std::vector<unsigned char> SET_TIMING_10040 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X40,0X04,0X38,0X02,0X1d};
const std::vector<unsigned char> SET_TIMING_10041 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X41,0X00,0X24,0X08,0X98};
const std::vector<unsigned char> SET_TIMING_10042 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X42,0X00,0X02,0X00,0X04};
const std::vector<unsigned char> SET_TIMING_10043 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X43,0X00,0X08,0X00,0X04};
const std::vector<unsigned char> SET_TIMING_10044 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X44,0X00,0X01,0X8a,0Xd7};
const std::vector<unsigned char> SET_TIMING_10046 = {0X42,0X01,0X00,0X82,0X00,0X08,0X00,0X01,0X00,0X01,0X00,0X46,0X00,0X00,0X00,0X06};
const std::vector<unsigned char> IC_RAM_TO_FLASH = {\
0X42,0X01,0X00,0X86,0X00,0X38,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
0X00,0X15,0X00,0X2a,0X01,0X80,0X00,0Xfe,\
0X01,0X20,0X00,0Xf2,0X01,0X04,0X00,0Xb1,\
0X01,0X00,0X00,0Xab,0X01,0X00,0X00,0Xac,\
0X01,0X00,0X00,0Xad,0X01,0X00,0X00,0Xae,\
0X01,0X1a,0X00,0Xaf,0X01,0Xb3,0X00,0Xb0,\
0X01,0X62,0X00,0Xf2,0X01,0X10,0X00,0X00 };
const std::vector<unsigned char> SET_HIGHSPEED = { 0x42,0X01,0X00,0X86,0X00,0X10,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X96,0X00,0X01,0X00,0X02,0Xc0,0X01,0X00,0X00 };
const std::vector<unsigned char> SET_PAGE40 = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X03,0X00,0X06,0X01,0X80,0X00,0Xfe,0X01,0X40,0X00,0X00 };
const std::vector<unsigned char> SET_PAGE20 = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X03,0X00,0X06,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0X00};
const std::vector<unsigned char> SET_PAGE00 = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X03,0X00,0X06,0X01,0X80,0X00,0Xfe,0X01,0X00,0X00,0X00 };
const std::vector<unsigned char> SET_DISPLAY_OFF = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X03,0X00,0X06,0X01,0X80,0X00,0X28,0X00,0X00,0X00,0X00 };
const std::vector<unsigned char> SleepIn = { 0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X03,0X00,0X06,0X01,0X80,0X00,0X10,0X00,0X00,0X00,0X00 };
const std::vector<unsigned char> BURN_RECORD_OTP1 = { 0X42,0X01,0X00,0X86,0X00,0X28,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X0d,0X00,0X1a,0X01,0X80,0X00,0Xfe,0X01,0X40,0X00,0Xf9,0X01,0X02,0X00,0Xf2,0X01,0X01,0X00,0Xf3,0X01,0XA5,0X00,0Xf4,0X01,0X5a,0X00,0Xf5,0X01,0X3c,0X00,0X00 };
const std::vector<unsigned char> BURN_RECORD_OTP0 = { 0X42,0X01,0X00,0X86,0X00,0X28,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X0d,0X00,0X1a,0X01,0X80,0X00,0Xfe,0X01,0X40,0X00,0Xf8,0X01,0X02,0X00,0Xf2,0X01,0X01,0X00,0Xf3,0X01,0XA5,0X00,0Xf4,0X01,0X5a,0X00,0Xf5,0X01,0X3c,0X00,0X00 };
const std::vector<unsigned char> IC_WRITE_FE20F302 = { 0X42,0X01,0X00,0X86,0X00,0X18,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,0X00,0X05,0X00,0X0a,0X01,0X80,0X00,0Xfe,0X01,0X20,0X00,0Xf3,0X01,0X02,0X00,0X00 };
const std::vector<unsigned char> IC_WRITE_PAGE20_DATA = {\
0X42,0X01,0X00,0X86,0X00,0Xd4,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
0X00,0X63,0X00,0Xc6,0X01,0X80,0X00,0Xfe,\
0X01,0X20,0X00,0X60,0X01,0X40,0X00,0X61,\
0X01,0X40,0X00,0X62,0X01,0X40,0X00,0X63,\
0X01,0X40,0X00,0X64,0X01,0X40,0X00,0X65,\
0X01,0X40,0X00,0X72,0X01,0X11,0X00,0X73,\
0X01,0X00,0X00,0X74,0X01,0X04,0X00,0X75,\
0X01,0X08,0X00,0X76,0X01,0X22,0X00,0X77,\
0X01,0X1d,0X00,0X78,0X01,0X10,0X00,0X79,\
0X01,0X11,0X00,0X7a,0X01,0X00,0X00,0X7b,\
0X01,0X00,0X00,0X7c,0X01,0X00,0X00,0X7d,\
0X01,0X00,0X00,0X7e,0X01,0X00,0X00,0X7f,\
0X01,0X00,0X00,0X80,0X01,0X00,0X00,0X81,\
0X01,0X00,0X00,0X82,0X01,0X00,0X00,0X83,\
0X01,0X00,0X00,0X84,0X01,0X00,0X00,0X85,\
0X01,0X00,0X00,0X86,0X01,0X14,0X00,0X87,\
0X01,0X01,0X00,0X88,0X01,0X20,0X00,0X89,\
0X01,0X25,0X00,0X8a,0X01,0X02,0X00,0X8b,\
0X01,0X10,0X00,0X8c,0X01,0X00,0X00,0X8d,\
0X01,0X00,0X00,0X8e,0X01,0X00,0X00,0X8f,\
0X01,0X00,0X00,0X90,0X01,0X00,0X00,0X91,\
0X01,0X00,0X00,0X92,0X01,0X00,0X00,0X93,\
0X01,0X00,0X00,0X94,0X01,0X00,0X00,0X95,\
0X01,0X00,0X00,0X96,0X01,0X00,0X00,0Xb2,\
0X01,0X40,0X00,0Xb7,0X01,0X42,0X00,0Xb8,\
0X01,0Xd0,0X00,0Xb9,0X01,0X06,0X00,0Xba,\
0X01,0X00,0X00,0X00 };
/////////////////////////////////////
//24.IC read  if ready E8的Bit 7==1
const std::vector<unsigned char> IC_READ_E8 = {0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,0Xe8,0X01,0X00,0X00 };

//26.check sum flow读寄存器IC read E6
const std::vector<unsigned char> CHECK_SUM_READ_E6 = {\
0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,\
0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,\
0Xe6,0X01,0X00,0X00 };//最后2个字节为crc校验低位，crc校验高位

//27.check sum flow读寄存器IC read E7
const std::vector<unsigned char> CHECK_SUM_READ_E7 = {\
0X42,0X01,0X00,0X86,0X00,0X14,0X00,0X01,\
0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X94,\
0X00,0X03,0X00,0X06,0X00,0X01,0X80,0X00,\
0Xe7,0X01,0X00,0X00 };//最后2个字节为crc校验低位，crc校验高位

const std::vector<unsigned char> DEMURA_OFF = {\
	0X42,0X01,0X00,0X86,0X00,0X24,0X00,0X01,\
	0X00,0X80,0X00,0X00,0X32,0X10,0X20,0X93,\
	0X00,0X0b,0X00,0X16,0X00,0X80,0X00,0Xfe,\
	0X01,0X20,0X00,0X09,0X01,0X10,0X00,0Xa6,\
	0X01,0Xc0,0X00,0Xa7,0X01,0Xff,0X00,0Xbc,\
	0X01,0X81,0X00,0X00 };

const std::vector<unsigned char> READ_U_AND_I = { 0X42,0X01,0X00,0X86,0X00,0X0C,0X00,0X01,0X00,0X20,0X00,0X00,0X32,0X03,0X20,0X39,0X00,0x50,0X00,0X00 };

class chronograph
{
public:
	chronograph()
	{
		QueryPerformanceFrequency(&m_freq);
		QueryPerformanceCounter(&m_bgn);
	}
	void start()
	{
		QueryPerformanceCounter(&m_bgn);
	}

	double duration()
	{
		QueryPerformanceCounter(&m_end);
		return (m_end.QuadPart - m_bgn.QuadPart) * 1000.0 / m_freq.QuadPart;
	}
	LARGE_INTEGER now()
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return now;
	}
	double DoubleNow()
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return now.QuadPart*1000.0 / m_freq.QuadPart;
	}
private:
	LARGE_INTEGER m_freq;
	LARGE_INTEGER m_bgn;
	LARGE_INTEGER m_end;
};

struct IMG_TEXT
{
	CString     m_strText;
	wstring     m_txtColorName;
	cv::Point2f m_point;
	IMG_TEXT() :m_strText(L""), m_txtColorName(L""), m_point(cv::Point2f(-1.0f, -1.0f))
	{

	}
	IMG_TEXT(CString strText, wstring txtColorName, cv::Point2f point)
	{
		m_strText = strText;
		m_txtColorName = txtColorName;
		m_point = point;
	}
	IMG_TEXT(const IMG_TEXT& other)
	{
		m_strText = other.m_strText;
		m_txtColorName = other.m_txtColorName;
		m_point = other.m_point;
	}
	IMG_TEXT(IMG_TEXT&& other)
	{
		m_strText = other.m_strText;
		m_txtColorName = other.m_txtColorName;
		m_point = other.m_point;
	}
	IMG_TEXT& operator=(const IMG_TEXT& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strText = other.m_strText;
		m_txtColorName = other.m_txtColorName;
		m_point = other.m_point;
		return *this;
	}
	IMG_TEXT& operator=(IMG_TEXT&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_strText = other.m_strText;
		m_txtColorName = other.m_txtColorName;
		m_point = other.m_point;
		return *this;
	}
};

struct IMG_LINE
{
	cv::Point2f m_pointStart;		
	cv::Point2f m_pointEnd;	
	wstring     m_lineColorName;
	int         m_nLineWide;
	IMG_LINE() :m_pointStart(cv::Point2f(-1.0f, -1.0f)), m_pointEnd(cv::Point2f(-1.0f, -1.0f)), m_lineColorName(L""), m_nLineWide(0)
	{

	}
	IMG_LINE(cv::Point2f pointStart, cv::Point2f pointEnd, wstring lineColorName,int nLineWide)
	{
		m_pointStart = pointStart;
		m_pointEnd = pointEnd;
		m_lineColorName = lineColorName;
		m_nLineWide = nLineWide;
	}
	IMG_LINE(const IMG_LINE &other)
	{
		m_pointStart = other.m_pointStart;
		m_pointEnd = other.m_pointEnd;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_LINE( IMG_LINE &&other)
	{
		m_pointStart = other.m_pointStart;
		m_pointEnd = other.m_pointEnd;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_LINE& operator=(const IMG_LINE &other)
	{
		 if (&other == this)
		{
			return *this;
		}
		m_pointStart = other.m_pointStart;
		m_pointEnd = other.m_pointEnd;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
	IMG_LINE& operator=( IMG_LINE &&other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_pointStart = other.m_pointStart;
		m_pointEnd = other.m_pointEnd;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
};			

struct IMG_CIRCLE
{
	cv::Point2f m_pointCenter;
	float       m_fRadius;
	wstring     m_lineColorName;
	int         m_nLineWide;
	IMG_CIRCLE() :m_pointCenter(cv::Point2f(-1.0f, -1.0f)), m_fRadius(-1.0f), m_lineColorName(L""), m_nLineWide(0)
	{

	}
	IMG_CIRCLE(cv::Point2f pointCenter, float fRadius, wstring lineColorName, int nLineWide)
	{
		m_pointCenter = pointCenter;
		m_fRadius = fRadius;
		m_lineColorName = lineColorName;
		m_nLineWide = nLineWide;
	}
	IMG_CIRCLE(const IMG_CIRCLE &other )
	{
		m_pointCenter = other.m_pointCenter;
		m_fRadius = other.m_fRadius;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_CIRCLE( IMG_CIRCLE &&other)
	{
		m_pointCenter = other.m_pointCenter;
		m_fRadius = other.m_fRadius;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_CIRCLE& operator=(const IMG_CIRCLE &other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_pointCenter = other.m_pointCenter;
		m_fRadius = other.m_fRadius;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
	IMG_CIRCLE& operator=( IMG_CIRCLE &&other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_pointCenter = other.m_pointCenter;
		m_fRadius = other.m_fRadius;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
};

struct IMG_RECTANGLE
{
	cv::Point2f m_pointCenter;
	float       m_length1; //半宽
	float       m_length2; //半长
	wstring     m_lineColorName;
	int         m_nLineWide;
	IMG_RECTANGLE() :m_pointCenter(cv::Point2f(-1.0f, -1.0f)), m_length1(-1.0f), m_length2(-1.0f), m_lineColorName(L""), m_nLineWide(0)
	{

	}
	IMG_RECTANGLE(cv::Point2f pointCenter, float length1,float length2, wstring lineColorName, int nLineWide)
	{
		m_pointCenter = pointCenter;
		m_length1 = length1; 
		m_length2 = length2;
		m_lineColorName = lineColorName;
		m_nLineWide = nLineWide;
	}
	IMG_RECTANGLE(const IMG_RECTANGLE &other)
	{
		m_pointCenter = other.m_pointCenter;
		m_length1 = other.m_length1;
		m_length2 = other.m_length2;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_RECTANGLE(IMG_RECTANGLE &&other)
	{
		m_pointCenter = other.m_pointCenter;
		m_length1 = other.m_length1;
		m_length2 = other.m_length2;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
	}
	IMG_RECTANGLE& operator=(const IMG_RECTANGLE &other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_pointCenter = other.m_pointCenter;
		m_length1 = other.m_length1;
		m_length2 = other.m_length2;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
	IMG_RECTANGLE& operator=(IMG_RECTANGLE &&other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_pointCenter = other.m_pointCenter;
		m_length1 = other.m_length1;
		m_length2 = other.m_length2;
		m_lineColorName = other.m_lineColorName;
		m_nLineWide = other.m_nLineWide;
		return *this;
	}
};

struct IMG_ROI_INFO
{
	eROI_TYPE m_eType;
	boost::array<double, 8> m_arrROIInfo;                  //m_eType          直线    矩形           旋转矩形      圆             椭圆
	IMG_ROI_INFO():m_eType(ROI_FULL)                       //m_arrROIInfo[0]  startX  pointCenterX   pointCenterX  pointCenterX   pointCenterX
	{                                                      //m_arrROIInfo[1]  startY  pointCenterY	 pointCenterY  pointCenterY	  pointCenterY
		memset(&m_arrROIInfo, 0, sizeof(m_arrROIInfo));    //m_arrROIInfo[2]  endX    length1		 length1	   radius		  length1	 
	}                                                      //m_arrROIInfo[3]  endY    length2		 length2					  length2
														   //m_arrROIInfo[4]                         angle						  angle
	IMG_ROI_INFO(eROI_TYPE eType):m_eType(eType)           //m_arrROIInfo[5]
	{                                                      //m_arrROIInfo[6]
		memset(&m_arrROIInfo, 0, sizeof(m_arrROIInfo));    //m_arrROIInfo[7]
	}
	IMG_ROI_INFO(const IMG_ROI_INFO& other)
	{
		m_eType = other.m_eType;
		m_arrROIInfo = other.m_arrROIInfo;
	}
	IMG_ROI_INFO(IMG_ROI_INFO&& other)
	{
		m_eType = other.m_eType;
		m_arrROIInfo = other.m_arrROIInfo;
	}
	IMG_ROI_INFO& operator=(const IMG_ROI_INFO& other)
	{
		if (&other == this)
			return *this;
		m_eType = other.m_eType;
		m_arrROIInfo = other.m_arrROIInfo;
		return *this;
	}
	IMG_ROI_INFO& operator=(IMG_ROI_INFO&& other)
	{
		if (&other == this)
			return *this;
		m_eType = other.m_eType;
		m_arrROIInfo = other.m_arrROIInfo;
		return *this;
	}
};
