#ifndef SRC_INCLUDE_BOARD_8258_DIY_H_
#define SRC_INCLUDE_BOARD_8258_DIY_H_

/************************* For 512K Flash only ***************************************/
/* Flash map:
  0x00000 Old Firmware bin
  0x34000 NV_1
  0x40000 OTA New bin storage Area
  0x76000 MAC address
  0x77000 C_Cfg_Info
  0x78000 U_Cfg_Info
  0x7A000 NV_2
  0x80000 End Flash
 */
#define USER_DATA_SIZE          0x34000
#define BEGIN_USER_DATA1        0x00000
#define END_USER_DATA1          (BEGIN_USER_DATA1 + USER_DATA_SIZE)
#define BEGIN_USER_DATA2        0x40000
#define END_USER_DATA2          (BEGIN_USER_DATA2 + USER_DATA_SIZE)
#define GEN_USER_CFG_DATA       END_USER_DATA2



#endif /* SRC_INCLUDE_BOARD_8258_DIY_H_ */
