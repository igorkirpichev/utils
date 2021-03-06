#pragma once

#include <string>

#define LOCALE "Russian"

#ifdef UNICODE
    #define tstring std::wstring
#else
    #define tstring std::string
#endif // UNICODE

#ifdef UNICODE
    #define ToString        WStringToString
    #define ToTString       StringToWString
#else
    #define ToString(str)   str
    #define ToTString(str)  str
#endif // UNICODE

template <typename StringType>
inline StringType& RemoveCarriageReturns(StringType& str)
{
    if (str.back() == '\n')
        str.pop_back();
    if (str.back() == '\r')
        str.pop_back();

    return str;
}

inline std::wstring StringToWString(std::string const& str)
{
    if (str.empty())
        return std::wstring();

    typedef std::string::traits_type::state_type state_type;
    typedef std::codecvt<wchar_t, char, state_type> convert;

    std::locale const locale(LOCALE);
    const convert& cvt = std::use_facet<convert>(locale);
    std::wstring resultString(str.size(), '\0');
    state_type state = state_type();
    const char* from_beg = &str[0];
    const char* from_end = from_beg + str.size();
    const char* from_nxt;
    wchar_t* to_beg = &resultString[0];
    wchar_t* to_end = to_beg + resultString.size();
    wchar_t* to_nxt;
    std::wstring::size_type sz = 0;
    std::codecvt_base::result r;

    do
    {
        r = cvt.in(state, from_beg, from_end, from_nxt, to_beg, to_end, to_nxt);
        switch (r)
        {
            case std::codecvt_base::error:
            {
                return std::wstring();
            }

            case std::codecvt_base::partial:
            {
                sz += to_nxt - to_beg;
                resultString.resize(2 * resultString.size());
                to_beg = &resultString[sz];
                to_end = &resultString[0] + resultString.size();
                break;
            }

            case std::codecvt_base::noconv:
            {
                resultString.resize(sz + (from_end - from_beg));
                std::memcpy(&resultString[sz], from_beg, (std::size_t)(from_end - from_beg));
                r = std::codecvt_base::ok;
                break;
            }

            case std::codecvt_base::ok:
            {
                sz += to_nxt - to_beg;
                resultString.resize(sz);
                break;
            }
        }
    }
    while (r != std::codecvt_base::ok);

    return resultString;
}

inline std::string WStringToString(std::wstring const& str)
{
    if (str.empty())
        return std::string();
        
    typedef std::wstring::traits_type::state_type   state_type;
    typedef std::codecvt<wchar_t, char, state_type> convert;
        
    std::locale const locale(LOCALE);
    convert const& cvt = std::use_facet<convert>(locale);
    std::string resultString(cvt.max_length() * str.size(), '\0');
    state_type state = state_type();
        
    wchar_t const* from_beg = str.data();
    wchar_t const* from_end = from_beg + str.size();
    wchar_t const* from_nxt;
    char* to_beg = &resultString[0];
    char* to_end = to_beg + resultString.size();
    char* to_nxt;
        
    std::string::size_type      sz = 0;
    std::codecvt_base::result   result;
    do
    {
        result = cvt.out(state, from_beg, from_end, from_nxt, to_beg, to_end, to_nxt);
        switch (result)
        {
            case std::codecvt_base::error:
            {
                return std::string();
            }

            case std::codecvt_base::partial:
            {
                sz += to_nxt - to_beg;
                resultString.resize(2 * resultString.size());
                to_beg = &resultString[sz];
                to_end = &resultString[0] + resultString.size();
                break;
            }

            case std::codecvt_base::noconv:
            {
                resultString.resize(sz + (from_end - from_beg) * sizeof(wchar_t));
                std::memcpy(&resultString[sz], from_beg, (from_end - from_beg) * sizeof(wchar_t));
                result = std::codecvt_base::ok;
                break;
            }

            case std::codecvt_base::ok:
            {
                sz += to_nxt - to_beg;
                resultString.resize(sz);
                break;
            }
        }
    }
    while (result != std::codecvt_base::ok);
        
    return resultString;
}