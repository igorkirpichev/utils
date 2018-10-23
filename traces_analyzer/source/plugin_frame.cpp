#include "resource.h"
#include "plugin_frame.h"
#include "registry.h"

#include "helpers/check.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PluginFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_WINDOWPOSCHANGED()
    ON_COMMAND(ID_FILE_OPEN, &PluginFrame::OnFileOpen)
END_MESSAGE_MAP()

PluginFrame::PluginFrame(PluginInfo const& info) :
    m_info(info)
{
    CWinApp* const application = AfxGetApp();
    CRect windowRect(0, 0,
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_WIDTH, CW_USEDEFAULT)),
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_HEIGHT, CW_USEDEFAULT)));
    windowRect.OffsetRect(
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_LEFT, CW_USEDEFAULT)),
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_TOP, CW_USEDEFAULT)));
    
    WIN_CHECK(Create(NULL, m_info.name.data(), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, windowRect, NULL,
        MAKEINTRESOURCE(IDR_MAINFRAME)));
}

PluginFrame::~PluginFrame()
{
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

void PluginFrame::OnFileOpen()
{
    CFileDialog dialog(TRUE, TEXT("Open analysis scheme"), TEXT(".xml")/*SCHEME_FILE_EXTENSTION*/, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT("*.xml")/*SCHEME_FILE_EXTENSTION*/);
    if (dialog.DoModal() == IDOK)
    {
        if (IsSchemeLoaded())
            CloseScheme();

        LoadScheme(dialog.GetPathName().GetBuffer());
    }
}

void PluginFrame::LoadScheme(std::string const& schemeFilePath)
{
    ASSERT(!schemeFilePath.empty());


}

void PluginFrame::CloseScheme()
{

}

bool PluginFrame::IsSchemeLoaded() const
{
    return false;//!!m_scheme;
}