#include "resource.h"
#include "plugin_frame.h"
#include "registry.h"
#include "scintilla.h"

#include "helpers/check.h"

#include "npp/scintilla.h"
#include "npp/menu_cmd_id.h"

#include "afxvisualmanagerofficexp.h"
#include <afxpriv.h>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

// User windows message
#define WM_UPDATE_TOOLBAR   WM_USER + 100


BEGIN_MESSAGE_MAP(PluginFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_WINDOWPOSCHANGED()
    ON_MESSAGE(WM_UPDATE_TOOLBAR,               &PluginFrame::OnUpdateToolbar)
	ON_UPDATE_COMMAND_UI(ID_TRACES_PARSERS,		&PluginFrame::OnUpdateTracesParserComboBox)
    ON_UPDATE_COMMAND_UI(ID_PROCESS_START_STOP, &PluginFrame::OnUpdateProcessStartButton)
    ON_COMMAND(ID_FILE_NEW,						&PluginFrame::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN,					&PluginFrame::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE,					&PluginFrame::OnFileSave)
	ON_COMMAND(ID_PROCESS_START_STOP,			&PluginFrame::OnToolbarProcesStart)
END_MESSAGE_MAP()

PluginFrame::PluginFrame(PluginInfo const& info) :
    m_info(info),
    m_notepad(info.npp),
    m_startProcessButtonState(StartProcessButtonState::Start)
{
	// Путь потом будем брать из настроек
	if (!m_tracesParserProvider.Create(m_notepad.GetPluginsConfigDir()))
		MessageBox(TEXT("Failed to load traces templates"), m_info.name.c_str(), MB_OK | MB_ICONERROR);

    CWinApp* const application = AfxGetApp();
    CRect windowRect(0, 0,
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_WIDTH, CW_USEDEFAULT)),
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_HEIGHT, CW_USEDEFAULT)));
    windowRect.OffsetRect(
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_LEFT, CW_USEDEFAULT)),
        static_cast<int>(application->GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_PLACE_TOP, CW_USEDEFAULT)));
    
    WIN_CHECK(Create(NULL, m_info.name.c_str(), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, windowRect, NULL,
        MAKEINTRESOURCE(IDR_MAINFRAME)));

    CMFCToolBar::GetImages()->Load(IDB_PROCESS_START, 0, TRUE);
    CMFCToolBar::GetImages()->Load(IDB_PROCESS_CANCEL, 0, TRUE);
}

PluginFrame::~PluginFrame()
{
    CMFCToolBar::GetImages()->CleanUp();
}

void PluginFrame::OnStartProcess()
{
    m_startProcessButtonState = StartProcessButtonState::Cancel;

    PostMessage(WM_UPDATE_TOOLBAR, 0, 0);
}

void PluginFrame::OnFinishProcess()
{
    m_startProcessButtonState = StartProcessButtonState::Start;

    PostMessage(WM_UPDATE_TOOLBAR, 0, 0);
}

LRESULT PluginFrame::OnUpdateToolbar(WPARAM wParam, LPARAM lParam)
{
    m_toolbar.OnUpdateCmdUI(this, TRUE);

    return 0;
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
    
    ResetToolbar();

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

void PluginFrame::OnUpdateTracesParserComboBox(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID)
	{
		CMFCToolBarComboBoxButton* tracesParserComboBox = 
            dynamic_cast<CMFCToolBarComboBoxButton*>(m_toolbar.GetButton(m_toolbar.CommandToIndex(pCmdUI->m_nID)));
        
        bool const enable = !!tracesParserComboBox->GetCount();
        if (!enable)
            tracesParserComboBox->SetText(TEXT("No parsers"));

		pCmdUI->Enable(enable);
	}
}

void PluginFrame::OnUpdateProcessStartButton(CCmdUI *pCmdUI)
{
    CMFCToolBarButton* processStartButton = m_toolbar.GetButton(m_toolbar.CommandToIndex(ID_PROCESS_START_STOP));

    int imageIndex = 0;
    if (m_startProcessButtonState == StartProcessButtonState::Start)
        imageIndex = 0;
    else if (m_startProcessButtonState == StartProcessButtonState::Cancel)
        imageIndex = 1;
    else 
        ASSERT(false);

    if (processStartButton->GetImage() != imageIndex)
    {
        processStartButton->SetImage(imageIndex);
        m_toolbar.InvalidateButton(m_toolbar.CommandToIndex(ID_PROCESS_START_STOP));
    }

    CMFCToolBarComboBoxButton* tracesParserComboBox = 
        dynamic_cast<CMFCToolBarComboBoxButton*>(m_toolbar.GetButton(m_toolbar.CommandToIndex(ID_TRACES_PARSERS)));
    
    pCmdUI->Enable(!!tracesParserComboBox->GetCount());
}

void PluginFrame::OnToolbarProcesStart()
{
    if (!m_schemeContext)
    {
        MessageBox(TEXT("Analysis scheme not loaded"), m_info.name.c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    switch (m_startProcessButtonState)
    {
        case StartProcessButtonState::Start:
        {
            CMFCToolBarComboBoxButton* tracesParserComboBox =
                dynamic_cast<CMFCToolBarComboBoxButton*>(m_toolbar.GetButton(m_toolbar.CommandToIndex(ID_TRACES_PARSERS)));

            if (tracesParserComboBox->GetCount() == 0)
            {
                MessageBox(TEXT("Parsers are not loaded"), m_info.name.c_str(), MB_OK | MB_ICONERROR);
                return;
            }

            int const parserIndex = tracesParserComboBox->GetItemData(tracesParserComboBox->GetCurSel());
            TracesParser const tracesParser = m_tracesParserProvider.GetParser(parserIndex);

            HWND const scintillaHandle = m_notepad.GetCurrentScintilla() ? m_info.scintillaSecond : m_info.scintillaMain;
            Scintilla const scintilla(scintillaHandle);

            bool result = m_schemeContext->StartAnalysis(m_notepad, scintilla, tracesParser, this);

            if (!result)
                MessageBox(TEXT("The analysis process is already in progress"), m_info.name.c_str(), MB_OK | MB_ICONINFORMATION);

            break;
        }

        case StartProcessButtonState::Cancel:
        {
            m_schemeContext->CancelAnalysis();
            break;
        }

        default:
            ASSERT(false);
    }
}

void PluginFrame::OnFileNew()
{
    MessageBox(TEXT("Feature not available yet"), m_info.name.c_str(), MB_OK | MB_ICONINFORMATION);
    return;

    if (SaveModifiedScheme())
    {
        m_schemeContext.reset(new SchemeContext(this));
        UpdateCaption();
    }
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
            {
                m_schemeContext = std::move(newSchemeContext);
                UpdateCaption();
            }
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

void PluginFrame::ResetToolbar()
{
    m_toolbar.RemoveAllButtons();

    m_toolbar.SetSizes(CSize(22, 22), CSize(16, 16));

    // ID_TRACES_PARSERS
    m_toolbar.InsertButton(
        CMFCToolBarComboBoxButton(ID_TRACES_PARSERS, -1, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP, 200));

    CMFCToolBarComboBoxButton* tracesParserComboBox =
        dynamic_cast<CMFCToolBarComboBoxButton*>(m_toolbar.GetButton(m_toolbar.CommandToIndex(ID_TRACES_PARSERS)));

    tracesParserComboBox->EnableWindow(true);
    tracesParserComboBox->SetCenterVert();
    tracesParserComboBox->SetDropDownHeight(25);
    tracesParserComboBox->SetFlatMode();

    size_t const parsersCount = m_tracesParserProvider.GetCountParsers();
    if (parsersCount)
    {
        for (size_t i = 0; i < parsersCount; ++i)
        {
            tstring const parserName = m_tracesParserProvider.GetParser(i).GetName();
            if (parserName.empty())
                continue;

            tracesParserComboBox->AddItem(parserName.c_str(), static_cast<DWORD_PTR>(i));
        }
    }

    tracesParserComboBox->SelectItem(0);

    // ID_PROCESS_START_STOP
    m_toolbar.InsertButton(
        CMFCToolBarButton(ID_PROCESS_START_STOP, 0));
}

void PluginFrame::UpdateCaption()
{
    tstring caption;
    
    if (m_schemeContext)
    {
        tstring schemeName = m_schemeContext->GetScheme().GetDisplayFileName();
        if (schemeName.empty())
            schemeName = SCHEME_DEFAULT_FILE_NAME;

        caption += schemeName;
        caption += TEXT(" - ");
    }

    caption += m_info.name;

    SetWindowText(caption.c_str());
}