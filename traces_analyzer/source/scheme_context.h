#pragma once

#include "scheme.h"
#include "scintilla.h"
#include "traces_parser_provider.h"
#include "analysis_processor.h"

#include "helpers/string.h"

#include <afxdockablepane.h>
#include <afxlistctrl.h>

#include <vector>

class PluginFrame;

class ResultDockablePane :
    public CDockablePane
{
        
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()

private:
    CMFCListCtrl m_list;
};

class ViewPanel
{
public:
    ViewPanel(PluginFrame* parentWnd, SchemeTemplate* schemeTemplate);
    ~ViewPanel();
    
private:
    PluginFrame*        m_parentWnd;
    ResultDockablePane  m_pane;
};

class SchemeContext
{
public:
    SchemeContext(PluginFrame* parentWnd);

private:
    typedef std::vector<std::unique_ptr<ViewPanel>> ViewPanels;

public:
    bool Create(tstring const& filePath);
    Scheme& GetScheme();

    bool StartAnalysis(
        Scintilla const& scintillaView,
        TracesParser const& tracesParser,
        IAnalysisProcessorFrameCallback* frameCallback);

private:
    void UpdateViewPanels();

private:
    PluginFrame*        m_parentWnd;

    AnalysisProcessor   m_analysisProcessor;

    Scheme              m_scheme;
    ViewPanels          m_viewPanels;
};