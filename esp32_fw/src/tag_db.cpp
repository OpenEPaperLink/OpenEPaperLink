#include <Arduino.h>

#include "tag_db.h"
#include <ArduinoJson.h>
#include <vector>

std::vector<tagRecord*> tagDB;

tagRecord* tagRecord::findByMAC(uint8_t mac[6]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 6) == 0) {
            return tag;
        }
    }
    return nullptr;
}

String tagDBtoJson(uint8_t mac[6]) {
    DynamicJsonDocument doc(1000);
    JsonArray tags = doc.createNestedArray("tags");

    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);

		bool select = false;
		if (mac) {
            if (memcmp(taginfo->mac, mac, 6) == 0) {
                select = true;
            }
        } else {
			select = true;
		}
		if (select) {
			JsonObject tag = tags.createNestedObject();
			char buffer[64];
			sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", taginfo->mac[0], taginfo->mac[1], taginfo->mac[2], taginfo->mac[3], taginfo->mac[4], taginfo->mac[5]);
			tag["mac"] = (String)buffer;
			tag["lastseen"] = taginfo->lastseen;
            tag["nextupdate"] = taginfo->nextupdate;
            tag["model"] = taginfo->model;
			tag["pending"] = taginfo->pending;
            tag["button"] = taginfo->button;
            tag["alias"] = taginfo->alias;
            tag["contentmode"] = taginfo->contentMode;
        }
    }
    return doc.as<String>();
}