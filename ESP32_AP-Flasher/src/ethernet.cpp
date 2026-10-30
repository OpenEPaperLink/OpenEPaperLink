#include "ethernet.h"

#ifdef HAS_ETHERNET

#include <ETH.h>
#include <Preferences.h>
#include <esp_eth.h>

#ifdef ETHERNET_W5500
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_mac.h>
#include <esp_netif.h>

#ifndef ETHERNET_SPI_MHZ
#define ETHERNET_SPI_MHZ 36
#endif

// Arduino's ETH class only knows RMII PHYs in this core. A W5500 goes through the IDF driver, on
// the same default Ethernet interface, so ETH.localIP(), ETH.config() and the ARDUINO_EVENT_ETH_*
// events work for it as well. These two come from WiFiGeneric, as in ETH.cpp.
extern bool tcpipInit();
extern void add_esp_interface_netif(esp_interface_t interface, esp_netif_t *esp_netif);

static esp_eth_handle_t ethHandle = nullptr;

// The driver doesn't notice a missing W5500, so look first: its version register (common block,
// address 0x0039) always reads 0x04. Without the sub-board, the pulled down MISO reads 0x00.
static bool w5500Found(spi_device_handle_t spi) {
    gpio_pulldown_en((gpio_num_t)ETHERNET_SPI_MISO);
    pinMode(ETHERNET_SPI_RST, OUTPUT);
    digitalWrite(ETHERNET_SPI_RST, LOW);
    delay(1);
    digitalWrite(ETHERNET_SPI_RST, HIGH);
    delay(10);
    spi_transaction_t t = {};
    t.cmd = 0x0039;  // address
    t.addr = 0x00;   // control byte: common register block, read
    t.length = 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    if (spi_device_polling_transmit(spi, &t) != ESP_OK) return false;
    if (t.rx_data[0] != 0x04) {
        Serial.printf("No W5500 (version register 0x%02X)\n", t.rx_data[0]);
        return false;
    }
    return true;
}

static bool w5500Begin() {
    tcpipInit();
    tcpip_adapter_set_default_eth_handlers();
    gpio_install_isr_service(0);  // for the interrupt pin; ESP_ERR_INVALID_STATE when it's already there

    spi_bus_config_t bus = {};
    bus.mosi_io_num = ETHERNET_SPI_MOSI;
    bus.miso_io_num = ETHERNET_SPI_MISO;
    bus.sclk_io_num = ETHERNET_SPI_SCLK;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;
    if (spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO) != ESP_OK) return false;

    spi_device_interface_config_t device = {};
    device.command_bits = 16;  // the W5500 frame: 16 bit address, 8 bit control
    device.address_bits = 8;
    device.mode = 0;
    device.clock_speed_hz = ETHERNET_SPI_MHZ * 1000 * 1000;
    device.spics_io_num = ETHERNET_SPI_CS;
    device.queue_size = 20;
    spi_device_handle_t spi = nullptr;
    if (spi_bus_add_device(SPI2_HOST, &device, &spi) != ESP_OK) {
        spi_bus_free(SPI2_HOST);
        return false;
    }
    if (!w5500Found(spi)) {
        spi_bus_remove_device(spi);
        spi_bus_free(SPI2_HOST);
        return false;
    }

    eth_w5500_config_t w5500Config = ETH_W5500_DEFAULT_CONFIG(spi);
    w5500Config.int_gpio_num = ETHERNET_SPI_INT;
    eth_mac_config_t macConfig = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phyConfig = ETH_PHY_DEFAULT_CONFIG();
    phyConfig.reset_gpio_num = ETHERNET_SPI_RST;
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500Config, &macConfig);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phyConfig);

    esp_eth_config_t ethConfig = ETH_DEFAULT_CONFIG(mac, phy);
    if (mac == nullptr || phy == nullptr || esp_eth_driver_install(&ethConfig, &ethHandle) != ESP_OK) {
        if (mac) mac->del(mac);
        if (phy) phy->del(phy);
        spi_bus_remove_device(spi);
        spi_bus_free(SPI2_HOST);
        ethHandle = nullptr;
        return false;
    }

    // the W5500 has no MAC address of its own
    uint8_t macAddress[6];
    esp_read_mac(macAddress, ESP_MAC_ETH);
    esp_eth_ioctl(ethHandle, ETH_CMD_S_MAC_ADDR, macAddress);

    esp_netif_config_t netifConfig = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *netif = esp_netif_new(&netifConfig);
    esp_netif_attach(netif, esp_eth_new_netif_glue(ethHandle));
    add_esp_interface_netif(ESP_IF_ETH, netif);
    return true;
}

#else  // RMII

#if !defined(ETHERNET_PHY_POWER) || !defined(ETHERNET_CLK_MODE)
#error "RMII Ethernet needs ETHERNET_PHY_POWER and ETHERNET_CLK_MODE (or ETHERNET_W5500 with ETHERNET_SPI_* for a W5500)"
#endif
// the others default to the board definition, through ETH.h
#ifndef ETHERNET_PHY_MDC
#define ETHERNET_PHY_MDC ETH_PHY_MDC
#endif
#ifndef ETHERNET_PHY_MDIO
#define ETHERNET_PHY_MDIO ETH_PHY_MDIO
#endif
#ifndef ETHERNET_PHY_TYPE
#define ETHERNET_PHY_TYPE ETH_PHY_TYPE
#endif

#endif

// Set before the link comes up: with DHCP stopped, the interface reports this address as soon as
// the cable is in.
static void applyStaticIP() {
    Preferences preferences;
    preferences.begin("wifi", true);
    const String ip = preferences.getString("eip", "");
    const String mask = preferences.getString("emask", "");
    const String gw = preferences.getString("egw", "");
    const String dns = preferences.getString("edns", "");
    preferences.end();
    if (ip.length() == 0) return;

    IPAddress staticIP, subnetMask, gatewayIP, dnsIP;
    if (staticIP.fromString(ip) && subnetMask.fromString(mask) && gatewayIP.fromString(gw)) {
        if (dns.length() > 0) dnsIP.fromString(dns);
        ETH.config(staticIP, gatewayIP, subnetMask, dnsIP);
        Serial.println("Ethernet static IP: " + ip);
    }
}

bool ethernetBegin() {
#ifdef ETHERNET_W5500
    if (!w5500Begin()) return false;
    applyStaticIP();
    return esp_eth_start(ethHandle) == ESP_OK;
#else
    if (!ETH.begin(ETH_PHY_ADDR, ETHERNET_PHY_POWER, ETHERNET_PHY_MDC, ETHERNET_PHY_MDIO, ETHERNET_PHY_TYPE, ETHERNET_CLK_MODE, false)) return false;
    applyStaticIP();
    return true;
#endif
}

String ethernetDescription() {
    uint8_t mac[6] = {0};
    int speed;
    bool fullDuplex;
#ifdef ETHERNET_W5500
    eth_speed_t linkSpeed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    esp_eth_ioctl(ethHandle, ETH_CMD_G_MAC_ADDR, mac);
    esp_eth_ioctl(ethHandle, ETH_CMD_G_SPEED, &linkSpeed);
    esp_eth_ioctl(ethHandle, ETH_CMD_G_DUPLEX_MODE, &duplex);
    speed = linkSpeed == ETH_SPEED_100M ? 100 : 10;
    fullDuplex = duplex == ETH_DUPLEX_FULL;
#else
    ETH.macAddress(mac);
    speed = ETH.linkSpeed();
    fullDuplex = ETH.fullDuplex();
#endif
    char text[64];
    snprintf(text, sizeof(text), "MAC %02X:%02X:%02X:%02X:%02X:%02X, %d Mbps, %s duplex",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], speed, fullDuplex ? "full" : "half");
    return String(text);
}

#endif
