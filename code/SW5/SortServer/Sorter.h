// Sorter.h : Declaration of the CSorter

#pragma once
#include "resource.h"       // main symbols



#include "SortServer_i.h"



using namespace ATL;


// CSorter

class ATL_NO_VTABLE CSorter :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CSorter, &CLSID_Sorter>,
	public IDispatchImpl<ISorter, &IID_ISorter, &LIBID_SortServerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	bool m_busy;
	bool m_canceled;
public:
	CSorter()
	{
		m_busy = false;
		m_canceled = false;
	}

DECLARE_REGISTRY_RESOURCEID(106)


BEGIN_COM_MAP(CSorter)
	COM_INTERFACE_ENTRY(ISorter)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(SortFile)(BSTR input, BSTR output, BYTE mode);
	STDMETHOD(Cancel)();
};

OBJECT_ENTRY_AUTO(__uuidof(Sorter), CSorter)
