//
// (C) Copyright by Victor Derks
//
// See README.TXT for the details of the software licence.
//
#pragma once


#include "cfeffect.h"


namespace MSF
{

class CCfTargetCLSID
{
public:

    static void Get(IDataObject* pdataobject, CLSID& clsid)
    {
        RaiseExceptionIfFailed(GetImpl(pdataobject, clsid));
    }


    static bool GetOptional(IDataObject* pdataobject, CLSID& clsid)
    {
        HRESULT hr = GetImpl(pdataobject, clsid);
        return SUCCEEDED(hr);
    }

private:

    static HRESULT GetImpl(IDataObject* pdataobject, CLSID& clsid)
    {
        CFormatEtc formatetc(CFSTR_TARGETCLSID);
        StorageMedium medium;
        HRESULT hr = pdataobject->GetData(&formatetc, &medium);
        if (FAILED(hr))
            return hr;

        GlobalLock<CLSID> globallock(medium.hGlobal);

        clsid = *globallock.get();
        return S_OK;
    }
};

} // end of MSF namespace
