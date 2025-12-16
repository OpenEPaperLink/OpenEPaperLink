#include "storage.h"
#include "user_cfg.h"
#include "util.h"

UserConfig userconfig;

void initUserconfig() {
    fs::File configFile = contentFS->open("/current/userconfig.json", "r");
    JsonDocument config;
    if (configFile) {
        DeserializationError error = deserializeJson(config, configFile);
        if (error) {
            configFile.close();
            Serial.println("failed to read userconfig.json. Using default config");
            Serial.println(error.c_str());
        }
        configFile.close();
    }
    userconfig.enable = config["enable"].is<bool>() ? config["enable"] : false;
    if (config["user"]) strlcpy(userconfig.user, config["user"], sizeof(userconfig.user));
    if (config["password"]) strlcpy(userconfig.password, config["password"], sizeof(userconfig.password));
}

void saveUserConfig() {
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    fs::File configFile = contentFS->open("/current/userconfig.json", "w");
    JsonDocument config;
    config["enable"] = userconfig.enable;
    config["user"] = userconfig.user;
    config["password"] = userconfig.password;
    serializeJsonPretty(config, configFile);
    configFile.close();
    xSemaphoreGive(fsMutex);
}

JsonDocument getUserConfig() { 
    xSemaphoreTake(fsMutex, portMAX_DELAY); 
    fs::File configFile = contentFS->open("/current/userconfig.json", "r"); 
    JsonDocument config; 
    
    if (configFile) { 
        DeserializationError error = deserializeJson(config, configFile); 
        configFile.close(); 
        
        if (error) { 
            config.clear(); 
        } 
    } else { 
        config.clear(); 
    } 
    
    xSemaphoreGive(fsMutex);
    return config; 
}
