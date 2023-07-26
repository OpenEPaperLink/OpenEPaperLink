/******************************************************************************
*  Filename:       rf_queue.h
*  Revised:        $ $
*  Revision:       $ $
*
*  Description:    Help functions for handling queues
*
*  Copyright (C) 2015-2017 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

//*****************************************************************************
//
//! \addtogroup rfqueue_api
//! @{
//
//*****************************************************************************

#ifndef RF_QUEUE_H
#define RF_QUEUE_H

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_data_entry.h)

#define RF_QUEUE_DATA_ENTRY_HEADER_SIZE  8 // Contant header size of a Generic Data Entry

#define RF_QUEUE_QUEUE_ALIGN_PADDING(length)  (4-((length + RF_QUEUE_DATA_ENTRY_HEADER_SIZE)%4)) // Padding offset

#define RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(numEntries, dataSize, appendedBytes)                                                    \
(numEntries*(RF_QUEUE_DATA_ENTRY_HEADER_SIZE + dataSize + appendedBytes + RF_QUEUE_QUEUE_ALIGN_PADDING(dataSize + appendedBytes)))

extern uint8_t RFQueue_nextEntry();
extern rfc_dataEntryGeneral_t* RFQueue_getDataEntry();
extern uint8_t RFQueue_defineQueue(dataQueue_t *queue ,uint8_t *buf, uint16_t buf_len, uint8_t numEntries, uint16_t length);

#endif

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//
//*****************************************************************************
