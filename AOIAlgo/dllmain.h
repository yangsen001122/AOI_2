// dllmain.h: 模块类的声明。

class CAOIAlgoModule : public ATL::CAtlDllModuleT< CAOIAlgoModule >
{
public :
	DECLARE_LIBID(LIBID_AOIAlgoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MURAALGO, "{C8F983AD-36E0-4640-B961-AF9083039A12}")
};

extern class CAOIAlgoModule _AtlModule;
