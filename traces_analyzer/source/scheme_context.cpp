#include "scheme_context.h"
#include "plugin_frame.h"


ViewPanel::ViewPanel(PluginFrame* parentWnd, SchemeTemplate* schemeTemplate) :
    m_parentWnd(parentWnd)
{
    //ASSERT(m_parentWnd && schemeTemplate);

    //BOOL const result = m_pane.Create(
    //    TEXT("dasdsadas"), this, CRect(0, 0, 200, 200), TRUE, 150, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI);

    //m_pane.EnableDocking(CBRS_ALIGN_ANY);
    ////m_parentWnd->DockPane(&m_pane);

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