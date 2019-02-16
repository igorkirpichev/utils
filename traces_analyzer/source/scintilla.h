#pragma once

#include "npp/scintilla.h"

#include "afx.h"
#include "windows.h"

class Scintilla
{
public:
    Scintilla(HWND scintillaHandle);

public:
    LRESULT DirectCall(UINT messageId, WPARAM wParam = 0, LPARAM lParam = 0) const;

private:
    SciFnDirect m_directCallFn;
    sptr_t      m_sciWndData;

};

