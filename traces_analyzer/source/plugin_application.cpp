#include "plugin_application.h"
#include "registry.h"
#include "helpers/check.h"
#include "traces_parser_provider.h"

#include "afxadv.h"

#include <memory>
#include <vector>

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
    _CrtSetBreakAlloc(0);

    CWinApp::InitInstance();

    SetRegistryKey(REG_KEY_NOTEPAD);
    
    LoadStdProfileSettings();

    InitializePluginMenu();

    return TRUE;
}

int PluginApplication::ExitInstance()
{
    CMFCVisualManager::DestroyInstance();

    return CWinApp::ExitInstance();
}

void PluginApplication::SetNppPluginData(NppData const& nppPluginData)
{
    m_pluginInfo.npp                = nppPluginData._nppHandle;
    m_pluginInfo.scintillaMain      = nppPluginData._scintillaMainHandle;
    m_pluginInfo.scintillaSecond    = nppPluginData._scintillaSecondHandle;
    m_pluginInfo.name               = TEXT(NPP_PLUGIN_NAME);
}

PluginApplication::PluginMenuItems& PluginApplication::GetPluginMenuHandlers() 
{
    return m_pluginMenuHandlers;
}

void PluginApplication::InitializePluginMenu()
{
    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("New scheme"), PluginApplication::OnPluginNewFile));
    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("Open scheme"), PluginApplication::OnPluginOpenFile));
    m_pluginMenuHandlers.push_back(
        MakePluginMenuItemHandler(TEXT("Save scheme"), PluginApplication::OnPluginSaveFile));
            
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
    if (!IsAnalyzerInitialized())
        m_pMainWnd = new PluginFrame(m_pluginInfo);

    return IsAnalyzerInitialized();
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
    if (IsAnalyzerInitialized())
        m_pMainWnd->SendMessage(WM_CLOSE, 0, 0);

    if (IsAnalyzerInitialized())
        m_pMainWnd->DestroyWindow();
}

bool PluginApplication::IsAnalyzerInitialized() const
{
    return m_pMainWnd;
}


void PluginApplication::OnPluginNewFile()
{
    if (PluginApplication* application = static_cast<PluginApplication*>(AfxGetApp()))
    {
        if (application->InitializeAnalyzer())
            application->m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_NEW, 0);
     
        if (application->IsAnalyzerInitialized())
            application->m_pMainWnd->ShowWindow(SW_SHOW);
    }
}

void PluginApplication::OnPluginOpenFile()
{
    if (PluginApplication* application = static_cast<PluginApplication*>(AfxGetApp()))
    {
        if (application->InitializeAnalyzer())
            application->m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_OPEN, 0);

        if (application->IsAnalyzerInitialized())
            application->m_pMainWnd->ShowWindow(SW_SHOW);
    }
}

void PluginApplication::OnPluginSaveFile()
{
    if (PluginApplication* application = static_cast<PluginApplication*>(AfxGetApp()))
    {
        if (application->IsAnalyzerInitialized())
            application->m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_SAVE, 0);
    }
}

void PluginApplication::OnPluginCloseFile()
{
    if (PluginApplication* application = static_cast<PluginApplication*>(AfxGetApp()))
        application->DestroyAnalyzer();
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
