#pragma once

#include <string>

#ifdef UNICODE
    #define tstring std::wstring
#else
    #define tstring std::string
#endif // UNICODE

#ifdef UNICODE

inline std::string ToString(tstring const& str)
{
    std::string resultString;

    // convert

    return resultString;
}

#else

inline std::string ToString(tstring const& str)
{
    return str;
}

#endif // UNICODE

//std::string ws2s(const std::wstring& wstr, const std::locale& loc)
//{
//    if (wstr.empty())
//        return std::string();
//
//    typedef std::wstring::traits_type::state_type state_type;
//    typedef std::codecvt<wchar_t, char, state_type> convert;
//
//    const convert& cvt = std::use_facet<convert>(loc);
//    std::string str(cvt.max_length()*wstr.size(), '\0');
//    state_type state = state_type();
//
//    const wchar_t* from_beg = &wstr[0];
//    const wchar_t* from_end = from_beg + wstr.size();
//    const wchar_t* from_nxt;
//    char* to_beg = &str[0];
//    char* to_end = to_beg + str.size();
//    char* to_nxt;
//
//    std::string::size_type sz = 0;
//    std::codecvt_base::result r;
//    do
//    {
//        r = cvt.out(state, from_beg, from_end, from_nxt,
//            to_beg, to_end, to_nxt);
//        switch (r)
//        {
//        case std::codecvt_base::error:
//            throw std::runtime_error("error converting wstring to string");
//
//        case std::codecvt_base::partial:
//            sz += to_nxt - to_beg;
//            str.resize(2 * str.size());
//            to_beg = &str[sz];
//            to_end = &str[0] + str.size();
//            break;
//
//        case std::codecvt_base::noconv:
//            str.resize(sz + (from_end - from_beg) * sizeof(wchar_t));
//            std::memcpy(&str[sz], from_beg, (from_end - from_beg) * sizeof(wchar_t));
//            r = std::codecvt_base::ok;
//            break;
//
//        case std::codecvt_base::ok:
//            sz += to_nxt - to_beg;
//            str.resize(sz);
//            break;
//        }
//    } while (r != std::codecvt_base::ok);
//
//    return str;
//}


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