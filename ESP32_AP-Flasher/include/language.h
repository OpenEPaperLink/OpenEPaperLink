#pragma once

#include <Arduino.h>

extern int defaultLanguage;

/*EN English language section*/
extern String languageEnDaysShort[];
extern String languageEnDays[];
extern String languageEnMonth[];
/*END English language section END*/

/*NL Dutch language section*/
extern String languageNlDaysShort[];
extern String languageNlDays[];
extern String languageNlMonth[];
/*END Dutch language section END*/

/*DE German language section*/
extern String languageDeDaysShort[];
extern String languageDeDays[];
extern String languageDeMonth[];
/*END German language section END*/

extern String* languageDaysShort[];
extern String* languageDays[];
extern String* languageMonth[];

extern void updateLanguageFromConfig();
extern int getDefaultLanguage();
extern int getCurrentLanguage();
