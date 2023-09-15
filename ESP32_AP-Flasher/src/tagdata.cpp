#include "tagdata.h"

#include "tag_db.h"
#include "util.h"

std::unordered_map<size_t, TagData::Parser> TagData::parsers = {};

void TagData::loadParsers(const String& filename) {
    Serial.println("Reading parsers from file");
    const long start = millis();

    Storage.begin();
    fs::File file = contentFS->open(filename, "r");
    if (!file) {
        Serial.println("loadParsers: Failed to open file");
        return;
    }

    if (file.find("[")) {
        StaticJsonDocument<1000> doc;
        bool parsing = true;
        while (parsing) {
            DeserializationError err = deserializeJson(doc, file);
            if (!err) {
                const JsonObject parserDoc = doc[0];
                const auto& id = parserDoc["id"];
                const auto& name = parserDoc["name"];
                if (!id || !name) {
                    Serial.printf("Error: Parser must have name and id\n");
                    continue;
                }

                Parser parser;
                parser.name = name.as<String>();

                for (const auto& parserField : parserDoc["parser"].as<JsonArray>()) {
                    const uint8_t type = parserField["type"].as<uint8_t>();
                    if (type >= (uint8_t)Type::MAX) {
                        Serial.printf("Error: Type %d is not a valid tag data parser data type\n", type);
                        continue;
                    }

                    const auto& mult = parserField["mult"];
                    const uint8_t decimals = parserField["decimals"].as<uint8_t>();
                    if (mult) {
                        parser.fields.emplace_back(parserField["name"].as<String>(),
                                                   static_cast<Type>(type),
                                                   parserField["length"].as<uint8_t>(),
                                                   decimals,
                                                   std::make_optional(mult.as<double>()));
                    } else {
                        parser.fields.emplace_back(parserField["name"].as<String>(),
                                                   static_cast<Type>(type),
                                                   parserField["length"].as<uint8_t>(),
                                                   decimals);
                    }
                }

                parsers.emplace(id.as<uint8_t>(), parser);
            } else {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(err.c_str());
                parsing = false;
            }
            parsing = parsing && file.find(",");
        }
    }

    file.close();
    Serial.printf("Loaded %d parsers in %d ms\n", parsers.size(), millis() - start);
}

void TagData::parse(const uint8_t src[8], const size_t id, const uint8_t* data, const uint8_t len) {
    char buffer[64];

    const auto it = parsers.find(id);
    if (it == parsers.end()) {
        sprintf(buffer, "Error: No parser with id %d found(%d)\n", id, parsers.size());
        wsLog((String)buffer);

        Serial.printf("Error: No parser with id %d found\n", id);
        return;
    }

    const String mac = util::formatString<64>(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X.", src[7], src[6], src[5], src[4], src[3], src[2], src[1], src[0]);

    uint16_t offset = 0;
    for (const Field& field : it->second.fields) {
        const String& name = field.name;
        const uint8_t length = field.length;

        if (offset + length > len) {
            sprintf(buffer, "Error: Not enough data for field %s", name.c_str());
            wsLog((String)buffer);

            Serial.printf("Error: Not enough data for field %s\n", name.c_str());
            return;
        }

        const Type type = field.type;
        const uint8_t* fieldData = data + offset;
        offset += length;
        String value = "";
        switch (type) {
            case Type::INT: {
                const double mult = field.mult.value_or(1.0);
                value = String(static_cast<int32_t>(bytesToInt(fieldData, length)) * mult, (unsigned int)field.decimals);
            } break;
            case Type::UINT: {
                const double mult = field.mult.value_or(1.0f);
                value = String(bytesToInt(fieldData, length) * mult, (unsigned int)field.decimals);
            } break;
            case Type::FLOAT: {
                const double mult = field.mult.value_or(1.0f);

                if (length == 4) {
                    value = String(bytesToFloat(fieldData) * mult, (unsigned int)field.decimals);
                } else if (length == 8) {
                    value = String(bytesToDouble(fieldData) * mult, (unsigned int)field.decimals);
                } else {
                    sprintf(buffer, "Error: Float can only be 4 or 8 bytes long");
                    wsLog((String)buffer);
                    Serial.printf("Error: Float can only be 4 or 8 bytes long\n");
                }
            } break;
            case Type::STRING: {
                value = bytesToString(fieldData, length);
            } break;

            default:
                sprintf(buffer, "Error: Type %d not implemented", type);
                wsLog((String)buffer);
                Serial.printf("Error: Type %d not implemented\n", type);
                break;
        }

        if (value.isEmpty()) {
            sprintf(buffer, "Error: Empty value for field %s", name.c_str());
            wsLog((String)buffer);
            Serial.printf("Error: Empty value for field %s\n", name.c_str());
            continue;
        }

        const std::string varName = (mac + name).c_str();
        setVarDB(varName, value);

        sprintf(buffer, "Set %s to %s", varName.c_str(), value.c_str());
        wsLog((String)buffer);
        Serial.printf("Set %s to %s\n", varName.c_str(), value.c_str());
    }
}
