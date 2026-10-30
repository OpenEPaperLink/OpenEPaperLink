#pragma once

#include <Arduino.h>

#ifdef HAS_ETHERNET

// Starts the board's Ethernet: a W5500 on SPI (ETHERNET_W5500, pins ETHERNET_SPI_*) or else the
// ESP32's own MAC with an RMII PHY (pins ETHERNET_PHY_*, ETHERNET_CLK_MODE). Applies the static Ethernet IP from the settings, if there is one.
// Returns false when the hardware isn't there; a W5500 sub-board can be left off.
bool ethernetBegin();

// MAC address, link speed and duplex, for the log
String ethernetDescription();

#endif
