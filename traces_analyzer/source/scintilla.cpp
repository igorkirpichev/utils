#include "scintilla.h"


Scintilla::Scintilla(HWND scintillaHandle)
{
    ASSERT(scintillaHandle);

    m_directCallFn = reinterpret_cast<SciFnDirect>(::SendMessage(scintillaHandle, SCI_GETDIRECTFUNCTION, 0, 0));
    m_sciWndData = static_cast<sptr_t>(::SendMessage(scintillaHandle, SCI_GETDIRECTPOINTER, 0, 0));

    ASSERT(m_directCallFn && m_sciWndData);
}


inline LRESULT Scintilla::DirectCall(UINT messageId, WPARAM wParam, LPARAM lParam) const
{
    return m_directCallFn(
        m_sciWndData,
        static_cast<sptr_t>(messageId),
        static_cast<uptr_t>(wParam), 
        static_cast<sptr_t>(lParam));
}

void Scintilla::SetReadOnly(bool readonly) const
{
	DirectCall(SCI_SETREADONLY, static_cast<WPARAM>(readonly));
}

size_t Scintilla::FindText(int searchFlags, Sci_TextToFind const& textToFind) const
{
	return static_cast<size_t>(DirectCall(SCI_FINDTEXT, static_cast<WPARAM>(searchFlags), reinterpret_cast<LPARAM>(&textToFind)));
}

size_t Scintilla::LineFromPosition(size_t position) const
{
	return static_cast<size_t>(DirectCall(SCI_LINEFROMPOSITION, static_cast<WPARAM>(position)));
}

size_t Scintilla::LineLength(size_t line) const
{
	return static_cast<size_t>(DirectCall(SCI_LINELENGTH, static_cast<WPARAM>(line)));
}

std::string Scintilla::GetLine(size_t line) const
{
	size_t const lineLength = LineLength(line);

	std::string lineString(lineLength, 0);

	if (DirectCall(SCI_GETLINE, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(lineString.data())))
		return lineString;
	else
		return std::string();
}

size_t Scintilla::GetLineEndPosition(size_t line) const
{
	return static_cast<size_t>(DirectCall(SCI_GETLINEENDPOSITION, static_cast<WPARAM>(line)));
}

size_t Scintilla::GetLength() const
{
	return static_cast<size_t>(DirectCall(SCI_GETLENGTH));
}

size_t Scintilla::GetLineCount() const
{
	return static_cast<size_t>(DirectCall(SCI_GETLINECOUNT));
}
