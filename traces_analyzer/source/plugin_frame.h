#pragma once

#include <afxwin.h>
#include <afxframewndex.h>

#include <string>

struct PluginInfo
{
    HWND npp;
    HWND scintillaMain;
    HWND scintillaSecond;
    std::wstring name;
};

class PluginFrame :
    public CFrameWndEx
{
public:
    PluginFrame(PluginInfo const& info);
    virtual ~PluginFrame();

protected:
    DECLARE_MESSAGE_MAP()

private:
    PluginFrame(PluginFrame const&) = delete;
    PluginFrame& operator=(PluginFrame const&) = delete;

private:
    afx_msg int OnCreate(LPCREATESTRUCT createStruct);
    afx_msg void OnWindowPosChanged(WINDOWPOS* wndPos);
    afx_msg void OnFileOpen();

private:
    PluginInfo const m_info;
};