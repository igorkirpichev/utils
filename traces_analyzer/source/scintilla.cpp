#include "scintilla.h"


Scintilla::Scintilla(HWND scintillaHandle)
{
    ASSERT(scintillaHandle);

    m_directCallFn = reinterpret_cast<SciFnDirect>(::SendMessage(scintillaHandle, SCI_GETDIRECTFUNCTION, 0, 0));
    m_sciWndData = static_cast<sptr_t>(::SendMessage(scintillaHandle, SCI_GETDIRECTPOINTER, 0, 0));

    ASSERT(m_directCallFn && m_sciWndData);
}


LRESULT Scintilla::DirectCall(UINT messageId, WPARAM wParam, LPARAM lParam) const
{
    return m_directCallFn(
        m_sciWndData,
        static_cast<sptr_t>(messageId),
        static_cast<uptr_t>(wParam), 
        static_cast<sptr_t>(lParam));
}

