#include "WifiManager.h"

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
