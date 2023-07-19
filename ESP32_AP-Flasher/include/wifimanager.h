#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

enum WifiStatus {
    NOINIT,
    WAIT_CONNECTING,
    CONNECTED,
    WAIT_RECONNECT,
    AP
};

class WifiManager {
   private:
    bool _connected;
    bool _savewhensuccessfull;
    int _reconnectIntervalCheck;
    int _retryIntervalCheck;
    int _connectionTimeout;
    String _ssid;
    String _pass;
    unsigned long _nextReconnectCheck;
    bool _APstarted;

    const int SERIAL_BUFFER_SIZE = 64;
    char serialBuffer[64];
    int serialIndex = 0;

    String WiFi_SSID();
    String WiFi_psk();

    bool waitForConnection();
    void pollSerial();

   public:
    WifiManager();

    WifiStatus wifiStatus;
    static uint8_t apClients;
    bool connectToWifi();
    bool connectToWifi(String ssid, String pass, bool savewhensuccessfull);

    void startManagementServer();
    void poll();
    static void WiFiEvent(WiFiEvent_t event);
};

#endif
