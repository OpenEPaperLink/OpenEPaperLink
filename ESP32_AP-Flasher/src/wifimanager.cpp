#include "wifimanager.h"

#include <Preferences.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t WifiManager::apClients = 0;

WifiManager::WifiManager() {
    _reconnectIntervalCheck = 5000;
    _retryIntervalCheck = 5 * 60000;
    _connectionTimeout = 15000;

    _nextReconnectCheck = 0;
    _connected = false;
    _savewhensuccessfull = false;
    _ssid = "";
    _APstarted = false;
    wifiStatus = NOINIT;

    WiFi.onEvent(WiFiEvent);
}

void WifiManager::poll() {
    if (wifiStatus == AP && millis() > _nextReconnectCheck && _ssid!="") {
        if (apClients == 0) {
            Serial.println("Attempting to reconnect to WiFi.");
            _APstarted = false;
            wifiStatus = NOINIT;
            connectToWifi();
        } else {
            _nextReconnectCheck = millis() + _retryIntervalCheck;
        }
    }

    if (wifiStatus == CONNECTED && millis() > _nextReconnectCheck) {
        if (WiFi.status() != WL_CONNECTED) {
            _connected = false;
            Serial.println("WiFi connection lost. Attempting to reconnect.");
            WiFi.reconnect();
            waitForConnection();
        } else {
            _nextReconnectCheck = millis() + _reconnectIntervalCheck;
        }
    }

    pollSerial();
}

bool WifiManager::connectToWifi() {
    Preferences preferences;
    preferences.begin("wifi", false);
    _ssid = preferences.getString("ssid", WiFi_SSID());
    _pass = preferences.getString("pw", WiFi_psk());
    if (_ssid == "") {
        Serial.println("No connection information specified");
        startManagementServer();
        return false;
    }
    Serial.println("Stored ssid: " + String(_ssid));

    String ip = preferences.getString("ip", "");
    String mask = preferences.getString("mask", "");
    String gw = preferences.getString("gw", "");
    String dns = preferences.getString("dns", "");

    if (ip.length() > 0) {
        IPAddress staticIP, subnetMask, gatewayIP, dnsIP;
        if (staticIP.fromString(ip) && subnetMask.fromString(mask) && gatewayIP.fromString(gw)) {
            if (dns.length() > 0) dnsIP.fromString(dns);
            WiFi.config(staticIP, gatewayIP, subnetMask, dnsIP);
            Serial.println("Setting static IP");
        }
    }

    _connected = connectToWifi(_ssid, _pass, false);
    return _connected;
}

bool WifiManager::connectToWifi(String ssid, String pass, bool savewhensuccessfull) {
    _ssid = ssid;
    _pass = pass;
    _savewhensuccessfull = savewhensuccessfull;

    _APstarted = false;
    WiFi.disconnect(false, true);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(WIFI_PS_NONE);

    Serial.println("Connecting to WiFi...");

    WiFi.persistent(savewhensuccessfull);
    WiFi.begin(_ssid.c_str(), _pass.c_str());
    _connected = waitForConnection();
    return _connected;
}

bool WifiManager::waitForConnection() {
    unsigned long timeout = millis() + _connectionTimeout;
    wifiStatus = WAIT_CONNECTING;

    while (WiFi.status() != WL_CONNECTED) {
        if (millis() > timeout) {
            Serial.println("Unable to connect to WIFI");
            startManagementServer();
            return false;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    if (_savewhensuccessfull) {
        Preferences preferences;
        preferences.begin("wifi", false);
        preferences.putString("ssid", _ssid);
        preferences.putString("pass", _pass);
        preferences.end();
        _savewhensuccessfull = false;
    }
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    IPAddress IP = WiFi.localIP();
    Serial.printf("Connected! IP Address: %s\n", IP.toString().c_str());
    _nextReconnectCheck = millis() + _reconnectIntervalCheck;
    wifiStatus = CONNECTED;
    return true;
}

void WifiManager::startManagementServer() {
    if (!_APstarted) {
        Serial.println("Starting configuration AP, ssid OpenEPaperLink");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("OpenEPaperLink", "", 1, false);
        WiFi.softAPsetHostname("OpenEPaperLink");
        IPAddress IP = WiFi.softAPIP();
        Serial.printf("IP Address: %s\n", IP.toString().c_str());
        _APstarted = true;
        _nextReconnectCheck = millis() + _retryIntervalCheck;
        wifiStatus = AP;
    }
}

String WifiManager::WiFi_SSID() {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<const char *>(conf.sta.ssid));
}

String WifiManager::WiFi_psk() {
    if (WiFiGenericClass::getMode() == WIFI_MODE_NULL) {
        return String();
    }
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<char *>(conf.sta.password));
}

void WifiManager::pollSerial() {
    while (Serial.available() > 0) {
        char receivedChar = Serial.read();

        if (parse_improv_serial_byte(x_position, receivedChar, x_buffer, onCommandCallback, onErrorCallback)) {
            x_buffer[x_position++] = receivedChar;
        } else {
            x_position = 0;

            if (receivedChar == 27) {
                memset(serialBuffer, 0, sizeof(serialBuffer));
                serialIndex = 0;
                Serial.println();
                continue;
            }

            if (receivedChar == 8) {
                if (serialIndex > 0) {
                    serialIndex--;
                    serialBuffer[serialIndex] = '\0';
                    Serial.print("\r");
                    Serial.print(serialBuffer);
                }
                continue;
            }
            if (receivedChar == '\r') {
                continue;
            }

            if (receivedChar == '\n') {
                serialBuffer[serialIndex] = '\0';
                String command = String(serialBuffer);

                if (command.startsWith("ssid ")) {
                    _ssid = command.substring(5);
                    Serial.println("\rSSID set to: " + _ssid);
                } else if (command.startsWith("pass ")) {
                    _pass = command.substring(5);
                    Serial.println("\rPassword set to: " + _pass);
                } else if (command.startsWith("connect")) {
                    connectToWifi(_ssid, _pass, true);
                }
                memset(serialBuffer, 0, sizeof(serialBuffer));
                serialIndex = 0;
            } else {
                if (serialIndex < SERIAL_BUFFER_SIZE - 1) {
                    serialBuffer[serialIndex] = receivedChar;
                    serialIndex++;
                    Serial.print("\r");
                    Serial.print(serialBuffer);
                }
            }
        }
    }
}

// temporary write some more debug info
void WifiManager::WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event %d] ", event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;

        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            apClients++;
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            apClients--;
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;

        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default:
            Serial.println();
            break;
    }
}

// *** Improv

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#ifndef BUILD_ENV_NAME
#define BUILD_ENV_NAME unknown
#endif
#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif
#ifndef BUILD_VERSION
#define BUILD_VERSION custom
#endif

std::vector<std::string> getLocalUrl() {
    return { String("http://" + WiFi.localIP().toString()).c_str() };
}

void onErrorCallback(improv::Error err) {
}

bool onCommandCallback(improv::ImprovCommand cmd) {
    switch (cmd.command) {
        case improv::Command::GET_CURRENT_STATE: {
            if ((WiFi.status() == WL_CONNECTED)) {
                set_state(improv::State::STATE_PROVISIONED);
                std::vector<uint8_t> data = improv::build_rpc_response(improv::GET_CURRENT_STATE, getLocalUrl(), false);
                send_response(data);
            } else {
                set_state(improv::State::STATE_AUTHORIZED);
            }
            break;
        }

        case improv::Command::WIFI_SETTINGS: {
            if (cmd.ssid.length() == 0) {
                set_error(improv::Error::ERROR_INVALID_RPC);
                break;
            }

            set_state(improv::STATE_PROVISIONING);

            WifiManager wm;
            if (wm.connectToWifi(String(cmd.ssid.c_str()), String(cmd.password.c_str()), true)) {
                set_state(improv::STATE_PROVISIONED);
                std::vector<uint8_t> data = improv::build_rpc_response(improv::WIFI_SETTINGS, getLocalUrl(), false);
                send_response(data);
            } else {
                set_state(improv::STATE_STOPPED);
                set_error(improv::Error::ERROR_UNABLE_TO_CONNECT);
            }

            break;
        }

        case improv::Command::GET_DEVICE_INFO: {
            std::vector<std::string> infos = {
                // Firmware name
                "OpenEPaperLink",
                // Firmware version
                STR(BUILD_VERSION),
                // Hardware chip/variant
                STR(BUILD_ENV_NAME),
                // Device name
                "Access Point"};
            std::vector<uint8_t> data = improv::build_rpc_response(improv::GET_DEVICE_INFO, infos, false);
            send_response(data);
            break;
        }

        case improv::Command::GET_WIFI_NETWORKS: {
            getAvailableWifiNetworks();
            break;
        }

        default: {
            set_error(improv::ERROR_UNKNOWN_RPC);
            return false;
        }
    }

    return true;
}

void getAvailableWifiNetworks() {
    int networkNum = WiFi.scanNetworks();

    for (int id = 0; id < networkNum; ++id) {
        std::vector<uint8_t> data = improv::build_rpc_response(
            improv::GET_WIFI_NETWORKS, {WiFi.SSID(id), String(WiFi.RSSI(id)), (WiFi.encryptionType(id) == WIFI_AUTH_OPEN ? "NO" : "YES")}, false);
        send_response(data);
        delay(1);
    }
    // final response
    std::vector<uint8_t> data =
        improv::build_rpc_response(improv::GET_WIFI_NETWORKS, std::vector<std::string>{}, false);
    send_response(data);
}

void set_state(improv::State state) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_CURRENT_STATE;
    data[8] = 1;
    data[9] = state;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    Serial.write(data.data(), data.size());
}

void send_response(std::vector<uint8_t> &response) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(9);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_RPC_RESPONSE;
    data[8] = response.size();
    data.insert(data.end(), response.begin(), response.end());

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data.push_back(checksum);

    Serial.write(data.data(), data.size());
}

void set_error(improv::Error error) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_ERROR_STATE;
    data[8] = 1;
    data[9] = error;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    Serial.write(data.data(), data.size());
}

// **** improv ****

namespace improv {

ImprovCommand parse_improv_data(const std::vector<uint8_t> &data, bool check_checksum) {
    return parse_improv_data(data.data(), data.size(), check_checksum);
}

ImprovCommand parse_improv_data(const uint8_t *data, size_t length, bool check_checksum) {
    ImprovCommand improv_command;
    Command command = (Command)data[0];
    uint8_t data_length = data[1];

    if (data_length != length - 2 - check_checksum) {
        improv_command.command = UNKNOWN;
        return improv_command;
    }

    if (check_checksum) {
        uint8_t checksum = data[length - 1];

        uint32_t calculated_checksum = 0;
        for (uint8_t i = 0; i < length - 1; i++) {
            calculated_checksum += data[i];
        }

        if ((uint8_t)calculated_checksum != checksum) {
            improv_command.command = BAD_CHECKSUM;
            return improv_command;
        }
    }

    if (command == WIFI_SETTINGS) {
        uint8_t ssid_length = data[2];
        uint8_t ssid_start = 3;
        size_t ssid_end = ssid_start + ssid_length;

        uint8_t pass_length = data[ssid_end];
        size_t pass_start = ssid_end + 1;
        size_t pass_end = pass_start + pass_length;

        std::string ssid(data + ssid_start, data + ssid_end);
        std::string password(data + pass_start, data + pass_end);
        return {.command = command, .ssid = ssid, .password = password};
    }

    improv_command.command = command;
    return improv_command;
}

bool parse_improv_serial_byte(size_t position, uint8_t byte, const uint8_t *buffer,
                              std::function<bool(ImprovCommand)> &&callback, std::function<void(Error)> &&on_error) {
    if (position == 0)
        return byte == 'I';
    if (position == 1)
        return byte == 'M';
    if (position == 2)
        return byte == 'P';
    if (position == 3)
        return byte == 'R';
    if (position == 4)
        return byte == 'O';
    if (position == 5)
        return byte == 'V';

    if (position == 6)
        return byte == IMPROV_SERIAL_VERSION;

    if (position <= 8)
        return true;

    uint8_t type = buffer[7];
    uint8_t data_len = buffer[8];

    if (position <= 8 + data_len)
        return true;

    if (position == 8 + data_len + 1) {
        uint8_t checksum = 0x00;
        for (size_t i = 0; i < position; i++)
            checksum += buffer[i];

        if (checksum != byte) {
            on_error(ERROR_INVALID_RPC);
            return false;
        }

        if (type == TYPE_RPC) {
            auto command = parse_improv_data(&buffer[9], data_len, false);
            return callback(command);
        }
    }

    return false;
}

std::vector<uint8_t> build_rpc_response(Command command, const std::vector<std::string> &datum, bool add_checksum) {
    std::vector<uint8_t> out;
    uint32_t length = 0;
    out.push_back(command);
    for (const auto &str : datum) {
        uint8_t len = str.length();
        length += len + 1;
        out.push_back(len);
        out.insert(out.end(), str.begin(), str.end());
    }
    out.insert(out.begin() + 1, length);

    if (add_checksum) {
        uint32_t calculated_checksum = 0;

        for (uint8_t byte : out) {
            calculated_checksum += byte;
        }
        out.push_back(calculated_checksum);
    }
    return out;
}

std::vector<uint8_t> build_rpc_response(Command command, const std::vector<String> &datum, bool add_checksum) {
    std::vector<uint8_t> out;
    uint32_t length = 0;
    out.push_back(command);
    for (const auto &str : datum) {
        uint8_t len = str.length();
        length += len;
        out.push_back(len);
        out.insert(out.end(), str.begin(), str.end());
    }
    out.insert(out.begin() + 1, length);

    if (add_checksum) {
        uint32_t calculated_checksum = 0;

        for (uint8_t byte : out) {
            calculated_checksum += byte;
        }
        out.push_back(calculated_checksum);
    }
    return out;
}

}  // namespace improv
