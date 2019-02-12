#pragma once

#include "traces_parser_provider.h"
#include "scheme_context.h"

#include "helpers/string.h"
#include "helpers/non_copyable.h"

#include <afxwin.h>
#include <afxframewndex.h>
#include <afxtoolbarcomboboxbutton.h>

struct PluginInfo
{
    HWND npp;
    HWND scintillaMain;
    HWND scintillaSecond;
    tstring name;
};

class PluginFrame :
    public NonCopyable,
    public CFrameWndEx
{
public:
    PluginFrame(PluginInfo const& info);
    virtual ~PluginFrame();

protected:
    DECLARE_MESSAGE_MAP()

private:
    afx_msg int OnCreate(LPCREATESTRUCT createStruct);
    afx_msg void OnWindowPosChanged(WINDOWPOS* wndPos);
	afx_msg void OnUpdateTracesParserComboBox(CCmdUI *pCmdUI);
    
	afx_msg void OnClose();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
	
	LRESULT OnToolbarReset(WPARAM wp, LPARAM);
	void OnToolbarMenuButtonClicked();

    bool SaveModifiedScheme();
    bool SaveScheme();

private:
    CMFCToolBar                     m_toolbar;
    CMFCToolBarComboBoxButton       m_comboTracesParsers;

    PluginInfo const                m_info;
    TracesParserProvider            m_tracesParserProvider;
    std::unique_ptr<SchemeContext>  m_schemeContext;
};