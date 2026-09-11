#include "NmeaModuleExt.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace
{
const char kShortName[] = "NMEA decoder";
const char kLongName[] = "NMEA 0183/61162-450 decoder";
const char kGroupName[] = "Navigation";
}

NmeaModuleExt::NmeaModuleExt()
    : m_validSentences(0), m_rejectedSentences(0)
{
    std::strncpy(aShortName, kShortName, sizeof(aShortName) - 1);
    std::strncpy(aLongName, kLongName, sizeof(aLongName) - 1);
    std::strncpy(aGroupName, kGroupName, sizeof(aGroupName) - 1);
    std::snprintf(aVersion, sizeof(aVersion), "%s.%s", MODULE_INTERFACE_VERSION, MODULE_RELEASE_VERSION);
    bIsInputModule = false;
    byteTypeData = true;
    typeData = '\n';
}

NmeaModuleExt::~NmeaModuleExt()
{
    Free();
    Destructor();
}

bool __stdcall NmeaModuleExt::Constructor(unsigned __int64 uid)
{
    moduleUid = static_cast<unsigned short>(uid);
    return true;
}

void __stdcall NmeaModuleExt::Destructor()
{
    m_lineBuffer.clear();
    m_source.clear();
}

bool __stdcall NmeaModuleExt::CreateGui(IfaceCallBackGui* ig)
{
    ifaceGui = ig;
    return true;
}

bool __stdcall NmeaModuleExt::CreateProc(IfaceCallBackProc* ip)
{
    ifaceProc = ip;
    return ip != NULL;
}

bool __stdcall NmeaModuleExt::Initialize()
{
    recv = 0;
    send = 0;
    m_validSentences = 0;
    m_rejectedSentences = 0;
    m_lineBuffer.clear();

    m_nmea450.SetOnSentenceAssembled([this](const std::string& sentence, const std::string& source) {
        ProcessSentence(sentence, source);
    });
    return true;
}

void __stdcall NmeaModuleExt::Free()
{
    m_lineBuffer.clear();
}

void __stdcall NmeaModuleExt::CreateForm() {}
void __stdcall NmeaModuleExt::DestroyForm() {}
void __stdcall NmeaModuleExt::ShowForm(bool show) { (void)show; }
void __stdcall NmeaModuleExt::setGuiModuleSettings(SAppModuleSettings* sets) { (void)sets; }
void __stdcall NmeaModuleExt::setGuiModuleOptions(SAppModuleOptions* opts) { (void)opts; }
void __stdcall NmeaModuleExt::setProcModuleSettings(SAppModuleSettings* sets) { (void)sets; }
void __stdcall NmeaModuleExt::setProcModuleOptions(SAppModuleOptions* opts) { (void)opts; }

SAppModuleSettings* __stdcall NmeaModuleExt::getModuleParamsSetting(SAppModuleSettings* sets)
{
    if (sets == NULL) sets = new SAppModuleSettings;
    sets->cnt_params = 0;
    sets->params = NULL;
    return sets;
}

SAppModuleOptions* __stdcall NmeaModuleExt::getModuleParamsOptions(SAppModuleOptions* opts)
{
    if (opts == NULL) opts = new SAppModuleOptions;
    opts->cnt_arg = 0;
    opts->args = NULL;
    return opts;
}

void __stdcall NmeaModuleExt::getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts)
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

void __stdcall NmeaModuleExt::setProcState(bool state)
{
    IModuleExt::setProcState(state);
}

bool __stdcall NmeaModuleExt::procGuiData(unsigned short type, void* data, int length)
{
    (void)type;
    (void)data;
    (void)length;
    return true;
}

bool __stdcall NmeaModuleExt::HookMessage(int wParam, int lParam)
{
    (void)wParam;
    (void)lParam;
    return true;
}

bool __stdcall NmeaModuleExt::workProc(bool rSleep)
{
    (void)rSleep;
    return true;
}

bool __stdcall NmeaModuleExt::workData(char* idsData, int idsDataLen, char* data, int length)
{
    if (data == NULL || length <= 0 || ifaceProc == NULL) return false;

    recv += static_cast<unsigned __int64>(length);
    if (idsData != NULL && idsDataLen > 0) {
        ifaceProc->PutPrevIdentify(idsData, idsDataLen);
    }

    if (length >= 6 && std::memcmp(data, "UdPbC\0", 6) == 0) {
        m_nmea450.ProcessPacket(reinterpret_cast<const uint8_t*>(data), static_cast<size_t>(length));
    } else {
        ProcessRawBytes(data, length);
    }
    return true;
}

void __stdcall NmeaModuleExt::timing()
{
    m_nmea450.CleanupTimeouts();
}

bool __stdcall NmeaModuleExt::recvGuiData(unsigned short type, char* data, int length)
{
    if (ifaceProc != NULL) ifaceProc->SendGuiData(type, data, length);
    delete[] data;
    return true;
}

void __stdcall NmeaModuleExt::updateStatistic()
{
    char message[128];
    std::snprintf(message, sizeof(message), "valid=%llu rejected=%llu",
                  static_cast<unsigned long long>(m_validSentences),
                  static_cast<unsigned long long>(m_rejectedSentences));
    SendStatistics(message);
}

void NmeaModuleExt::ProcessRawBytes(const char* data, int length)
{
    m_lineBuffer.append(data, static_cast<size_t>(length));
    size_t lineEnd = std::string::npos;
    while ((lineEnd = m_lineBuffer.find_first_of("\r\n")) != std::string::npos) {
        std::string line = m_lineBuffer.substr(0, lineEnd);
        size_t consume = lineEnd;
        while (consume < m_lineBuffer.size() &&
               (m_lineBuffer[consume] == '\r' || m_lineBuffer[consume] == '\n')) {
            ++consume;
        }
        m_lineBuffer.erase(0, consume);
        if (!line.empty()) ProcessSentence(line, m_source);
    }
}

void NmeaModuleExt::ProcessSentence(const std::string& sentence, const std::string& source)
{
    std::string clean = sentence;
    while (!clean.empty() && (clean.back() == '\r' || clean.back() == '\n')) clean.pop_back();
    if (clean.empty()) return;
    m_source = source;

    bool parsed = false;
    m_decoder.SetOnHeaderParsed([&parsed](const NmeaHeaderInfo&) { parsed = true; });
    m_decoder.SetOnStandardMessage([](const std::string&, const std::string&, const std::vector<std::string>&) {});
    m_decoder.SetOnAisStringDetected([](const std::string&) {});
    m_decoder.ParseSentence(clean);

    if (parsed && SendSentence(clean)) {
        ++m_validSentences;
    } else {
        ++m_rejectedSentences;
    }
}

bool NmeaModuleExt::SendSentence(const std::string& sentence)
{
    if (ifaceProc == NULL) return false;
    std::string packet = sentence + "\r\n";
    int result = ifaceProc->SendDataProc(&packet[0], static_cast<int>(packet.size()));
    if (result > 0) send += static_cast<unsigned __int64>(result);
    return result == static_cast<int>(packet.size());
}

void NmeaModuleExt::SendStatistics(const char* message)
{
    if (ifaceProc != NULL) ifaceProc->SendStatString(const_cast<char*>(message));
}

extern "C" DHF_IFACE_MODULE_EXT_DLL IModuleExt* __stdcall getIModuleExt()
{
    return new NmeaModuleExt();
}

extern "C" DHF_IFACE_MODULE_EXT_DLL void __stdcall removeIModuleExt(IModuleExt* module)
{
    delete module;
}