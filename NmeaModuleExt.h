#ifndef NMEA_MODULE_EXT_H
#define NMEA_MODULE_EXT_H

#define __DHF_RELEASE_IFACE_MODULE_EXT_H__
#include "DHF/_example/include/IfaceModuleExt.h"

#include "NMEA/nmea_decoder.h"
#include "NMEA/nmea450_decoder.h"

#include <string>

const char MODULE_INTERFACE_VERSION[16] = "0.0";
const char MODULE_RELEASE_VERSION[16] = "1.0";

class NmeaModuleExt : public IModuleExt
{
public:
    NmeaModuleExt();
    ~NmeaModuleExt();

    bool __stdcall Constructor(unsigned __int64 uid) override;
    void __stdcall Destructor() override;
    bool __stdcall CreateGui(IfaceCallBackGui* ig) override;
    bool __stdcall CreateProc(IfaceCallBackProc* ip) override;
    bool __stdcall Initialize() override;
    void __stdcall Free() override;

    void __stdcall CreateForm() override;
    void __stdcall DestroyForm() override;
    void __stdcall ShowForm(bool show = true) override;

    void __stdcall setGuiModuleSettings(SAppModuleSettings* sets) override;
    void __stdcall setGuiModuleOptions(SAppModuleOptions* opts) override;
    void __stdcall setProcModuleSettings(SAppModuleSettings* sets) override;
    void __stdcall setProcModuleOptions(SAppModuleOptions* opts) override;
    SAppModuleSettings* __stdcall getModuleParamsSetting(SAppModuleSettings* sets = NULL) override;
    SAppModuleOptions* __stdcall getModuleParamsOptions(SAppModuleOptions* opts = NULL) override;
    void __stdcall getDefaultModuleParams(SAppModuleSettings* sets, SAppModuleOptions* opts) override;

    void __stdcall setProcState(bool state) override;
    bool __stdcall procGuiData(unsigned short type, void* data, int length) override;
    bool __stdcall HookMessage(int wParam, int lParam) override;
    bool __stdcall workProc(bool rSleep) override;
    bool __stdcall workData(char* idsData, int idsDataLen, char* data, int length) override;
    void __stdcall timing() override;
    bool __stdcall recvGuiData(unsigned short type, char* data, int length) override;
    void __stdcall updateStatistic() override;

private:
    void ProcessSentence(const std::string& sentence, const std::string& source);
    void ProcessRawBytes(const char* data, int length);
    bool SendSentence(const std::string& sentence);
    void SendStatistics(const char* message);

    nmea_decoder m_decoder;
    nmea450_decoder m_nmea450;
    std::string m_lineBuffer;
    std::string m_source;
    unsigned __int64 m_validSentences;
    unsigned __int64 m_rejectedSentences;
};

#endif