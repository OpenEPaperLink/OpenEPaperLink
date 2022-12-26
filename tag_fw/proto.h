#ifndef _PROTO_H_
#define _PROTO_H_

#include <stdint.h>

/*
	All communications are direct from tag to station, EXCEPT association (tag will broadcast).
	All comms shall be encrypted and authenticated with AES-CCM. Shared key shall be burned into the firmware.
	Master shall provision new key at association. All non-bcast packets shall have pan id compression.
	Master may skip "from" field. Tag checking in confirms it got the master's provisioning reply.
	
	Sadly filtering on MZ100 fails for long addr with no src addr. so short addr for src is used
	
	T = tag, S = station
	
	PACKET TYPE		USE		PAYLOAD STRUCT		NOTES
	ASSOC_REQ		T2bcast	TagInfo				tag's info and assoc request (encrypted with shared key)
	ASSOC_RESP		S2T		AssocInfo			tag's association info (encrypted with shared key)
	CHECKIN			T2S		CheckinInfo			tag checking in occasionally
	CHECKOUT		S2T		PendingInfo			station's checkin reply telling tag what we have for it
	CHUNK_REQ		T2S		ChunkReqInfo		tag requesting a piece of data
	CHUNK_RESP		S2T		ChunkInfo			station provides chunk

*/

#define PROTO_PRESHARED_KEY		{0x34D906D3, 0xE3E5298E, 0x3429BF58, 0xC1022081}

#define PROTO_PAN_ID			(0x4447)	//PAN ID compression shall be used

#define PKT_ASSOC_REQ			(0xF0)
#define PKT_ASSOC_RESP			(0xF1)
#define PKT_CHECKIN				(0xF2)
#define PKT_CHECKOUT			(0xF3)
#define PKT_CHUNK_REQ			(0xF4)
#define PKT_CHUNK_RESP			(0xF5)

#define PROTO_VER_0				(0)
#define PROTO_VER_CURRENT		(PROTO_VER_0)

#define PROTO_COMPR_TYPE_LZ			(0x0001)
#define PROTO_COMPR_TYPE_BITPACK	(0x0002)

#define PROTO_MAX_DL_LEN		(88)

enum TagScreenType {
	TagScreenEink_BW_1bpp,
	TagScreenEink_BW_2bpp,
	TagScreenEink_BW_4bpp,
	TagScreenEink_BWY_only,		//2bpp, but only 3 colors (BW?Y)
	TagScreenEink_BWY_2bpp,
	TagScreenEink_BWY_4bpp,
	TagScreenEink_BWR_only,		//2bpp, but only 3 colors (BW?R)
	TagScreenEink_BWR_2bpp,
	TagScreenEink_BWR_4bpp,
	
	TagScreenEink_BWY_3bpp,
	TagScreenEink_BWR_3bpp,
	TagScreenEink_BW_3bpp,
	
	TagScreenPersistentLcd_1bpp,
	
	TagScreenEink_BWY_5colors,
	TagScreenEink_BWR_5colors,
	
	TagScreenEink_BWY_6colors,
	TagScreenEink_BWR_6colors,
	
	TagScreenTypeOther = 0x7f,
};

#ifndef __packed
#define __packed		__attribute__((packed))
#endif

struct TagState {
	uint64_t swVer;
	uint16_t hwType;
	uint16_t batteryMv;
} __packed;

struct TagInfo {
	uint8_t protoVer;						//PROTO_VER_*
	struct TagState state;
	uint8_t rfu1[1];						//shall be ignored for now
	uint16_t screenPixWidth;
	uint16_t screenPixHeight;
	uint16_t screenMmWidth;
	uint16_t screenMmHeight;
	uint16_t compressionsSupported;			//COMPR_TYPE_* bitfield
	uint16_t maxWaitMsec;					//how long tag will wait for packets before going to sleep
	uint8_t screenType;						//enum TagScreenType
	uint8_t rfu[11];						//shall be zero for now
} __packed;

struct AssocInfo {
	uint32_t checkinDelay;					//space between checkins, in msec
	uint32_t retryDelay;					//if download fails mid-way wait thi smany msec to retry (IFF progress was made)
	uint16_t failedCheckinsTillBlank;		//how many fails till we go blank
	uint16_t failedCheckinsTillDissoc;		//how many fails till we dissociate
	uint32_t newKey[4];
	uint8_t rfu[8];							//shall be zero for now
} __packed;

#define CHECKIN_TEMP_OFFSET					0x7f

struct CheckinInfo {
	struct TagState state;
	uint8_t lastPacketLQI;					//zero if not reported/not supported to be reported
	int8_t lastPacketRSSI;					//zero if not reported/not supported to be reported
	uint8_t temperature;					//zero if not reported/not supported to be reported. else, this minus CHECKIN_TEMP_OFFSET is temp in degrees C
	uint8_t rfu[6];							//shall be zero for now
} __packed;

struct PendingInfo {
	uint64_t imgUpdateVer;
	uint32_t imgUpdateSize;
	uint64_t osUpdateVer;					//version of OS update avail
	uint32_t osUpdateSize;
	uint8_t rfu[8];							//shall be zero for now
} __packed;

struct ChunkReqInfo {
	uint64_t versionRequested;
	uint32_t offset;
	uint8_t len;
	uint8_t osUpdatePlz	: 1;
	uint8_t rfu[6];							//shall be zero for now
} __packed;

struct ChunkInfo {
	uint32_t offset;
	uint8_t osUpdatePlz	: 1;
	uint8_t rfu;							//shall be zero for now
	uint8_t data[];							//no data means request is out of bounds of this version no longer exists
} __packed;



#define MACFMT		"%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
#define MACCVT(x)	((const uint8_t*)(x))[7], ((const uint8_t*)(x))[6], ((const uint8_t*)(x))[5], ((const uint8_t*)(x))[4], ((const uint8_t*)(x))[3], ((const uint8_t*)(x))[2], ((const uint8_t*)(x))[1], ((const uint8_t*)(x))[0]


#define VERSION_SIGNIFICANT_MASK				(0x0000ffffffffffffull)

#define HW_TYPE_42_INCH_SAMSUNG					(1)
#define HW_TYPE_42_INCH_SAMSUNG_ROM_VER_OFST	(0xEFF8)

#define HW_TYPE_74_INCH_DISPDATA				(2)
#define HW_TYPE_74_INCH_DISPDATA_FRAME_MODE		(3)
#define HW_TYPE_74_INCH_DISPDATA_ROM_VER_OFST	(0x008b)

#define HW_TYPE_ZBD_EPOP50						(4)
#define HW_TYPE_ZBD_EPOP50_ROM_VER_OFST			(0x008b)

#define HW_TYPE_ZBD_EPOP900						(5)
#define HW_TYPE_ZBD_EPOP900_ROM_VER_OFST		(0x008b)

#define HW_TYPE_29_INCH_DISPDATA				(6)
#define HW_TYPE_29_INCH_DISPDATA_FRAME_MODE		(7)
#define HW_TYPE_29_INCH_DISPDATA_ROM_VER_OFST	(0x008b)

#define HW_TYPE_29_INCH_ZBS_026					(8)
#define HW_TYPE_29_INCH_ZBS_026_FRAME_MODE		(9)

#define HW_TYPE_154_INCH_ZBS_033				(18)
#define HW_TYPE_154_INCH_ZBS_033_FRAME_MODE		(19)

#define HW_TYPE_42_INCH_ZBS_026					(28)
#define HW_TYPE_42_INCH_ZBS_026_FRAME_MODE		(29)

#define HW_TYPE_29_INCH_ZBS_025					(10)
#define HW_TYPE_29_INCH_ZBS_025_FRAME_MODE		(11)

#define HW_TYPE_29_INCH_ZBS_ROM_VER_OFST		(0x008b)



#endif




