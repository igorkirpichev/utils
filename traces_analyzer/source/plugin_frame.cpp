#include "resource.h"
#include "plugin_frame.h"
#include "registry.h"

#include "afxvisualmanagerofficexp.h"

#include "helpers/check.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PluginFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_WINDOWPOSCHANGED()
    ON_COMMAND(ID_FILE_NEW,     &PluginFrame::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN,    &PluginFrame::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE,    &PluginFrame::OnFileSave)
    ON_COMMAND(ID_FILE_CLOSE,   &PluginFrame::OnFileClose)
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
    
    WIN_CHECK(Create(NULL, m_info.name.c_str(), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, windowRect, NULL,
        MAKEINTRESOURCE(IDR_MAINFRAME)));
}

PluginFrame::~PluginFrame()
{
    //CMFCVisualManager::DestroyInstance();
}

int PluginFrame::OnCreate(LPCREATESTRUCT createStruct)
{
    if (CFrameWndEx::OnCreate(createStruct) == -1)
        return -1;

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
    RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    HICON const smallIcon   = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICONSM));
    HICON const bigIcon     = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICON));
    ASSERT(smallIcon && bigIcon);
    
    SetIcon(smallIcon, false);
    SetIcon(bigIcon, true);

    ShowWindow(SW_SHOW);

    return 0;
}

void PluginFrame::OnDestroy()
{

}

void PluginFrame::OnClose()
{

    CFrameWndEx::OnClose();
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

void PluginFrame::OnFileNew()
{
    if (CloseScheme())
        m_scheme.reset(new Scheme());
}

void PluginFrame::OnFileOpen()
{
    CFileDialog dialog(TRUE, TEXT("Open analysis scheme"), TEXT(".xml")/*SCHEME_FILE_EXTENSTION*/, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT/*, TEXT(".xml")*//*SCHEME_FILE_EXTENSTION*/);
    if (dialog.DoModal() == IDOK)
    {
        if (IsSchemeLoaded())
            CloseScheme();

        LoadScheme(dialog.GetPathName().GetBuffer());
    }
    else
        DestroyWindow();
}

void PluginFrame::OnFileSave()
{
    SaveScheme();
}

void PluginFrame::OnFileClose()
{
    if (CloseScheme())
        DestroyWindow();
}

void PluginFrame::LoadScheme(tstring const& schemeFilePath)
{
    ASSERT(!schemeFilePath.empty());


}

bool PluginFrame::SaveScheme()
{
    if (m_scheme)
    {
        tstring const fileName = m_scheme->GetFileName();

        bool result = false;
        if (fileName.empty())
        {
            CFileDialog dialog(
                FALSE,
                SCHEME_FILE_EXTENSTION,
                SCHEME_DEFAULT_FILE_NAME,
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                SCHEME_DEFAULT_FILE_NAME_FILTER);

            if (dialog.DoModal() == IDOK)
                result = m_scheme->Save(dialog.GetPathName().GetBuffer());
            else
                return false;
        }
        else
            result = m_scheme->Save();

        if (!result)
            MessageBox(TEXT("Failed to save analyzes scheme"), m_info.name.c_str(), MB_OK | MB_ICONERROR);

        return result;
    }
    else
    {
        return false;
    }
}

bool PluginFrame::CloseScheme()
{
    if (m_scheme)
    {
        if (m_scheme->IsModified())
        {
            tstring const displayFileName = m_scheme->GetDisplayFileName();

            tstring messageText(TEXT("You want to save changes to the file \""));
            messageText += displayFileName.empty() ? SCHEME_DEFAULT_FILE_NAME : displayFileName;
            messageText += TEXT("\"?");

            switch (MessageBox(messageText.c_str(), m_info.name.c_str(), MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1))
            {
                case IDYES:
                {
                    if (!SaveScheme())
                        return false;

                    break;
                }

                case IDNO:
                {
                    break;
                }

                case IDCANCEL:
                default:
                {
                    return false;
                }
            }
        }

        m_scheme.reset(nullptr);
    }
    
    return true;
}

bool PluginFrame::IsSchemeLoaded() const
{
    return false;//!!m_scheme;
}