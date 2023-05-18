#pragma once

#include <Arduino.h>

static int defaultLanguage = 0;

static String languageList[] = {"EN - English", "NL - Nederlands", "DE - Deutsch"};

/*EN English language section*/
static String languageEnDaysShort[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
static String languageEnDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static String languageEnMonth[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
/*END English language section END*/

/*NL Dutch language section*/
static String languageNlDaysShort[] = {"ZO", "MA", "DI", "WO", "DO", "VR", "ZA"};
static String languageNlDays[] = {"zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag"};
static String languageNlMonth[] = {"januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december"};
/*END Dutch language section END*/

/*DE German language section*/
static String languageDeDaysShort[] = {"SO", "MO", "DI", "MI", "DO", "FR", "SA"};
static String languageDeDays[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
static String languageDeMonth[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
/*END German language section END*/

static String* languageDaysShort[] = {languageEnDaysShort, languageNlDaysShort, languageDeDaysShort};
static String* languageDays[] = {languageEnDays, languageNlDays, languageDeDays};
static String* languageMonth[] = {languageEnMonth, languageNlMonth, languageDeMonth};

void updateLanguageFromConfig();
int getDefaultLanguage();
int getCurrentLanguage();
