#pragma once
#include <locale>

namespace string_transform
{

constexpr wchar_t SeparatorChar = '_';

inline void SetSeparator(std::wstring& str)
{
    if (!str.empty() && (str.back() != SeparatorChar))
    {
        str.push_back(SeparatorChar);
    }
}

inline void RemoveSeparator(std::wstring& str)
{
    if (!str.empty() && (str.back() == SeparatorChar))
    {
        str.erase(--str.end());
    }
}

inline std::wstring CyrillicTranslit(const std::wstring& str)
{
    const std::wstring rus = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЭЮЯабвгдеёжзийклмнопрстуфхцчшщьыъэюя";
    const std::wstring lat[] = { L"A", L"B", L"V", L"G", L"D", L"E", L"E", L"Zh", L"Z", L"I", L"I", L"K",
        L"L", L"M", L"N", L"O", L"P", L"R", L"S", L"T", L"U", L"F", L"H", L"C", L"Ch", L"Sh", L"Sh'", L"E",
        L"Yu", L"Ya", L"a", L"b", L"v", L"g", L"d", L"e", L"e", L"zh", L"z", L"i", L"i", L"k", L"l", L"m", L"n",
        L"o", L"p", L"r", L"s", L"t", L"u", L"f", L"h", L"c", L"ch", L"sh", L"sh'", L"", L"i", L"", L"e", L"yu",
        L"ya" };
    
    std::wstring translitSource;
    for (wchar_t ch : str)
    {
        const size_t position = rus.find(ch);
        if (position != std::string::npos)
        {
            translitSource += lat[position];
        }
        else
        {
            translitSource += ch;
        }
    }

    return translitSource;
}

inline std::wstring TransformString(const std::wstring& str)
{
    std::wstring destinationStr;

    std::wstring sourceStr(CyrillicTranslit(str));

    wchar_t prevCh = 0;
    for (wchar_t ch : sourceStr)
    {
        if (std::isalpha(ch))
        {
            if (prevCh && std::islower(prevCh) && std::isupper(ch))
            {
                SetSeparator(destinationStr);
            }

            destinationStr.push_back(std::tolower(ch));
        }
        else if (std::isdigit(ch))
        {
            destinationStr.push_back(ch);
        }
        else
        {
            SetSeparator(destinationStr);
        }

        prevCh = ch;
    }

    return destinationStr;
}

inline void ReplaceSubstring(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    const auto itFind = str.find(from);
    if (itFind != std::wstring::npos)
    {
        str.replace(itFind, from.length(), to);
    }
}

inline bool ShrinkShort(std::wstring& str)
{
    const auto itFind1 = str.find(SeparatorChar);
    if (itFind1 != std::wstring::npos)
    {
        const auto itFind2 = str.find(SeparatorChar, itFind1 + 1);
        if (itFind2 != std::wstring::npos)
        {
            str.erase(itFind2);
            return true;
        }
    }
    return false;
}

inline bool ShrinkN(std::wstring& str, const size_t toPos)
{
    if (toPos < str.size())
    {
        str.erase(toPos);
        return true;
    }
    return false;
}

} // string_transform

