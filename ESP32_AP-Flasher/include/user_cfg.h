#include "string"
#include "util.h"


struct UserConfig {
    bool enable;
    char user[32];
    char password[32];
};

extern UserConfig userconfig;
extern void initUserconfig();
extern void saveUserConfig();
extern JsonDocument getUserConfig();
