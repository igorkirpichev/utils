#pragma once

#include "traces_parser_provider.h"
#include "scheme_context.h"
#include "notepad_plus_plus.h"

#include "helpers/string.h"
#include "helpers/non_copyable.h"

#include <afxwin.h>
#include <afxframewndex.h>
#include <afxtoolbarcomboboxbutton.h>

namespace StartProcessButtonState
{

    enum Enum
    {
        Start = 0,
        Cancel
    };

}

struct PluginInfo
{
    HWND npp;
    HWND scintillaMain;
    HWND scintillaSecond;
    tstring name;
};

class PluginFrame :
    public CFrameWndEx,
    public IAnalysisProcessorFrameCallback,
    public NonCopyable
{
public:
    PluginFrame(PluginInfo const& info);
    virtual ~PluginFrame();

protected:
    DECLARE_MESSAGE_MAP()

private:
    // implement IAnalysisProcessorFrameCallback
    void OnStartProcess();
    void OnFinishProcess();

private:

    afx_msg int OnCreate(LPCREATESTRUCT createStruct);
    afx_msg void OnWindowPosChanged(WINDOWPOS* wndPos);
	
    LRESULT OnUpdateToolbar(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateTracesParserComboBox(CCmdUI *pCmdUI);
    afx_msg void OnUpdateProcessStartButton(CCmdUI *pCmdUI);
    
	afx_msg void OnClose();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
	
	void OnToolbarProcesStart();

    bool SaveModifiedScheme();
    bool SaveScheme();

	void ResetToolbar();
    void UpdateCaption();

private:
    CMFCToolBar                     m_toolbar;
    CMFCToolBarComboBoxButton       m_comboTracesParsers;

    PluginInfo const                m_info;
    TracesParserProvider            m_tracesParserProvider;
    std::unique_ptr<SchemeContext>  m_schemeContext;
    NotepadPlusPlus const           m_notepad;

    StartProcessButtonState::Enum   m_startProcessButtonState;
};