#pragma once
#include <string>
#include <cctype>
#include <Windows.h>
#include <Shlwapi.h>

namespace clipboard
{

inline void SetClipboard(const std::wstring& str)
{
    if (::OpenClipboard(NULL))
    {
        ::EmptyClipboard();
        size_t const size = (str.size() + 1) * sizeof(wchar_t);
        const HGLOBAL hClipboardData = ::GlobalAlloc(NULL, size);
        if (hClipboardData)
        {
            wchar_t* pchData = (wchar_t*)::GlobalLock(hClipboardData);
            if (pchData)
            {
                memcpy(pchData, (wchar_t*)str.data(), size);
                ::GlobalUnlock(hClipboardData);
                ::SetClipboardData(CF_UNICODETEXT, hClipboardData);
            }
        }
        ::CloseClipboard();
    }
}

inline std::wstring GetClipboard()
{
    std::wstring text;

    if (::OpenClipboard(NULL))
    {
        HANDLE hData = ::GetClipboardData(CF_UNICODETEXT);
        if (hData)
        {
            wchar_t* pszText = static_cast<wchar_t*>(::GlobalLock(hData));
            if (pszText)
            {
                text = std::wstring(pszText);
                ::GlobalUnlock(hData);
            }
        }

        ::CloseClipboard();
    }

    return text;
}

} // clipboard
