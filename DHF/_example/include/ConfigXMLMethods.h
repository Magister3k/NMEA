#ifndef __MAIN_CONFIG_XML_METHODS_H__
#define __MAIN_CONFIG_XML_METHODS_H__

#include <winsock2.h>
#include <windows.h>
#include <string>
#include "ConfigXMLDefs.h"

//-----------------		Параметры модуля		---------------------
inline void clearModulesSettings(SAppModuleSettings* setting)
{
    if( setting )
    {
        if( setting->cnt_params )
        {
            if( setting->params )
            {
                delete[] setting->params;
                setting->params = NULL;
            }
            setting->cnt_params = 0;
        }
    }
}

inline void clearModulesOptions(SAppModuleOptions* opts)
{
    if( opts )
    {
        if( opts->cnt_arg )
        {
            if( opts->args )
            {
                for( int i = 0; i < opts->cnt_arg; i++ )
                {
                    clearModulesSettings(&opts->args[i]);
                }
                delete[] opts->args;
                opts->args = NULL;
            }
            opts->cnt_arg = 0;
        }
    }
}

inline void clearModulesInfo(SAppModuleInfo* modules, int size)
{
    if( modules && size )
    {
        for( int i = 0 ; i < size; i++ )
        {
            clearModulesSettings(&modules[i].settings);
            clearModulesOptions(&modules[i].options);
        }
        delete[] modules;
    }
}

//-----------------		Параметры связей		---------------------
inline void clearLinkersInfo(SAppLinkOfModule* links, int size)
{
    if( links && size )
    {
        delete[] links;
        links = NULL;
    }
}
//-------------------------------------------------------------------
//--------------		Сравнивание параметров		-----------------
inline bool compareSettings(SAppModuleSettings* sets1, SAppModuleSettings* sets2)
{
	bool change = false;
    std::string str1, str2;

    str1 = sets1->name;
    str2 = sets2->name;
	if( str1.compare(str2) ) return true;
    if( sets1->cnt_params != sets2->cnt_params ) return true;

    SAppModuleParam* ps1;
    SAppModuleParam* ps2;

    //Сравнивание основных параметров
    int p = 0;
    for( ; p < sets1->cnt_params; p++ )
    {
        ps1 = &sets1->params[p];
        ps2 = &sets2->params[p];

        str1 = ps1->name;
        str2 = ps2->name;
        if( str1.compare(str2) ) break;

        str1 = ps1->value;
        str2 = ps2->value;
        if( str1.compare(str2) ) break;
    }
    if( p != sets1->cnt_params )
    {
        change = true;
    }

	return change;
}

inline bool compareOptions(SAppModuleOptions* opts1, SAppModuleOptions* opts2)
{
	bool change = false;

    if( opts1->cnt_arg != opts2->cnt_arg ) return true;

    //Сравнивание динамических параметров
    int p;
    SAppModuleSettings* ss1;
    SAppModuleSettings* ss2;
    SAppModuleParam* ps1;
    SAppModuleParam* ps2;
    std::string str1, str2;

    int o = 0;
    for( ; o < opts1->cnt_arg; o++ )
    {
        ss1 = &opts1->args[o];
        ss2 = &opts2->args[o];

        str1 = ss1->name;
        str2 = ss2->name;
        if( ss1->cnt_params != ss2->cnt_params ||
            str1.compare(str2) ) break;

        p = 0;
        for( ; p < ss1->cnt_params; p++ )
        {
            ps1 = &ss1->params[p];
            ps2 = &ss2->params[p];

            str1 = ps1->name;
            str2 = ps2->name;
            if( str1.compare(str2) ) break;

            str1 = ps1->value;
            str2 = ps2->value;
            if( str1.compare(str2) ) break;
        }
        if( p != ss1->cnt_params )
        {
            change = true;
            break;
        }
    }
    if( o != opts1->cnt_arg )
    {
        change = true;
    }

	return change;
}

inline bool compareModules(SAppModuleInfo* modules1, int ms1, SAppModuleInfo* modules2, int ms2)
{
    if( ms1 != ms2 ) return true;

    bool change = false;
    bool fnd;
    std::string str1, str2;
    for( int i = 0; i < ms1; i++ )
    {
        fnd = false;
        for( int j = 0; j < ms2; j++ )
        {
            str1 = modules1[i].indetify.module_name;
            str2 = modules2[i].indetify.module_name;
            if( modules1[i].indetify.uid == modules2[j].indetify.uid &&
                str1.compare(str2) == 0 &&
                modules1[i].position.left == modules2[j].position.left &&
                modules1[i].position.top == modules2[j].position.top
                )
            {
				change = compareSettings(&modules1[i].settings, &modules2[j].settings);
				if( change ) break;

				change = compareOptions(&modules1[i].options, &modules2[j].options);
				if( change ) break;

                fnd = true;
                break;
            }
        }
        if( !fnd ) { change = true; }
        if( change ) { break; }
    }

    return change;
}
//-----------------------------------------------------------------------
//--------------	Сравнивание установленных связей	-----------------
inline bool compareLinks(SAppLinkOfModule* links1, int ls1, SAppLinkOfModule* links2, int ls2)
{
    if( ls1 != ls2 ) return true;

    bool change = false;
    bool fnd;
    for( int i = 0; i < ls1; i++ )
    {
        fnd = false;
        for( int j = 0; j < ls2; j++ )
        {
            if( links1[i].uid == links2[j].uid &&
                links1[i].uid_src == links2[j].uid_src &&
                links1[i].uid_dst == links2[j].uid_dst )
            {
                fnd = true;
                break;
            }
        }
        if( !fnd ) { change = true; break; }
    }

    return change;
}
//------------------------------------------------------------------------------
#endif //__MAIN_CONFIG_XML_METHODS_H__
