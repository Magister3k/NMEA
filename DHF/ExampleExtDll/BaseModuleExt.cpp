#include "BaseModuleExt.h"
#include "ModuleDefParams.h"
#include "ModuleParamMethods.h"
#include "TypeGuiPack.h"

#include <stdio.h>

CIModuleExt::CIModuleExt():
	IModuleExt(),
    settinsForm(NULL)
{
    hSettingForm = NULL;
    hBitmap = NULL;

    memset(aShortName, 0, sizeof(aShortName));
    memset(aLongName, 0, sizeof(aLongName));
    memset(aGroupName, 0, sizeof(aGroupName));
    memset(aVersion, 0, sizeof(aVersion));

    snprintf(aShortName, sizeof(aShortName), defModuleShortName);
    snprintf(aLongName, sizeof(aLongName), defModuleLongName);
    snprintf(aGroupName, sizeof(aGroupName), defModuleGroupName);
    snprintf(aVersion, sizeof(aVersion), "%s.%s",
                        MODULE_INTERFACE_VERSION,
                        MODULE_RELEASE_VERSION);

    bIsInputModule = defModuleTypeInput;

    byteTypeData = true;        //Если true - размерность модуля в байтах,
                                //если false - размерность определяется буквой "typeData"
    typeData = 'Б';             //Размерность

	settinsForm = NULL;
}

CIModuleExt::~CIModuleExt()
{
    DestroyForm();
    Destructor();
}

void __stdcall CIModuleExt::getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts)
{
    //--------------------------------------------------------------------------
    //Тут определяем пакеты которые приходят из обработчика при инициализации
    /**/
    getDefaultModuleSettings(sets, defaultSetting, MODULE_CNT, defModuleSettingsName);
    if( opts )
    {
        opts->cnt_arg = 1;
        opts->args = new SAppModuleSettings[opts->cnt_arg];
        getDefaultModuleSettings(&opts->args[0], defaultOption, OPTION_CNT, DEFAULT_NAME_OPTION);
    }
    /**/
    //--------------------------------------------------------------------------
}

bool __stdcall CIModuleExt::CreateGui(IfaceCallBackGui* ig)
{
    ifaceGui = ig;
}

bool __stdcall CIModuleExt::CreateProc(IfaceCallBackProc* ip)
{
    ifaceProc = ip;
}

//uid - уникальный идентификатор модуля схемы
bool __stdcall CIModuleExt::Constructor(unsigned __int64 uid)
{
	return true;
}

void __stdcall CIModuleExt::Destructor()
{
    hSettingForm = NULL;
}

void __stdcall CIModuleExt::CreateForm()
{
    if( settinsForm ) return;
    
    settinsForm = new TFormSettings(NULL);
    hBitmap = settinsForm->ImageModule->Picture->Bitmap->Handle;
    hSettingForm = settinsForm->Handle;
}

void __stdcall CIModuleExt::DestroyForm()
{
    if( settinsForm )
    {
        delete settinsForm;
        settinsForm = NULL;
    }
}

void __stdcall CIModuleExt::ShowForm(bool show)
{
    if( settinsForm ) settinsForm->Visible = show;
}

void __stdcall CIModuleExt::setGuiModuleSettings(SAppModuleSettings* sets)
{
    //Основные параметры
	std::string name, value;
    if( sets )
    {
        for( int i = 0; i < sets->cnt_params; i++ )
        {
			name   = sets->params[i].name;
			value  = sets->params[i].value;

			//--------------------------------------------------------------------------
			//Тут устанавливаем на форме основные параметры
			/**/
            if( settinsForm )
            {
                int index = getSettingIndex(name, defaultSetting, MODULE_CNT);
                switch( index )
                {
                case MODULE_STR:  settinsForm->StrEdit->Text = value.c_str(); break;
                case MODULE_BOOL: settinsForm->BoolCheckBox->Checked = getBoolValue(value); break;
                case MODULE_INT:  settinsForm->CSpinEdit1->Value = getIntValue(value); break;
                default: break;
                }
            }
			/**/
        }
    }
}
void __stdcall CIModuleExt::setGuiModuleOptions(SAppModuleOptions* opts)
{
    //----------------------------------------------------------------------
    //Тут сбрасываем динамические параметры при инициализации
    if( !settinsForm ) return; 
    settinsForm->ListView1->Items->Clear();
    //----------------------------------------------------------------------
    //Динамические параметры
    if( opts )
    {
        std::string name, value;
		SAppModuleSettings* opt;
        for( int i = 0; i < opts->cnt_arg; i++ )
        {
            opt = &opts->args[i];
			//--------------------------------------------------------------------------
			//Тут устанавливаем на форме динамические параметры
			/**/
			name = opt->name;
			if( !name.compare(DEFAULT_NAME_OPTION) )
			{
				SDynParam p;
				for( int j = 0; j < opt->cnt_params; j++ )
				{
					name    = opt->params[j].name;
					value   = opt->params[j].value;
					int index = getSettingIndex(name, defaultOption, OPTION_CNT);
					switch( index )
					{
					case OPTION_STR: p.value = value.c_str(); break;
					case OPTION_BOOL: p.value_bool = getBoolValue(value); break;
					case OPTION_INT: p.value_int = getIntValue(value); break;
					}
				}

                TListItem* item = settinsForm->ListView1->Items->Add();
                if( item )
                {
                    item->Caption = p.value;
                    
                    if( p.value_bool ) item->SubItems->Add(AnsiString("Да"));
                    else item->SubItems->Add(AnsiString("Нет"));

                    item->SubItems->Add(AnsiString(p.value_int));
                }
			}
			/**/
			//--------------------------------------------------------------------------
        }
    }
}

void __stdcall CIModuleExt::setProcModuleSettings(SAppModuleSettings* sets)
{
    std::string name, value;
    //Основные параметры
    if( sets )
    {
        for( int i = 0; i < sets->cnt_params; i++ )
        {
            name = sets->params[i].name;
			value = sets->params[i].value;
			//--------------------------------------------------------------------------
			//Тут устанавливаем в обработчике основные параметры
			/**/
			int index = getSettingIndex(name, defaultSetting, MODULE_CNT);
			switch( index )
			{
			case MODULE_STR:  value_str = value.c_str(); break;
			case MODULE_BOOL: value_bool = getBoolValue(value); break;
			case MODULE_INT:  value_int = getIntValue(value); break;
			default: break;
			}
			/**/
			//--------------------------------------------------------------------------
        }
        clearModulesSettings(sets);
    }
}
void __stdcall CIModuleExt::setProcModuleOptions(SAppModuleOptions* opts)
{
    //Динамические параметры

    if( opts )
    {
        std::string name, value;
		SAppModuleSettings* opt;
        for( int i = 0; i < opts->cnt_arg; i++ )
        {
			opt = &opts->args[i];
			/**/
			name = opt->name;
			if( !name.compare(DEFAULT_NAME_OPTION) )
			{
				SDynParam p;
				for( int j = 0; j < opt->cnt_params; j++ )
				{
					name    = opt->params[j].name;
					value   = opt->params[j].value;
					int index = getSettingIndex(name, defaultOption, OPTION_CNT);
					switch( index )
					{
					case OPTION_STR: p.value = value.c_str(); break;
					case OPTION_BOOL: p.value_bool = getBoolValue(value); break;
					case OPTION_INT: p.value_int = getIntValue(value); break;
					}
				}
				setDynParam(&p);
			}
			/**/
            clearModulesSettings(&opts->args[i]);
        }
        clearModulesOptions(opts);
    }
}

SAppModuleSettings* __stdcall CIModuleExt::getModuleParamsSetting(SAppModuleSettings* sets)
{
    if( !sets )
    {
        sets = new SAppModuleSettings;
    }
    //--------------------------------------------------------------------------
    //Тут извлекаем из формы установленные пользователем основные параметры
    /**/
    getDefaultModuleSettings(sets, defaultSetting, MODULE_CNT, defModuleSettingsName);

    if( settinsForm )
    {
        setStrValue(sets->params, MODULE_STR, settinsForm->StrEdit->Text.c_str(), defaultSetting);
        setBoolValue(sets->params, MODULE_BOOL, settinsForm->BoolCheckBox->Checked, defaultSetting);
        setIntValue(sets->params, MODULE_INT, settinsForm->CSpinEdit1->Value, defaultSetting);
    }
    /**/
    //--------------------------------------------------------------------------
	return sets;
}

SAppModuleOptions* __stdcall CIModuleExt::getModuleParamsOptions(SAppModuleOptions* opt)
{
    if( opt == NULL ) {
        opt = new SAppModuleOptions;
    }
    //--------------------------------------------------------------------------
    //Тут извлекаем из формы установленные пользователем динамические параметры
    /**/
    opt->cnt_arg = 0;
    
    if( !settinsForm ) return opt;
    
    opt->cnt_arg = settinsForm->ListView1->Items->Count;
    if( opt->cnt_arg )
    {
        opt->args = new SAppModuleSettings[opt->cnt_arg];

        TListItem* m;
        for( int i = 0; i < opt->cnt_arg; i++ )
        {
            m = settinsForm->ListView1->Items->Item[i];
             
            snprintf(opt->args[i].name, sizeof(opt->args[0].name), "%s", DEFAULT_NAME_OPTION);
            opt->args[i].cnt_params = OPTION_CNT;
            opt->args[i].params = new SAppModuleParam[OPTION_CNT];

            setStrValue(opt->args[i].params, OPTION_STR, m->Caption.c_str(), defaultOption);
            setBoolValue(opt->args[i].params, OPTION_BOOL, !m->SubItems->Strings[0].AnsiCompareIC("Да"), defaultOption);
            setIntValue(opt->args[i].params, OPTION_INT, m->SubItems->Strings[0].ToIntDef(0), defaultOption);
        }
    }
    /**/
    //--------------------------------------------------------------------------
    return opt;
}

void __stdcall CIModuleExt::setProcState(bool s)
{
    //--------------------------------------------------------------------------
    //Тут включаем или выключаем параметры при запуске и остановке процесса обработки
    /**/
    if( settinsForm )
    {
        settinsForm->StrEdit->Enabled = !s;
        settinsForm->BoolCheckBox->Enabled = !s;
        settinsForm->CSpinEdit1->Enabled = !s;
        settinsForm->ListView1->Enabled = !s;
    }
    /**/
    //--------------------------------------------------------------------------
    IModuleExt::setProcState(s);
}

bool __stdcall CIModuleExt::procGuiData(unsigned short type, void* d, int l)
{
    switch ( type )
    {
    //--------------------------------------------------------------------------
    //Тут определяем пакеты которые приходят из обработчика в процессе обработки
    /**
        case MT_PACK_GUI_PARAM:
            setGuiParam(reinterpret_cast<SPackGuiParam*> (d));
            break;
    /**/
    //--------------------------------------------------------------------------
        default:
            break;
    }

    return true;
}

bool __stdcall CIModuleExt::HookMessage(int wParam, int lParam)
{
    //--------------------------------------------------------------------------
    //Тут принимаем отосланные сообщения

    //--------------------------------------------------------------------------
    return true;
} 

void CIModuleExt::setDynParam(SDynParam* p)
{
    SDynParamVec.push_back(*p);
}

bool __stdcall CIModuleExt::Initialize()
{
    //--------------------------------------------------------------------------
    //Инициализация локальных данных

    //--------------------------------------------------------------------------

    return true;
}

void __stdcall CIModuleExt::Free()
{
    //--------------------------------------------------------------------------
    //Освобождение локальных данных
    SDynParamVec.clear();
    //--------------------------------------------------------------------------
}

bool __stdcall CIModuleExt::workProc(bool rSleep)
{
    //--------------------------------------------------------------------------
    //Тут определяем по необходимости свою обработку, независимую от входных данных

    //--------------------------------------------------------------------------
    return true;
}

bool __stdcall CIModuleExt::workData(char* idsData, int idsDataLen, char* d, int l)
{
    recv += l;

    //--------------------------------------------------------------------------

    /**
	//Извлекаем идентификаторы записанные предыдущими модулями
    SOffInfo offs;
    ParseAllOffIdentify(idsData, idsDataLen, offs);

    pOff = GetOffIdent(NAppIdentify::AID_NUMBER_SRC, offs);
    if( pOff >= 0 )
	{
		//Найден идентификатор : номер отправителя
		memcpy(info.snumber, idsData+pOff, sizeof(info.snumber));
	}

    pOff = GetOffIdent(NAppIdentify::AID_NUMBER_DST, offs);
    if( pOff >= 0 )
	{
		//Найден идентификатор : номер получателя
		memcpy(info.dnumber, idsData+pOff, sizeof(info.dnumber));
	}
	/**

	/**
	//Определяем по необходимости свою обработку, зависимую от входных данных
	//Например, отрезаем первые два байта из каждого пакета:
	d += 2; l -= 2;
	if( l <= 0 ) return;
	/**/

	//Добавляем для отправки идентификаторы предыдущих модулей
    ifaceProc->PutPrevIdentify(idsData, idsDataLen);

	/**
	//Добавляем для отправки свои идентификаторы (Например: страна отправителя и страна получателя)
    NAppIdentify::Type32Int8 country;
    snprintf(country.name, sizeof(country.name), "RUS");
    iface->PutIdentify(NAppIdentify::AID_COUNTRY_SRC, &country);

    snprintf(country.name, sizeof(country.name), "RUS");
    iface->PutIdentify(NAppIdentify::AID_COUNTRY_DST, &country);
	/**/

	//Отправляем данные следующему модулю.
	//Вместе с отправкой данных отправляются добавленные идентфикаторы от предыдущих модулей и добавленные в данном модуле.
	ifaceProc->SendDataProc(d, l);
    send += l;

    //--------------------------------------------------------------------------

    return true;
}

void __stdcall CIModuleExt::timing()
{
    //--------------------------------------------------------------------------
    //Тут обрабатываются данные по таймауту. Таймаут примерно 1 сек.

    //--------------------------------------------------------------------------
}

bool __stdcall CIModuleExt::recvGuiData(unsigned short type, char* d, int l)
{
    if( ifaceProc )
    {
        switch( type )
        {
        //----------------------------------------------------------------------
        //Тут определяем пакеты которые приходят от формы (интерфейса) в процессе обработки
        /**
            case MT_PACK_GUI_PARAM:
                setGuiParam(reinterpret_cast<SPackGuiParam*> (d));
                break;
        /**/
        //----------------------------------------------------------------------
        default:
            ifaceProc->SendGuiData(type, d, l);
        }
        delete[] d;
    }

    return true;
}

void __stdcall CIModuleExt::updateStatistic()
{
    if( ifaceProc )
    {
        AnsiString str;
    //--------------------------------------------------------------------------
    //Тут определяем строки которые будут видны в окне статистики данного модуля
    /**/
        str = "Строка 1"; ifaceProc->SendStatString(str.c_str());
        str = "Строка 2"; ifaceProc->SendStatString(str.c_str());
        str = "Строка 3"; ifaceProc->SendStatString(str.c_str());
    /**/
    //--------------------------------------------------------------------------
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Тут реализуем специфичные данному модулю методы

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! метод для получения интерфейса
extern "C" DHF_IFACE_MODULE_EXT_DLL IModuleExt* __stdcall getIModuleExt()
{
	return new CIModuleExt();
}
//-------------------------------------------------------------------
//! метод для удаления интерфейса
extern "C" DHF_IFACE_MODULE_EXT_DLL void __stdcall removeIModuleExt(IModuleExt* aVal)
{
	if ( aVal != NULL )	
	{
		delete aVal;
		aVal = NULL;
	}
}
//---------------------------------------------------------------------------
