#ifndef _SUBGIG_RADIO_H_
#define _SUBGIG_RADIO_H_

//sub-GHz 866 Mhz channels start at 100
#define FIRST_866_CHAN              (100)
#define NUM_866_CHANNELS            (6)

//sub-GHz 915 Mhz channels start at 200
#define FIRST_915_CHAN              (200)
#define NUM_915_CHANNELS            (6)

typedef enum {
   SUBGIG_ERR_NONE,
   SUBGIG_CC1101_NOT_FOUND,
   SUBGIG_NOT_INITIALIZED,
   SUBGIG_NOT_ENABLED,
   SUBGIG_TX_FAILED,
   SUBGIG_TX_BAD_LEN,
   SUBGIG_INVALID_CHANNEL,
} SubGigErr;

typedef struct {
   uint8_t Present:1;
   uint8_t Enabled:1;
   uint8_t FreqTest:1;
   uint8_t RxAvailable:1;
   uint8_t Initialized:1;
   uint8_t FixedRegsSet:1;
} SubGigData;

extern SubGigData gSubGigData;

SubGigErr SubGig_radio_init(uint8_t ch);
SubGigErr SubGig_radioTx(uint8_t *packet);
SubGigErr SubGig_radioSetChannel(uint8_t ch);
int8_t SubGig_commsRxUnencrypted(uint8_t *data);
SubGigErr SubGig_FreqTest(bool b866Mhz,bool bStart);

#endif   // _SUBGIG_RADIO_H_

