#include "language.h"

#include <Arduino.h>

#include "settings.h"
#include "tag_db.h"

int currentLanguage = defaultLanguage;

void updateLanguageFromConfig() {
    int tempLang = APconfig["language"].as<int>();
    if (tempLang < 0 || tempLang >= sizeof(languageList)) {
        Serial.println("Language not supported");
        return;
    }
    currentLanguage = tempLang;
    Serial.println("New set Language: " + languageList[currentLanguage]);
}

int getDefaultLanguage() {
    return defaultLanguage;
}

int getCurrentLanguage() {
    return currentLanguage;
}
