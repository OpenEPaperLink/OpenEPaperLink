#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// #include <iostream>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../tagprofile_struct.h"

struct tagHardwareProfile* getProfileFromJson();

int main() {
    struct tagHardwareProfile* thwp = getProfileFromJson();
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X, xRes: %d\n", thwp->macAddress[0],thwp->macAddress[1],thwp->macAddress[2],thwp->macAddress[3],thwp->macAddress[4],thwp->macAddress[5],thwp->macAddress[6],thwp->macAddress[7], thwp->xRes);
    FILE* wptr = fopen("tagprofile.bin", "wb");
    fwrite((uint8_t*)thwp, sizeof(struct tagHardwareProfile), 1, wptr);
    fclose(wptr);
}

struct tagHardwareProfile* getProfileFromJson() {
    struct tagHardwareProfile* thwp = new struct tagHardwareProfile;

    std::ifstream f("tagprofile.json");
    json jsonData = json::parse(f);

    // Check if the 'mac' key exists in the JSON object
    if (jsonData.find("mac") != jsonData.end()) {
        std::string macString = jsonData["mac"];
        thwp->xRes = jsonData["xRes"];
        thwp->yRes = jsonData["yRes"];
        thwp->bpp = jsonData["bpp"];
        thwp->controllerType = jsonData["controllerType"];
        thwp->OEPLType = jsonData["OEPLType"];

        // Attempt to parse the MAC address with ":" separators
        int resultWithColons = sscanf(macString.c_str(), "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
                                      &thwp->macAddress[0], &thwp->macAddress[1], &thwp->macAddress[2], &thwp->macAddress[3],
                                      &thwp->macAddress[4], &thwp->macAddress[5], &thwp->macAddress[6], &thwp->macAddress[7]);
        if (resultWithColons != 8) {
            int resultWithoutColons = sscanf(macString.c_str(), "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                                             &thwp->macAddress[0], &thwp->macAddress[1], &thwp->macAddress[2], &thwp->macAddress[3],
                                             &thwp->macAddress[4], &thwp->macAddress[5], &thwp->macAddress[6], &thwp->macAddress[7]);

            // Check if either format successfully parsed the MAC address
            if (resultWithoutColons != 8) {
                std::cerr << "Error parsing MAC address from JSON string." << std::endl;
                delete thwp;
                return nullptr;
            }
        }
        return thwp;
    } else {
        std::cerr << "Error: 'mac' key not found in JSON." << std::endl;
        delete thwp;
        return nullptr;
    }
}
