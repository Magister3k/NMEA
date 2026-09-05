#ifndef __DHF_IFACE_MODULE_EXT_H__
#define __DHF_IFACE_MODULE_EXT_H__

#include "ConfigXMLDefs.h"
#include "ConfigXMLMethods.h"
#include "IfaceMethodGui.h"
#include "IfaceMethodProc.h"

#ifdef __DHF_RELEASE_IFACE_MODULE_EXT_H__
#define DHF_IFACE_MODULE_EXT_DLL __declspec(dllexport)
#else
#define DHF_IFACE_MODULE_EXT_DLL __declspec(dllimport)
#endif

class IModuleExt
{
public:
    IModuleExt()
	{
		ifaceGui = NULL;
		ifaceProc = NULL;
	}

protected:
	char				aShortName[16];		//< Название модуля
	char				aLongName[128];		//< Расширенное описание модуля
	char				aGroupName[128];	//< Название группы модуля
	char				aVersion[32];		//< Версия модуля

    unsigned short      moduleUid;
	bool				bIsInputModule;

	HWND				hSettingForm;
    HBITMAP             hBitmap;

	bool bStateProc;
    bool byteTypeData;
    char typeData;
    unsigned __int64 recv;
    unsigned __int64 send;

    IfaceCallBackGui* ifaceGui;
    IfaceCallBackProc* ifaceProc;

public:
	char*				__stdcall getShortModuleName()	{ return aShortName; };
	char*				__stdcall getLongModuleName()	{ return aLongName; };
	char*				__stdcall getModuleGroupName()	{ return aGroupName; };

	bool				__stdcall isInputModule()		{ return bIsInputModule; };

	char*               __stdcall getVersion()          { return aVersion; };
    
	HWND				__stdcall getSettingFormHandle(){ return hSettingForm; };
	HBITMAP				__stdcall getBitmap()	        { return hBitmap; };

    //--------------------------------------------------------------------------

	//uid - уникальный идентификатор модуля схемы
	virtual bool        __stdcall Constructor(unsigned __int64 uid) = 0;
	virtual void        __stdcall Destructor() = 0;

    virtual bool        __stdcall CreateGui(IfaceCallBackGui* ig) { ifaceGui = ig; };
    virtual bool        __stdcall CreateProc(IfaceCallBackProc* ip) { ifaceProc = ip; }

	virtual bool		__stdcall Initialize() { recv = 0; send = 0; };
	virtual void		__stdcall Free() = 0;

    virtual void        __stdcall CreateForm() = 0;
    virtual void        __stdcall DestroyForm() = 0;
    virtual void        __stdcall ShowForm(bool show = true) = 0;

	virtual void		__stdcall setGuiModuleSettings(SAppModuleSettings* sets) = 0;
    virtual void		__stdcall setGuiModuleOptions(SAppModuleOptions* opts) = 0;
	virtual void		__stdcall setProcModuleSettings(SAppModuleSettings* sets) = 0;
	virtual void		__stdcall setProcModuleOptions(SAppModuleOptions* opts) = 0;

	virtual SAppModuleSettings* __stdcall getModuleParamsSetting(SAppModuleSettings* sets = NULL) = 0;
	virtual SAppModuleOptions* __stdcall getModuleParamsOptions(SAppModuleOptions* opt = NULL) = 0;
	virtual void		__stdcall getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts) = 0;
	virtual void __stdcall removeModuleParamsSetting(SAppModuleSettings* set) { clearModulesSettings(set); };
    virtual void __stdcall removeModuleParamsOptions(SAppModuleOptions* opt) { clearModulesOptions(opt); };
	virtual void __stdcall removeDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts)
    {
        clearModulesSettings(sets);
        clearModulesOptions(opts);
    };

	virtual void __stdcall setProcState(bool s)	{ bStateProc = s; };
    virtual bool __stdcall procGuiData(unsigned short type, void* d, int l) = 0;
    
    virtual bool __stdcall isByteTypeData() { return byteTypeData; };
    virtual char __stdcall getTypeData() { return typeData; };

    virtual bool __stdcall HookMessage(int wParam, int lParam) { return true; };

    //--------------------------------------------------------------------------

	virtual bool __stdcall workProc(bool rSleep) = 0;
	virtual bool __stdcall workData(char* idsData, int idsDataLen, char* d, int l) = 0;
    virtual void __stdcall timing() = 0;

	virtual bool __stdcall recvGuiData(unsigned short type, char* d, int l) = 0;
    virtual void __stdcall updateStatistic() = 0;

    //--------------------------------------------------------------------------

    virtual void __stdcall resetCommonParams() { recv = 0; send = 0; };
    virtual unsigned __int64 __stdcall getRecvData() { return recv; };
    virtual unsigned __int64 __stdcall getSendData() { return send; };
    
    //--------------------------------------------------------------------------
};

//-------------------------------------------------------------------
//! метод для получения интерфейса
extern "C" DHF_IFACE_MODULE_EXT_DLL IModuleExt* __stdcall getIModuleExt( void );
//! метод для удаления интерфейса
extern "C" DHF_IFACE_MODULE_EXT_DLL void __stdcall removeIModuleExt( IModuleExt* );
//-------------------------------------------------------------------

#endif //__DHF_IFACE_MODULE_EXT_H__