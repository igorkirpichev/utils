#include "scheme_context.h"
#include "plugin_frame.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(ResultDockablePane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int ResultDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    CRect clientRect;
    GetClientRect(&clientRect);

    if (!m_list.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL
        | LVS_REPORT, clientRect, this, 0))
    {
        return -1;
    }

    m_list.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    return 0;
}

void ResultDockablePane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    CRect clientRect;
    GetClientRect(&clientRect);

    m_list.SetWindowPos(NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

ViewPanel::ViewPanel(PluginFrame* parentWnd, SchemeTemplate* schemeTemplate) :
    m_parentWnd(parentWnd)
{
    ASSERT(m_parentWnd && schemeTemplate);

    tstring const caption = schemeTemplate->GetName();

    BOOL const result = m_pane.Create(
        caption.c_str(), m_parentWnd, CRect(0, 0, 200, 200), TRUE, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI);

    if (result)
    {
        m_pane.EnableDocking(CBRS_ALIGN_ANY);
        m_parentWnd->DockPane(&m_pane);
    }
    else
    {
        ::MessageBox(m_parentWnd->GetSafeHwnd(), TEXT("Failed to create view panel"), caption.c_str(), MB_OK | MB_ICONERROR);
    }
}

ViewPanel::~ViewPanel()
{
    m_pane.UndockPane();
    m_pane.DestroyWindow();
}

SchemeContext::SchemeContext(PluginFrame* parentWnd) :
    m_parentWnd(parentWnd)
{
    ASSERT(m_parentWnd);
}

bool SchemeContext::Create(tstring const& filePath)
{
    bool const result = m_scheme.Load(filePath);
    if (result)
        UpdateViewPanels();

    return result;
}

Scheme& SchemeContext::GetScheme()
{
    return m_scheme;
}

void SchemeContext::UpdateViewPanels()
{
    m_viewPanels.clear();

    for (size_t i = 0; i < m_scheme.GetCountSchemeTemplates(); ++i)
    {
        m_viewPanels.emplace_back(new ViewPanel(m_parentWnd, m_scheme.GetSchemeTemplate(i)));
    }
}