idf_component_register(SRCS "sniffer.c"
                       INCLUDE_DIRS "."
                       REQUIRES ieee802154 esp_driver_usb_serial_jtag esp_driver_gpio esp_timer nvs_flash hal)
