#ifndef __MAIN_CONFIG_XML_DEFS_H__
#define __MAIN_CONFIG_XML_DEFS_H__

#include <winsock2.h>
#include <windows.h>

#pragma pack(push,1)

//-----------------		Параметры модуля		---------------------

struct SAppModuleIndetify
{
	unsigned __int64 uid;
	char module_name[128];

	SAppModuleIndetify()
	{
		memset(module_name, 0, sizeof(module_name));
	}
};

struct SAppModulePosition
{
	int left;
	int top;
};

struct SParam
{
	char name[128];
	char value[1024];
};

struct SAppModuleParam : public SParam 
{
	SAppModuleParam()
	{
		memset(name, 0, sizeof(name));
		memset(value, 0, sizeof(value));
	}
};

struct SAppModuleSettings
{
	char name[128];
	int cnt_params;
	SAppModuleParam* params;

    SAppModuleSettings()
    {
		memset(name, 0, sizeof(name));
        cnt_params = 0;
        params = NULL;
    }
};

struct SAppModuleOptions
{
	int cnt_arg;
	SAppModuleSettings* args;

    SAppModuleOptions()
    {
        cnt_arg = 0;
        args = NULL;
    }
};

struct SAppModuleInfo
{
	SAppModuleIndetify indetify;
	SAppModulePosition position;
	SAppModuleSettings settings;
	SAppModuleOptions options;
};

//-----------------		Параметры связей		---------------------

struct SAppLinkOfModule
{
	unsigned __int64 uid;
	unsigned __int64 uid_src;
	unsigned __int64 uid_dst;
};

//---------------- 	Параметры модуля по-умолчанию  ------------------

struct SAppModuleDefs
{
	char module_name[128];
	SAppModuleSettings* settings;
	SAppModuleOptions* options;
};

//-------------------------------------------------------------------

#pragma pack(pop)

#endif //__MAIN_CONFIG_XML_DEFS_H__
