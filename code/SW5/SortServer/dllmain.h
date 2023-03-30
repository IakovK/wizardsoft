// dllmain.h : Declaration of module class.

class CSortServerModule : public ATL::CAtlDllModuleT< CSortServerModule >
{
public :
	DECLARE_LIBID(LIBID_SortServerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SORTSERVER, "{30e4c862-ef57-4400-80ad-7b07ea29fe21}")
};

extern class CSortServerModule _AtlModule;
