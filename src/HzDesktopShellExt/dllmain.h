// dllmain.h: 模块类的声明。

class CHzDesktopShellExtModule : public ATL::CAtlDllModuleT< CHzDesktopShellExtModule >
{
public :
	DECLARE_LIBID(LIBID_HzDesktopShellExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HZDESKTOPSHELLEXT, "{c56958dd-a53f-4818-9307-bc345c50e9af}")
};

extern class CHzDesktopShellExtModule _AtlModule;
