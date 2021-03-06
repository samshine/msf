//
// (C) Copyright by Victor Derks
//
// See README.TXT for the details of the software licence.
//

#include "stdafx.h"

#include "shellfolderclsid.h"
#include "propertypagevvv.h"
#include "resource.h"

using std::wstring;
using std::vector;


class __declspec(novtable) __declspec(uuid("E53D1A20-D87F-42ad-A6CD-F2E155CAAADC")) ShellPropSheetExt :
    public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
    public ATL::CComCoClass<ShellPropSheetExt, &__uuidof(ShellPropSheetExt)>,
    public MSF::ShellPropSheetExtImpl<ShellPropSheetExt>
{
public:
    BEGIN_COM_MAP(ShellPropSheetExt)
        COM_INTERFACE_ENTRY(IShellExtInit)
        COM_INTERFACE_ENTRY(IShellPropSheetExt)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    static HRESULT WINAPI UpdateRegistry(BOOL registerInRegistry) noexcept
    {
        return ShellPropSheetExtImpl<ShellPropSheetExt>::UpdateRegistry(registerInRegistry, IDR_PROPERTYSHEETEXT,
            L"VVV Sample Property Sheet ShellExtension", wszVVVFileRootExt);
    }

    // Purpose: called by MSF when it is time to add our pages to the property sheet.
    void AddPagesCore(const CAddPage& addPage, const vector<wstring>& filenames) override
    {
        // Only add the page if only 1 file is selected and is of our own extension.
        if (filenames.size() != 1 || ContainsUnknownExtension(filenames))
            return;

        addPage(PropertyPageVVV::CreateInstance(filenames.front()));
    }

protected:

    ShellPropSheetExt()
    {
        RegisterExtension(tszVVVExtension);
    }
};


OBJECT_ENTRY_AUTO(__uuidof(ShellPropSheetExt), ShellPropSheetExt)
