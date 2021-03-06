//
// (C) Copyright by Victor Derks
//
// See README.TXT for the details of the software licence.
//
#pragma once

#include "msfbase.h"
#include "updateregistry.h"


namespace MSF
{

/// <summary>Basic implementation functionality for a Drop Target Handler shell extension COM object.</summary>
/// <remarks>
/// Classes can use this mix-in implementation class to add support to act as an Drop Target Handler COM object.
/// Standard ATL template classes can be used for the generic COM implementation (addref, release, etc).
/// The actual Drop Target Handler class need to setup the COM interface map.
/// The following interfaces should be enabled:
///  - IPersistsFile
///  - IDropTarget
/// </remarks>
template <typename T>
class __declspec(novtable) DropTargetImpl :
    public IPersistFile,
    public IDropTarget
{
public:
    /// <summary>Registration function to register the infotip COM object and a ProgId/extension.</summary>
    static HRESULT WINAPI UpdateRegistry(BOOL bRegister, UINT nResId,
        PCWSTR szDescription, PCWSTR szRootKey) noexcept
    {
        return UpdateRegistryFromResource(nResId, bRegister,
            szDescription, T::GetObjectCLSID(), szRootKey);
    }

    // IPersistFile
    HRESULT __stdcall GetClassID(__RPC__out CLSID* pClassID) noexcept override
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::GetClassID (instance=%p, pClassID=%p)\n", this, pClassID);

        if (!pClassID)
            return E_POINTER;

        *pClassID = T::GetObjectCLSID();
        return S_OK;
    }

    HRESULT __stdcall IsDirty() noexcept override
    {
        ATLTRACENOTIMPL(L"DropTargetImpl::IsDirty");
    }

    HRESULT __stdcall Save(LPCOLESTR, BOOL) noexcept override
    {
        ATLTRACENOTIMPL(L"DropTargetImpl::Save");
    }

    HRESULT __stdcall SaveCompleted(LPCOLESTR) noexcept override
    {
        ATLTRACENOTIMPL(L"DropTargetImpl::SaveCompleted");
    }

    HRESULT __stdcall GetCurFile(LPOLESTR*) noexcept override
    {
        ATLTRACENOTIMPL(L"DropTargetImpl::GetCurFile");
    }

    HRESULT __stdcall Load(LPCOLESTR wszFilename, DWORD dwMode) noexcept override
    {
        UNREFERENCED_PARAMETER(dwMode); // unused in release.

        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::Load (instance=%p, mode=%d, filename=%s)\n", this, dwMode, wszFilename);
        try
        {
            _filename = wszFilename;
            return S_OK;
        }
        catch (...)
        {
            return ExceptionToHResult();
        }
    }

    // IDropTarget
    HRESULT __stdcall DragEnter(_In_ IDataObject* dataObject, DWORD grfKeyState, POINTL pt, _In_ DWORD* pdwEffect) noexcept override
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::IDropTarget::DragEnter (instance=%p, grfKeyState=%d, dwEffect=%d)\n", this, grfKeyState, *pdwEffect);

        try
        {
            // Derived class needs to implement: bool IsSupportedClipboardFormat(IDataObject* dataObject)
            if (dataObject == nullptr || !static_cast<T*>(this)->IsSupportedClipboardFormat(dataObject))
            {
                *pdwEffect = DROPEFFECT_NONE;
                _isCachedSupportedClipboardFormat = false;
            }
            else
            {
                *pdwEffect = static_cast<T*>(this)->OnDragOver(grfKeyState, pt, *pdwEffect);
                _isCachedSupportedClipboardFormat = true;
            }

            return S_OK;
        }
        catch (...)
        {
            return ExceptionToHResult();
        }
    }

    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, _In_ DWORD* pdwEffect) noexcept override
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::IDropTarget::DragOver (instance=%p, grfKeyState=%d, dwEffect=%d)\n", this, grfKeyState, *pdwEffect);

        try
        {
            if (_isCachedSupportedClipboardFormat)
            {
                // Derived class needs to implement: DWORD OnDragOver(DWORD grfKeyState, POINTL pt, DWORD dwEffect)
                *pdwEffect = static_cast<T*>(this)->OnDragOver(grfKeyState, pt, *pdwEffect);
            }
            else
            {
                *pdwEffect = DROPEFFECT_NONE;
            }
            return S_OK;
        }
        catch (...)
        {
            return ExceptionToHResult();
        }
    }

    HRESULT __stdcall DragLeave() noexcept override
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::IDropTarget::DragLeave (instance=%p)\n", this);
        _isCachedSupportedClipboardFormat = false;
        return S_OK;
    }

    HRESULT __stdcall Drop(_In_ IDataObject* dataObject, DWORD grfKeyState, POINTL pt, _In_ DWORD* pdwEffect) noexcept override
    {
        UNREFERENCED_PARAMETER(grfKeyState);
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::IDropTarget::Drop (instance=%p, grfKeyState=%d, dwEffect=%d)\n", this, grfKeyState, *pdwEffect);

        try
        {
            // Derived class needs to implement: DWORD OnDrop(IDataObject* dataObject, DWORD grfKeyState, POINTL pt, DWORD dwEffect)
            *pdwEffect = static_cast<T*>(this)->OnDrop(dataObject, grfKeyState, pt, *pdwEffect);
            return S_OK;
        }
        catch (...)
        {
            return ExceptionToHResult();
        }
    }

protected:

    DropTargetImpl() :
        _isCachedSupportedClipboardFormat(false)
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::DropTargetImpl (instance=%p)\n", this);
    }

    ~DropTargetImpl()
    {
        ATLTRACE2(atlTraceCOM, 0, L"DropTargetImpl::~DropTargetImpl (instance=%p)\n", this);
    }

private:
    ATL::CString _filename;
    bool _isCachedSupportedClipboardFormat;
};

} // namespace MSF
