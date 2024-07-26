// dllmain.h: 模块类的声明。

class CHzDesktopMenuExtModule : public ATL::CAtlDllModuleT< CHzDesktopMenuExtModule >
{
public :
	DECLARE_LIBID(LIBID_HzDesktopMenuExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HZDESKTOPMENUEXT, "{bb7d1490-abca-412a-a175-26e1c150c3fb}")
};

extern class CHzDesktopMenuExtModule _AtlModule;
