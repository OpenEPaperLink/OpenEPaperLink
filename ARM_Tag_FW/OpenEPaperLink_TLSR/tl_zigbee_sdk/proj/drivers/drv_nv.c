/********************************************************************************************************
 * @file    drv_nv.c
 *
 * @brief   This is the source file for drv_nv
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "../tl_common.h"

#if FLASH_CAP_SIZE_1M
u32 g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_1M;
u32 g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_1M;
#elif FLASH_CAP_SIZE_2M
u32 g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_2M;
u32 g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_2M;
#elif FLASH_CAP_SIZE_4M
u32 g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_4M;
u32 g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_4M;
#else
u32 g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_512K;
u32 g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_512K;
#endif


#define NV_ITEM_LEN_CHK_TABBLE_NUM	 16

#define UPDATE_FRAMECOUNT_THRES		 1024

typedef struct{
	u8   itemId;
	u16  len;
}nv_itemLenChk_t;

enum{
	NV_ERASE_NONE = 0x00,
	NV_ERASE_ALL = 0x55,
	NV_ERASE_FOR_FN = 0x33,
}nv_erase_mode_e;



#define SECT_VALID_CHECK(s)                 (s.usedFlag == NV_SECTOR_VALID || \
		                                     s.usedFlag == NV_SECTOR_VALID_CHECKCRC)

#define ITEM_VALID_FLAG_CHECK(v)  			(v == ITEM_FIELD_VALID ||   \
		                           	   	   	 v == ITEM_FIELD_VALID_SINGLE)

#define ITEM_FLAG_CHECK(v)  				(v == ITEM_FIELD_VALID ||   \
		                     	 	 	 	 v == ITEM_FIELD_VALID_SINGLE || \
		                     	 	 	 	 v == ITEM_FIELD_INVALID || \
		                     	 	 	 	 v == ITEM_FIELD_OPERATION)


#define ITEM_OFT_CHECK(oft, size, id, sec)  (((oft >= MODULE_CONTEXT_START(id, sec, 0)) &&   \
	                                         (oft < MODULE_SECT_END(id, sec))) && \
		                                    ((MODULE_SECT_END(id, sec) - oft) >= size))

#define ITEM_HDR_VALID_CHECK(v)             (v == ITEM_HDR_FIELD_VALID_CHECKSUM ||   \
                                             v == ITEM_HDR_FIELD_VALID_CHECKCRC)

#define  NV_ITEMLEN_MATCH(len, unitLen) ((len % unitLen) ? FALSE : TRUE)

static u8 g_nvItemLengthCheckNum = 0;



nv_itemLenChk_t  g_nvItemLenCheckTbl[NV_ITEM_LEN_CHK_TABBLE_NUM];



void nv_itemLengthCheckAdd(u8 itemId, u16 len){
	for(s32 i = 0; i < g_nvItemLengthCheckNum; i++){
		if(itemId == g_nvItemLenCheckTbl[i].itemId){
			g_nvItemLenCheckTbl[i].len = len;
			return;
		}
	}
	if(g_nvItemLengthCheckNum >= NV_ITEM_LEN_CHK_TABBLE_NUM){
		ZB_EXCEPTION_POST(SYS_EXCEPTTION_NV_CHECK_TABLE_FULL);
		return;
	}
	g_nvItemLenCheckTbl[g_nvItemLengthCheckNum].itemId = itemId;
	g_nvItemLenCheckTbl[g_nvItemLengthCheckNum].len = len;
	g_nvItemLengthCheckNum++;
}

u16 nv_itemLengthValidCheck(u8 itemId, u16 len){
	for(s32 i= 0; i < NV_ITEM_LEN_CHK_TABBLE_NUM; i++){
		if(g_nvItemLenCheckTbl[i].itemId == itemId){
			return min2(g_nvItemLenCheckTbl[i].len , len);
		}
	}
	return len;
}

static nv_sts_t nv_sect_info_check(u8 sect, u16 id){
	u32 sectStartAddr = MODULE_SECT_START(id, sect);
	nv_sect_info_t s;

	flash_read(sectStartAddr, sizeof(nv_sect_info_t), (u8*)&s);

	if(!SECT_VALID_CHECK(s)){
		return NV_MODULE_NOT_FOUND;
	}
	return NV_SUCC;
}


static nv_sts_t nv_index_read_op(u16 id, u8 itemId, u16 *itemLen, u8 opSect, u32 totalItemNum, u16 *idxNo, bool lenCheck){
	nv_sts_t ret = NV_SUCC;
	u32 idxTotalNum = totalItemNum;
	u32 idxStartAddr = MODULE_IDX_START(id, opSect) + (idxTotalNum) * sizeof(nv_info_idx_t);
	u16 opItemIdx = idxTotalNum;
	u16 readIdxNum = 4;
	nv_info_idx_t idxInfo[4];
	u8 jump = 0;
	u8 itemFlag = ITEM_FIELD_VALID;
	u16 len = *itemLen;
	u16 itemTotalSize = ITEM_TOTAL_LEN(len);

	if(itemId == ITEM_FIELD_IDLE){
		itemFlag = ITEM_FIELD_IDLE;
		itemTotalSize = 0xffff;
	}

	ret = NV_ITEM_NOT_FOUND;
	s32 i = 0;
	while(idxTotalNum > 0){
		readIdxNum = (idxTotalNum > 4) ? 4 : idxTotalNum;
		idxStartAddr -= readIdxNum * sizeof(nv_info_idx_t);
		opItemIdx -= readIdxNum;
		idxTotalNum -= readIdxNum;
		flash_read(idxStartAddr, readIdxNum * sizeof(nv_info_idx_t), (u8*)idxInfo);
		for(i = readIdxNum - 1; i >= 0; i--){
			if(itemId != ITEM_FIELD_IDLE){
				if(ITEM_VALID_FLAG_CHECK(idxInfo[i].usedState) &&
					idxInfo[i].itemId == itemId){
					if(lenCheck){
						if(NV_ITEMLEN_MATCH((idxInfo[i].size - sizeof(itemHdr_t)), len)){
							jump = 1;
							break;
						}
					}else{
						*itemLen = idxInfo[i].size - sizeof(itemHdr_t);
						ret = NV_SUCC;
						break;
					}
				}
			}else{
				if(idxInfo[i].usedState != itemFlag || idxInfo[i].size != itemTotalSize || idxInfo[i].itemId != itemId){
					jump = 1;
					break;
				}
			}
		}
		if(jump){
			opItemIdx += i;
			*idxNo = opItemIdx;
			ret = NV_SUCC;
			break;
		}
	}

	return ret;
}

nv_sts_t nv_index_read(u16 id, u8 itemId, u16 len, u8 opSect, u32 totalItemNum, u16 *idxNo){
	return nv_index_read_op(id, itemId, &len, opSect, totalItemNum, idxNo, 1);
}


static inline bool nv_sectInfoCrcCheck(nv_sect_info_t s){
	u32 sectCrc = 0xffffffff;
	u8 crcR = 0;

	crcR = (s.opSect >> NV_SECT_INFO_SECTNO_BITS) & NV_SECT_INFO_CHECK_BITMASK;
	s.opSect &= NV_SECT_INFO_SECTNO_BITMASK;
	sectCrc = xcrc32(&s.idName, 2, sectCrc);
	if((sectCrc & NV_SECT_INFO_CHECK_BITMASK) == crcR){
		return TRUE;
	}else{
		return FALSE;
	}
}

nv_sts_t nv_sector_read(u16 id, u8 sectTotalNum, nv_sect_info_t *sectInfo){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t s;
	u32 moduleStartAddr = MODULES_START_ADDR(id);
	s32 i = 0;
	u8 sectNo = 0;
	u8 bitValid = 0;
	for(s32 i = 0; i < NV_SECT_INFO_SECTNO_BITS; i++){
		bitValid |= (1 << i);
	}

	for(i = 0; i < sectTotalNum; i++){
		flash_read(moduleStartAddr, sizeof(nv_sect_info_t), (u8 *)&s);
		sectNo = (s.opSect & bitValid);

		if(s.usedFlag == NV_SECTOR_VALID_READY_CHECKCRC){
			if(nv_sectInfoCrcCheck(s)){
				/* if another sector is valid, set it as invalid */
				u8 sectDel = (sectNo + 1) & (MODULE_SECTOR_NUM - 1);
				s.usedFlag = NV_SECTOR_INVALID;
				if(flash_writeWithCheck(MODULE_SECT_START(id, sectDel), sizeof(s.usedFlag), (u8 *)&s.usedFlag) == TRUE){
					/* set itself as valid */
					s.usedFlag = NV_SECTOR_VALID_CHECKCRC;
					flash_writeWithCheck(moduleStartAddr, sizeof(s.usedFlag), (u8 *)&s.usedFlag);
				}

				s.opSect = sectNo;
				memcpy(sectInfo, &s, sizeof(nv_sect_info_t));
				break;
			}
		}else{
			if((SECT_VALID_CHECK(s)) && s.idName == id){
				if(s.usedFlag == NV_SECTOR_VALID_CHECKCRC){
					if(nv_sectInfoCrcCheck(s)){
						s.opSect = sectNo;
						memcpy(sectInfo, &s, sizeof(nv_sect_info_t));
						break;
					}
				}else{
					s.opSect = sectNo;
					memcpy(sectInfo, &s, sizeof(nv_sect_info_t));
					break;
				}
			}
		}

		moduleStartAddr += NV_SECTOR_SIZE(id);
	}
	if( i == sectTotalNum){
		ret = NV_ITEM_NOT_FOUND;
	}
	return ret;
}


nv_sts_t nv_write_item(u8 single, u16 id, u8 itemId, u8 opSect, u16 opItemIdx, u16 len, u8 *buf, bool isFlashCopy){
	/* write index to flash */
	nv_info_idx_t idxInfo;
	memset((u8 *)&idxInfo, 0, sizeof(nv_info_idx_t));
	u32 idxStartAddr = MODULE_IDX_START(id, opSect);
	u32 offset = 0;
	u32 crcOri = 0xffffffff;
	u32 crcDst = 0xffffffff;
	itemHdr_t hdr;

	if(opItemIdx == 0){
		offset = MODULE_CONTEXT_START(id, opSect, len);
	}else{
		flash_read(idxStartAddr + (opItemIdx - 1)*sizeof(nv_info_idx_t), sizeof(nv_info_idx_t), (u8*)&idxInfo);

		/* check item idex */
		flash_read(idxInfo.offset, sizeof(itemHdr_t), (u8 *)&hdr);
		if(hdr.used == ITEM_HDR_FIELD_VALID_CHECKCRC){
			crcOri = 0xffffffff;
			crcOri = xcrc32((u8 *)&idxInfo, sizeof(nv_info_idx_t)-1, crcOri);
			if((crcOri & 0xffff) != ((hdr.checkSum >> 16)&0xffff)){
				return NV_ITEM_CHECK_ERROR;
			}
		}
		if(ITEM_FLAG_CHECK(idxInfo.usedState) && ITEM_OFT_CHECK(idxInfo.offset, idxInfo.size, id, opSect)){
			offset = ((idxInfo.offset + idxInfo.size + 3) & (~0x03));
		}else{
			return NV_ITEM_CHECK_ERROR;
		}
	}
	idxInfo.usedState = ITEM_FIELD_OPERATION;
	idxInfo.size = len + sizeof(itemHdr_t);
	idxInfo.offset = offset;
	idxInfo.itemId = itemId;

	if(!ITEM_OFT_CHECK(idxInfo.offset, idxInfo.size, id, opSect)){
		return NV_ITEM_CHECK_ERROR;
	}
	if(flash_writeWithCheck(idxStartAddr+opItemIdx*sizeof(nv_info_idx_t), sizeof(nv_info_idx_t), (u8 *)&idxInfo) != TRUE){
		return NV_ITEM_CHECK_ERROR;
	}

	/* write context to flash */
	if(isFlashCopy){
		/* if need copy th data from flash, read it into ram, and then write it to flash */
		u8 copyLen = 48;
		u8 pTemp[48];

		u8 wLen = 0;
		s32 toalLen = idxInfo.size - sizeof(itemHdr_t); //len;
		u32 sAddr = (u32)buf;
		u32 dAddr = idxInfo.offset + sizeof(itemHdr_t);
		u32 checkSum = 0;
		crcOri = 0xffffffff;
		u32 crcOld = 0xffffffff;
		flash_read(sAddr, sizeof(itemHdr_t), (u8 *)&hdr);
		sAddr += sizeof(itemHdr_t);

		while(toalLen > 0){
			wLen = (toalLen > copyLen) ? copyLen : toalLen;
			flash_read(sAddr, wLen, pTemp);
			if(hdr.used == ITEM_HDR_FIELD_VALID_CHECKSUM){
				for(s32 i = 0; i < wLen; i++){
					checkSum += pTemp[i];
				}
				crcOld = xcrc32(pTemp, wLen, crcOld);
			}else{
				crcOri = xcrc32(pTemp, wLen, crcOri);
			}
			if(flash_writeWithCheck(dAddr, wLen, pTemp) != TRUE){
				return NV_CHECK_SUM_ERROR;
			}

			toalLen -= wLen;
			sAddr += wLen;
			dAddr += wLen;
		}

		if(hdr.used == ITEM_HDR_FIELD_VALID_CHECKSUM){
			if(hdr.checkSum != checkSum){
				return NV_DATA_CHECK_ERROR;
			}
			crcOri = crcOld;
			hdr.used = ITEM_HDR_FIELD_VALID_CHECKCRC;
			crcOri = xcrc32((u8 *)(&(hdr.size)), 4, crcOri);
		}else{
			crcOri = xcrc32((u8 *)(&(hdr.size)), 4, crcOri);
			if((crcOri & 0xffff) != (hdr.checkSum&0xffff)){
				return NV_DATA_CHECK_ERROR;
			}
		}
	}else{
		u32 payloadAddr = idxInfo.offset+sizeof(itemHdr_t);
		crcOri = 0xffffffff;
		crcDst = 0xffffffff;

		crcOri = xcrc32(buf, len, crcOri);
		flash_write(payloadAddr, len, buf);

		/* check the checksum */
		u8 vCheck[16];
		s32 tLen = len;
		u16 unitLen;

		while(tLen > 0){
			unitLen = (tLen > 16) ? 16 : tLen;
			flash_read(payloadAddr, unitLen, vCheck);
			crcDst = xcrc32(vCheck, unitLen, crcDst);
			tLen -= unitLen;
			payloadAddr += unitLen;
		}

		if(crcDst == crcOri){
			hdr.itemId = itemId;
			hdr.size = len;
			hdr.used = ITEM_HDR_FIELD_VALID_CHECKCRC;
			crcOri = xcrc32((u8 *)(&(hdr.size)), 4, crcOri);
		}else{
			return NV_CHECK_SUM_ERROR;
		}
	}

	/* check the correct, if correct, change the state in the index */
	idxInfo.usedState = ITEM_FIELD_VALID;
	if(single){
		idxInfo.usedState = ITEM_FIELD_VALID_SINGLE;
	}

	u32 crcIdx = 0xffffffff;
	crcIdx = xcrc32((u8 *)&idxInfo, sizeof(nv_info_idx_t)-1, crcIdx);
	hdr.checkSum = ((crcIdx & 0xffff ) << 16) | (crcOri & 0xffff);  //checkSum;

	if(flash_writeWithCheck(idxInfo.offset, sizeof(itemHdr_t), (u8*)&hdr) != TRUE){
		return NV_CHECK_SUM_ERROR;
	}

	u8 staOffset = OFFSETOF(nv_info_idx_t, usedState);
	flash_writeWithCheck(idxStartAddr+opItemIdx*sizeof(nv_info_idx_t)+staOffset, 1, (u8 *)&idxInfo.usedState);

	return NV_SUCC;
}


nv_sts_t nv_flashReadByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx, u16 len, u8 *buf){
	nv_sts_t ret = NV_SUCC;
	u32 idxStartAddr = MODULE_IDX_START(id, opSect);
	nv_info_idx_t idx;

	flash_read(idxStartAddr + opIdx * sizeof(nv_info_idx_t), sizeof(nv_info_idx_t), (u8 *)&idx);
	if(!ITEM_VALID_FLAG_CHECK(idx.usedState)){
		return NV_ITEM_NOT_FOUND;
	}

	itemHdr_t hdr;
	flash_read(idx.offset, sizeof(itemHdr_t), (u8*)&hdr);

	bool lenMatch = NV_ITEMLEN_MATCH(hdr.size, len);
	u16  realLen = nv_itemLengthValidCheck(itemId, hdr.size);
	u32 readOft = 0;

	if(lenMatch && (ITEM_HDR_VALID_CHECK(hdr.used)) && hdr.itemId == itemId){
		flash_read(idx.offset + sizeof(itemHdr_t), realLen, buf);
		readOft = idx.offset + sizeof(itemHdr_t) + realLen;

		u8 pTemp[32] = {0};
		u32 opLen = 0;
		s32 leftSize = 0;

		// check check_sum
		if(hdr.used == ITEM_HDR_FIELD_VALID_CHECKSUM){
			u32 checkSum = 0;

			for(s32 i = 0; i < realLen; i++){
				checkSum += buf[i];
			}

			if(realLen < hdr.size){
				leftSize = hdr.size - realLen;
				while(leftSize > 0){
					opLen = (leftSize < 32) ? leftSize : 32;
					flash_read(readOft, opLen, pTemp);
					for(s32 i = 0; i < opLen; i++){
						checkSum += pTemp[i];
					}
					leftSize -= opLen;
					readOft += opLen;
				}
			}
			if(checkSum != hdr.checkSum){
				ret = NV_CHECK_SUM_ERROR;
			}
		}else{
			u32 crcRead = 0xffffffff;
			crcRead = xcrc32(buf, realLen, crcRead);

			if(realLen < hdr.size){
				leftSize = hdr.size - realLen;
				while(leftSize > 0){
					opLen = (leftSize < 32) ? leftSize : 32;
					flash_read(readOft, opLen, pTemp);
					crcRead = xcrc32(pTemp, opLen, crcRead);
					leftSize -= opLen;
					readOft += opLen;
				}
			}

			crcRead = xcrc32((u8 *)(&(hdr.size)), 4, crcRead);
			if((crcRead & 0xffff) != (hdr.checkSum & 0xffff)){
				ret = NV_CHECK_SUM_ERROR;
			}
		}
	}else{
		ret = NV_ITEM_NOT_FOUND;
	}
	return ret;
}

nv_sts_t nv_itemDeleteByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx){
	nv_sts_t ret = NV_SUCC;
	u32 idxStartAddr = MODULE_IDX_START(id, opSect);
	nv_info_idx_t idx;
	u8 staOffset = OFFSETOF(nv_info_idx_t, usedState);
	idx.usedState = ITEM_FIELD_INVALID;
	flash_write(idxStartAddr+opIdx*sizeof(nv_info_idx_t)+staOffset, 1, &(idx.usedState));
	return ret;
}

nv_sts_t nv_flashSingleItemRemove(u8 id, u8 itemId, u16 len){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t sectInfo;
	u8 opSect = 0;
	s32 idxTotalNum = 0;
	u16 opIdx = 0;

	ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
	if(ret != NV_SUCC){
		return ret;
	}

	opSect = sectInfo.opSect;
	idxTotalNum = MODULE_IDX_NUM(id);

	ret = NV_ITEM_NOT_FOUND;
	ret = nv_index_read(id, itemId, len, opSect, idxTotalNum, &opIdx);
	if(ret == NV_SUCC){
		ret = nv_itemDeleteByIndex(id, itemId, opSect, opIdx);
	}

	return ret;
}

nv_sts_t nv_flashSingleItemSizeGet(u8 id, u8 itemId, u16 *len){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t sectInfo;
	u8 opSect = 0;
	s32 idxTotalNum = 0;
	u16 opIdx = 0;

	ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
	if(ret != NV_SUCC){
		return ret;
	}

	opSect = sectInfo.opSect;
	idxTotalNum = MODULE_IDX_NUM(id);

	ret = NV_ITEM_NOT_FOUND;
	ret = nv_index_read_op(id, itemId, len, opSect, idxTotalNum, &opIdx, 0);
	return ret;
}

static void nv_exceptionDataHandler(bool single, u8 opSect, u16 id, u8 itemId, u16 len){
	s16 idxTotalNum = MODULE_IDX_NUM(id);
	s16 validNum = 0;
	s16 opIdx = 0;
	nv_info_idx_t idx;
	u8 *idxTbl = ev_buf_allocate(idxTotalNum);
	opIdx = idxTotalNum - 1;
	bool add = 0;
	u32 idxStartAddr = MODULE_IDX_START(id, opSect);

	if(idxTbl){
		//T_nv_exceptionDataHandler[0]++;
		memset(idxTbl, 0, idxTotalNum);
		for(; opIdx >= 0; opIdx--){
			flash_read(idxStartAddr + opIdx * sizeof(nv_info_idx_t), sizeof(nv_info_idx_t), (u8 *)&idx);
			if(!ITEM_VALID_FLAG_CHECK(idx.usedState)){
				continue;
			}

			add = 1;
			if(idx.usedState == ITEM_FIELD_VALID_SINGLE || (idx.itemId == id && single)){
				for(s32 i = 0; i < validNum; i++){
					if(idx.itemId == idxTbl[i]){
						nv_itemDeleteByIndex(id, itemId, opSect, opIdx);
						add =0;
						//T_nv_exceptionDataHandler[1]++;
					}
				}
				if(add){
					idxTbl[validNum++] = idx.itemId;
				}
			}
		}
		ev_buf_free(idxTbl);
	}
}


static nv_sts_t nv_flashWriteNewHandler(bool forceChgSec, u8 single, u16 id, u8 itemId, u16 len, u8 *buf){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t sectInfo;
	u8 opSect = 0;
	u32 moduleStartAddr = MODULES_START_ADDR(id);

	s32 i = 0;

	/* check item length, if  */
	if(NV_SECTOR_SIZE(id) < ITEM_TOTAL_LEN(len) + MODULE_INFO_SIZE(id)){
		return NV_NOT_ENOUGH_SAPCE;
	}

	/* search valid operation sub-sector */
	ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
	if(ret != NV_SUCC){
		opSect = 0;
		for(s32 j = 0; j < NV_SECTOR_SIZE(id)/FLASH_SECTOR_SIZE; j++){
			flash_erase(moduleStartAddr + j * FLASH_SECTOR_SIZE);
		}
	}else{
		opSect = sectInfo.opSect;
	}


	/*
	 * search the index, find a valid space to write
	 * read 4 indexes once */
	u32 idxStartAddr = MODULE_IDX_START(id, opSect);
	u16 wItemIdx = 0;
	nv_info_idx_t idxInfo[4];
	u8 sectorUpdate = 0;
	s32 idxTotalNum = MODULE_IDX_NUM(id);
	itemHdr_t hdr;

	if(forceChgSec){
		sectorUpdate = 1;
	}else{
		ret = nv_index_read(id, ITEM_FIELD_IDLE, len, opSect, idxTotalNum, &wItemIdx);
		if(ret == NV_SUCC){
			flash_read(idxStartAddr + wItemIdx * sizeof(nv_info_idx_t), sizeof(nv_info_idx_t), (u8 *)idxInfo );

			if((wItemIdx == idxTotalNum - 1) ||
					(idxInfo[0].offset + idxInfo[0].size + ITEM_TOTAL_LEN(len)) > MODULE_SECT_END(id, opSect)){
				sectorUpdate = 1;
			}else{
				if(ITEM_FLAG_CHECK(idxInfo[0].usedState) && ITEM_OFT_CHECK(idxInfo[0].offset, idxInfo[0].size, id, opSect)){
					wItemIdx += 1;
				}else{
					sectorUpdate = 1;
				}
			}
		}
	}

	u8 oldSect = opSect;
	if(sectorUpdate){
		wItemIdx = 0;
		opSect = (opSect + 1) & (MODULE_SECTOR_NUM - 1);

		u8 nv_realSectNum = NV_SECTOR_SIZE(id)/FLASH_SECTOR_SIZE;
		u32 eraseAddr = moduleStartAddr + opSect * NV_SECTOR_SIZE(id);
		for(s32 k = 0; k < nv_realSectNum; k++){
			//flash_erase(moduleStartAddr + opSect * FLASH_SECTOR_SIZE);
			flash_erase(eraseAddr);
			eraseAddr += FLASH_SECTOR_SIZE;
		}

		u32 sizeusedAddr = 0;
		u8 readIdxNum = 0;
		bool startChk = 1;
		bool search = 1;
		u32 offsetPrev = 0;
		u32 sizePrev = 0;
		sizeusedAddr = MODULE_CONTEXT_START(id, opSect, len);
		idxStartAddr = MODULE_IDX_START(id, oldSect);
		idxTotalNum = MODULE_IDX_NUM(id);
		/* copy valid items to new sector */
		nv_exceptionDataHandler(single, oldSect, id, itemId, len);

		ret = nv_sect_info_check(oldSect, id);
		if(ret == NV_SUCC){
			while(idxTotalNum > 0 && search){
				readIdxNum = (idxTotalNum > 4) ? 4 : idxTotalNum;
				flash_read(idxStartAddr, readIdxNum * sizeof(nv_info_idx_t), (u8*)idxInfo);
				for( i = 0; i < readIdxNum; i++){
					if(startChk){
						if(idxInfo[i].offset != MODULE_CONTEXT_START(id, oldSect, len) ||
						  (MODULE_SECT_END(id, oldSect)-idxInfo[i].offset) < idxInfo[i].size){
							search = 0;
							break;
						}

						startChk = 0;
						offsetPrev = idxInfo[i].offset;
						sizePrev = idxInfo[i].size;
					}else{
						if(idxInfo[i].offset != (((offsetPrev + sizePrev)+0x03)&(~0x03)) ||
						  (MODULE_SECT_END(id, oldSect)-idxInfo[i].offset) < idxInfo[i].size){
							search = 0;
							break;
						}
						offsetPrev = idxInfo[i].offset;
						sizePrev = idxInfo[i].size;
					}

					if(ITEM_VALID_FLAG_CHECK(idxInfo[i].usedState) &&
						(idxInfo[i].itemId != itemId || ((idxInfo[i].itemId == itemId) && !single))){
						u8 sgl = (idxInfo[i].usedState == ITEM_FIELD_VALID_SINGLE);

						/* read item header, valid it */
						flash_read(idxInfo[i].offset, sizeof(itemHdr_t), (u8 *)&hdr);
						if(hdr.used == ITEM_HDR_FIELD_VALID_CHECKCRC){
							u32 crcIdx = 0xffffffff;
							crcIdx = xcrc32((u8 *)&idxInfo[i], sizeof(nv_info_idx_t)-1, crcIdx);
							if((crcIdx & 0xffff) != ((hdr.checkSum >> 16)&0xffff)){
								continue;
							}
						}

						ret = nv_write_item(sgl, id, idxInfo[i].itemId, opSect, wItemIdx, idxInfo[i].size-sizeof(itemHdr_t), (u8*)idxInfo[i].offset, TRUE);
						if(ret != NV_SUCC){
							if(ret == NV_ITEM_CHECK_ERROR){
								search = 0;
								break;
							}else if(ret == NV_NOT_ENOUGH_SAPCE || ret == NV_CHECK_SUM_ERROR){
								return ret;
							}else{
								continue;
							}
						}
						sizeusedAddr += idxInfo[i].size;
						sizeusedAddr = ((sizeusedAddr + 0x03) & (~0x03));
						wItemIdx += 1;
					}
				}
				idxTotalNum -= readIdxNum;
				idxStartAddr += readIdxNum * sizeof(nv_info_idx_t);
			}
		}

		idxTotalNum = MODULE_IDX_NUM(id);
		if(wItemIdx == idxTotalNum || (sizeusedAddr + ITEM_TOTAL_LEN(len)) > MODULE_SECT_END(id, opSect)){
			return NV_NOT_ENOUGH_SAPCE;
		}
	}


	/* search the same item id, need to delete after update */
	u8 deleteItemEn = 0;
	u16 deleteIdx = 0;
	idxTotalNum = MODULE_IDX_NUM(id);
	if(single){
		if(NV_SUCC == nv_index_read(id, itemId, len, opSect, idxTotalNum, &deleteIdx)){
			deleteItemEn = 1;
		}
	}

	/* sector is full, and then need write in the another sector */
	ret = nv_write_item(single, id, itemId, opSect, wItemIdx, len, buf, FALSE);
	if(ret != NV_SUCC){
		return ret;
	}

	/* the last item set as invalid */
	idxStartAddr = MODULE_IDX_START(id, opSect);
	if(deleteItemEn){
		u8 staOffset = OFFSETOF(nv_info_idx_t, usedState);
		idxInfo[0].usedState = ITEM_FIELD_INVALID;
		if(flash_writeWithCheck(idxStartAddr+deleteIdx*sizeof(nv_info_idx_t)+staOffset, 1, &(idxInfo[0].usedState)) != TRUE){
			return NV_CHECK_SUM_ERROR;
		}
	}


	if(ret == NV_SUCC){
		if(sectorUpdate){
			sectInfo.idName = id;

			sectInfo.usedFlag = NV_SECTOR_VALID_READY_CHECKCRC;
			sectInfo.opSect = opSect;

			u32 sectCrc = 0xffffffff;
			sectCrc = xcrc32(&(sectInfo.idName), 2, sectCrc);   //add (idName, opsect) to crc validation
			sectCrc = sectCrc & 0x3f;
			sectInfo.opSect = (sectCrc << NV_SECT_INFO_SECTNO_BITS) | sectInfo.opSect;
			if(flash_writeWithCheck(MODULE_SECT_START(id, opSect), sizeof(nv_sect_info_t), (u8*)&sectInfo) != TRUE){
				return NV_CHECK_SUM_ERROR;
			}

			sectInfo.usedFlag = NV_SECTOR_INVALID;
			if(flash_writeWithCheck(MODULE_SECT_START(id, oldSect), sizeof(sectInfo.usedFlag), (u8*)&sectInfo.usedFlag) == TRUE){
				sectInfo.usedFlag = NV_SECTOR_VALID_CHECKCRC;
				flash_writeWithCheck(MODULE_SECT_START(id, opSect), sizeof(sectInfo.usedFlag), (u8*)&sectInfo.usedFlag);
			}
		}else{
			 if(wItemIdx == 0){
				sectInfo.idName = id;
				sectInfo.usedFlag = NV_SECTOR_VALID_CHECKCRC;
				sectInfo.opSect = opSect;

				u32 sectCrc = 0xffffffff;
				sectCrc = xcrc32(&(sectInfo.idName), 2, sectCrc);  //add (idName, opsect) to crc validation
				sectInfo.opSect = (sectCrc << NV_SECT_INFO_SECTNO_BITS) | sectInfo.opSect;
				flash_writeWithCheck(MODULE_SECT_START(id, opSect), sizeof(nv_sect_info_t), (u8*)&sectInfo);
			 }
		}
	}

	return ret;
}

nv_sts_t nv_flashWriteNew(u8 single, u16 id, u8 itemId, u16 len, u8 *buf){
	if(id == NV_MODULE_NWK_FRAME_COUNT){
		return NV_INVALID_MODULS;
	}

	nv_sts_t ret = nv_flashWriteNewHandler(0, single, id, itemId, len, buf);

	if(ret == NV_ITEM_CHECK_ERROR){
		ret = nv_flashWriteNewHandler(1, single, id, itemId, len, buf);
	}

	return ret;
}


nv_sts_t nv_flashReadNew(u8 single, u8 id, u8 itemId, u16 len, u8 *buf){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t sectInfo;
	u8 opSect = 0;
	s32 idxTotalNum = 0;
	u16 opIdx = 0;

	if(id == NV_MODULE_NWK_FRAME_COUNT){
		return NV_INVALID_MODULS;
	}

	ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
	if(ret != NV_SUCC){
		return ret;
	}

	opSect = sectInfo.opSect;
	idxTotalNum = MODULE_IDX_NUM(id);

	ret = NV_ITEM_NOT_FOUND;
	ret = nv_index_read(id, itemId, len, opSect, idxTotalNum, &opIdx);
	if(single){
		if(ret == NV_SUCC){
			ret = nv_flashReadByIndex(id, itemId, opSect, opIdx, len, buf);
			if(ret == NV_CHECK_SUM_ERROR){
				nv_itemDeleteByIndex(id, itemId, opSect, opIdx);
			}
		}
	}else{
		itemIfno_t *pInfo = (itemIfno_t*)buf;
		pInfo->opIndex = opIdx;
		pInfo->opSect = opSect;
	}

	return ret;
}


nv_sts_t nv_nwkFrameCountSearch(u8 id, u8 opSect, u32 *frameCount, u32 *validAddr){
	nv_sts_t ret = NV_ITEM_NOT_FOUND;
	u16 frmCntTotalNum = FRAMECOUNT_NUM_PER_SECT();
	u32 sizeUsed = frmCntTotalNum * 4;
	u32 pageUsed = (sizeUsed & 0xff) ? sizeUsed/FLASH_PAGE_SIZE+1 : sizeUsed/FLASH_PAGE_SIZE;
	u16 checkSize = 0;
	*validAddr = FRAMECOUNT_PAYLOAD_START(opSect);

	u32 *opPageAddr = (u32 *)(MODULE_SECT_END(id, opSect) - FLASH_PAGE_SIZE);
	s32 i = 0;
	u32 checkValid[4];

	for(i = 0; i < pageUsed-1; i++){
		flash_read((u32)opPageAddr, 4, (u8 *)checkValid);
		if(checkValid[0] != 0xffffffff){
			checkSize = FLASH_PAGE_SIZE;
			break;
		}else{
			*validAddr = (u32)opPageAddr;	//???validAddr = opPageAddr;
		}
		opPageAddr -= FLASH_PAGE_SIZE/4;
	}
	if( i == pageUsed-1){
		opPageAddr = (u32 *)(FRAMECOUNT_PAYLOAD_START(opSect));
		checkSize = (FLASH_PAGE_SIZE - sizeof(nv_sect_info_t)) & (~0x03);
	}

	opPageAddr = (u32 *)(((u32)opPageAddr + FLASH_PAGE_SIZE) & (~(FLASH_PAGE_SIZE - 1)));
	u8 unitSize = 16;
	while(checkSize){
		u8 readSize = (checkSize > unitSize) ? unitSize : checkSize;
		opPageAddr -= readSize/4;
		checkSize -= readSize;

		flash_read((u32)opPageAddr, readSize, (u8 *)checkValid);
		if(checkValid[0] != 0xffffffff){
			*frameCount = checkValid[0];
			for(s32 i = 1; i < readSize/4; i++){
				if(checkValid[i] != 0xffffffff){
					*frameCount = checkValid[i];
					if(i == (readSize/4 - 1)){
						*validAddr = (u32)(opPageAddr + i + 1);
						return NV_SUCC;
					}
				}else{
					*validAddr = (u32)(opPageAddr + i);
					return NV_SUCC;
				}
			}
		}else{
			*validAddr = (u32)opPageAddr;
		}
	}

	return ret;
}


nv_sts_t nv_nwkFrameCountSaveToFlashHandler(bool forceChgSec, u8 *errSect, u32 frameCount){
	nv_sts_t ret = NV_SUCC;
	nv_sect_info_t sectInfo;
	u8 opSect = 0;
	u8 id = NV_MODULE_NWK_FRAME_COUNT;
	u32 moduleStartAddr = MODULES_START_ADDR(id);

	/* search valid operation sub-sector */
	if(forceChgSec){
		opSect = (*errSect + 1) & (MODULE_SECTOR_NUM - 1);
		u8 nv_realSectNum = NV_SECTOR_SIZE(id)/FLASH_SECTOR_SIZE;
		u32 eraseAddr = moduleStartAddr + opSect * NV_SECTOR_SIZE(id);
		for(s32 k = 0; k < nv_realSectNum; k++){
			//flash_erase(moduleStartAddr + opSect * FLASH_SECTOR_SIZE);
			flash_erase(eraseAddr);
			eraseAddr += FLASH_SECTOR_SIZE;
		}

	}else{
		ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
		if(ret != NV_SUCC){
			for(s32 j = 0; j < MODULE_SECTOR_NUM; j++){
				flash_erase(moduleStartAddr + j * FLASH_SECTOR_SIZE);
			}
			opSect = 0;
		}else{
			opSect = sectInfo.opSect;
		}
	}

	u32 lastFrmCnt;
	u32 wAddr;
	u8 oldSect = opSect;
	u8 sectorUpdate = 0;
	if(NV_SUCC == nv_nwkFrameCountSearch(id, opSect, &lastFrmCnt, &wAddr)){
		if(wAddr == (MODULE_SECT_END(id, opSect))){
			opSect = (opSect + 1) & (MODULE_SECTOR_NUM - 1);
			flash_erase(moduleStartAddr + opSect * FLASH_SECTOR_SIZE);

			if(flash_writeWithCheck(FRAMECOUNT_PAYLOAD_START(opSect), 4, (u8*)&lastFrmCnt) != TRUE){
				return NV_CHECK_SUM_ERROR;
			}

			wAddr = FRAMECOUNT_PAYLOAD_START(opSect) + 4;
			sectorUpdate = 1;
		}
	}

	if(flash_writeWithCheck(wAddr, 4, (u8*)&frameCount) != TRUE){
		return NV_CHECK_SUM_ERROR;
	}

	if(sectorUpdate || (wAddr == FRAMECOUNT_PAYLOAD_START(opSect))){
		sectInfo.idName = id;
		sectInfo.usedFlag = NV_SECTOR_VALID_READY_CHECKCRC;
		sectInfo.opSect = opSect;
		u32 sectCrc = 0xffffffff;
		sectCrc = xcrc32(&(sectInfo.idName), 2, sectCrc);   //add (idName, opsect) to crc validation
		sectCrc = sectCrc & 0x3f;
		sectInfo.opSect = (sectCrc << NV_SECT_INFO_SECTNO_BITS) | sectInfo.opSect;
		if(flash_writeWithCheck(MODULE_SECT_START(id, opSect), sizeof(nv_sect_info_t), (u8*)&sectInfo) != TRUE){
			return NV_CHECK_SUM_ERROR;
		}

		if(sectorUpdate || forceChgSec){
			if(forceChgSec){
				oldSect = *errSect;
			}
			sectInfo.usedFlag = NV_SECTOR_INVALID;
			if(flash_writeWithCheck((u32)MODULE_SECT_START(id, oldSect), sizeof(sectInfo.usedFlag), (u8 *)&sectInfo.usedFlag) != TRUE){
				return NV_CHECK_SUM_ERROR;
			}

			sectInfo.usedFlag = NV_SECTOR_VALID_CHECKCRC;
			flash_writeWithCheck((u32)MODULE_SECT_START(id, opSect), sizeof(sectInfo.usedFlag), (u8 *)&sectInfo.usedFlag);
		}
	}

	return ret;
}

nv_sts_t nv_nwkFrameCountSaveToFlash(u32 frameCount){
	u8 sect = 0xff;
	nv_sts_t ret = nv_nwkFrameCountSaveToFlashHandler(0, &sect, frameCount);
	if(ret == NV_CHECK_SUM_ERROR){
		nv_nwkFrameCountSaveToFlashHandler(1, &sect, frameCount);
	}
	return NV_SUCC;
}


nv_sts_t nv_nwkFrameCountFromFlash(u32 *frameCount){
	nv_sts_t ret = NV_SUCC;
	u8 id = NV_MODULE_NWK_FRAME_COUNT;
	nv_sect_info_t sectInfo;
	u32 lastFrmCnt;
	u32 wAddr;
	u8 opSect = 0;

	/* search valid operation sub-sector */
	ret = nv_sector_read(id, MODULE_SECTOR_NUM, &sectInfo);
	if(ret != NV_SUCC){
		return ret;
	}
	opSect = sectInfo.opSect;

	ret = nv_nwkFrameCountSearch(id, opSect, &lastFrmCnt, &wAddr);
	if(ret == NV_SUCC){
		*frameCount = lastFrmCnt;

		if(wAddr > FRAMECOUNT_PAYLOAD_START(opSect) + 8){
			u32 pCnt[2];
			flash_read(wAddr-8, 8, (u8 *)pCnt);

			if((pCnt[1] - pCnt[0]) > UPDATE_FRAMECOUNT_THRES){
				/* backoff valid framecount to another sector */
				*frameCount = pCnt[0]+UPDATE_FRAMECOUNT_THRES;
				nv_nwkFrameCountSaveToFlashHandler(1, &opSect, pCnt[0]+UPDATE_FRAMECOUNT_THRES);
			}
		}
	}

	return ret;
}

nv_sts_t nv_resetModule(u8 modules){
	u32 eraseAddr = MODULES_START_ADDR(modules);
	u8 sectNumber = NV_SECTOR_SIZE(modules)/FLASH_SECTOR_SIZE;
	for(s32 i = 0; i < MODULE_SECTOR_NUM; i++){
		for(s32 j = 0; j < sectNumber; j++){
			flash_erase(eraseAddr);
			eraseAddr += FLASH_SECTOR_SIZE;
		}
	}
	return SUCCESS;
}


nv_sts_t nv_resetAll(void){
#if NV_ENABLE
	foreach(i, NV_MAX_MODULS){
		nv_resetModule(i);
	}
#endif
	return SUCCESS;
}

nv_sts_t nv_init(u8 rst){
	if(rst){
		/* if reset is true, erase all flash for NV */
		nv_resetAll();
	}
	return NV_SUCC;
}

nv_sts_t nv_resetToFactoryNew(void){
#if NV_ENABLE
	if(!nv_facrotyNewRstFlagCheck()){
		nv_facrotyNewRstFlagSet();
	}

	foreach(i, NV_MAX_MODULS){
		if(i != NV_MODULE_NWK_FRAME_COUNT){
			nv_resetModule(i);
		}
	}

	nv_facrotyNewRstFlagClear();
#endif
	return SUCCESS;
}

bool nv_facrotyNewRstFlagCheck(void){
	u8 flag = ITEM_FIELD_IDLE;
	flash_read(CFG_FACTORY_RST_CNT, 1, &flag);

	return ((flag == 0xff) ? FALSE : TRUE);
}

void nv_facrotyNewRstFlagSet(void){
	u8 flag = ITEM_FIELD_VALID;
	flash_writeWithCheck(CFG_FACTORY_RST_CNT, 1, &flag);
}

void nv_facrotyNewRstFlagClear(void){
	flash_erase(CFG_FACTORY_RST_CNT);
}
