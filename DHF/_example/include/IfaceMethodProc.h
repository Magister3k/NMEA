#ifndef __IFACE_CALLBACK_PROC_METHOD_H__
#define __IFACE_CALLBACK_PROC_METHOD_H__

class IfaceCallBackProc
{
public:
    virtual void __stdcall PutPrevIdentify(char* idents, int len) = 0;
    virtual void __stdcall PutIdentify(unsigned char type, void* ident) = 0;
    virtual void __stdcall ClearIdentify() = 0;

    virtual int  __stdcall SendDataProc(char* data, int dataLen) = 0;
    virtual int  __stdcall SendDataRaw(char* d, int l) = 0;
    virtual bool __stdcall SendDataForce() = 0;

    virtual void __stdcall SendGuiData(unsigned short type, void* d, int l) = 0;
    virtual void __stdcall SendStatString(char* str) = 0;

    virtual void __stdcall LogInfo(char* str) = 0;
    virtual void __stdcall LogWarning(char* str) = 0;
    virtual void __stdcall LogError(char* str) = 0;

    void __stdcall setCanCutData(bool s){ canCutData = s; };
    bool __stdcall isCanCutData(){ return canCutData; };

protected:
    bool canCutData;    //Возможность разбиения больших размеров данных на более мелкие
};

#endif //__IFACE_CALLBACK_PROC_METHOD_H__

 