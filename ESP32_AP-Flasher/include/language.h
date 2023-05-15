#pragma once

#include <Arduino.h>

static int defaultLanguage = 0;

static String languageList[] = {"EN - English", "NL - Dutch", "DE - Deutsch"};

/*EN English language section*/
static String languageEnDaysShort[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
static String languageEnDays[] = {"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
static String languageEnMonth[] = {"january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
/*END English language section END*/

/*NL Dutch language section*/
static String languageNlDaysShort[] = {"ZO", "MA", "DI", "WO", "DO", "VR", "ZA"};
static String languageNlDays[] = {"zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag"};
static String languageNlMonth[] = {"januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december"};
/*END Dutch language section END*/

/*DE German language section*/
static String languageDeDaysShort[] = {"SO", "MO", "DI", "MI", "DO", "FR", "SA"};
static String languageDeDays[] = {"sonntag", "montag", "dienstag", "mittwoch", "donnerstag", "freitag", "samstag"};
static String languageDeMonth[] = {"januar", "februar", "maerz", "april", "mai", "juni", "juli", "august", "september", "oktober", "november", "dezember"};
/*END German language section END*/

static String* languageDaysShort[] = {languageEnDaysShort, languageNlDaysShort, languageDeDaysShort};
static String* languageDays[] = {languageEnDays, languageNlDays, languageDeDays};
static String* languageMonth[] = {languageEnMonth, languageNlMonth, languageDeMonth};

void updateLanguageFromConfig();
int getDefaultLanguage();
int getCurrentLanguage();
