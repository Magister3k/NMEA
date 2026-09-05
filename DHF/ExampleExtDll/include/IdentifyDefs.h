#ifndef __DHF_IDENTIFY_DEFS_H__
#define __DHF_IDENTIFY_DEFS_H__

#include <winsock2.h>
#include <windows.h>
#include <string>

#pragma push(1, pack) 

//------------------------------------------------------------------------------
/*! Перечисление типов идентификаторов
*\remarks
* Значение идетификаторов может быть от 0 до 256
*/

namespace NAppIdentify
{
	const unsigned char AID_DATA						    = 0;		///< Данные
	const unsigned char AID_MODULE							= 1;		///< Идентификаторы модуля
	const unsigned char AID_MODULE_AND_SCHEME				= 2;		///< Идентификаторы модуля и схемы

	const unsigned char AID_TIMESTAMP						= 10;		//< Временная метка
	const unsigned char AID_ID_EXT_SOURCE					= 11;		//< Идентификатор источника данных
	const unsigned char AID_ID_EXT_TASK						= 12;		//< Идентификатор задания
	const unsigned char AID_ID_EXT_SESSION					= 13;		//< Идентификатор сеанса
	const unsigned char AID_CHANNEL_NUMBER					= 14;		//< Номер канала
	const unsigned char AID_SUB_CHANNEL_NUMBER				= 15;		//< Номер подканала
	const unsigned char AID_CRYPT_KEY						= 16;       //< Ключ шифрования
	const unsigned char AID_WIFI_BSSID						= 17;       //< BSSID сети
	const unsigned char AID_NET_NAME						= 18;       //< Имя сети
	const unsigned char AID_NET_TYPE						= 19;		//< Тип сети
	const unsigned char AID_STANDART						= 20;		//< Стандарт
	const unsigned char AID_ATM_VCI							= 21;		//< Номер канала VCI
	const unsigned char AID_ATM_VPI							= 22;		//< Номер канала VPI
	const unsigned char AID_ATM_MID							= 23;		//< Номер канала MID
	const unsigned char AID_ATM_CID							= 24;		//< Номер канала CID
	const unsigned char AID_MAC_SRC							= 25;		//< MAC-адрес отправителя
	const unsigned char AID_MAC_DST							= 26;		//< МАС-адрес получателя
	const unsigned char AID_IPV4_SRC						= 27;		//< IPv4-адрес отправителя
	const unsigned char AID_IPV4_DST						= 28;		//< IPv4-адрес получателя
	const unsigned char AID_IPV6_SRC						= 29;		//< IPv6-адрес отправителя
	const unsigned char AID_IPV6_DST						= 30;		//< IPv6-адрес получателя
	const unsigned char AID_PROTOCOL_PHYSICAL_LAYER			= 31;		//< Протокол физического уровня
	const unsigned char AID_PROTOCOL_DATA_LINK_LAYER		= 32;		//< Протокол канального уровня
	const unsigned char AID_PROTOCOL_NETWORK_LAYER			= 33;		//< Протокол сетевого уровня
	const unsigned char AID_PROTOCOL_TRANSPORT_LAYER		= 34;		//< Протокол транспортного уровня
	const unsigned char AID_PROTOCOL_SESSION_LAYER			= 35;		//< Протокол сеансового уровня
	const unsigned char AID_PROTOCOL_PRESENTATION_LAYER		= 36;		//< Протокол уровня представления
	const unsigned char AID_PROTOCOL_APPLICATION_LAYER		= 37;		//< Протокол прикладного уровня
	const unsigned char AID_PORT_SRC						= 38;		//< Порт отправителя
	const unsigned char AID_PORT_DST						= 39;		//< Порт получателя
	const unsigned char AID_FILE_OPERATION_PARAMS			= 40;		//< Параметры файловой операции
	const unsigned char AID_ID_SESSION_TRANSPORT_LAYER		= 41;		//< Идентификатор сеанса транспортного уровня
	const unsigned char AID_SOURCE_NAME						= 42;		//< Устройство регистрации
	const unsigned char AID_DURATION						= 43;		//< Длительность сеанса
	const unsigned char AID_DB_SKPD_TYPE_SESSION			= 44;		//< Тип сеанса БД SKPD 1.0
	const unsigned char AID_DB_SKPD_TYPE_SYSTEM				= 45;		//< Тип системы БД SKPD 1.0
	const unsigned char AID_THEMES							= 46;		//< Тематика
	const unsigned char AID_TYPE_CRYPT						= 47;		//< Тип шифрования
	const unsigned char AID_INCOMPLETE_SESSION				= 48;		//< Флаг незавершенного сеанса
	const unsigned char AID_GATEWAY_SRC						= 49;		//< Номер шлюза отправителя
	const unsigned char AID_GATEWAY_DST						= 50;		//< Номер шлюза получателя
	const unsigned char AID_PRIORITY						= 51;		//< Номер приоритета
	const unsigned char AID_FILE_ACTION						= 52;		//< Файловое действие
	const unsigned char AID_DIRECTION						= 53;		//< Направление связи
	const unsigned char AID_FILE_SIZE						= 54;		//< Размер файла
	const unsigned char AID_SYSTEM_NUMBER					= 55;		//< Номер комплекса
	const unsigned char AID_SYSTEM_NAME						= 56;		//< Название комплекса
	const unsigned char AID_OPERATOR						= 57;		//< Оператор
	const unsigned char AID_INFO_SPEED						= 58;		//< Информационная скорость
	const unsigned char AID_SYMB_SPEED						= 59;		//< Символьная скорость
	const unsigned char AID_CARRIER_FREQ					= 60;		//< Несущая частота, кГц
	const unsigned char AID_CARRIER_FREQ_UP					= 61;		//< Несущая частота обратного канала, кГц
	const unsigned char AID_CARRIER_FREQ_DOWN				= 62;		//< Несущая частота прямого канала, кГц
	const unsigned char AID_POLARIZATION					= 63;		//< Поляризация
	const unsigned char AID_CODEC_TYPE						= 64;		//< Тип кодека
	const unsigned char AID_EQUIPMENT						= 65;		//< Вид оборудования
	const unsigned char AID_LANGUAGE						= 66;		//< Язык
	const unsigned char AID_ENCODING						= 67;		//< Кодировка
	const unsigned char AID_KEYWORD							= 68;		//< Найденные ключевые слова
	const unsigned char AID_ORIGINAL_FILENAME				= 69;		//< Оригинальное имя файла
	const unsigned char AID_COUNTRY_SRC						= 70;		//< Страна отправителя
	const unsigned char AID_COUNTRY_DST						= 71;		//< Страна получателя
	const unsigned char AID_EMAIL_SRC						= 72;		//< E-Mail отправителя
	const unsigned char AID_EMAIL_DST						= 73;		//< E-Mail получателя
	const unsigned char AID_UIN_SRC							= 74;		//< UIN отправителя
	const unsigned char AID_UIN_DST							= 75;		//< UIN получателя
	const unsigned char AID_NAME_SRC						= 76;		//< Имя отправителя
	const unsigned char AID_NAME_DST						= 77;		//< Имя получателя
	const unsigned char AID_TERMINATE_SRC					= 78;		//< Номер терминала отправителя
	const unsigned char AID_TERMINATE_DST					= 79;		//< Номер терминала получателя
	const unsigned char AID_CONNECTION_OBJECT				= 80;		//< Объект связи
	const unsigned char AID_SYSTEM_TYPE						= 81;		//< Тип системы
	const unsigned char AID_BD_CONNECTION_PATH				= 82;		//< Путь к БД
	const unsigned char AID_BD_CONNECTION_SERVER_ADDR		= 83;		//< Адрес соединения к БД
	const unsigned char AID_BD_CONNECTION_PORT				= 84;		//< Порт соединения к БД
	const unsigned char AID_BD_CONNECTION_USERNAME			= 85;		//< Логин соединения к БД
	const unsigned char AID_BD_CONNECTION_PASSWORD			= 86;		//< Пароль соединения к БД
	const unsigned char AID_LONGITUDE						= 87;		//< Долгота
	const unsigned char AID_LATITUDE						= 88;		//< Широта
	const unsigned char AID_SOURCE_FILENAME					= 89;		//< Имя файла источника
	const unsigned char AID_SOURCE_FILE_FORMAT				= 90;		//< Формат файла источника
	const unsigned char AID_NEXT_PROTOCOL_PCAP				= 91;		//< Следующий pcap-протокол
	const unsigned char AID_NEXT_PROTOCOL_UNKNOWN_LAYER		= 92;		//< Следующий протокол неизвестного уровня
	const unsigned char AID_NEXT_PROTOCOL_PHYSICAL_LAYER	= 93;		//< Следующий протокол физического уровня
	const unsigned char AID_NEXT_PROTOCOL_DATA_LINK_LAYER	= 94;		//< Следующий протокол канального уровня
	const unsigned char AID_NEXT_PROTOCOL_NETWORK_LAYER		= 95;		//< Следующий протокол сетевого уровня
	const unsigned char AID_NEXT_PROTOCOL_TRANSPORT_LAYER	= 96;		//< Следующий протокол транспортного уровня
	const unsigned char AID_NEXT_PROTOCOL_SESSION_LAYER		= 97;		//< Следующий протокол сеансового уровня
	const unsigned char AID_NEXT_PROTOCOL_PRESENTATION_LAYER= 98;		//< Следующий протокол уровня представления
	const unsigned char AID_NEXT_PROTOCOL_APPLICATION_LAYER	= 99;		//< Следующий протокол прикладного уровня
	const unsigned char AID_SESSIONG_PARAMS					= 100;		//< Параметры для протокола сеансового уровня
	const unsigned char AID_X25_LGN_LCN						= 101;		//< Номер группы логических каналов и логического канала X.25
	const unsigned char AID_PID								= 102;		//< Номер PID канала
	const unsigned char AID_NUMBER_SRC						= 103;		//< Номер телефона отправителя
	const unsigned char AID_NUMBER_DST						= 104;		//< Номер телефона получателя
	const unsigned char AID_FILENAME						= 105;		//< Имя файла

	const unsigned char AID_DB_MEU_TYPE_SESSION				= 106;		//< Тип сеанса MEU.1.0 (все остальные)
	const unsigned char AID_DB_MEU_TYPE_SYSTEM				= 107;		//< Тип системы MEU.1.0(GSM)
	const unsigned char AID_IMEI_SRC						= 108;		//< IMEI отправителя
	const unsigned char AID_IMSI_SRC						= 109;		//< IMSI отправителя
	const unsigned char AID_TMSI_SRC						= 110;		//< TMSI отправителя
	const unsigned char AID_OPER_CODE						= 111;		//< код оператора связи
	const unsigned char AID_LAC_SRC							= 112;		//< LAC отправителя
	const unsigned char AID_LAC_DST							= 113;		//< LAC получателя
	const unsigned char AID_CELLID_SRC						= 114;		//< CELLID отправителя
	const unsigned char AID_CELLID_DST						= 115;		//< CELLID получателя
	const unsigned char AID_DISTANCE_BASE_S					= 116;		//< Расстояние от базовой станции
	const unsigned char AID_DUPLEX_FILENAME					= 117;		//< Имена файлов дуплекса
    const unsigned char AID_HARDWARE_CHANNEL				= 118;		//< Имена файлов дуплекса


	//----------------------------Константы для идентификаторов----------------------------------

	const int FILE_ACTION_OFF						        = -2;
    const int FILE_ACTION_DELETE					        = -1;
    const int FILE_ACTION_CLOSE						        = 0;
    const int FILE_ACTION_FULL						        = 1;
    const int FILE_ACTION_INC						        = 2;
    const int FILE_ACTION_NULL						        = 3;
    const int FILE_ACTION_BREAK						        = 4;
    const int FILE_ACTION_DUPLEX_WAIT				        = 10;      // будет дуплекс
    const int FILE_ACTION_FLAG_PARSE_DATA			        = 128;     // Флаг дополнительной обработки (вставляется на ТСР уровне, удаляется в модел FILE_OPER)
    const int FILE_ACTION_FLAG_UNZIP_DATA			        = 256;     // Флаг дополнительной обработки (вставляется на ТСР уровне, удаляется в модел FILE_OPER)

	const unsigned char DIRECTION_UNKNOWN			        = 0;
	const unsigned char DIRECTION_TO_SERVER			        = 1;
	const unsigned char DIRECTION_FROM_SERVER		        = 2;
	const unsigned char DIRECTION_TO_FROM_SERVER	        = 3;

	//-------------------------------------------------------------------------------------------

	struct TModuleAID
	{
		unsigned char module_id;
		unsigned char module_type;
	};

	struct TModuleSchemeAID
	{
		unsigned char scheme_id;
		unsigned char module_id;
		unsigned char module_type;
	};

	typedef int TypeInt8;

	typedef int TypeInt16;

	//< Файловое действие
	//< Номер комплекса
	typedef int TypeInt32;

	//< Временная метка
	//< Длительность сеанса
	//< Идентификатор источника данных
	//< Идентификатор задания
	//< Идентификатор сеанса
	//< Идентификатор сеанса транспортного уровня
	//< Номер канала
	//< Номер подканала
	//< Расстояние от базовой станции
	//< Код оператора связи
	typedef __int64 TypeInt64;

	//< Тип сеанса БД SKPD 1.0
	//< Тип системы БД SKPD 1.0
	//< Флаг незавершенного сеанса
	//< Направление
	//< Тип сеанса MEU.1.0
	typedef unsigned char TypeUInt8;

	//< Номер канала VCI
	//< Номер канала VPI
	//< Номер канала MID
	//< Номер канала CID
	//< Номер канала PID
	//< Протокол физического уровня
	//< Протокол канального уровня	
	//< Протокол сетевого уровня
	//< Протокол транспортного уровня
	//< Протокол сеансового уровня
	//< Протокол уровня представления
	//< Протокол прикладного уровня
	//< Порт отправителя
	//< Порт получателя
	//< Порт соединения к БД
	typedef unsigned short TypeUInt16;

	//< Номер приоритета
	//< Информационная скорость
	//< Символьная скорость
	//< Несущая частота, кГц
	//< Несущая частота обратного канала, кГц
	//< Несущая частота прямого канала, кГц
	typedef unsigned int TypeUInt32;

	//< Размер файла
	typedef unsigned __int64 TypeUInt64;

	//< Номер терминала отправителя
	//< Номер терминала получателя
	//< Тип сеанса (MEU)
	//< Тип системы
	//< TMSI отправителя
	struct Type8Int8
	{
		char name[8];
	};

	//< Тип сети
	//< Устройство регистрации
	//< Поляризация
	//< IMEI отправителя
	//< IMSI отправителя
	struct Type16Int8
	{
		char name[16];
	};

	//< Имя сети
	//< Стандарт
	//< Тематика
	//< Номер шлюза отправителя
	//< Номер шлюза получателя
	//< Название комплекса
	//< Оператор
	//< Тип кодека
	//< Вид оборудования
	//< Язык
	//< Кодировка
	//< Страна отправителя
	//< Страна получателя
	//< E-Mail отправителя
	//< E-Mail получателя
	//< UIN отправителя
	//< UIN получателя
	//< Имя отправителя
	//< Имя получателя
	//< Объект связи
	//< Тип системы
	//< Номер телефона отправителя
	//< Номер телефона получателя
	//< LAC "свой"
	//< LAC "чужой"
	//< CELLID "свой"
	//< CELLID "чужой"
	struct Type32Int8
	{
		char name[32];
	};

	//< Тип шифрования
	struct Type64Int8
	{
		char name[64];
	};

	//< Адрес соединения к БД
	//< Логин соединения к БД
	//< Пароль соединения к БД
	struct Type128Int8
	{
		char name[128];
	};

	//< Оригинальное имя файла
	struct Type256Int8
	{
		char name[256];
	};

	//< Ключ шифрования
	struct Type512Int8
	{
		char name[256];
	};

	//< Найденные ключевые слова
	//< Путь к БД
	//< Имя файла
	struct Type1024Int8
	{
		char name[1024];
	};

	struct Type2x1024Int8
	{
		char uplink[1024];
		char downlink[1024];
	};

	//< Долгота
	//< Широта
	typedef double TypeFloat;

	//< IPv4-адрес отправителя
	//< IPv4-адрес получателя
	struct Type4UInt8
	{
		unsigned char ipv4[4];
	};

	//< BSSID сети
	//< MAC-адрес отправителя
	//< МАС-адрес получателя
	struct Type6UInt8
	{
		unsigned char mac[6];
	};

	//< IPv6-адрес отправителя
	//< IPv6-адрес получателя
	struct Type16UInt8
	{
		unsigned char ipv6[16];
	};

	//< Файловая операция
	typedef enum
	{
		F_O_OPEN			= 0,
		F_O_CREATE			= 1,
		F_O_WRITE			= 2,
		F_O_CLOSE			= 3,
		F_O_MOVE			= 4,
		F_O_MKDIR			= 5
	}
	EFileOperation;
           
	struct TypeFileOperation
	{
		int h;						///< Дескриптор файла
		int o;                      ///< Тип файловой операции
		unsigned int b;				///< Начало последнего записанного блока
		unsigned int e;				///< Конец последнего записанного блока
		unsigned int off;			///< Смещение блока в файле
		unsigned int t;				///< Время последнего действия с файлом
	};

	struct TypeFileType
	{
        unsigned char type;
    };

	const char DHF_TCP_FLAG_FIN	= 0x01;
	const char DHF_TCP_FLAG_SYN	= 0x02;
	const char DHF_TCP_FLAG_RST	= 0x04;
	const char DHF_TCP_FLAG_PSH	= 0x08;
	const char DHF_TCP_FLAG_ACK	= 0x10;
	const char DHF_TCP_FLAG_URG	= 0x20;

	struct TypeSessionParams
	{
        unsigned int seq;
        unsigned int ack;
        unsigned char flags;
    };

	//< Номер группы логических каналов и номер логического канала X.25
	struct TypeLgnLcnX25
	{
		unsigned char lgn;
		unsigned char lcn;
	};

	const int DHF_DB_SKPD_TYPE_SESSION_UNKNOWN				= 0;
	const int DHF_DB_SKPD_TYPE_SESSION_VOICE				= 1;
	const int DHF_DB_SKPD_TYPE_SESSION_MAIL					= 2;
	const int DHF_DB_SKPD_TYPE_SESSION_DATA					= 3;
	const int DHF_DB_SKPD_TYPE_SESSION_MSG					= 4;
	const int DHF_DB_SKPD_TYPE_SESSION_FAX					= 5;

	const int DHF_DB_SKPD_TYPE_SYSTEM_TELEFONIYA			= 3;
	const int DHF_DB_SKPD_TYPE_SYSTEM_INTERNET				= 4;

    	
	inline char* getIdentifyName(unsigned char type)
	{
		switch( type )
		{
		case AID_MODULE:								return "Идентификаторы модуля";
		case AID_MODULE_AND_SCHEME:						return "Идентификаторы модуля и схемы";
		case AID_TIMESTAMP:								return "Временная метка";
		case AID_ID_EXT_SOURCE:							return "Идентификатор источника данных";
		case AID_ID_EXT_TASK:							return "Идентификатор задания";
		case AID_ID_EXT_SESSION:						return "Идентификатор сеанса";
		case AID_CHANNEL_NUMBER:						return "Номер канала";
		case AID_SUB_CHANNEL_NUMBER:					return "Номер подканала";
		case AID_CRYPT_KEY:								return "Ключ шифрования";
		case AID_WIFI_BSSID:							return "BSSID сети";
		case AID_NET_NAME:								return "Имя сети";
		case AID_NET_TYPE:								return "Тип сети";
		case AID_STANDART:								return "Стандарт";
		case AID_ATM_VCI:								return "Номер канала VCI";
		case AID_ATM_VPI:								return "Номер канала VPI";
		case AID_ATM_MID:								return "Номер канала MID";	
		case AID_ATM_CID:								return "Номер канала CID";
		case AID_MAC_SRC:								return "MAC-адрес отправителя";
		case AID_MAC_DST:								return "МАС-адрес получателя";
		case AID_IPV4_SRC:								return "IPv4-адрес отправителя";
		case AID_IPV4_DST:								return "IPv4-адрес получателя";
		case AID_IPV6_SRC:								return "IPv6-адрес отправителя";
		case AID_IPV6_DST:								return "IPv6-адрес получателя";
		case AID_PROTOCOL_PHYSICAL_LAYER:				return "Протокол физического уровня";
		case AID_PROTOCOL_DATA_LINK_LAYER:				return "Протокол канального уровня";	
		case AID_PROTOCOL_NETWORK_LAYER:				return "Протокол сетевого уровня";
		case AID_PROTOCOL_TRANSPORT_LAYER:				return "Протокол транспортного уровня";
		case AID_PROTOCOL_SESSION_LAYER:				return "Протокол сеансового уровня";
		case AID_PROTOCOL_PRESENTATION_LAYER:			return "Протокол уровня представления";
		case AID_PROTOCOL_APPLICATION_LAYER:			return "Протокол прикладного уровня";
		case AID_PORT_SRC:								return "Порт отправителя";
		case AID_PORT_DST:								return "Порт получателя";
		case AID_FILE_OPERATION_PARAMS:					return "Параметры файловой операции";
		case AID_ID_SESSION_TRANSPORT_LAYER:			return "Идентификатор сеанса транспортного уровня";
		case AID_SOURCE_NAME:							return "Устройство регистрации";
		case AID_DURATION:								return "Длительность сеанса";
		case AID_DB_SKPD_TYPE_SESSION:					return "Тип сеанса БД SKPD 1.0";
		case AID_DB_SKPD_TYPE_SYSTEM:					return "Тип системы БД SKPD 1.0";
		case AID_THEMES:								return "Тематика";
		case AID_TYPE_CRYPT:							return "Тип шифрования";
		case AID_INCOMPLETE_SESSION:					return "Флаг незавершенного сеанса";
		case AID_GATEWAY_SRC:							return "Номер шлюза отправителя";
		case AID_GATEWAY_DST:							return "Номер шлюза получателя";
		case AID_PRIORITY:								return "Номер приоритета";
		case AID_FILE_ACTION:							return "Файловая операция";
		case AID_DIRECTION:								return "Направление связи";
		case AID_FILE_SIZE:								return "Размер файла";
		case AID_SYSTEM_NUMBER:							return "Номер комплекса";
		case AID_SYSTEM_NAME:							return "Название комплекса";
		case AID_OPERATOR:								return "Оператор";
		case AID_INFO_SPEED:							return "Информационная скорость";
		case AID_SYMB_SPEED:							return "Символьная скорость";
		case AID_CARRIER_FREQ:							return "Несущая частота, кГц";
		case AID_CARRIER_FREQ_UP:						return "Несущая частота обратного канала, кГц";
		case AID_CARRIER_FREQ_DOWN:						return "Несущая частота прямого канала, кГц";
		case AID_POLARIZATION:							return "Поляризация";
		case AID_CODEC_TYPE:							return "Тип кодека";
		case AID_EQUIPMENT:								return "Вид оборудования";
		case AID_LANGUAGE:								return "Язык";
		case AID_ENCODING:								return "Кодировка";
		case AID_KEYWORD:								return "Найденные ключевые слова";
		case AID_ORIGINAL_FILENAME:						return "Оригинальное имя файла";
		case AID_COUNTRY_SRC:							return "Страна отправителя";
		case AID_COUNTRY_DST:							return "Страна получателя";
		case AID_EMAIL_SRC:								return "E-Mail отправителя";
		case AID_EMAIL_DST:								return "E-Mail получателя";
		case AID_UIN_SRC:								return "UIN отправителя";
		case AID_UIN_DST:								return "UIN получателя";
		case AID_NAME_SRC:								return "Имя отправителя";
		case AID_NAME_DST:								return "Имя получателя";
		case AID_TERMINATE_SRC:							return "Номер терминала отправителя";
		case AID_TERMINATE_DST:							return "Номер терминала получателя";
		case AID_CONNECTION_OBJECT:						return "Объект связи";
		case AID_SYSTEM_TYPE:							return "Тип системы";
		case AID_BD_CONNECTION_PATH:					return "Путь к БД";
		case AID_BD_CONNECTION_SERVER_ADDR:				return "Адрес соединения к БД";
		case AID_BD_CONNECTION_PORT:					return "Порт соединения к БД";
		case AID_BD_CONNECTION_USERNAME:				return "Логин соединения к БД";
		case AID_BD_CONNECTION_PASSWORD:				return "Пароль соединения к БД";
		case AID_LONGITUDE:								return "Долгота";
		case AID_LATITUDE:								return "Широта";
		case AID_SOURCE_FILENAME:						return "Имя файла источника";
        case AID_SOURCE_FILE_FORMAT:					return "Формат файла источника";
        case AID_NEXT_PROTOCOL_PCAP:					return "Следующий pcap-протокол";
        case AID_NEXT_PROTOCOL_UNKNOWN_LAYER:           return "Следующий протокол неизвестного уровня";
        case AID_NEXT_PROTOCOL_PHYSICAL_LAYER:          return "Следующий протокол физического уровня";
        case AID_NEXT_PROTOCOL_DATA_LINK_LAYER:         return "Следующий протокол канального уровня";
        case AID_NEXT_PROTOCOL_NETWORK_LAYER:           return "Следующий протокол сетевого уровня";
        case AID_NEXT_PROTOCOL_TRANSPORT_LAYER:         return "Следующий протокол транспортного уровня";
        case AID_NEXT_PROTOCOL_SESSION_LAYER:           return "Следующий протокол сеансового уровня";
        case AID_NEXT_PROTOCOL_PRESENTATION_LAYER:      return "Следующий протокол уровня представления";
        case AID_NEXT_PROTOCOL_APPLICATION_LAYER:       return "Следующий протокол прикладного уровня";
		case AID_SESSIONG_PARAMS:						return "Параметры протокола сеансового уровня";
		case AID_X25_LGN_LCN:							return "Номер группы логических каналов X.25";
		case AID_PID:									return "PID";
		case AID_NUMBER_SRC:							return "Номер телефона отправителя";
		case AID_NUMBER_DST:							return "Номер телефона получателя";
        case AID_FILENAME:								return "Имя файла";

		case AID_DB_MEU_TYPE_SESSION:					return "Тип сеанса MEU.1.0";
		case AID_DB_MEU_TYPE_SYSTEM:					return "Тип системы MEU.1.0";	//(GSM)
		case AID_IMEI_SRC:								return "IMEI отправителя";
		case AID_IMSI_SRC:								return "IMSI отправителя";
		case AID_TMSI_SRC:								return "TMSI отправителя";
		case AID_OPER_CODE:								return "Код оператора связи";
		case AID_LAC_SRC:								return "LAC отправителя";
		case AID_LAC_DST:								return "LAC получателя";
		case AID_CELLID_SRC:							return "CELLID отправителя";
		case AID_CELLID_DST:							return "CELLID получателя";
		case AID_DISTANCE_BASE_S:						return "Расстояние от базовой станции";
		case AID_DUPLEX_FILENAME:						return "Имена файлов трафика (дуплекс)";
        case AID_HARDWARE_CHANNEL:                      return "Номер аппаратного канала";
		};
		return "";
	}

	inline int getIdentifySize(unsigned char type)
	{
		switch( type )
		{
		case AID_MODULE:								return sizeof(TModuleAID);
		case AID_MODULE_AND_SCHEME:						return sizeof(TModuleSchemeAID);
		case AID_TIMESTAMP:								return sizeof(TypeInt64);
		case AID_ID_EXT_SOURCE:							return sizeof(TypeInt64);
		case AID_ID_EXT_TASK:							return sizeof(TypeInt64);
		case AID_ID_EXT_SESSION:						return sizeof(TypeInt64);
		case AID_CHANNEL_NUMBER:						return sizeof(TypeUInt64);
		case AID_SUB_CHANNEL_NUMBER:					return sizeof(TypeInt64);
		case AID_CRYPT_KEY:								return sizeof(Type512Int8);
		case AID_WIFI_BSSID:							return sizeof(Type6UInt8);
		case AID_NET_NAME:								return sizeof(Type32Int8);
		case AID_NET_TYPE:								return sizeof(Type16Int8);
		case AID_STANDART:								return sizeof(Type32Int8);
		case AID_ATM_VCI:								return sizeof(TypeUInt16);
		case AID_ATM_VPI:								return sizeof(TypeUInt16);
		case AID_ATM_MID:								return sizeof(TypeUInt16);	
		case AID_ATM_CID:								return sizeof(TypeUInt8);
		case AID_MAC_SRC:								return sizeof(Type6UInt8);
		case AID_MAC_DST:								return sizeof(Type6UInt8);
		case AID_IPV4_SRC:								return sizeof(Type4UInt8);
		case AID_IPV4_DST:								return sizeof(Type4UInt8);
		case AID_IPV6_SRC:								return sizeof(Type16UInt8);
		case AID_IPV6_DST:								return sizeof(Type16UInt8);
		case AID_PROTOCOL_PHYSICAL_LAYER:				return sizeof(TypeUInt16);
		case AID_PROTOCOL_DATA_LINK_LAYER:				return sizeof(TypeUInt16);
		case AID_PROTOCOL_NETWORK_LAYER:				return sizeof(TypeUInt16);
		case AID_PROTOCOL_TRANSPORT_LAYER:				return sizeof(TypeUInt16);
		case AID_PROTOCOL_SESSION_LAYER:				return sizeof(TypeUInt16);
		case AID_PROTOCOL_PRESENTATION_LAYER:			return sizeof(TypeUInt16);
		case AID_PROTOCOL_APPLICATION_LAYER:			return sizeof(TypeUInt16);
		case AID_PORT_SRC:								return sizeof(TypeUInt16);
		case AID_PORT_DST:								return sizeof(TypeUInt16);
		case AID_FILE_OPERATION_PARAMS:					return sizeof(TypeFileOperation);
		case AID_ID_SESSION_TRANSPORT_LAYER:			return sizeof(TypeUInt64);
		case AID_SOURCE_NAME:							return sizeof(Type32Int8);
		case AID_DURATION:								return sizeof(TypeUInt32);
		case AID_DB_SKPD_TYPE_SESSION:					return sizeof(TypeUInt8);
		case AID_DB_SKPD_TYPE_SYSTEM:					return sizeof(TypeUInt8);
		case AID_THEMES:								return sizeof(Type32Int8);
		case AID_TYPE_CRYPT:							return sizeof(Type32Int8);
		case AID_INCOMPLETE_SESSION:					return sizeof(TypeUInt8);
		case AID_GATEWAY_SRC:							return sizeof(Type32Int8);
		case AID_GATEWAY_DST:							return sizeof(Type32Int8);
		case AID_PRIORITY:								return sizeof(TypeInt32);
		case AID_FILE_ACTION:							return sizeof(TypeInt32);
		case AID_DIRECTION:								return sizeof(TypeUInt8);
		case AID_FILE_SIZE:								return sizeof(TypeUInt64);
		case AID_SYSTEM_NUMBER:							return sizeof(TypeUInt32);
		case AID_SYSTEM_NAME:							return sizeof(Type32Int8);
		case AID_OPERATOR:								return sizeof(Type32Int8);
		case AID_INFO_SPEED:							return sizeof(TypeInt32);
		case AID_SYMB_SPEED:							return sizeof(TypeInt32);
		case AID_CARRIER_FREQ:							return sizeof(TypeInt32);
		case AID_CARRIER_FREQ_UP:						return sizeof(TypeInt32);
		case AID_CARRIER_FREQ_DOWN:						return sizeof(TypeInt32);
		case AID_POLARIZATION:							return sizeof(Type16Int8);
		case AID_CODEC_TYPE:							return sizeof(Type32Int8);
		case AID_EQUIPMENT:								return sizeof(Type32Int8);
		case AID_LANGUAGE:								return sizeof(Type32Int8);
		case AID_ENCODING:								return sizeof(Type32Int8);
		case AID_KEYWORD:								return sizeof(Type1024Int8);
		case AID_ORIGINAL_FILENAME:						return sizeof(Type256Int8);
		case AID_COUNTRY_SRC:							return sizeof(Type32Int8);
		case AID_COUNTRY_DST:							return sizeof(Type32Int8);
		case AID_EMAIL_SRC:								return sizeof(Type128Int8);
		case AID_EMAIL_DST:								return sizeof(Type128Int8);
		case AID_UIN_SRC:								return sizeof(Type16Int8);
		case AID_UIN_DST:								return sizeof(Type16Int8);
		case AID_NAME_SRC:								return sizeof(Type32Int8);
		case AID_NAME_DST:								return sizeof(Type32Int8);
		case AID_TERMINATE_SRC:							return sizeof(Type8Int8);
		case AID_TERMINATE_DST:							return sizeof(Type8Int8);
		case AID_CONNECTION_OBJECT:						return sizeof(Type32Int8);
		case AID_SYSTEM_TYPE:							return sizeof(Type32Int8);
		case AID_BD_CONNECTION_PATH:					return sizeof(Type1024Int8);
		case AID_BD_CONNECTION_SERVER_ADDR:				return sizeof(Type128Int8);
		case AID_BD_CONNECTION_PORT:					return sizeof(TypeUInt16);
		case AID_BD_CONNECTION_USERNAME:				return sizeof(Type128Int8);
		case AID_BD_CONNECTION_PASSWORD:				return sizeof(Type128Int8);
		case AID_LONGITUDE:								return sizeof(TypeFloat);
		case AID_LATITUDE:								return sizeof(TypeFloat);
		case AID_SOURCE_FILENAME:						return sizeof(Type1024Int8);
        case AID_SOURCE_FILE_FORMAT:					return sizeof(TypeUInt8);
        case AID_NEXT_PROTOCOL_PCAP:					return sizeof(TypeUInt32);
        case AID_NEXT_PROTOCOL_UNKNOWN_LAYER:           return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_PHYSICAL_LAYER:          return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_DATA_LINK_LAYER:         return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_NETWORK_LAYER:           return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_TRANSPORT_LAYER:         return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_SESSION_LAYER:           return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_PRESENTATION_LAYER:      return sizeof(TypeUInt16);
        case AID_NEXT_PROTOCOL_APPLICATION_LAYER:       return sizeof(TypeUInt16);
		case AID_SESSIONG_PARAMS:						return sizeof(TypeSessionParams);
		case AID_X25_LGN_LCN:							return sizeof(TypeLgnLcnX25);
		case AID_PID:									return sizeof(TypeUInt16);
		case AID_NUMBER_SRC:							return sizeof(Type32Int8);
		case AID_NUMBER_DST:							return sizeof(Type32Int8);
		case AID_FILENAME:								return sizeof(Type1024Int8);

		case AID_DB_MEU_TYPE_SESSION:					return sizeof(Type8Int8);
		case AID_DB_MEU_TYPE_SYSTEM:					return sizeof(Type8Int8);
		case AID_IMEI_SRC:								return sizeof(Type16Int8);
		case AID_IMSI_SRC:								return sizeof(Type16Int8);
		case AID_TMSI_SRC:								return sizeof(Type8Int8);
		case AID_OPER_CODE:								return sizeof(TypeInt64);
		case AID_LAC_SRC:								return sizeof(Type32Int8);
		case AID_LAC_DST:								return sizeof(Type32Int8);
		case AID_CELLID_SRC:							return sizeof(Type32Int8);
		case AID_CELLID_DST:							return sizeof(Type32Int8);
		case AID_DISTANCE_BASE_S:						return sizeof(TypeInt64);
		case AID_DUPLEX_FILENAME:						return sizeof(Type2x1024Int8);

        case AID_HARDWARE_CHANNEL:                      return sizeof(TypeUInt64);
        };
		return 0;
	}

    //----------------------    Форматы файлов      ----------------------------
    
    const unsigned char FILE_TYPE_FORMAT_UNKNOWN            = 0;
    const unsigned char FILE_TYPE_FORMAT_SIG                = 1;
    const unsigned char FILE_TYPE_FORMAT_LSIG               = 2;
    const unsigned char FILE_TYPE_FORMAT_PCAP               = 3;
    const unsigned char FILE_TYPE_FORMAT_SIG_CHANNEL        = 4;
    const unsigned char FILE_TYPE_FORMAT_LSIG_CHANNEL       = 5;
    const unsigned char FILE_TYPE_FORMAT_LSIG_DHF           = 6;
    const unsigned char FILE_TYPE_FORMAT_COUNT              = 7;

    
    inline int indexFileFormat(const char* str_file_format)
    {
        std::string str = str_file_format;
        if( !str.compare("bit") )                return FILE_TYPE_FORMAT_UNKNOWN;
        if( !str.compare("sig") )                return FILE_TYPE_FORMAT_SIG;
        if( !str.compare("lsig") )               return FILE_TYPE_FORMAT_LSIG;
        if( !str.compare("pcap") )               return FILE_TYPE_FORMAT_PCAP;
        if( !str.compare("sig channel") )        return FILE_TYPE_FORMAT_SIG_CHANNEL;
        if( !str.compare("lsig channel") )       return FILE_TYPE_FORMAT_LSIG_CHANNEL;
        if( !str.compare("lsig dhf") )           return FILE_TYPE_FORMAT_LSIG_DHF;
        return 0;
    }

    inline char* strFileFormat(int index)
    {
        switch( index )
        {
        case FILE_TYPE_FORMAT_UNKNOWN:        return "bit";
        case FILE_TYPE_FORMAT_SIG:            return "sig";
        case FILE_TYPE_FORMAT_LSIG:           return "lsig";
        case FILE_TYPE_FORMAT_PCAP:           return "pcap";
        case FILE_TYPE_FORMAT_SIG_CHANNEL:    return "sig channel";
        case FILE_TYPE_FORMAT_LSIG_CHANNEL:   return "lsig channel";
        case FILE_TYPE_FORMAT_LSIG_DHF:       return "lsig dhf";
        }
        return "bin";
    }
    
    //--------------------------------------------------------------------------
};

#pragma pop(pack)

#endif //__DHF_IDENTIFY_DEFS_H__