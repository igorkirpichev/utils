#include "plugin_application.h"
#include "registry.h"
#include "helpers/check.h"

#include <memory>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

extern PluginApplication application;

enum PluginMenuIndex
{
    enableAnalyzer = 0x0,
};

PluginMenuItem MakePluginMenuItemHandler(
    TCHAR const* itemName, PFUNCPLUGINCMD handler, ShortcutKey *shortcutKey, bool checked = false, bool checkInitState = false, HMENU menu = NULL)
{
    ASSERT(handler);

    PluginMenuItem item;
    lstrcpy(item._itemName, itemName);
    item._cmdID         = 0;
    item._pFunc         = handler;
    item._init2Check    = checked ? checkInitState : false;
    item._pShKey        = shortcutKey;
    item.menu           = menu;
    item.checked        = checked;

    return item;
}


PluginApplication::PluginApplication() :
    m_pluginMenuHandlers(
        {
            MakePluginMenuItemHandler(TEXT("Enable/disable analyzer"), PluginApplication::PluginMenuItemHandlerEnable, NULL, true) // enableAnalyzer
        })
{
}

BOOL PluginApplication::InitInstance()
{
    CWinApp::InitInstance();
    
    SetRegistryKey(REG_KEY_NOTEPAD);
    
    return TRUE;
}

int PluginApplication::ExitInstance()
{
    WriteProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_STATE, (m_pMainWnd) ? 1 : 0);
    
    DestroyAnalyzer();

    return CWinApp::ExitInstance();
}

void PluginApplication::SetNppPluginData(NppData const& nppPluginData)
{
    m_pluginInfo.npp                = nppPluginData._nppHandle;
    m_pluginInfo.scintillaMain      = nppPluginData._scintillaMainHandle;
    m_pluginInfo.scintillaSecond    = nppPluginData._scintillaSecondHandle;
    m_pluginInfo.name               = TEXT(NPP_PLUGIN_NAME);

    if (GetProfileInt(REG_SECTION_FRAME, REG_ENTRY_FRAME_LAST_STATE, 0))
    {
        bool const isInitialized = InitializeAnalyzer();
        
        ASSERT(!m_pluginMenuHandlers.empty());
        m_pluginMenuHandlers[enableAnalyzer].SetCheck(isInitialized);
    }
}

PluginApplication::PluginMenuItems& PluginApplication::GetPluginMenuHandlers() 
{
    return m_pluginMenuHandlers;
}

bool PluginApplication::InitializeAnalyzer()
try
{
    if (!m_pMainWnd)
    {
        std::unique_ptr<PluginFrame> pluginFrame(new PluginFrame(m_pluginInfo));

        pluginFrame->ShowWindow(SW_SHOW);
        pluginFrame->UpdateWindow();

        m_pMainWnd = pluginFrame.release();
    }
    return true;
}
catch (std::exception const& ex)
{
    std::string exceptionText("Could not initialize plugin: ");
    exceptionText += ex.what();
    ::MessageBoxA(m_pluginInfo.npp, exceptionText.c_str(), NPP_PLUGIN_NAME, MB_OK);
    return false;
}

void PluginApplication::DestroyAnalyzer()
{
    if (m_pMainWnd)
        m_pMainWnd->DestroyWindow();
}

void PluginApplication::PluginMenuItemHandlerEnable()
{
    if (application.m_pMainWnd)
        application.DestroyAnalyzer();
    else
        application.InitializeAnalyzer();
}