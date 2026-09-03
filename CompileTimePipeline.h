#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include "NmeaPositionStructures.h"

// Опережающее объявление нашего изолированного бинарного декодера АИС
class ais_decoder;

template <typename AisDecoder, typename GeoParser>
class FastNmeaDecoder {
public:
    FastNmeaDecoder(AisDecoder& ais, GeoParser& geo) : m_ais(ais), m_geo(geo) {}

    inline void ParseSentence(const std::string& sentence) {
        if (sentence.length() < 7) return;

        // Нарезаем на поля по запятым "in-place" без аллокаций
        std::vector<std::string> fields = SplitString(sentence, ',');
        if (fields.empty()) return;

        const std::string& header = fields[0]; // $GPGGA, !AIVDM, $PASHR и т.д.

        // 1. ДИСПЕТЧЕРИЗАЦИЯ ИНКАПСУЛИРОВАННОГО БИНАРНОГО ТРАФИКА (!)
        if (sentence[0] == '!') {
            if (header.length() < 6) return;
            std::string type = header.substr(3, 3); // VDM или VDO
            
            // Обрабатываем !AIVDM, !AIVDO, !B2VDM, !B2VDO, !BSVDM, !BSVDO
            if ((type == "VDM" || type == "VDO") && fields.size() >= 6) {
                // Полезная нагрузка AIS всегда в 5-м поле (индекс 5)
                m_ais.DecodePayload(fields[5]); 
            }
            return;
        }

        // 2. ДИСПЕТЧЕРИЗАЦИЯ СТАНДАРТНОГО И ПРОПРИЕТАРНОГО ГЕО-ТРАФИКА ($)
        if (sentence[0] == '$') {
            m_geo.ParseIncomingSentence(header, fields, sentence);
        }
    }

private:
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        if (!str.empty() && str.back() == delimiter) tokens.push_back("");
        return tokens;
    }

    AisDecoder& m_ais;
    GeoParser& m_geo;
};
