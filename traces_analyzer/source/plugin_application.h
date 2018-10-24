#pragma once

#include "plugin_frame.h"

#include "npp/plugin_interface.h"

#include <afxwin.h>
#include "atlbase.h"

#include <vector>

#define NPP_PLUGIN_NAME "Traces analyzer"


class PluginApplication :
    public CWinApp
{
public:
    PluginApplication();

private:
    typedef std::vector<FuncItem> PluginMenuItems;

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    void SetNppPluginData(NppData const& nppPluginData);
    PluginMenuItems& GetPluginMenuHandlers();

    void DestroyAnalyzer();

private:
    void InitPluginMenu();
    bool InitializeAnalyzer();

    void OpenFile();

    static void OnPluginNewFile();
    static void OnPluginOpenFile();
    static void OnPluginCloseFile();
    static void OnPluginOpenRecentFile0();
    static void OnPluginOpenRecentFile1();
    static void OnPluginOpenRecentFile2();
    static void OnPluginOpenRecentFile3();

private:
    PluginMenuItems m_pluginMenuHandlers;
    PluginInfo      m_pluginInfo;
};