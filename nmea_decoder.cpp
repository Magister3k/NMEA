#include "nmea_decoder.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace
{
const char kShortName[] = "Декодер NMEA";
const char kLongName[] = "Декодер сообщений NMEA/AIS (0183/61162-450)";
const char kGroupName[] = "PineCode Lab";
}

NMEA_Decoder::NMEA_Decoder()
    : m_validMsgs(0), m_rejectedMsgs(0)
{
    std::strncpy(aShortName, kShortName, sizeof(aShortName) - 1);
    std::strncpy(aLongName, kLongName, sizeof(aLongName) - 1);
    std::strncpy(aGroupName, kGroupName, sizeof(aGroupName) - 1);
    std::snprintf(aVersion, sizeof(aVersion), "%s.%s", MODULE_INTERFACE_VERSION, MODULE_RELEASE_VERSION);
    bIsInputModule = false;
    byteTypeData = true;
    typeData = '\n';
}

NMEA_Decoder::~NMEA_Decoder()
{
    Free();
    Destructor();
}

bool __stdcall NMEA_Decoder::Constructor(unsigned __int64 uid)
{
    moduleUid = static_cast<unsigned short>(uid);
    return true;
}

void __stdcall NMEA_Decoder::Destructor()
{
    m_lineBuffer.clear();
    m_src.clear();
}

bool __stdcall NMEA_Decoder::CreateGui(IfaceCallBackGui* ig)
{
    ifaceGui = ig;
    return true;
}

bool __stdcall NMEA_Decoder::CreateProc(IfaceCallBackProc* ip)
{
    ifaceProc = ip;
    return ip != NULL;
}

bool __stdcall NMEA_Decoder::Initialize()
{
    recv = 0;
    send = 0;
    m_validMsgs = 0;
    m_rejectedMsgs = 0;
    m_lineBuffer.clear();

    m_nmea450.SetOnMsgAssembled([this](const std::string& msg, const std::string& src) {
        ProcMsg(msg, src);
    });
    return true;
}

void __stdcall NMEA_Decoder::Free()
{
    m_lineBuffer.clear();
}

void __stdcall NMEA_Decoder::CreateForm() {}
void __stdcall NMEA_Decoder::DestroyForm() {}
void __stdcall NMEA_Decoder::ShowForm(bool show) { (void)show; }
void __stdcall NMEA_Decoder::setGuiModuleSettings(SAppModuleSettings* sets) { (void)sets; }
void __stdcall NMEA_Decoder::setGuiModuleOptions(SAppModuleOptions* opts) { (void)opts; }
void __stdcall NMEA_Decoder::setProcModuleSettings(SAppModuleSettings* sets) { (void)sets; }
void __stdcall NMEA_Decoder::setProcModuleOptions(SAppModuleOptions* opts) { (void)opts; }

SAppModuleSettings* __stdcall NMEA_Decoder::getModuleParamsSetting(SAppModuleSettings* sets)
{
    if (sets == NULL) sets = new SAppModuleSettings;
    sets->cnt_params = 0;
    sets->params = NULL;
    return sets;
}

SAppModuleOptions* __stdcall NMEA_Decoder::getModuleParamsOptions(SAppModuleOptions* opts)
{
    if (opts == NULL) opts = new SAppModuleOptions;
    opts->cnt_arg = 0;
    opts->args = NULL;
    return opts;
}

void __stdcall NMEA_Decoder::getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts)
{
    if (sets != NULL) {
        std::strncpy(sets->name, "NMEA decoder", sizeof(sets->name) - 1);
        sets->cnt_params = 0;
        sets->params = NULL;
    }
    if (opts != NULL) {
        opts->cnt_arg = 0;
        opts->args = NULL;
    }
}

void __stdcall NMEA_Decoder::setProcState(bool state)
{
    IModuleExt::setProcState(state);
}

bool __stdcall NMEA_Decoder::procGuiData(unsigned short type, void* data, int len)
{
    (void)type;
    (void)data;
    (void)len;
    return true;
}

bool __stdcall NMEA_Decoder::HookMsg(int wParam, int lParam)
{
    (void)wParam;
    (void)lParam;
    return true;
}

bool __stdcall NMEA_Decoder::workProc(bool rSleep)
{
    (void)rSleep;
    return true;
}

bool __stdcall NMEA_Decoder::workData(char* idsData, int idsDataLen, char* data, int len)
{
    if (data == NULL || len <= 0 || ifaceProc == NULL) return false;

    recv += static_cast<unsigned __int64>(len);
    if (idsData != NULL && idsDataLen > 0) {
        ifaceProc->PutPrevIdentify(idsData, idsDataLen);
    }

    if (len >= 6 && std::memcmp(data, "UdPbC\0", 6) == 0) {
        m_nmea450.ProcPacket(reinterpret_cast<const uint8_t*>(data), static_cast<size_t>(len));
    } else {
        ProcRawBytes(data, len);
    }
    return true;
}

void __stdcall NMEA_Decoder::timing()
{
    m_nmea450.CleanupTimeouts();
}

bool __stdcall NMEA_Decoder::recvGuiData(unsigned short type, char* data, int len)
{
    if (ifaceProc != NULL) ifaceProc->SendGuiData(type, data, len);
    delete[] data;
    return true;
}

void __stdcall NMEA_Decoder::updateStat()
{
    char msg[128];
    std::snprintf(msg, sizeof(msg), "valid=%llu rejected=%llu",
                  static_cast<unsigned long long>(m_validMsgs),
                  static_cast<unsigned long long>(m_rejectedMsgs));
    SendStats(msg);
}

void NMEA_Decoder::ProcRawBytes(const char* data, int len)
{
    m_lineBuffer.append(data, static_cast<size_t>(len));
    size_t lineEnd = std::string::npos;
    while ((lineEnd = m_lineBuffer.find_first_of("\r\n")) != std::string::npos) {
        std::string line = m_lineBuffer.substr(0, lineEnd);
        size_t consume = lineEnd;
        while (consume < m_lineBuffer.size() &&
               (m_lineBuffer[consume] == '\r' || m_lineBuffer[consume] == '\n')) {
            ++consume;
        }
        m_lineBuffer.erase(0, consume);
        if (!line.empty()) ProcMsg(line, m_src);
    }
}

void NMEA_Decoder::ProcMsg(const std::string& msg, const std::string& src)
{
    std::string clean = msg;
    while (!clean.empty() && (clean.back() == '\r' || clean.back() == '\n')) clean.pop_back();
    if (clean.empty()) return;
    m_src = src;

    bool parsed = false;
    m_decoder.SetOnHeaderParsed([&parsed](const NmeaHeaderInfo&) { parsed = true; });
    m_decoder.SetOnStandardMsg([](const std::string&, const std::string&, const std::vector<std::string>&) {});
    m_decoder.SetOnAisDetected([](const std::string&) {});
    m_decoder.ParseMsg(clean);

    if (parsed && SendMsg(clean)) {
        ++m_validMsgs;
    } else {
        ++m_rejectedMsgs;
    }
}

bool NMEA_Decoder::SendMsg(const std::string& msg)
{
    if (ifaceProc == NULL) return false;
    std::string packet = msg + "\r\n";
    int result = ifaceProc->SendDataProc(&packet[0], static_cast<int>(packet.size()));
    if (result > 0) send += static_cast<unsigned __int64>(result);
    return result == static_cast<int>(packet.size());
}

void NMEA_Decoder::SendStats(const char* msg)
{
    if (ifaceProc != NULL) ifaceProc->SendStatString(const_cast<char*>(msg));
}

extern "C" DHF_IFACE_MODULE_EXT_DLL IModuleExt* __stdcall getIModuleExt()
{
    return new NMEA_Decoder();
}

extern "C" DHF_IFACE_MODULE_EXT_DLL void __stdcall removeIModuleExt(IModuleExt* module)
{
    delete module;
}