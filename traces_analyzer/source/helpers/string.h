#pragma once

#include <string>

#define LOCALE "English"

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


inline std::wstring StringToWString(std::string const& str)
{
    std::wstring resultString;

    // convert

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


//std::wstring s2ws(const std::string& str, const std::locale& loc)
//{
//    if (str.empty())
//        return std::wstring();
//
//    typedef std::string::traits_type::state_type state_type;
//    typedef std::codecvt<wchar_t, char, state_type> convert;
//    const convert& cvt = std::use_facet<convert>(loc);
//    std::wstring wstr(str.size(), '\0');
//    state_type state = state_type();
//    const char* from_beg = &str[0];
//    const char* from_end = from_beg + str.size();
//    const char* from_nxt;
//    wchar_t* to_beg = &wstr[0];
//    wchar_t* to_end = to_beg + wstr.size();
//    wchar_t* to_nxt;
//    std::wstring::size_type sz = 0;
//    std::codecvt_base::result r;
//    do
//    {
//        r = cvt.in(state, from_beg, from_end, from_nxt,
//            to_beg, to_end, to_nxt);
//        switch (r)
//        {
//        case std::codecvt_base::error:
//            throw std::runtime_error("error converting string to wstring");
//        case std::codecvt_base::partial:
//            sz += to_nxt - to_beg;
//            wstr.resize(2 * wstr.size());
//            to_beg = &wstr[sz];
//            to_end = &wstr[0] + wstr.size();
//            break;
//        case std::codecvt_base::noconv:
//            wstr.resize(sz + (from_end - from_beg));
//            std::memcpy(&wstr[sz], from_beg, (std::size_t)(from_end - from_beg));
//            r = std::codecvt_base::ok;
//            break;
//        case std::codecvt_base::ok:
//            sz += to_nxt - to_beg;
//            wstr.resize(sz);
//            break;
//        }
//    } while (r != std::codecvt_base::ok);
//
//    return wstr;
//}