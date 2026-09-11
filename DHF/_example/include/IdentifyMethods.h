#ifndef __DHF_IDENTIFY_METHODS_H__
#define __DHF_IDENTIFY_METHODS_H__

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <set>
#include "IdentifyDefs.h"

//------------------------------------------------------------------------------
/*! Извлечение идентификаторов из массива идентификаторов
*/

struct SSpecifyIdentifiers
{
	unsigned char typeIdent;
	void* ident;
};
typedef std::vector<SSpecifyIdentifiers> SSpecIdentsVec;

struct SOffIdentifiers
{
	unsigned char typeIdent;
	int off;

	bool operator<(const SOffIdentifiers& s) const
    {
		if( this->typeIdent < s.typeIdent ) return true; if(this->typeIdent > s.typeIdent) return false;
        return false;
    }
};
typedef std::vector<SOffIdentifiers> SOffIdentsVec;
typedef std::multiset<SOffIdentifiers> SOffIdentsMultiSet;

struct SOffInfo
{
	SOffIdentsVec v_idents;
	SOffIdentsMultiSet ms_idents;
};

void PutSearchOffIdent(unsigned char typeIdent, SOffIdentsVec &idents)
{
    SOffIdentifiers ident;
    ident.typeIdent = typeIdent;
    ident.off = -1;
    idents.push_back(ident);
}

void SetOffIdent(unsigned char typeIdent, int off, SOffInfo &info)
{
    SOffIdentifiers ident;
    ident.typeIdent = typeIdent;
    ident.off = off;
    info.v_idents.push_back(ident);
	info.ms_idents.insert(ident);
}

int GetOffIdent(unsigned char typeIdent, SOffInfo &info)
{
	int r = -1;
    SOffIdentifiers ident;
    ident.typeIdent = typeIdent;
	SOffIdentsMultiSet::iterator it = info.ms_idents.find(ident);
	if( it != info.ms_idents.end() )
	{
		r = it->off;
	}
	return r;
}

int GetOffLastIdent(unsigned char typeIdent, SOffInfo &info)
{
	int r = -1;
    SOffIdentifiers ident;
    ident.typeIdent = typeIdent;
	SOffIdentsMultiSet::iterator it_b = info.ms_idents.lower_bound(ident);
	SOffIdentsMultiSet::iterator it_e = info.ms_idents.upper_bound(ident);
	while( it_b != it_e )
	{
		r = it_b->off;
        it_b++;
	}
	return r;
}

void PutSearchIdent(unsigned char typeIdent, SSpecIdentsVec &idents)
{
    SSpecifyIdentifiers ident;
    ident.typeIdent = typeIdent;
    ident.ident = new char[NAppIdentify::getIdentifySize(typeIdent)];
    idents.push_back(ident);
}

void ClearAllSearchIdents(SSpecIdentsVec &idents)
{
	for( int i = 0; i < idents.size(); i++ )
	{
		if( idents[i].ident )
		{
			delete[] idents[i].ident;
			idents[i].ident = NULL;
		}
	}
}

//Извлечение указанного идентификатора последнего модуля из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// typeIdent	: in - тип искомого идентификатора;
// ident		: in/out - искомый идентификатор;
// Результат	: true - если был найден идентификатор, false - если не найден.
bool ParseIdentify(void* d, int l, unsigned char typeIdent, void* ident)
{
	char* h = (char*)d; int hl = l;
	unsigned char type;
	unsigned char type_mod;
	int size;

	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			type_mod = type;
		}
		else
		{
			if( type == typeIdent )
			{
				memcpy(ident, h, size);
				return true;
			}
		}
		h += size; hl -= size;
	}

	return false;
}

//Извлечение указанного идентификатора последнего модуля из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// typeIdent	: in - тип искомого идентификатора;
// ident		: in/out - искомый идентификатор;
// Результат	: true - если был найден идентификатор, false - если не найден.
bool ParseIdentifyFromLastModule(void* d, int l, unsigned char typeIdent, void* ident)
{
	bool r = false;

	char* h = (char*)d; int hl = l;
	unsigned char type;
	unsigned char type_mod;
	int size;

	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			type_mod = type;
			r = false;
		}
		else
		{
			if( type == typeIdent )
			{
				memcpy(ident, h, size);
				r = true;
			}
		}
		h += size; hl -= size;
	}

	return r;
}

//Извлечение указанных идентификаторов последнего модуля из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents		: in/out - искомые идентификаторы;
// Результат	: true - если были найдены все идентификаторы, false - если не найдены.
bool ParseIdentifyFromLastModule(void* d, int l, SSpecIdentsVec &idents)
{
	char* h = (char*)d; int hl = l;
	unsigned char type;
	unsigned char type_mod;
	int size;
	
	std::set<unsigned char> types;

	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			type_mod = type;
			types.clear();
		}
		else
		{
			for( int i = 0; i < idents.size(); i++ )
			{
				if( type == idents[i].typeIdent )
				{
					memcpy(idents[i].ident, h, size);
				
					std::set<unsigned char>::iterator it = types.find(type);
					if( it == types.end() )
					{
						types.insert(type);
					}

					break;
				}
			}
		}
		h += size; hl -= size;
	}
	return ( types.size() == idents.size() );
}

//Извлечение указанных идентификаторов последнего модуля из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
int ParseIdentifyFromLastModule(void* d, int l, SSpecIdentsVec &idents1, SSpecIdentsVec &idents2)
{
	char* h = (char*)d; int hl = l;
	unsigned char type;
	unsigned char type_mod;
	int size;
	
	std::set<unsigned char> types1;
	std::set<unsigned char> types2;
    int res = 0;

	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			type_mod = type;
			types1.clear();
			types2.clear();
		}
		else
		{
			for( int i = 0; i < idents1.size(); i++ )
			{
				if( type == idents1[i].typeIdent )
				{
					memcpy(idents1[i].ident, h, size);
				
					std::set<unsigned char>::iterator it = types1.find(type);
					if( it == types1.end() )
					{
						types1.insert(type);
					}
                    res = 1;
					break;
				}
			}
			for( int i = 0; i < idents2.size(); i++ )
			{
				if( type == idents2[i].typeIdent )
				{
					memcpy(idents2[i].ident, h, size);
				
					std::set<unsigned char>::iterator it = types2.find(type);
					if( it == types2.end() )
					{
						types2.insert(type);
					}
                    res = 2;
					break;
				}
			}
		}
		h += size; hl -= size;
	}
	if( types1.size() == idents1.size() ) return 1;
	if( types2.size() == idents2.size() ) return 2;
	return 0;
}

//Извлечение всех идентификаторов последнего модуля из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
void ParseAllIdentifyFromLastModule(void* d, int l, SSpecIdentsVec &idents, bool put = true)
{
	char* h = (char*)d; int hl = l;
	unsigned char type;
	unsigned char type_mod;
	int size;
	
	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			type_mod = type;
			idents.clear();
		}
		else
		{
			
			SSpecifyIdentifiers ident;
			ident.typeIdent = type;
			if( put )
			{
				ident.ident = new char[size];
				memcpy(ident.ident, h, size);
			}
			else
			{
				ident.ident = h;
			}
			idents.push_back(ident);
		}
		h += size; hl -= size;
	}
	return;
}

//Извлечение смещения идентификаторов из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
bool ParseOffSeqIdentify(void* d, int l, SOffIdentsVec &idents)
{
	bool res = 0;
    int s1 = idents.size();
	if( s1 == 0 ) return res;

    char* h = (char*)d;
	unsigned char type;
	//unsigned char type_mod;
	int size;

	int i = 0;
	int off = 0;
	while( l > sizeof(type) )
	{
		type = (h[off]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		off++;
		if( l-off < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			//type_mod = type;
		}
		else
		{
			if( idents[i].typeIdent == type )
			{
				idents[i].off = off;
                i++;
                if( i == s1 ) res = true; else res = 0;
				i %= s1;
			}
		}
		off += size;
	}
	return res;
}

//Извлечение смещения идентификаторов из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
int ParseOffSeqIdentify(void* d, int l, SOffIdentsVec &idents1, SOffIdentsVec &idents2)
{
	int res = 0;
	int s1 = idents1.size();
	int s2 = idents2.size();
	if( s1 == 0 || s2 == 0 ) return res;

    char* h = (char*)d;
	unsigned char type;
	//unsigned char type_mod;
	int size;

	int i1 = 0;
	int j2 = 0;
	int off = 0;
	while( l > sizeof(type) )
	{
		type = (h[off]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		off++;
		if( l-off < size ) break;

		if( type == NAppIdentify::AID_MODULE || type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			//type_mod = type;
		}
		else
		{
			if( idents1[i1].typeIdent == type )
			{
				idents1[i1].off = off;
                i1++;
                if( i1 == s1 ) res = 1; else res = 0;
				i1 %= s1;
			}
			else if( idents2[j2].typeIdent == type )
			{
				idents2[j2].off = off;
                j2++;
                if( j2 == s2 ) res = 2; else res = 0;
				j2 %= s2;
			}
		}
		off += size;
	}
	return res;
}

//Извлечение смещения идентификаторов из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
unsigned char ParseAllOffIdentify(void* d, int l, SOffInfo &info)
{
    char* h = (char*)d;
	unsigned char type;
	unsigned char moduleID;
	int size;

	int off = 0;
	while( l > sizeof(type) )
	{
		type = (h[off]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		off++;
		if( l-off < size ) break;

		SetOffIdent(type, off, info);
		if( type == NAppIdentify::AID_MODULE )
		{
			NAppIdentify::TModuleAID* mid = (NAppIdentify::TModuleAID*)(h+off);
			moduleID = mid->module_id;
		}
		else if( type == NAppIdentify::AID_MODULE_AND_SCHEME )
		{
			NAppIdentify::TModuleSchemeAID* mid = (NAppIdentify::TModuleSchemeAID*)(h+off);
			moduleID = mid->module_id;
		}

		off += size;
	}
	return moduleID;
}

//Извлечение смещения идентификаторов из массива данных:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents1		: in/out - искомые идентификаторы;
// Результат	: 0 - если не были найдены все идентификаторы, 1 - если найдены в первом, 2 - если найдены в во втором.
int ParseOffIdentify(void* d, int l, unsigned char s_type)
{
    char* h = (char*)d;
	unsigned char type;
	int size;

	int off = 0;
	while( l > sizeof(type) )
	{
		type = (h[off]&0xff);

		size = NAppIdentify::getIdentifySize(type);
		off++;
		if( l-off < size ) break;

		if( type == s_type ) return off;
		off += size;
	}
	return -1;
}
//Извлечение списка всех идентификаторов из массива данных включая идентификаторы модуля:
// d			: in - массив данных;
// l			: in - длина массива данных;
// idents		: out - искомые идентификаторы;
void ParseAllIdentifys(void* d, int l, SSpecIdentsVec &idents)
{
	char* h = (char*)d; int hl = l;
	unsigned char type;
	//unsigned char type_mod;
	int size;
	
	while( hl > sizeof(type) )
	{
		type = (h[0]&0xff);
		size = NAppIdentify::getIdentifySize(type);
		h++; hl--;
		if( hl < size ) break;

		SSpecifyIdentifiers ident;
		ident.typeIdent = type;
		ident.ident = new char[size];
		memcpy(ident.ident, h, size);
		idents.push_back(ident);

		h += size; hl -= size;
	}
}

//Очистка списка всех идентификаторов 
// idents		: in - идентификаторы;
void ClearAllIdentifys(SSpecIdentsVec &idents)
{
	for( int i = 0; i < idents.size(); i++ )
	{
		if( idents[i].ident ) delete[] idents[i].ident;
	}
	idents.clear();
}

#endif //__DHF_IDENTIFY_METHODS_H__