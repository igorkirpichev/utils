#include "plugin_application.h"
#include "registry.h"
#include "helpers/check.h"

#include "afxadv.h"

#include <memory>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

inline FuncItem MakePluginMenuSeparator()
{
    return {0};
}

inline FuncItem MakePluginMenuItemHandler(TCHAR const* itemName, PFUNCPLUGINCMD handler)
{
    ASSERT(handler);

    FuncItem item = {0};
    lstrcpy(item._itemName, itemName);
    item._cmdID         = 0;
    item._pFunc         = handler;
    item._init2Check    = false;
    item._pShKey        = NULL;

    return item;
}

PluginApplication::PluginApplication()
{
}

BOOL PluginApplication::InitInstance()
{
    CWinApp::InitInstance();
    
    SetRegistryKey(REG_KEY_NOTEPAD);
    
    LoadStdProfileSettings();

    InitPluginMenu();

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
    }
}

PluginApplication::PluginMenuItems& PluginApplication::GetPluginMenuHandlers() 
{
    return m_pluginMenuHandlers;
}

void PluginApplication::InitPluginMenu()
{
    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("New scheme"), PluginApplication::OnPluginNewFile));
    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("Open scheme"), PluginApplication::OnPluginOpenFile));
            
    if (m_pRecentFileList && m_pRecentFileList->m_nSize && !(*m_pRecentFileList)[0].IsEmpty())
    {
        m_pluginMenuHandlers.push_back(MakePluginMenuSeparator());
        
        if (m_pRecentFileList->m_nSize > 0)
        {
            m_pluginMenuHandlers.push_back(
                MakePluginMenuItemHandler((*m_pRecentFileList)[0].GetBuffer(), PluginApplication::OnPluginOpenRecentFile0));
        }

        if ((m_pRecentFileList->m_nSize > 1) && !(*m_pRecentFileList)[1].IsEmpty())
        {
            m_pluginMenuHandlers.push_back(
                MakePluginMenuItemHandler((*m_pRecentFileList)[1].GetBuffer(), PluginApplication::OnPluginOpenRecentFile1));
        }

        if ((m_pRecentFileList->m_nSize > 2) && !(*m_pRecentFileList)[2].IsEmpty())
        {
            m_pluginMenuHandlers.push_back(
                MakePluginMenuItemHandler((*m_pRecentFileList)[2].GetBuffer(), PluginApplication::OnPluginOpenRecentFile2));
        }

        if ((m_pRecentFileList->m_nSize > 3) && !(*m_pRecentFileList)[3].IsEmpty())
        {
            m_pluginMenuHandlers.push_back(
                MakePluginMenuItemHandler((*m_pRecentFileList)[3].GetBuffer(), PluginApplication::OnPluginOpenRecentFile3));
        }

        m_pluginMenuHandlers.push_back(MakePluginMenuSeparator());
    }

    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("Close"), PluginApplication::OnPluginCloseFile));
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
catch (std::exception const& /*ex*/)
{
    //std::string exceptionText("Could not initialize plugin: ");
    //exceptionText += ex.what();
    //::MessageBoxA(m_pluginInfo.npp, exceptionText.c_str(), NPP_PLUGIN_NAME, MB_OK);
    return false;
}

void PluginApplication::DestroyAnalyzer()
{
    if (m_pMainWnd)
        m_pMainWnd->DestroyWindow();
}

void PluginApplication::OnPluginNewFile()
{
}

void PluginApplication::OnPluginOpenFile()
{

}

void PluginApplication::OnPluginCloseFile()
{
}

void PluginApplication::OnPluginOpenRecentFile0()
{

}

void PluginApplication::OnPluginOpenRecentFile1()
{
}

void PluginApplication::OnPluginOpenRecentFile2()
{
}

void PluginApplication::OnPluginOpenRecentFile3()
{

}
