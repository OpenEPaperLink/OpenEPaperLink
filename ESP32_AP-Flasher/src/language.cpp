#include "language.h"

#include <Arduino.h>

#include "settings.h"
#include "tag_db.h"

int defaultLanguage = 0;

String languageList[] = {"EN - English", "NL - Nederlands", "DE - Deutsch", "NO - Norwegian", "FR - French"};

/*EN English language section*/
String languageEnDaysShort[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
String languageEnDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String languageEnMonth[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
/*END English language section END*/

/*NL Dutch language section*/
String languageNlDaysShort[] = {"ZO", "MA", "DI", "WO", "DO", "VR", "ZA"};
String languageNlDays[] = {"zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag"};
String languageNlMonth[] = {"januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december"};
/*END Dutch language section END*/

/*DE German language section*/
String languageDeDaysShort[] = {"SO", "MO", "DI", "MI", "DO", "FR", "SA"};
String languageDeDays[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
String languageDeMonth[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
/*END German language section END*/

/*NO Norwegian language section*/
String languageNoDaysShort[] = {"SØ", "MA", "TI", "ON", "TO", "FR", "LØ"};
String languageNoDays[] = {"Søndag", "Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Lørdag"};
String languageNoMonth[] = {"Januar", "Februar", "Mars", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Desember"};
/*END Norwegian language section END*/

/*FR French language section*/
String languageFrDaysShort[] = {"DI", "LU", "MA", "ME", "JE", "VE", "SA"};
String languageFrDays[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
String languageFrMonth[] = {"Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"};
/*END French language section END*/

String* languageDaysShort[] = {languageEnDaysShort, languageNlDaysShort, languageDeDaysShort, languageNoDaysShort, languageFrDaysShort};
String* languageDays[] = {languageEnDays, languageNlDays, languageDeDays, languageNoDays, languageFrDays};
String* languageMonth[] = {languageEnMonth, languageNlMonth, languageDeMonth, languageNoMonth, languageFrMonth};

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
