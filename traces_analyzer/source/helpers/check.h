#pragma once

#include <windows.h>
#include <exception>

#define WIN_CHECK_RESULT(resultCode) \
    if (resultCode == ERROR_SUCCESS) \
    { \
        char errorMessage[22] = { 0 }; \
        sprintf_s(errorMessage, "Win error: 0x%08x", resultCode); \
        throw std::exception(errorMessage); \
    }

#define WIN_CHECK(booleanExpr) \
    if (!booleanExpr) \
        WIN_CHECK_RESULT(::GetLastError()); \
