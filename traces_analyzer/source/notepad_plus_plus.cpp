#include "notepad_plus_plus.h"

#include "npp/notepad_plus_msgs.h"

NotepadPlusPlus::NotepadPlusPlus(HWND notepadHandle) :
    m_notepadHandle(notepadHandle)
{
    ASSERT(m_notepadHandle);
}

LRESULT NotepadPlusPlus::SendMsg(UINT message, WPARAM wParam, LPARAM lParam) const
{
    ASSERT(m_notepadHandle && message);

    return ::PostMessage(m_notepadHandle, message, wParam, lParam);
}

tstring NotepadPlusPlus::GetPluginsConfigDir() const
{
    TCHAR pluginConfigDir[MAX_PATH] = { 0 };
    ::SendMessage(m_notepadHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(&pluginConfigDir));

    return pluginConfigDir;
}

tstring NotepadPlusPlus::GetFullCurrentPath() const
{
    TCHAR fullCurrentPath[MAX_PATH] = { 0 };
    ::SendMessage(m_notepadHandle, NPPM_GETFULLCURRENTPATH, MAX_PATH, reinterpret_cast<LPARAM>(&fullCurrentPath));
    return fullCurrentPath;
}

int NotepadPlusPlus::GetCurrentScintilla() const
{
    int currentEdit = 0;
    ::SendMessage(m_notepadHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&currentEdit));
    return currentEdit;
}

void NotepadPlusPlus::SaveCurrentFile() const
{
	::SendMessage(m_notepadHandle, NPPM_SAVECURRENTFILE, 0, 0);
}

void NotepadPlusPlus::HideTabBar(bool hide) const
{
	::SendMessage(m_notepadHandle, NPPM_HIDETABBAR, 0, static_cast<LPARAM>(hide));
}