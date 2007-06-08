//
// (C) Copyright by Victor Derks
//
// See README.TXT for the details of the software licence.
//
#include "stdafx.h"
#include "resource.h"
#include "vvvfile.h"
#include "columnproviderclsid.h"
#include "../include/columnproviderimpl.h"
#include "../include/strutil.h"
#include <shlobj.h>
#include <msidefs.h> // for PID_AUTHOR

class ATL_NO_VTABLE CColumnProvider :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColumnProvider, &__uuidof(CColumnProvider)>,
	public IColumnProviderImpl<CColumnProvider>
{
public:
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister) throw()
	{
		return IColumnProviderImpl<CColumnProvider>::UpdateRegistry(
			IDR_COLUMNPROVIDER, bRegister, L"Sample ShellExtension ColumnProvider");
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CColumnProvider)
		COM_INTERFACE_ENTRY(IColumnProvider)
	END_COM_MAP()


	void OnInitialize(PCWSTR /*wszFolder*/)
	{
		// Register the supported columns.
		RegisterColumn(IDS_SHELLEXT_LABEL, 9);
		RegisterColumn(IDS_SHELLEXT_FILECOUNT, 14, LVCFMT_RIGHT, SHCOLSTATE_TYPE_INT);

		GUID guidSummaryInformation = PSGUID_SUMMARYINFORMATION;
		RegisterColumn(guidSummaryInformation, PID_AUTHOR, L"Author", 
			14, LVCFMT_RIGHT, SHCOLSTATE_TYPE_STR | SHCOLSTATE_SLOW);

		RegisterExtension(wszVVVExtension);
		// .. this would be the place to add other extensions this columnprovider also supports.
	}


	// Purpose: this function is called by MSF when it needs column info for a file.
	void GetAllColumnInfo(const CString& strFilename, vector<CString>& strColumnInfos)
	{
		CVVVFile vvvfile(strFilename);

		strColumnInfos.push_back(vvvfile.GetLabel());
		strColumnInfos.push_back(MSF::ToString(vvvfile.GetFileCount()));
		strColumnInfos.push_back(_T("author"));
	}
};


OBJECT_ENTRY_AUTO(__uuidof(CColumnProvider), CColumnProvider)
