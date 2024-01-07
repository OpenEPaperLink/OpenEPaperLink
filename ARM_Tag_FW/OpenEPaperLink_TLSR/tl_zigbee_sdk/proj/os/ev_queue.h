/********************************************************************************************************
 * @file    ev_queue.h
 *
 * @brief   This is the header file for ev_queue
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

#pragma once




/** @addtogroup  TELINK_COMMON_MODULE TELINK Common Module
 *  @{
 */

/** @addtogroup  EV_QUEUE EV Queue
 *  @{
 */
 
/** @defgroup EV_QUEUE_TYPE EV QUEUE Types
 *  @{
 */

/**        
 *  @brief Definition for priority calculate function
 */
typedef u32 (*ev_priFunc_t)(arg_t);

/**        
 *  @brief Structure of an item in the EV Queue
 */
typedef struct queue_item {
    struct queue_item *next;        //!<  Pointer to the next element
    u32 idx;
    u8 data[1];                     //!<  Pointer to the data
} queue_item_t;


/**
 *  @brief Definition for the EV Queue structure
 */
typedef struct priQueue {
    queue_item_t *head;             //!<  Pointer to the head item of the queue
    queue_item_t *tail;             //!<  Pointer to the tail item of the queue
    ev_priFunc_t priFunc;           //!<  Priority function, NULL means not use priority
    u32 curNum;                     //!<  Current number of entries in the queue
} ev_queue_t;

/**
 *  @brief Definition for the status of the EV Queue Module
 */
typedef enum ev_queue_sts_e {
    // SUCCESS should always be ZERO
	QUEUE_SUCC = 0,
    QUEUE_INVALID_PARAMETER = 1,    //!< Invalid parameter of the API
    QUEUE_EMPTY,                    //!< Queue is empty
    QUEUE_NOT_FOUND,                //!< Queue is not found
    QUEUE_OVERFLOW,                 //!< Queue is overflowed
} ev_queue_sts_t;

/**  @} end of group EV_QUEUE_TYPE */


/** @defgroup EV_QUEUE_FUNCTIONS EV Queue API
 *  @brief Function declaration of EV Queue module
 *  @{
 */

 /**
   * @brief       Initialize the EV queue 
   *
   * @param[in]   q        - The queue need to use
   * @param[in]   priFunc  - Pointer to the function that calculates a priority
   *                         NULL means not use the priority feature
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_init( ev_queue_t *q, ev_priFunc_t priFunc);


 /**
   * @brief       Push a data part of an element into a queue
   *
   * @param[in]   q        - The queue that a new element need to push to
   * @param[in]   payload  - The payload of the new element  
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_push( ev_queue_t* q, u8* payload );


/**
  * @brief       Pop data part of the element from the specified queue.
  *
  * @param[in]   q - The queue that element need to pop from
  *
  * @return      Pointer to data part of the @ev_bufItem_t
  */
u8* ev_queue_pop( ev_queue_t* q );


  /**
   * @brief       Delete an element from the queue
   *
   * @param[in]   q        - The specified queue
   * @param[in]   payload  - Pointer to data part of the buffer that is to be deleted
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_delete( ev_queue_t* q, u8* payload );


  /**
   * @brief       Free a queue. This also deallocates all buffers in the queue.
   *
   * @param[in]   q - The specified queue to free
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_freeQ( ev_queue_t* q);


  /**
   * @brief       Push a raw data into a queue
   *              The element must compatible with the format @ref queue_item_t
   *
   * @param[in]   q       - The specified queue
   * @param[in]   element - Pointer to the new element  
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_rawPush( ev_queue_t* q, queue_item_t* element );


 /**
   * @brief       Delete an element from the queue
   *              The element must compatible with the format @ref queue_item_t
   *
   * @param[in]   q       - The specified queue
   * @param[in]   element - Pointer to data part of the bufferItem that is to be deleted
   *
   * @return      Status
   */
ev_queue_sts_t ev_queue_rawDelete(ev_queue_t* q, queue_item_t* element);


 /**
  * @brief       Pop data part of the element from the speified queue. 
  *
  * @param[in]   q - The specified queue
  *
  * @return      Pointer to first element in the queue
  */
queue_item_t* ev_queue_rawPop(ev_queue_t* q);

/**  @} end of group EV_QUEUE_FUNCTIONS */

/**  @} end of group EV_QUEUE */

/**  @} end of group TELINK_COMMON_MODULE */

