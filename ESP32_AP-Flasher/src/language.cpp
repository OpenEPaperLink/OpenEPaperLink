#include "language.h"

#include <Arduino.h>

#include "settings.h"
#include "tag_db.h"

int currentLanguage = defaultLanguage;

void updateLanguageFromConfig() {
    int tempLang = config.language;
    if (tempLang < 0 || tempLang >= sizeof(languageList)) {
        Serial.println("Language not supported");
        return;
    }
    currentLanguage = tempLang;
}

int getDefaultLanguage() {
    return defaultLanguage;
}

int getCurrentLanguage() {
    return currentLanguage;
}
