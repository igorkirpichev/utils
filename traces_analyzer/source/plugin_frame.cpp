#include "plugin_frame.h"
#include "registry.h"

#include "helpers/check.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PluginFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

PluginFrame::PluginFrame(PluginInfo const& info) :
    m_info(info)
{
    WIN_CHECK(Create(NULL, m_info.name.data()));

}

PluginFrame::~PluginFrame()
{
}

BOOL PluginFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWndEx::PreCreateWindow(cs))
        return FALSE;

    CWinApp* pApp = AfxGetApp();
    cs.x    = pApp->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_LEFT,   CW_USEDEFAULT);
    cs.y    = pApp->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_TOP,    CW_USEDEFAULT);
    cs.cx   = pApp->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_WIDTH,  CW_USEDEFAULT);
    cs.cy   = pApp->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_HEIGHT, CW_USEDEFAULT);

    return TRUE;
}

int PluginFrame::OnCreate(LPCREATESTRUCT createStruct)
{
    if (CFrameWndEx::OnCreate(createStruct) == -1)
        return -1;

    HICON const smallIcon   = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICONSM));
    HICON const bigIcon     = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICON));
    ASSERT(smallIcon && bigIcon);
    
    SetIcon(smallIcon, false);
    SetIcon(bigIcon, true);

    return 0;
}

void PluginFrame::OnWindowPosChanged(WINDOWPOS* wndPos)
{
    CFrameWndEx::OnWindowPosChanged(wndPos);

    CWinApp* pApp = AfxGetApp();
    pApp->WriteProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_LEFT,   wndPos->x);
    pApp->WriteProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_TOP,    wndPos->y);
    pApp->WriteProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_WIDTH,  wndPos->cx);
    pApp->WriteProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_HEIGHT, wndPos->cy);
}