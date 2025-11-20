# WireGuard VPN Integration für OpenEPaperLink ESP32 Access Point

## Übersicht

Diese Erweiterung fügt WireGuard VPN-Unterstützung zum OpenEPaperLink ESP32 Access Point hinzu. WireGuard ist ein modernes, schnelles und sicheres VPN-Protokoll, das es ermöglicht, den Access Point sicher mit einem entfernten Netzwerk zu verbinden.

## Funktionen

- ✅ Vollständige WireGuard-Client-Implementierung
- ✅ Web-basierte Konfigurationsoberfläche
- ✅ Automatische Wiederverbindung bei Verbindungsverlust
- ✅ Status-Überwachung und Fehlerbehandlung
- ✅ Persistente Speicherung der Konfiguration
- ✅ Unterstützung für Persistent Keepalive (NAT-Durchquerung)
- ✅ Optional: Preshared Key für zusätzliche Sicherheit

## Voraussetzungen

### Server-Seite
Sie benötigen einen funktionierenden WireGuard-Server. Installation z.B. unter Linux:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install wireguard

# Server-Schlüssel generieren
cd /etc/wireguard
umask 077
wg genkey | tee server_private.key | wg pubkey > server_public.key
```

Beispiel-Serverkonfiguration `/etc/wireguard/wg0.conf`:
```ini
[Interface]
PrivateKey = <SERVER_PRIVATE_KEY>
Address = 10.0.0.1/24
ListenPort = 51820

# ESP32 Client
[Peer]
PublicKey = <ESP32_PUBLIC_KEY>
AllowedIPs = 10.0.0.2/32
PersistentKeepalive = 25
```

Server starten:
```bash
sudo systemctl enable wg-quick@wg0
sudo systemctl start wg-quick@wg0
```

### Client-Seite (ESP32)
1. ESP32 mit WiFi-Verbindung
2. Ausreichend Speicher (PSRAM empfohlen für größere Projekte)
3. Generierte WireGuard-Schlüssel

## Installation

### 1. Code kompilieren

Das WireGuard-Addon ist bereits in den Quellcode integriert. Kompilieren Sie das Projekt mit PlatformIO:

```bash
cd ESP32_AP-Flasher
pio run -e <your_environment>
```

### 2. Firmware flashen

```bash
pio run -e <your_environment> -t upload
```

## Konfiguration

### Schlüssel generieren

Generieren Sie auf Ihrem Computer (mit installiertem WireGuard) die Schlüssel für den ESP32:

```bash
# Private Key generieren
wg genkey | tee esp32_private.key | wg pubkey > esp32_public.key

# Optional: Preshared Key generieren
wg genpsk > esp32_preshared.key
```

### Web-Interface Konfiguration

1. Verbinden Sie sich mit dem ESP32 Access Point
2. Öffnen Sie im Browser: `http://<ESP32_IP>/wireguard.html`
3. Füllen Sie folgende Felder aus:

   - **WireGuard aktivieren**: Checkbox aktivieren
   - **Lokale IP-Adresse**: Die IP im VPN-Netzwerk (z.B. `10.0.0.2`)
   - **Private Key**: Inhalt von `esp32_private.key`
   - **Server-Adresse**: IP oder Hostname des WireGuard-Servers
   - **Server-Port**: Port des Servers (Standard: `51820`)
   - **Server Public Key**: Public Key des Servers
   - **Preshared Key**: Optional, für zusätzliche Sicherheit
   - **Erlaubte IPs**: 
     - `0.0.0.0/0, ::/0` - Gesamter Traffic durch VPN
     - `10.0.0.0/24` - Nur VPN-Netzwerk
     - Mehrere Netzwerke möglich
   - **Persistent Keepalive**: `25` (empfohlen für NAT)

4. Klicken Sie auf "Konfiguration speichern"
5. Klicken Sie auf "Verbinden"

### API-Endpunkte

Für fortgeschrittene Anwendungen stehen folgende HTTP-Endpunkte zur Verfügung:

#### Konfiguration abrufen
```http
GET /get_wg_config
```
Antwort (JSON):
```json
{
  "enabled": true,
  "localIP": "10.0.0.2",
  "endpointAddress": "vpn.example.com",
  "endpointPort": 51820,
  "publicKey": "...",
  "allowedIPs": "0.0.0.0/0",
  "persistentKeepalive": 25,
  "status": "Connected",
  "connected": true
}
```

#### Konfiguration speichern
```http
POST /save_wg_config
Content-Type: application/x-www-form-urlencoded

enabled=1&localIP=10.0.0.2&privateKey=...&endpointAddress=...
```

#### Status abrufen
```http
GET /wg_status
```

#### Verbinden
```http
POST /wg_connect
```

#### Trennen
```http
POST /wg_disconnect
```

## Verwendung

### Automatischer Start

Wenn WireGuard in der Konfiguration aktiviert ist, wird die Verbindung automatisch beim Systemstart hergestellt (nach erfolgreicher WiFi-Verbindung).

### Verbindungsüberwachung

Der WireGuard-Manager überwacht kontinuierlich:
- WiFi-Verbindungsstatus
- Handshake-Timeout (3 Minuten)
- Automatische Wiederverbindung bei Verbindungsverlust

### Logging

WireGuard-Logs sind über die serielle Konsole sichtbar:
```
[WireGuard] Initializing...
[WireGuard] Successfully connected!
[WireGuard] Local IP: 10.0.0.2
[WireGuard] Endpoint: vpn.example.com:51820
```

## Fehlerbehandlung

### Häufige Probleme

**Problem**: "No WiFi connection"
- **Lösung**: Stellen Sie sicher, dass der ESP32 mit WiFi verbunden ist

**Problem**: "Invalid local IP address"
- **Lösung**: Überprüfen Sie das IP-Format (z.B. `10.0.0.2`)

**Problem**: "Connection failed"
- **Lösung**: 
  - Überprüfen Sie Server-Erreichbarkeit
  - Prüfen Sie Firewall-Regeln (UDP Port 51820)
  - Verifizieren Sie die Schlüssel

**Problem**: "Handshake timeout"
- **Lösung**: 
  - Erhöhen Sie Persistent Keepalive
  - Überprüfen Sie NAT/Firewall-Einstellungen

### Debug-Modus

Aktivieren Sie detailliertes Logging über die serielle Konsole:
```cpp
// In platformio.ini hinzufügen:
build_flags = 
    -D WG_DEBUG=1
```

## Sicherheitshinweise

⚠️ **Wichtig**: 
- Bewahren Sie Private Keys sicher auf
- Verwenden Sie starke, zufällig generierte Schlüssel
- Erwägen Sie die Verwendung von Preshared Keys
- Beschränken Sie AllowedIPs auf notwendige Netzwerke
- Aktualisieren Sie regelmäßig die Firmware

## Architektur

### Komponenten

```
┌─────────────────────────────────────────┐
│         ESP32 Application               │
├─────────────────────────────────────────┤
│  main.cpp                               │
│  ├─ wgManager.begin()                   │
│  └─ wgManager.poll()  (in loop)        │
├─────────────────────────────────────────┤
│  WireGuardManager                       │
│  ├─ Konfigurationsverwaltung            │
│  ├─ Verbindungsaufbau                   │
│  ├─ Status-Überwachung                  │
│  └─ Automatische Wiederverbindung       │
├─────────────────────────────────────────┤
│  WireGuard-ESP32 Library                │
│  └─ Crypto & Netzwerk                   │
├─────────────────────────────────────────┤
│  ESP32 Networking Stack                 │
│  └─ WiFi / Ethernet                     │
└─────────────────────────────────────────┘
```

### Dateien

- `include/wireguard_manager.h` - Header mit Definitionen
- `src/wireguard_manager.cpp` - Hauptimplementierung
- `src/main.cpp` - Integration in Hauptanwendung
- `src/web.cpp` - Web-API-Endpunkte
- `wwwroot/wireguard.html` - Web-Interface

## Leistung

- **Latenz**: +1-5ms (abhängig von Server-Distanz)
- **Durchsatz**: ~10-20 Mbps (ESP32-abhängig)
- **Memory**: ~30-50KB RAM
- **CPU**: ~5-10% bei aktiver Verbindung

## Lizenz

Dieses Addon verwendet folgende Open-Source-Komponenten:
- [WireGuard-ESP32-Arduino](https://github.com/ciniml/WireGuard-ESP32-Arduino) (verschiedene Lizenzen)
- OpenEPaperLink (siehe Haupt-Repository)

## Support & Beiträge

- **Issues**: https://github.com/OpenEPaperLink/OpenEPaperLink/issues
- **Diskussionen**: https://github.com/OpenEPaperLink/OpenEPaperLink/discussions
- **Wiki**: https://github.com/OpenEPaperLink/OpenEPaperLink/wiki

## Changelog

### Version 1.0.0 (2025-11-18)
- ✨ Initiale WireGuard-Integration
- ✨ Web-Interface für Konfiguration
- ✨ API-Endpunkte
- ✨ Automatische Wiederverbindung
- ✨ Persistente Konfiguration

## Autor

Entwickelt als Addon für das OpenEPaperLink-Projekt.

## Danksagungen

- WireGuard® - Jason A. Donenfeld
- WireGuard-ESP32-Arduino Library - Kenta Ida (ciniml)
- OpenEPaperLink Community
