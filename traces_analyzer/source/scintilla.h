#pragma once

#include "helpers/string.h"

#include "npp/scintilla.h"

#include "afx.h"
#include "windows.h"


class Scintilla
{
public:
    Scintilla(HWND scintillaHandle);

public:
    inline LRESULT DirectCall(UINT messageId, WPARAM wParam = 0, LPARAM lParam = 0) const;

public:
	void		SetReadOnly(bool readonly) const;
	
	size_t		FindText(int searchFlags, Sci_TextToFind const& textToFind) const;
	
	size_t		LineFromPosition(size_t position) const;
	size_t		LineLength(size_t line) const;
	std::string GetLine(size_t line) const;
	size_t		GetLineEndPosition(size_t line) const;
	size_t		GetLength() const;
	size_t		GetLineCount() const; 

private:
    SciFnDirect m_directCallFn;
    sptr_t      m_sciWndData;

};

