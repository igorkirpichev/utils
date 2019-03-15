#pragma once

#include "helpers/string.h"

#include "afx.h"
#include "windows.h"

class NotepadPlusPlus
{
public:
    NotepadPlusPlus(HWND notepadHandle);

public:
    LRESULT SendMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;

public:
    tstring GetPluginsConfigDir() const;
    tstring GetFullCurrentPath() const;
    int     GetCurrentScintilla() const;
	void	SaveCurrentFile() const;
	void	HideTabBar(bool hide) const;

private:
    HWND const m_notepadHandle;
};

