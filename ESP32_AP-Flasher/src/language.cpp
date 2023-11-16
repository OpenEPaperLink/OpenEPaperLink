#include "language.h"

#include <Arduino.h>

#include "settings.h"
#include "tag_db.h"

int defaultLanguage = 0;

String languageList[] = {"EN - English", "NL - Nederlands", "DE - Deutsch", "NO - Norwegian", "CZ - Czech", "SK - Slovak", "PL - Polish", "ES - Spanish"};

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

/*CZ Czech language section*/
String languageCzDaysShort[] = {"NE", "PO", "ÚT", "ST", "ČT", "PÁ", "SO"};
String languageCzDays[] = {"Neděle", "Pondělí", "Úterý", "Středa", "Čtvrtek", "Pátek", "Sobota"};
String languageCzMonth[] = {"Leden", "Únor", "Březen", "Duben", "Květen", "Červen", "Červenec", "Srpen", "Září", "Říjen", "Listopad", "Prosinec"};
/*END Czech language section END*/

/*SK Slovak language section*/
String languageSkDaysShort[] = {"NE", "PO", "UT", "ST", "ŠT", "PI", "SO"};
String languageSkDays[] = {"Nedeľa", "Pondelok", "Utorok", "Streda", "Štvrtok", "Piatok", "Sobota"};
String languageSkMonth[] = {"Január", "Február", "Marec", "Apríl", "Máj", "Jún", "Júl", "August", "September", "Oktober", "November", "December"};
/*END Slovak language section END*/

/*PL Polish language section*/
String languagePlDaysShort[] = {"Ni", "Po", "Wt", "Śr", "Cz", "Pt", "So"};
String languagePlDays[] = {"Niedziela", "Poniedziałek", "Wtorek", "Środa", "Czwartek", "Piątek", "Sobota"};
String languagePlMonth[] = {"Styczeń", "Luty", "Marzec", "Kwiecień", "Maj", "Czerwiec", "Lipiec", "Sierpień", "Wrzesień", "Październik", "Listopad", "Grudzień"};
/*END Polish language section END*/

/*ES Spanish language section*/
String languageEsDaysShort[] = {"D", "L", "MA", "MI", "J", "V", "S"};
String languageEsDays[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
String languageEsMonth[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
/*END Spanish language section END*/

String* languageDaysShort[] = {languageEnDaysShort, languageNlDaysShort, languageDeDaysShort, languageNoDaysShort, languageCzDaysShort, languageSkDaysShort, languagePlDaysShort, languageEsDaysShort};
String* languageDays[] = {languageEnDays, languageNlDays, languageDeDays, languageNoDays, languageCzDays, languageSkDays, languagePlDays, languageEsDays};
String* languageMonth[] = {languageEnMonth, languageNlMonth, languageDeMonth, languageNoMonth, languageCzMonth, languageSkMonth, languagePlMonth, languageEsMonth};

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
