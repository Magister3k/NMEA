#ifndef __IFACE_CALLBACK_GUI_METHOD_H__
#define __IFACE_CALLBACK_GUI_METHOD_H__

const char MODULE_DEFAULT_STR_TRUE[5] = "true";

class IfaceCallBackGui
{
public:
    virtual bool __stdcall SendGuiData(unsigned short type, void* d, int l) = 0;

    virtual void __stdcall LogInfo(char* str) = 0;
    virtual void __stdcall LogWarning(char* str) = 0;
    virtual void __stdcall LogError(char* str) = 0;

    virtual void __stdcall SendPostMess(int wParam, int lParam) = 0;
};

#endif //__IFACE_CALLBACK_GUI_METHOD_H__

 