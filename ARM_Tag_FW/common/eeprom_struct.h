struct EepromImageHeader {				
	uint64_t version;
	uint32_t validMarker;
	uint32_t size;
	uint8_t dataType;
	uint32_t id;
	uint8_t argument;
};