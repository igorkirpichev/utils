#include "resource.h"
#include "plugin_frame.h"
#include "registry.h"

#include "helpers/check.h"

#include "npp/notepad_plus_msgs.h"
#include "npp/scintilla.h"
#include "npp/menu_cmd_id.h"

#include "afxvisualmanagerofficexp.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(PluginFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_WINDOWPOSCHANGED()
    ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
    ON_COMMAND(ID_FILE_NEW,     &PluginFrame::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN,    &PluginFrame::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE,    &PluginFrame::OnFileSave)
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
    
    // Путь потом будем брать из настроек
    TCHAR pluginConfigDir[MAX_PATH] = { 0 };
    LRESULT const result = ::SendMessage(m_info.npp, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)&pluginConfigDir);

    if (!m_tracesParserProvider.Create(tstring(pluginConfigDir)))
        MessageBox(TEXT("Failed to load traces templates"), m_info.name.c_str(), MB_OK | MB_ICONERROR);
    
    // Чисто парсер потестить
    /*{
        TracesParser parser = m_tracesParserProvider.GetParser(0);

        int currentEdit = 0;

        ::SendMessage(m_info.npp, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
        HWND const scintilla = currentEdit ? m_info.scintillaSecond : m_info.scintillaMain;
        ASSERT(scintilla);

        std::vector<tstring> lines;

        LRESULT const lineCount = ::SendMessage(scintilla, SCI_GETLINECOUNT, 0, 0);
        std::vector<char> buffer(1000);
        for (int i = 0; i < lineCount; ++i)
        {
            memset(buffer.data(), 0, buffer.size());
            if (!::SendMessage(scintilla, SCI_GETLINE, i, (LPARAM)buffer.data()))
                continue;

            lines.push_back(ToTString(buffer.data()));
        }

        TraceDescription traceDescription = { 0 };
        tstring::const_iterator traceTextBegin;

        DWORD begin = ::GetTickCount();

        for (tstring const& line : lines)
        {
            parser.Parse(line, traceDescription, traceTextBegin, false);
        }

        DWORD ens = ::GetTickCount() - begin;

        int d = 0;
    }*/
}

PluginFrame::~PluginFrame()
{
    //CMFCVisualManager::DestroyInstance();
}

int PluginFrame::OnCreate(LPCREATESTRUCT createStruct)
{
    if (CFrameWndEx::OnCreate(createStruct) == -1)
        return -1;

    //CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
    RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    HICON const smallIcon   = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICONSM));
    HICON const bigIcon     = reinterpret_cast<HICON>(::GetClassLongPtrW(m_info.npp, GCLP_HICON));
    ASSERT(smallIcon && bigIcon);
    
    SetIcon(smallIcon, false);
    SetIcon(bigIcon, true);

    if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
        return -1;
    if (!m_toolbar.LoadToolBar(IDR_MAINFRAME))
        return -1;
    
    //m_comboTracesParsers.SetStyle(WS_DLGFRAME);

    CMFCToolBarComboBoxButton deviceCombo(ID_TRACES_PARSERS, -1, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP, 300);
    m_toolbar.ReplaceButton(ID_TRACES_PARSERS, deviceCombo, TRUE);
            
    m_toolbar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY); 
    DockPane(&m_toolbar);

    
    CDockingManager::SetDockingMode(DT_SMART);
    EnableAutoHidePanes(CBRS_ALIGN_ANY);
    
    return 0;
}

void PluginFrame::OnClose()
{
    if (SaveModifiedScheme())
    {
        m_schemeContext.reset(nullptr);
        CFrameWndEx::OnClose();
    }
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

LRESULT PluginFrame::OnToolbarReset(WPARAM wp, LPARAM)
{
	UINT uiToolBarId = (UINT)wp;

	switch (uiToolBarId)
	{
		case IDR_MAINFRAME:
		{
			CMFCToolBarComboBoxButton deviceCombo(ID_TRACES_PARSERS, -1, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP, 300);
			deviceCombo.AddItem(TEXT("ITEM"));
			m_toolbar.ReplaceButton(ID_TRACES_PARSERS, deviceCombo, TRUE);

			//CMFCToolBarEditBoxButton nColumnsEdit(
			//	ID_DummyButtonForEditBox,
			//	GetCmdMgr()->GetCmdImage(ID_DummyButtonForEditBox, FALSE));
			////nColumnsEdit.SetContextMenuID(ID_DummyButtonForEditBox);
			//nColumnsEdit.CanBeStretched();
			//nColumnsEdit.HaveHotBorder();
			//nColumnsEdit.SetContents(_T("edit box button"));
			//nColumnsEdit.SetFlatMode(true);
			//nColumnsEdit.SetStyle(TBBS_PRESSED);
			//int nReplaced = m_wndToolBar.ReplaceButton(ID_DummyButtonForEditBox, nColumnsEdit);
			//nColumnsEdit.EnableWindow(TRUE);

			break;
			}
	}
	return 0;
}

void PluginFrame::OnFileNew()
{
    MessageBox(TEXT("Feature not available yet"), m_info.name.c_str(), MB_OK | MB_ICONINFORMATION);
    return;

    if (SaveModifiedScheme())
        m_schemeContext.reset(new SchemeContext(this));
}

void PluginFrame::OnFileOpen()
{
    if (SaveModifiedScheme())
    {
        CFileDialog dialog(
            TRUE,
            SCHEME_FILE_EXTENSTION,
            NULL,
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            SCHEME_DEFAULT_FILE_NAME_FILTER);
        
        //if (dialog.DoModal() == IDOK)
        {
            std::unique_ptr<SchemeContext> newSchemeContext(new SchemeContext(this));
            if (newSchemeContext->Create(TEXT("D:\\scheme.xml") /*dialog.GetPathName().GetBuffer()*/))
                m_schemeContext = std::move(newSchemeContext);
            else
                MessageBox(TEXT("Failed to open analyzes scheme"), m_info.name.c_str(), MB_OK | MB_ICONERROR);
        }
    }

    if (!m_schemeContext)
        DestroyWindow();
}

void PluginFrame::OnFileSave()
{
    SaveScheme();
}


bool PluginFrame::SaveModifiedScheme()
{
    if (m_schemeContext)
    {
        Scheme const& scheme = m_schemeContext->GetScheme();
        if (scheme.IsModified())
        {
            tstring const displayFileName = scheme.GetDisplayFileName();

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
    }

    return true;
}

bool PluginFrame::SaveScheme()
{
    MessageBox(TEXT("Feature not available yet"), m_info.name.c_str(), MB_OK | MB_ICONINFORMATION);
    return false;

    if (m_schemeContext)
    {
        Scheme& scheme = m_schemeContext->GetScheme();
        tstring const fileName = scheme.GetFileName();

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
                result = scheme.Save(dialog.GetPathName().GetBuffer());
            else
                return false;
        }
        else
            result = scheme.Save();

        if (!result)
            MessageBox(TEXT("Failed to save analyzes scheme"), m_info.name.c_str(), MB_OK | MB_ICONERROR);

        return result;
    }
    else
    {
        return false;
    }
}