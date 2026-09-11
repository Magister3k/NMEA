#ifndef __PROTOCOL_INFO_H__
#define __PROTOCOL_INFO_H__

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

//Протоколы канального уровня
#define DHF_CHANNEL_PROTOCOL_UNKNOWN        0
#define DHF_CHANNEL_PROTOCOL_MPEG2          1
#define DHF_CHANNEL_PROTOCOL_HDLC           2
#define DHF_CHANNEL_PROTOCOL_CISCO_ROUTER   3
#define DHF_CHANNEL_PROTOCOL_FRAME_RELAY    4
#define DHF_CHANNEL_PROTOCOL_PPP            5
#define DHF_CHANNEL_PROTOCOL_802_11         6
#define DHF_CHANNEL_PROTOCOL_DTX_240        7
#define DHF_CHANNEL_PROTOCOL_VSAT           9 ///< Добавлен для работы со входными пакетами с комплекса SVAT
#define DHF_CHANNEL_PROTOCOL_ETHERNET_II    10
#define DHF_CHANNEL_PROTOCOL_ATM            11
#define DHF_CHANNEL_PROTOCOL_ATM_AAL1       12
#define DHF_CHANNEL_PROTOCOL_ATM_AAL2       13
#define DHF_CHANNEL_PROTOCOL_ATM_AAL3_4     14
#define DHF_CHANNEL_PROTOCOL_ATM_AAL5       15
#define DHF_CHANNEL_PROTOCOL_X25            16
#define DHF_CHANNEL_PROTOCOL_IDIRECT        17
#define DHF_CHANNEL_PROTOCOL_GPRS           18
#define DHF_CHANNEL_PROTOCOL_LLC			19

//Протоколы физического уровня (Ethernet)
#define DHF_ETHERNET_PROTOCOL_PPPoE                 0x8864
#define DHF_ETHERNET_PROTOCOL_IPv4                  0x0021
#define DHF_ETHERNET_PROTOCOL_IPv4_I                0x0800
#define DHF_ETHERNET_PROTOCOL_IPv6                  0x88dd
#define DHF_ETHERNET_PROTOCOL_IPv6_II               0x86dd
#define DHF_ETHERNET_PROTOCOL_802_1Q_VIRTUAL_LAN    0x8100
#define DHF_ETHERNET_PROTOCOL_PPP                   0x880B
#define DHF_ETHERTYPE_3GPP2	                        0x88d2
#define DHF_ETHERTYPE_CDMA2000_A10_UBS	            0x8881
#define DHF_GRE_WCCP	                            0x883E
//#define DHF_ETHERNET_PROTOCOL_802_1Q_VIRTUAL_LAN    0x8100

//Протоколы сетевого уровня
#define DHF_NETWORK_PROTOCOL_UNKNOWN                    0
#define DHF_NETWORK_PROTOCOL_IPv4                       1
#define DHF_NETWORK_PROTOCOL_IPv6                       2
#define DHF_NETWORK_PROTOCOL_IPX                        3
#define DHF_NETWORK_PROTOCOL_IP                         4
#define DHF_NETWORK_PROTOCOL_PPPoE                      5
#define DHF_NETWORK_PROTOCOL_PPP                        6
#define DHF_NETWORK_PROTOCOL_802_1Q_VIRTUAL_LAN         7
#define DHF_NETWORK_PROTOCOL_GRE                        8

//Значения поля типа протокола в заголовке IP в соответствии со стандартом
#define PROTOCOL_ICMP 1
#define PROTOCOL_IGMP 2
#define PROTOCOL_IP 4   //IP в IP (Инкапсуляция)
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17
#define PROTOCOL_XTP 36
#define PROTOCOL_IPv6 41    //IPv6 в IP (Инкапсуляция)
#define PROTOCOL_GRE 47
#define PROTOCOL_ESP 50
#define PROTOCOL_SWIPE 53   //IP c шифрованием

//HTTP
#define SPORT_HTTP80	80
#define SPORT_HTTP8080	8080
#define SPORT_HTTP3128	3128
#define SPORT_HTTPS	    443
//FTP
#define SPORT_FTP		20
#define DPORT_FTP		20
//MAIL
#define DPORT_SMTP		25
#define SPORT_POP3		110
#define SPORT_IMAP4		143
#define SPORT_NNTP		119
#define SPORT_IRC		6667
#define SPORT_IRC2		6669
//#define SPORT_IRC2      194
#define SPORT_MSN		1863
#define SPORT_OSCAR		5190
#define SPORT_LOTUS		1352
#define SPORT_YAHOO		5050
#define SPORT_YAHOO1    5101
#define SPORT_YAHOO2    1188
#define SPORT_JABBER    5222


//Значения протокола TCP (назначенные самим)
#define UNKNOWN_PROTOCOL 0
#define HTTP_PROTOCOL 1
#define NNTP_PROTOCOL 2
#define SMTP_PROTOCOL 3
#define IMAP4_PROTOCOL 4
#define POP3_PROTOCOL 5
#define FTP_PROTOCOL 6
#define IRC_PROTOCOL 7
#define OSCAR_PROTOCOL 8
#define MSN_PROTOCOL 9
#define YAHOO_PROTOCOL 10
#define LOTUS_PROTOCOL 11
#define JABBER_PROTOCOL 12
#define H323_PROTOCOL 20
#define RTP_PROTOCOL 21
#define SIP_PROTOCOL 22
#define ATM_PROTOCOL 23
#define HTTPS_PROTOCOL 24
#define SKINNY_PROTOCOL 25

int get_name_channel_protocol(unsigned short protocol, char* s, int l)
{
	int n = 0;
	if( protocol == DHF_CHANNEL_PROTOCOL_UNKNOWN ) { n = _snprintf(s, l, ""); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_MPEG2 ) { n = _snprintf(s, l, "MPEG-2"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_HDLC ) { n = _snprintf(s, l, "HDLC"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_CISCO_ROUTER ) { n = _snprintf(s, l, "Cisco Router"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_FRAME_RELAY ) { n = _snprintf(s, l, "Frame Relay"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_PPP ) { n = _snprintf(s, l, "PPP"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_802_11 ) { n = _snprintf(s, l, "IEEE 802.11"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_DTX_240 ) { n = _snprintf(s, l, "DTX-240"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_VSAT ) { n = _snprintf(s, l, "VSAT"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ETHERNET_II ) { n = _snprintf(s, l, "ETHERNET"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ATM ) { n = _snprintf(s, l, "ATM"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ATM_AAL1 ) { n = _snprintf(s, l, "ATM AAL1"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ATM_AAL2 ) { n = _snprintf(s, l, "ATM AAL2"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ATM_AAL3_4 ) { n = _snprintf(s, l, "ATM AAL3_4"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_ATM_AAL5 ) { n = _snprintf(s, l, "ATM AAL5"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_IDIRECT ) { n = _snprintf(s, l, "iDirect"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_X25 ) { n = _snprintf(s, l, "X.25"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_GPRS ) { n = _snprintf(s, l, "GPRS"); } else
	if( protocol == DHF_CHANNEL_PROTOCOL_LLC ) { n = _snprintf(s, l, "LLC"); } else
	{ n = _snprintf(s, l, "CP Unknown"); }
	return n;
}

int get_name_network_protocol(unsigned short protocol, char* s, int l)
{
	int n = 0;
	if( protocol == DHF_NETWORK_PROTOCOL_UNKNOWN ) { n = _snprintf(s, l, ""); } else
	if( protocol == DHF_NETWORK_PROTOCOL_IPv4 ) { n = _snprintf(s, l, "IPv4"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_IPv6 ) { n = _snprintf(s, l, "IPv6"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_IP ) { n = _snprintf(s, l, "IP"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_PPP ) { n = _snprintf(s, l, "PPP"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_PPPoE ) { n = _snprintf(s, l, "PPPoE"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_IPX ) { n = _snprintf(s, l, "IPX"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_802_1Q_VIRTUAL_LAN ) { n = _snprintf(s, l, "802.1Q Virtual Lan"); } else
	if( protocol == DHF_NETWORK_PROTOCOL_GRE ) { n = _snprintf(s, l, "GRE"); } else
	{ n = _snprintf(s, l, "NP Unknown"); }
    return n;
}

int get_name_trans_protocol(unsigned short protocol, char* s, int l)
{
	int n = 0;
	if( protocol == PROTOCOL_ICMP ) { n = _snprintf(s, l, "ICMP"); } else
	if( protocol == PROTOCOL_IGMP ) { n = _snprintf(s, l, "IGMP"); } else
	if( protocol == PROTOCOL_IP ) { n = _snprintf(s, l, "IP"); } else
	if( protocol == PROTOCOL_TCP ) { n = _snprintf(s, l, "TCP"); } else
	if( protocol == PROTOCOL_UDP ) { n = _snprintf(s, l, "UDP"); } else
	if( protocol == PROTOCOL_XTP ) { n = _snprintf(s, l, "XTP"); } else
	if( protocol == PROTOCOL_IPv6 ) { n = _snprintf(s, l, "IPv6"); } else
	if( protocol == PROTOCOL_GRE ) { n = _snprintf(s, l, "GRE"); } else
	if( protocol == PROTOCOL_ESP ) { n = _snprintf(s, l, "ESP"); } else
	if( protocol == PROTOCOL_SWIPE ) { n = _snprintf(s, l, "SWIPE"); } else
	{ n = _snprintf(s, l, "TP Unknown"); }
	return n;
}

int get_name_protocol(int protocol, char* s, int l)
{
	int n = 0;
	if( protocol == HTTP_PROTOCOL ) { n = _snprintf(s, l, "HTTP"); } else
	if( protocol == NNTP_PROTOCOL ) { n = _snprintf(s, l, "NNTP"); } else
	if( protocol == SMTP_PROTOCOL ) { n = _snprintf(s, l, "SMTP"); } else
	if( protocol == POP3_PROTOCOL ) { n = _snprintf(s, l, "POP3"); } else		
	if( protocol == IMAP4_PROTOCOL ) { n = _snprintf(s, l, "IMAP4"); } else
	if( protocol == FTP_PROTOCOL ) { n = _snprintf(s, l, "FTP"); } else
	if( protocol == IRC_PROTOCOL ) { n = _snprintf(s, l, "IRC"); } else
	if( protocol == OSCAR_PROTOCOL ) { n = _snprintf(s, l, "OSCAR"); } else
	if( protocol == MSN_PROTOCOL ) { n = _snprintf(s, l, "MSN"); } else
	if( protocol == YAHOO_PROTOCOL ) { n = _snprintf(s, l, "YAHOO"); } else
	if( protocol == LOTUS_PROTOCOL ) { n = _snprintf(s, l, "LOTUS"); } else
	if( protocol == JABBER_PROTOCOL ) { n = _snprintf(s, l, "JABBER"); } else
	if( protocol == H323_PROTOCOL ) { n = _snprintf(s, l, "H323/H225"); } else
    if( protocol == SKINNY_PROTOCOL ) { n = _snprintf(s, l, "SKINNY"); } else
    if( protocol == RTP_PROTOCOL ) { n = _snprintf(s, l, "RTP"); } else
    if( protocol == SIP_PROTOCOL ) { n = _snprintf(s, l, "SIP"); } else
    if( protocol == ATM_PROTOCOL ) { n = _snprintf(s, l, "ATM"); } else
    if( protocol == HTTPS_PROTOCOL ) { n = _snprintf(s, l, "HTTPS"); } else
	{ n = _snprintf(s, l, "AP Unknown"); }
	return n;
}

int get_protocol(std::string str_protocol)
{
    if( !str_protocol.compare("HTTP") )         return HTTP_PROTOCOL;
	if( !str_protocol.compare("NNTP") )         return NNTP_PROTOCOL;
	if( !str_protocol.compare("SMTP") )         return SMTP_PROTOCOL;
	if( !str_protocol.compare("POP3") )         return POP3_PROTOCOL;
	if( !str_protocol.compare("IMAP4") )        return IMAP4_PROTOCOL;
	if( !str_protocol.compare("FTP") )          return FTP_PROTOCOL;
	if( !str_protocol.compare("IRC") )          return IRC_PROTOCOL;
	if( !str_protocol.compare("OSCAR") )        return OSCAR_PROTOCOL;
	if( !str_protocol.compare("MSN") )          return MSN_PROTOCOL;
	if( !str_protocol.compare("YAHOO") )        return YAHOO_PROTOCOL;
	if( !str_protocol.compare("LOTUS") )        return LOTUS_PROTOCOL;
	if( !str_protocol.compare("JABBER") )       return JABBER_PROTOCOL;
	if( !str_protocol.compare("H323/H255") )    return H323_PROTOCOL;
    if( !str_protocol.compare("RTP") )          return RTP_PROTOCOL;
    if( !str_protocol.compare("SIP") )          return SIP_PROTOCOL;
    if( !str_protocol.compare("ATM") )          return ATM_PROTOCOL;
    if( !str_protocol.compare("HTTPS") )        return HTTPS_PROTOCOL;
	return 0;
}

#endif
