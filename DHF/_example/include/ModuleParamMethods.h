#ifndef __MODULE_PARAMS_METHODS_H__
#define __MODULE_PARAMS_METHODS_H__

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string>
#include "ConfigXMLDefs.h"

inline void getDefaultModuleSettings(SAppModuleSettings* sets, const SParam* defSets, int cnt, const char* name)
{
    if( sets )
    {
        if( name ) _snprintf(sets->name, sizeof(sets->name), "%s", name);
        sets->cnt_params = cnt;
        sets->params = new SAppModuleParam[cnt];
        for( int i = 0; i < cnt; i++ )
        {
            _snprintf(sets->params[i].name,  sizeof(sets->params[i].name),  "%s", defSets[i].name );
            _snprintf(sets->params[i].value, sizeof(sets->params[i].value), "%s", defSets[i].value );
        }
    }
}

inline void getDefaultModuleSettings(SAppModuleSettings* sets, const char* name,
                                const SParam* defSets1, int cnt1,
                                const SParam* defSets2, int cnt2)
{
    if( sets )
    {
        if( name ) _snprintf(sets->name, sizeof(sets->name), "%s", name);
        sets->cnt_params = cnt1+cnt2;
        sets->params = new SAppModuleParam[cnt1+cnt2];
        int c = 0;
        for( int i = 0; i < cnt1; i++ )
        {
            _snprintf(sets->params[c].name,  sizeof(sets->params[c].name),  "%s", defSets1[i].name );
            _snprintf(sets->params[c].value, sizeof(sets->params[c].value), "%s", defSets1[i].value );
            c++;
        }
        for( int i = 0; i < cnt2; i++ )
        {
            _snprintf(sets->params[c].name,  sizeof(sets->params[c].name),  "%s", defSets2[i].name );
            _snprintf(sets->params[c].value, sizeof(sets->params[c].value), "%s", defSets2[i].value );
            c++;
        }
    }
}

inline int getSettingIndex(std::string name, const SParam* defSets, int cnt)
{
    int i = 0;
    for( ; i < cnt; i++ )
    {
        if( !name.compare(defSets[i].name) )
        {
            break;
        }
    }
    return i;
};

inline bool getBoolValue(std::string value)
{
    if( !value.compare("true") ) return true;
    return false;
}

inline std::string getStrBoolValue(bool value)
{
    if( value ) return "true";
    return "false";
}

inline void setBoolValue(SAppModuleParam* params, int index, bool value, const SParam* defSets)
{
    if( params )
    {
        _snprintf(params[index].name,   sizeof(params[index].name),  "%s", defSets[index].name);
        _snprintf(params[index].value,  sizeof(params[index].value), "%s", getStrBoolValue(value).c_str());
    }
}

inline int getIntValue(std::string value, int defaultValue = 0)
{
 	int r = defaultValue;
	char* s = (char*)value.c_str();
    if( value.length() > 2 )
    {
        if( s[0] == '0' &&
            s[1] == 'x' )
        {
			s += 2;

            r = 0;
			char c;
			while((c = *s++))
			{	
				if(c >= '0' && c <= '9') r = r * 16 + (c - '0');
				else if(c >= 'a' && c <= 'f') r = r * 16 + (c - 'a' + 10);
				else if(c >= 'A' && c <= 'F') r = r * 16 + (c - 'A' + 10);
				else break;
			}
			return r;
        }
    }
	int m;
	if(*s == '-')
	{	
		m = 1;
		s++;
	}
	else 
		m = 0;
	if( !s ) return r;

    r = 0;
	char c;
	while((c = *s++))
	{	
		if(c < '0' || c > '9') break;
		r = r * 10 + (c - '0');
	}
	
	if (m) r = -r;
	return r;
}

inline std::string getStrIntValue(int value)
{
	char str[12];
	_snprintf(str, sizeof(str), "%d", value);
    return std::string(str);
}

inline void setIntValue(SAppModuleParam* params, int index, int value, const SParam* defSets)
{
    if( params )
    {
        _snprintf(params[index].name,   sizeof(params[index].name),  "%s", defSets[index].name);
        _snprintf(params[index].value,  sizeof(params[index].value), "%s", getStrIntValue(value).c_str());
    }
}

inline void setStrValue(SAppModuleParam* params, int index, std::string value, const SParam* defSets)
{
    if( params )
    {
        _snprintf(params[index].name,   sizeof(params[index].name),  "%s", defSets[index].name);
        _snprintf(params[index].value,  sizeof(params[index].value), "%s", value.c_str());
    }
}

inline void setDefaultValue(SAppModuleParam* params, int index, const SParam* defSets)
{
    if( params )
    {
        _snprintf(params[index].name,   sizeof(params[index].name),  "%s", defSets[index].name);
        _snprintf(params[index].value,  sizeof(params[index].value), "%s", defSets[index].value);
    }
}

#endif //__MODULE_PARAMS_H__