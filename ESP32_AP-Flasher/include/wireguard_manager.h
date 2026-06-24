#pragma once

#include <Arduino.h>
#include <cstdint>

#ifndef WIREGUARD_MANAGER_H
#define WIREGUARD_MANAGER_H

// WireGuard configuration structure
struct WireGuardConfig {
    bool enabled;
    String localIP;          // Local IP address (e.g., "10.0.0.2")
    String privateKey;       // Private key (base64 encoded)
    String endpointAddress;  // Server address (IP or hostname)
    uint16_t endpointPort;   // Server port (default: 51820)
    String publicKey;        // Server public key (base64 encoded)
    String presharedKey;     // Optional preshared key (base64 encoded)
    String allowedIPs;       // Allowed IPs (e.g., "0.0.0.0/0, ::/0" for all traffic)
    uint16_t persistentKeepalive;  // Keepalive interval in seconds (0 = disabled)
};

enum WireGuardStatus {
    WG_DISABLED,
    WG_INITIALIZING,
    WG_CONNECTING,
    WG_CONNECTED,
    WG_ERROR,
    WG_DISCONNECTED
};

class WireGuardManager {
private:
    WireGuardConfig _config;
    WireGuardStatus _status;
    unsigned long _lastHandshake;
    unsigned long _reconnectInterval;
    unsigned long _nextReconnectAttempt;
    bool _initialized;
    String _errorMessage;
    uint8_t _reconnectAttempts;
    TaskHandle_t _taskHandle;
    bool _taskRunning;

    bool validateConfig();
    void loadConfig();
    void saveConfig();
    
    // Task-Funktionen
    static void wireguardTask(void* parameter);
    void taskLoop();
    void startTask();
    void stopTask();

public:
    WireGuardManager();
    
    // Initialize WireGuard with saved configuration
    bool begin();
    
    // Connect to WireGuard server
    bool connect();
    
    // Disconnect from WireGuard
    void disconnect();
    
    // Start/Stop WireGuard background task
    void start();
    void stop();
    
    // Configuration management
    bool setConfig(const WireGuardConfig& config);
    WireGuardConfig getConfig() const;
    
    // Status information
    WireGuardStatus getStatus() const;
    String getStatusString() const;
    String getErrorMessage() const;
    unsigned long getLastHandshake() const;
    bool isConnected() const;
    bool isEnabled() const;
    bool isTaskRunning() const;
    
    // Enable/Disable WireGuard
    void enable();
    void disable();
    
    // Generate key pair (utility function)
    static bool generateKeyPair(String& privateKey, String& publicKey);
};

extern WireGuardManager wgManager;

#endif
