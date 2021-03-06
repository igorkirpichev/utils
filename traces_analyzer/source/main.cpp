#include "plugin_application.h"

PluginApplication application;

extern "C" __declspec(dllexport) void setInfo(NppData nppPluginData)
{
    application.SetNppPluginData(nppPluginData);
}

extern "C" __declspec(dllexport) const TCHAR* getName()
{
    return TEXT(NPP_PLUGIN_NAME);
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int *nbF)
{
    std::vector<FuncItem>& funcItems = application.GetPluginMenuHandlers();
    *nbF = static_cast<int>(funcItems.size());
    return static_cast<FuncItem*>(funcItems.data());
}


extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
    if (notifyCode->nmhdr.code == NPPN_SHUTDOWN)
    {
        application.DestroyAnalyzer();
    }

    // очень временно
    if (notifyCode->nmhdr.code == NPPN_READY)
    {
        PluginApplication::OnPluginOpenFile();
        //PluginApplication::OnPluginNewFile();
    }
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
    MSG msg = {0};
    msg.message     = Message;
    msg.wParam      = wParam;
    msg.lParam      = lParam;

    application.PreTranslateMessage(&msg);
    return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}

#endif //UNICODE

