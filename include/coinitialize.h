//
// (C) Copyright by Victor Derks
//
// See README.TXT for the details of the software licence.
//
#pragma once

#if (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400))
#error WIN32_NT must be at least version 4.0 (0x400) to access CoInitaliseEx
#endif

#include <objbase.h>

namespace MSF
{

/// <summary>Scope based COM initialization class (based on RAII pattern).</summary>
class CCoInitialize
{
public:
    CCoInitialize()
    {
        ATLVERIFY(SUCCEEDED(::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));
    }

    ~CCoInitialize() throw()
    {
        ::CoUninitialize();
    }
};

} // end namespace MSF.
