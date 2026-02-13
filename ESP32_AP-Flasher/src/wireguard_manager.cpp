#include "wireguard_manager.h"
#include <Preferences.h>
#include <WireGuard-ESP32.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// Global instances
// CRITICAL: Use pointer so we can recreate instance without calling end()
static WireGuard* wg = nullptr;
WireGuardManager wgManager;

static const char* TAG = "WireGuard";
static const char* PREFS_NAMESPACE = "wireguard";
static const unsigned long RECONNECT_INTERVAL = 30000; // 30 Sekunden
static const unsigned long HANDSHAKE_TIMEOUT = 180000; // 3 Minuten

WireGuardManager::WireGuardManager() {
    _status = WG_DISABLED;
    _lastHandshake = 0;
    _reconnectInterval = RECONNECT_INTERVAL;
    _nextReconnectAttempt = 0;
    _initialized = false;
    _errorMessage = "";
    _reconnectAttempts = 0;
    _taskHandle = nullptr;
    _taskRunning = false;
    
    // Default-Werte
    _config.enabled = false;
    _config.localIP = "";
    _config.privateKey = "";
    _config.endpointAddress = "";
    _config.endpointPort = 51820;
    _config.publicKey = "";
    _config.presharedKey = "";
    _config.allowedIPs = "0.0.0.0/0, ::/0";
    _config.persistentKeepalive = 25;
}

bool WireGuardManager::begin() {
    Serial.println("[WireGuard] Initializing...");
    
    loadConfig();
    
    if (!_config.enabled) {
        Serial.println("[WireGuard] Disabled in configuration");
        _status = WG_DISABLED;
        return false;
    }
    
    if (!validateConfig()) {
        Serial.println("[WireGuard] Invalid configuration");
        _status = WG_ERROR;
        _errorMessage = "Invalid configuration";
        return false;
    }
    
    _initialized = true;
    _status = WG_DISCONNECTED;
    
    Serial.println("[WireGuard] Initialized. Will connect when WiFi is ready.");
    return true;
}

bool WireGuardManager::connect() {
    if (!_initialized || !_config.enabled) {
        Serial.println("[WireGuard] Cannot connect: not initialized or disabled");
        return false;
    }
    
    if (_status == WG_CONNECTED) {
        Serial.println("[WireGuard] Already connected");
        return true;
    }
    
    // Warte auf WiFi-Verbindung
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WireGuard] Waiting for WiFi connection...");
        _status = WG_ERROR;
        _errorMessage = "No WiFi connection";
        return false;
    }
    
    Serial.println("[WireGuard] Connecting to server...");
    _status = WG_CONNECTING;
    
    try {
        // WireGuard initialisieren
        IPAddress local_ip;
        if (!local_ip.fromString(_config.localIP.c_str())) {
            Serial.printf("[WireGuard] Invalid local IP: %s\n", _config.localIP.c_str());
            _status = WG_ERROR;
            _errorMessage = "Invalid local IP address";
            return false;
        }
        
        // WireGuard-Interface konfigurieren
        // Note: WireGuard-ESP32 begin() signature: (localIP, privateKey, peerAddress, peerPublicKey, peerPort)
        bool success = wg->begin(
            local_ip,
            _config.privateKey.c_str(),
            _config.endpointAddress.c_str(),
            _config.publicKey.c_str(),
            _config.endpointPort
        );
        
        if (!success) {
            Serial.println("[WireGuard] Failed to initialize WireGuard interface");
            _status = WG_ERROR;
            _errorMessage = "Failed to initialize WireGuard";
            return false;
        }
        
        // Note: WireGuard-ESP32 library doesn't support preshared keys or keepalive configuration via API
        // These parameters are stored but not actively used by the library
        if (_config.presharedKey.length() > 0) {
            Serial.println("[WireGuard] Note: Preshared key is configured but not supported by WireGuard-ESP32 library");
        }
        if (_config.persistentKeepalive > 0) {
            Serial.printf("[WireGuard] Note: Persistent keepalive (%d sec) is configured but not supported by WireGuard-ESP32 library\n", _config.persistentKeepalive);
        }
        
        _status = WG_CONNECTED;
        _lastHandshake = millis();
        _errorMessage = "";
        
        Serial.println("[WireGuard] Successfully connected!");
        Serial.printf("[WireGuard] Local IP: %s\n", local_ip.toString().c_str());
        Serial.printf("[WireGuard] Endpoint: %s:%d\n", _config.endpointAddress.c_str(), _config.endpointPort);
        
        return true;
        
    } catch (const std::exception& e) {
        Serial.printf("[WireGuard] Connection failed: %s\n", e.what());
        _status = WG_ERROR;
        _errorMessage = String("Connection failed: ") + e.what();
        return false;
    } catch (...) {
        Serial.println("[WireGuard] Connection failed: Unknown error");
        _status = WG_ERROR;
        _errorMessage = "Unknown connection error";
        return false;
    }
}

void WireGuardManager::disconnect() {
    if (_status == WG_DISABLED || _status == WG_DISCONNECTED) {
        return;
    }
    
    Serial.println("[WireGuard] Disconnecting...");
    
    // Status sofort auf DISCONNECTED setzen damit keine neuen Pakete gesendet werden
    _status = WG_DISCONNECTED;
    _lastHandshake = 0;
    
    // WireGuard-Interface richtig herunterfahren
    // WICHTIG: Wir müssen SEHR vorsichtig sein beim Cleanup!
    try {
        if (wg != nullptr && wg->is_initialized()) {
            Serial.println("[WireGuard] Preparing to cleanup interface...");
            
            // CRITICAL: Do NOT call wg->end() - causes crash!
            // Instead: abandon old instance and create new one on next start
            Serial.println("[WireGuard] Abandoning instance (no end() call to prevent crash)");
            wg = nullptr; // Mark as ready for new instance
            
            // Nochmal warten damit Cleanup komplett abgeschlossen ist
            vTaskDelay(pdMS_TO_TICKS(200));
            
            Serial.println("[WireGuard] Interface cleanup completed");
        }
    } catch (const std::exception& e) {
        Serial.printf("[WireGuard] Exception during wg.end(): %s\n", e.what());
    } catch (...) {
        Serial.println("[WireGuard] Unknown exception during wg.end()");
    }
    
    Serial.println("[WireGuard] Disconnected");
}

// FreeRTOS Task-Funktion (statisch)
void WireGuardManager::wireguardTask(void* parameter) {
    WireGuardManager* manager = static_cast<WireGuardManager*>(parameter);
    Serial.println("[WireGuard] Background task started");
    
    while (manager->_taskRunning) {
        try {
            manager->taskLoop();
        } catch (const std::exception& e) {
            Serial.printf("[WireGuard] Task exception: %s\n", e.what());
            manager->_status = WG_ERROR;
            manager->_errorMessage = "Task exception: ";
            manager->_errorMessage += e.what();
            vTaskDelay(pdMS_TO_TICKS(5000)); // 5 Sekunden warten bei Exception
        } catch (...) {
            Serial.println("[WireGuard] Unknown task exception");
            manager->_status = WG_ERROR;
            manager->_errorMessage = "Unknown exception in task";
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde warten zwischen Checks
        
        // Bei anhaltendem Error: Task beenden
        if (manager->_status == WG_ERROR && manager->_reconnectAttempts >= 5) {
            Serial.println("[WireGuard] Persistent error - stopping task");
            manager->_taskRunning = false;
        }
    }
    
    Serial.println("[WireGuard] Background task stopped");
    manager->_taskHandle = nullptr;
    vTaskDelete(nullptr);
}

// Task-Loop Logik
void WireGuardManager::taskLoop() {
    if (!_config.enabled || _status == WG_DISABLED || !_initialized) {
        return;
    }
    
    unsigned long now = millis();
    
    // WiFi-Verbindung prüfen
    if (WiFi.status() != WL_CONNECTED) {
        if (_status == WG_CONNECTED) {
            Serial.println("[WireGuard] WiFi connection lost");
            _status = WG_DISCONNECTED;
            _reconnectAttempts = 0;
        }
        return;
    }
    
    // Erste Verbindung oder Wiederverbindung nach Fehler
    if (_status == WG_ERROR || _status == WG_DISCONNECTED) {
        // Maximal 5 Reconnect-Versuche, dann aufgeben
        if (_reconnectAttempts >= 5) {
            if (_status != WG_ERROR) {
                Serial.println("[WireGuard] Max reconnect attempts reached, giving up");
                _status = WG_ERROR;
                _errorMessage = "Max reconnect attempts reached";
            }
            return; // Keine weiteren Versuche
        }
        
        if (now >= _nextReconnectAttempt) {
            Serial.printf("[WireGuard] Attempting to connect... (attempt %d/5)\n", _reconnectAttempts + 1);
            
            // WICHTIG: KEIN wg.end() hier! Das verursacht Crashes.
            // Die WireGuard Library kann mehrfach begin() aufrufen.
            // Falls wirklich cleanup nötig ist, muss der Task gestoppt werden.
            
            if (connect()) {
                _nextReconnectAttempt = 0;
                _reconnectAttempts = 0;
                Serial.println("[WireGuard] Connection successful");
            } else {
                _reconnectAttempts++;
                // Exponentielles Backoff: 30s, 60s, 120s, 240s, 480s
                unsigned long backoff = _reconnectInterval * (1 << (_reconnectAttempts - 1));
                if (backoff > 480000) backoff = 480000; // Max 8 Minuten
                _nextReconnectAttempt = now + backoff;
                Serial.printf("[WireGuard] Connection failed, retry in %lu seconds\n", backoff / 1000);
            }
        }
        return;
    }
    
    // Handshake-Timeout prüfen (nur wenn wirklich verbunden)
    if (_status == WG_CONNECTED) {
        if (_config.persistentKeepalive > 0 && now - _lastHandshake > HANDSHAKE_TIMEOUT) {
            Serial.println("[WireGuard] Handshake timeout - stopping task to allow safe cleanup");
            // WICHTIG: wg.end() darf NICHT aus dem Task aufgerufen werden!
            // Das verursacht Race Conditions mit dem TCP/IP Stack.
            // Stattdessen: Task stoppen, dann wird end() von außen aufgerufen
            _status = WG_ERROR;
            _errorMessage = "Handshake timeout";
            _taskRunning = false; // Task wird sich selbst beenden
            Serial.println("[WireGuard] Task will stop, cleanup will happen from outside");
        }
    }
}

// Task starten
void WireGuardManager::startTask() {
    if (_taskHandle != nullptr || _taskRunning) {
        Serial.println("[WireGuard] Task already running");
        return;
    }
    
    _taskRunning = true;
    BaseType_t result = xTaskCreate(
        wireguardTask,       // Task-Funktion
        "WireGuard",         // Task-Name
        12288,               // Stack-Size (12KB) - erhöht für WireGuard
        this,                // Parameter (this pointer)
        1,                   // Priority (niedrig - unter loop())
        &_taskHandle         // Task-Handle
    );
    
    if (result != pdPASS) {
        Serial.println("[WireGuard] Failed to create background task");
        _taskRunning = false;
        _taskHandle = nullptr;
    } else {
        Serial.println("[WireGuard] Background task created successfully");
    }
}

// Task stoppen
void WireGuardManager::stopTask() {
    if (_taskHandle == nullptr || !_taskRunning) {
        return;
    }
    
    Serial.println("[WireGuard] Stopping background task...");
    _taskRunning = false;
    
    // Warte bis Task beendet ist (max 5 Sekunden)
    int timeout = 50; // 50 * 100ms = 5 Sekunden
    while (_taskHandle != nullptr && timeout > 0) {
        vTaskDelay(pdMS_TO_TICKS(100));
        timeout--;
    }
    
    if (_taskHandle != nullptr) {
        Serial.println("[WireGuard] Force deleting task");
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
}

// Public start/stop Methoden
void WireGuardManager::start() {
    if (!_initialized || !_config.enabled) {
        Serial.println("[WireGuard] Cannot start - not initialized or disabled");
        return;
    }
    
    Serial.println("[WireGuard] Starting WireGuard service...");
    
    // CRITICAL FIX: Use new shutdown_timer_only() method
    // This stops the keepalive timer WITHOUT calling netif_remove()
    // which prevents the TCP race condition crash!
    if (wg != nullptr && wg->is_initialized()) {
        Serial.println("[WireGuard] Old instance detected - safe timer shutdown...");
        
        // Stop ONLY the timer, leave netif in place (prevents TCP crash)
        wg->shutdown_timer_only();
        Serial.println("[WireGuard] Timer stopped safely");
        
        // Now create new instance
        wg = new WireGuard();
        Serial.println("[WireGuard] New WireGuard instance created");
    } else if (wg == nullptr) {
        // First time initialization
        wg = new WireGuard();
        Serial.println("[WireGuard] Initial WireGuard instance created");
    }
    
    // Status zurücksetzen
    _status = WG_DISCONNECTED;
    _reconnectAttempts = 0;
    _errorMessage = "";
    
    startTask();
}

void WireGuardManager::stop() {
    Serial.println("[WireGuard] Stopping WireGuard service...");
    
    // Erst Task stoppen (damit keine weiteren Pakete gesendet werden)
    stopTask();
    
    // DANN Interface beenden - aber nur wenn wirklich nötig
    // Vorsicht: disconnect() kann Crashes verursachen wenn TCP Connections aktiv sind
    if (wg != nullptr && wg->is_initialized() && _status != WG_DISABLED) {
        Serial.println("[WireGuard] Final cleanup of interface...");
        disconnect();
    }
}

bool WireGuardManager::setConfig(const WireGuardConfig& config) {
    _config = config;
    
    if (!validateConfig()) {
        Serial.println("[WireGuard] Invalid configuration provided");
        _errorMessage = "Invalid configuration";
        return false;
    }
    
    saveConfig();
    
    // Bei aktivem Tunnel: Neuverbindung mit neuer Konfiguration
    if (_status == WG_CONNECTED) {
        disconnect();
        if (_config.enabled) {
            return connect();
        }
    }
    
    return true;
}

WireGuardConfig WireGuardManager::getConfig() const {
    // Sicherheitskopie ohne Private Key für API-Anfragen
    WireGuardConfig safeCopy = _config;
    // Optional: Private Key maskieren
    // safeCopy.privateKey = "***HIDDEN***";
    return safeCopy;
}

WireGuardStatus WireGuardManager::getStatus() const {
    return _status;
}

String WireGuardManager::getStatusString() const {
    switch (_status) {
        case WG_DISABLED:      return "Disabled";
        case WG_INITIALIZING:  return "Initializing";
        case WG_CONNECTING:    return "Connecting";
        case WG_CONNECTED:     return "Connected";
        case WG_ERROR:         return "Error";
        case WG_DISCONNECTED:  return "Disconnected";
        default:               return "Unknown";
    }
}

String WireGuardManager::getErrorMessage() const {
    return _errorMessage;
}

unsigned long WireGuardManager::getLastHandshake() const {
    return _lastHandshake;
}

bool WireGuardManager::isConnected() const {
    return _status == WG_CONNECTED;
}

void WireGuardManager::enable() {
    if (_config.enabled) {
        return;
    }
    
    _config.enabled = true;
    saveConfig();
    
    if (_initialized) {
        // Task starten wenn WiFi connected ist (WiFiManager wird es später starten)
        Serial.println("[WireGuard] Enabled - will auto-start when WiFi connects");
    } else {
        begin();
    }
}

void WireGuardManager::disable() {
    if (!_config.enabled) {
        return;
    }
    
    _config.enabled = false;
    saveConfig();
    
    // Task stoppen
    stop();
    
    _status = WG_DISABLED;
}

bool WireGuardManager::isEnabled() const {
    return _config.enabled;
}

bool WireGuardManager::isTaskRunning() const {
    return _taskRunning && (_taskHandle != nullptr);
}

bool WireGuardManager::validateConfig() {
    if (_config.localIP.length() == 0) {
        Serial.println("[WireGuard] Validation failed: No local IP");
        return false;
    }
    
    if (_config.privateKey.length() == 0) {
        Serial.println("[WireGuard] Validation failed: No private key");
        return false;
    }
    
    if (_config.endpointAddress.length() == 0) {
        Serial.println("[WireGuard] Validation failed: No endpoint address");
        return false;
    }
    
    if (_config.publicKey.length() == 0) {
        Serial.println("[WireGuard] Validation failed: No public key");
        return false;
    }
    
    if (_config.endpointPort == 0) {
        Serial.println("[WireGuard] Validation failed: Invalid port");
        return false;
    }
    
    // IP-Adresse validieren
    IPAddress test_ip;
    if (!test_ip.fromString(_config.localIP.c_str())) {
        Serial.println("[WireGuard] Validation failed: Invalid local IP format");
        return false;
    }
    
    return true;
}

void WireGuardManager::loadConfig() {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, true); // read-only
    
    _config.enabled = prefs.getBool("enabled", false);
    _config.localIP = prefs.getString("localIP", "");
    _config.privateKey = prefs.getString("privKey", "");
    _config.endpointAddress = prefs.getString("endpoint", "");
    _config.endpointPort = prefs.getUShort("port", 51820);
    _config.publicKey = prefs.getString("pubKey", "");
    _config.presharedKey = prefs.getString("psk", "");
    _config.allowedIPs = prefs.getString("allowedIPs", "0.0.0.0/0, ::/0");
    _config.persistentKeepalive = prefs.getUShort("keepalive", 25);
    
    prefs.end();
    
    Serial.println("[WireGuard] Configuration loaded from preferences");
    if (_config.enabled) {
        Serial.printf("[WireGuard] Local IP: %s\n", _config.localIP.c_str());
        Serial.printf("[WireGuard] Endpoint: %s:%d\n", _config.endpointAddress.c_str(), _config.endpointPort);
    }
}

void WireGuardManager::saveConfig() {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, false); // read-write
    
    prefs.putBool("enabled", _config.enabled);
    prefs.putString("localIP", _config.localIP);
    prefs.putString("privKey", _config.privateKey);
    prefs.putString("endpoint", _config.endpointAddress);
    prefs.putUShort("port", _config.endpointPort);
    prefs.putString("pubKey", _config.publicKey);
    prefs.putString("psk", _config.presharedKey);
    prefs.putString("allowedIPs", _config.allowedIPs);
    prefs.putUShort("keepalive", _config.persistentKeepalive);
    
    prefs.end();
    
    Serial.println("[WireGuard] Configuration saved to preferences");
}

bool WireGuardManager::generateKeyPair(String& privateKey, String& publicKey) {
    // Note: Die WireGuard-ESP32 Library bietet möglicherweise keine Key-Generierung
    // Diese müsste extern (z.B. mit 'wg genkey') gemacht werden
    Serial.println("[WireGuard] Key generation not implemented");
    Serial.println("[WireGuard] Please generate keys using: wg genkey | tee privatekey | wg pubkey > publickey");
    return false;
}
