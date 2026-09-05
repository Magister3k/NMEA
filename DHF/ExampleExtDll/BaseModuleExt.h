#ifndef __DHF_RELEASE_IFACE_MODULE_EXT_H__
#define __DHF_RELEASE_IFACE_MODULE_EXT_H__

#include "IfaceModuleExt.h"
#include "SettingsForm.h"
#include "ExtCtrls.hpp"
#include "ModuleDefs.h"

const char MODULE_INTERFACE_VERSION[16] = "0.0";
const char MODULE_RELEASE_VERSION[16] = "0.0";

#include <vector>

class CIModuleExt : public IModuleExt
{
public:
	CIModuleExt();
	~CIModuleExt();

	//uid - уникальный идентификатор модуля схемы
	virtual bool        __stdcall Constructor(unsigned __int64 uid);
	virtual void        __stdcall Destructor();

    virtual bool        __stdcall CreateGui(IfaceCallBackGui* ig);
    virtual bool        __stdcall CreateProc(IfaceCallBackProc* ip);

	virtual bool		__stdcall Initialize();
	virtual void		__stdcall Free();

    virtual void        __stdcall CreateForm();
    virtual void        __stdcall DestroyForm();
    virtual void        __stdcall ShowForm(bool show = true);

	virtual void		__stdcall setGuiModuleSettings(SAppModuleSettings* sets);
    virtual void		__stdcall setGuiModuleOptions(SAppModuleOptions* opts);
	virtual void		__stdcall setProcModuleSettings(SAppModuleSettings* sets);
	virtual void		__stdcall setProcModuleOptions(SAppModuleOptions* opts);

	virtual SAppModuleSettings* __stdcall getModuleParamsSetting(SAppModuleSettings* set = NULL);
	virtual SAppModuleOptions* __stdcall getModuleParamsOptions(SAppModuleOptions* opt = NULL);
	virtual void		__stdcall getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts);

	virtual void __stdcall setProcState(bool s);
    virtual bool __stdcall procGuiData(unsigned short type, void* d, int l);
    
    virtual bool __stdcall HookMessage(int wParam, int lParam);

	virtual bool __stdcall workProc(bool rSleep);
	virtual bool __stdcall workData(char* idsData, int idsDataLen, char* d, int l);
    virtual void __stdcall timing();

	virtual bool __stdcall recvGuiData(unsigned short type, char* d, int l);
    virtual void __stdcall updateStatistic();

protected:
    TFormSettings*  settinsForm;

    /**/
    //Основные параметры
    AnsiString value_str;
    bool value_bool;
    int value_int;

    //Динамические параметры
    struct SDynParam
    {
        AnsiString value;
        bool value_bool;
        int value_int;
    };
    std::vector<SDynParam> SDynParamVec;

    void setDynParam(SDynParam* p);
    /**/
};

#endif //__DHF_RELEASE_IFACE_MODULE_EXT_H__