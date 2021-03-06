//
// (C) Copyright by Victor Derks
//
// See README.TXT for detailed details of the software licence.
//
#pragma once


#include <msf.h>
#include <vector>


class VVVItem : public MSF::ItemBase
{
public:
    static int GetMaxNameLength(LPCWSTR /*pszName*/) noexcept
    {
        return 10; // note: limit is 10 for easy testing.
    }

    static PUIDLIST_RELATIVE CreateItemIdList(unsigned int nId, unsigned int nSize, bool bFolder, const ATL::CString& strName)
    {
        PUIDLIST_RELATIVE pidl = MSF::ItemIDList::CreateItemIdListWithTerminator(sizeof(SItemData));

        InitializeItemData(reinterpret_cast<SItemData*>(pidl->mkid.abID),
            nId, nSize, bFolder, strName);

        return pidl;
    }

    explicit VVVItem(PCUIDLIST_RELATIVE pidl) : ItemBase(pidl)
    {
        // Shell item IDs can be passed from external sources, validate
        // It is valid to pass a PIDL that is larger then the original (done by Search functionality in XP).
        bool bValid = GetDataSize() >= sizeof(SItemData) && GetItemData().nTypeID == TYPE_ID;
#ifdef _DEBUG
        if (!bValid)
        {
            ATLTRACE2(ATL::atlTraceCOM, 0, L"VVVItem::Constructor, PIDL not valid (dsize=%d, ssize=%d)\n", GetDataSize(), sizeof(SItemData));
        }
#endif
        MSF::RaiseExceptionIf(!bValid);
    }

    std::wstring GetDisplayName(SHGDNF shgdnf = SHGDN_NORMAL) const;

    SFGAOF GetAttributeOf(bool bSingleSelect, bool bReadOnly) const;

    unsigned int GetID() const noexcept
    {
        return GetItemData().nID;
    }

    unsigned int GetSize() const noexcept
    {
        return GetItemData().nSize;
    }

    std::wstring GetName() const
    {
        return GetItemData().wszName;
    }

    bool IsFolder() const noexcept
    {
        return GetItemData().bFolder;
    }

    int Compare(const VVVItem& directoryitem, int nCompareBy, bool bCanonicalOnly) const;
    std::wstring GetItemDetailsOf(UINT iColumn) const;
    std::wstring GetInfoTipText() const;
    int GetIconOf(UINT flags) const;

private:

    // By setting and checking for a TypeId (or cookie) we can ensure that the PIDL
    // was created by us. Using a version # will allows to handle older persisted PIDLs
    const static unsigned int TYPE_ID = 0x5601; // 'V' + version #

    // By using a struct with a version # it becomes possible to detected old persisted PIDLs.
    #pragma pack(1) // By using an explicit pack the memory layout is better fixed then trusting the project settings.
    struct SItemData
    {
        unsigned short nTypeID;
        bool         bFolder;
        unsigned int nID;
        unsigned int nSize;
        wchar_t      wszName[MAX_PATH];
    };
    #pragma pack()

    static void InitializeItemData(SItemData* pitemdata, unsigned int nId, unsigned int nSize, bool bFolder, const ATL::CString& strName)
    {
        pitemdata->nTypeID = TYPE_ID;
        pitemdata->nID     = nId;
        pitemdata->bFolder = bFolder;
        pitemdata->nSize   = nSize;
        wcscpy_s(pitemdata->wszName, _countof(pitemdata->wszName), ATL::CStringW(strName));
    }

    const SItemData& GetItemData() const noexcept
    {
        return *reinterpret_cast<const SItemData*>(GetData());
    }

    int CompareByName(const VVVItem& item) const;
};

using CVVVItemList = std::vector<VVVItem>;
