#pragma once

#include "plugin_frame.h"

#include "npp/plugin_interface.h"

#include <afxwin.h>
#include "atlbase.h"

#include <vector>

#define NPP_PLUGIN_NAME "Traces analyzer"

struct PluginMenuItem :
    FuncItem
{
    PluginMenuItem() :
        FuncItem(),
        menu(0),
        checked(false)
    {};

    void SetCheck(bool state)
    {
        if (checked)
            _init2Check = state;
    }

    void Check()
    {
        if (checked)
        {
         
        }
    }

    HMENU menu;
    bool  checked;
};

class PluginApplication :
    public CWinApp
{
public:
    PluginApplication();

private:
    typedef std::vector<PluginMenuItem> PluginMenuItems;

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    void SetNppPluginData(NppData const& nppPluginData);
    PluginMenuItems& GetPluginMenuHandlers();

private:
    bool InitializeAnalyzer();
    void DestroyAnalyzer();

    static void PluginMenuItemHandlerEnable();

private:
    PluginMenuItems m_pluginMenuHandlers;
    PluginInfo      m_pluginInfo;
};