#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "comms.h"
#include "proto.h"
#include "ccm.h"

extern uint8_t Zigbee_tx_buffer(uint8_t tx_buffer[], int len);

static uint8_t packet[128];
static uint8_t mSeq = 0;
uint8_t mLastLqi = 0;
int8_t mLastRSSI = 0;

uint8_t commsGetLastPacketLQI(void)
{
	return mLastLqi;
}

int8_t commsGetLastPacketRSSI(void)
{
	return mLastRSSI;
}

static inline void __attribute__((always_inline)) macCopy(uint8_t *restrict dst, const uint8_t *restrict src)
{
	((uint32_t *)dst)[0] = ((const uint32_t *)src)[0];
	((uint32_t *)dst)[1] = ((const uint32_t *)src)[1];
}

static inline bool __attribute__((always_inline)) macIsEq(const uint8_t *restrict dst, const uint8_t *restrict src)
{
	return ((uint32_t *)dst)[0] == ((const uint32_t *)src)[0] && ((uint32_t *)dst)[1] == ((const uint32_t *)src)[1];
}

bool commsTx(struct CommsInfo *info, bool bcast, const void *packet_in, uint32_t len)
{
	uint8_t nonce[AES_CCM_NONCE_SIZE] = {};
	struct MacFrameNormal *mfn;
	struct MacFrameBcast *mfb;
	uint32_t hdrSz;
	char *payload;
	static const struct MacFcs normalFcs = {
		.frameType = FRAME_TYPE_DATA,
		.panIdCompressed = 1,
		.destAddrType = ADDR_MODE_LONG,
		.srcAddrType = ADDR_MODE_LONG,
	};
	static const struct MacFcs broadcastFcs = {
		.frameType = FRAME_TYPE_DATA,
		.destAddrType = ADDR_MODE_SHORT,
		.srcAddrType = ADDR_MODE_LONG,
	};

	if (len > COMMS_MAX_PACKET_SZ)
		return false;

	if (bcast)
	{
		mfb = (struct MacFrameBcast *)packet;
		hdrSz = sizeof(struct MacFrameBcast);
		payload = (char *)(mfb + 1);
		mfb->fcs = broadcastFcs;
		mfb->seq = mSeq++;
		mfb->dstPan = 0xffff;
		mfb->dstAddr = 0xffff;
		mfb->srcPan = PROTO_PAN_ID;
		macCopy(mfb->src, info->myMac);
	}
	else
	{
		mfn = (struct MacFrameNormal *)packet;
		hdrSz = sizeof(struct MacFrameNormal);
		payload = (char *)(mfn + 1);
		mfn->fcs = normalFcs;
		mfn->seq = mSeq++;
		mfn->pan = PROTO_PAN_ID;
		macCopy(mfn->dst, info->masterMac);
		macCopy(mfn->src, info->myMac);
	}

	*(uint32_t *)nonce = (*info->nextIV)++;
	macCopy(nonce + sizeof(uint32_t), info->myMac);
	memcpy(payload, packet_in, len);

	aesCcmEnc((void *)packet, (void *)packet, hdrSz, len, info->encrKey, nonce);
	*(uint32_t *)(payload + len + AES_CCM_MIC_SIZE) = *(uint32_t *)nonce; // send nonce

	len += hdrSz;
	len += AES_CCM_MIC_SIZE;
	len += sizeof(uint32_t);

	return !Zigbee_tx_buffer((uint8_t *)&packet, len);
}

extern volatile uint8_t rx_buffer[0x400];
extern volatile uint8_t new_rx;
extern volatile uint8_t new_rssi;
extern volatile int rx_len;

int32_t __attribute__((noinline)) commsRx(struct CommsInfo *info, void *data, uint8_t *fromMacP)
{
	uint8_t *buf = packet, nonce[13] = {}, fromMac[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t len, minNeedLen, hdrLen = 0;
	struct MacFrameFromMaster *mfm;
	struct MacFrameNormal *mfn;

	// sort out how many bytes minimum are a valid packet
	minNeedLen = sizeof(struct MacFrameFromMaster); // mac header
	minNeedLen += sizeof(uint8_t);					// packet type
	minNeedLen += AES_CCM_MIC_SIZE;					// MIC
	minNeedLen += sizeof(uint32_t);					// nonce counter
	minNeedLen += 2 * sizeof(uint8_t);				// RSSI/LQI

	if (!new_rx)
		return COMMS_RX_ERR_NO_PACKETS;

	// some basic checks
	mfm = (struct MacFrameFromMaster *)rx_buffer;
	if (rx_len >= sizeof(packet) || rx_len < minNeedLen || mfm->fcs.frameType != FRAME_TYPE_DATA ||
		mfm->fcs.secure || mfm->fcs.frameVer || mfm->fcs.destAddrType != ADDR_MODE_LONG || !mfm->fcs.panIdCompressed ||
		(mfm->fcs.srcAddrType != ADDR_MODE_LONG && mfm->fcs.srcAddrType != ADDR_MODE_SHORT) ||
		mfm->pan != PROTO_PAN_ID || !macIsEq(mfm->dst, info->myMac))
	{
		new_rx = 0;
		return COMMS_RX_ERR_INVALID_PACKET;
	}

	// copy out and release buffer
	memcpy(buf, &rx_buffer, len = rx_len - 2 * sizeof(uint8_t));
	mLastLqi = rx_buffer[len + 0];
	mLastRSSI = rx_buffer[len + 1];

	mfm = (struct MacFrameFromMaster *)buf;
	mfn = (struct MacFrameNormal *)buf;
	new_rx = 0;

	// sort out header len, copy mac into nonce
	if (mfm->fcs.srcAddrType == ADDR_MODE_LONG)
	{

		macCopy(fromMac, mfn->src);
		hdrLen = sizeof(struct MacFrameNormal);

		// re-verify needed length
		minNeedLen -= sizeof(struct MacFrameFromMaster);
		minNeedLen += sizeof(struct MacFrameNormal);

		if (rx_len < minNeedLen)
			return COMMS_RX_ERR_INVALID_PACKET;
	}
	else if (mfm->fcs.srcAddrType == ADDR_MODE_SHORT)
	{

		macCopy(fromMac, info->masterMac);
		hdrLen = sizeof(struct MacFrameFromMaster);
	}

	// sort out the nonce
	macCopy(nonce + sizeof(uint32_t), fromMac);
	*(uint32_t *)nonce = *(uint32_t *)(buf + len - sizeof(uint32_t));

	// decrypt and auth
	len -= hdrLen + AES_CCM_MIC_SIZE + sizeof(uint32_t);

	if (!aesCcmDec(buf, buf, hdrLen, len, info->encrKey, nonce))
		return COMMS_RX_ERR_MIC_FAIL;

	if (fromMacP)
		macCopy(fromMacP, fromMac);

	memcpy(data, buf + hdrLen, len);

	return len;
}

int32_t __attribute__((noinline)) commsRxUnenc(void *data)
{
	if (!new_rx)
		return COMMS_RX_ERR_NO_PACKETS;
	memcpy(data, &rx_buffer, rx_len);
	mLastLqi = 255 - new_rssi;
	mLastRSSI = new_rssi;
	new_rx = 0;
	return rx_len;
}

void commsTxNoCpy(uint8_t *packetp)
{
	Zigbee_tx_buffer((uint8_t *)&packetp[1], (packetp[0] - 2));
}
