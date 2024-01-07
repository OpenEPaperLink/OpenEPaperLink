/********************************************************************************************************
 * @file    ev_queue.c
 *
 * @brief   This is the source file for ev_queue
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



/*********************************************************************
 * @fn      ev_queue_rawPush
 *
 * @brief   Push a raw data into a queue
 *          The element must compatible with the format @ref queue_item_t
 *
 * @param   q - The specified queue
 * @param   payload - Pointer to the new element 
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_rawPush( ev_queue_t* q, queue_item_t* newElement )
{
    queue_item_t* previous;
    queue_item_t* current;

    if (NULL == q || NULL == newElement) {
        return QUEUE_INVALID_PARAMETER;
    }

    u32 r = drv_disable_irq();

    /* if the Q was empty, then update the head */
    if (NULL==q->head) {
        q->head = newElement;
        q->tail = newElement;
        newElement->next = NULL;
        q->curNum++;
        drv_restore_irq(r);
        return (ev_queue_sts_t)SUCCESS;
    }
    /* find a place for insertion */
    previous = NULL;
    current = q->head;

    if (NULL == q->priFunc) { /* if priority is not used, insert at the end as in a FIFO */
        q->tail->next = newElement;
        newElement->next = NULL;
        q->tail = newElement;
    } else { /* if priority is used, insert at the end */
        while (current!=NULL) {
            /* Here, a small priority value has a higher priority */
            if(q->priFunc((arg_t)(newElement)) < q->priFunc((arg_t)(current))) {
                break;
            } else {
                previous = current;
                current = current->next;
            }
        }
        /* insert between previous and current */
        if(NULL==previous) { /* insert at the head */
            q->head = newElement;
        } else {
            previous->next = newElement;
        }
        newElement->next = current;
        if (NULL==current){
            q->tail = newElement;
        }
    }
    q->curNum++;

    drv_restore_irq(r);
    return (ev_queue_sts_t)SUCCESS;
}

/*********************************************************************
 * @fn      ev_queue_rawPop
 *
 * @brief   Pop data part of the element from the speified queue.
 *
 * @param   q - The specified queue
 *
 * @return  Pointer to first element in the queue
 */
queue_item_t* ev_queue_rawPop(ev_queue_t* q)
{
    queue_item_t* oldHead;

    u32 r = drv_disable_irq();

    oldHead = q->head;
    if (NULL!=oldHead){
        if (oldHead == q->tail) {
            q->tail = NULL;
        }
        /* update the head pointer */
        q->head = oldHead->next;
        q->curNum--;
    }

	if ( q->curNum == 0 ) {
		q->head = q->tail = NULL;
	}
	drv_restore_irq(r);
    return oldHead;
}

/*********************************************************************
 * @fn      ev_queue_rawDelete
 *
 * @brief   Delete an element from the queue
 *
 * @param   q - The specified queue
 * @param   payload - Pointer to data part of the bufferItem that is to be deleted
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_rawDelete(ev_queue_t* q, queue_item_t* delElement)
{
    queue_item_t* previous;
    queue_item_t* current;

    if (NULL == q || NULL == delElement) {
        return QUEUE_INVALID_PARAMETER;
    }

    u32 r = drv_disable_irq();

    if (NULL == q->head) { /* invalid q or newElement */
    	drv_restore_irq(r);
        return QUEUE_EMPTY;
    }

    if (q->head == delElement) {
        q->head = q->head->next;
        if (NULL == q->head){
            q->tail = NULL;
        }
        q->curNum--;
        drv_restore_irq(r);
        return (ev_queue_sts_t)SUCCESS;
    }

    /* find the to be delete node */
    previous = NULL;
    current = q->head;

    while(current != delElement && current) {
        previous = current;
        current = current->next;
    }

    if (current) {
        /* find it. */
        previous->next = current->next;
        if (NULL == current->next) {
            q->tail = previous;
        }
        q->curNum--;
    }
    else {
        /* element not in the Queue */
    	drv_restore_irq(r);
        return QUEUE_NOT_FOUND;

    }

    drv_restore_irq(r);
    return (ev_queue_sts_t)SUCCESS;
}



/*********************************************************************
 * @fn      ev_queue_init
 *
 * @brief   Initialize the EV queue 
 *
 * @param   q - The queue need to use
 * @param   priFunc - Pointer to the function that calculates a priority
 *                    NULL means not use the priority feature
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_init( ev_queue_t *q, ev_priFunc_t priFunc)
{
    if (NULL == q) {
        return QUEUE_INVALID_PARAMETER;
    }

    memset((u8*)q, 0 , sizeof(ev_queue_t));
    q->priFunc = priFunc;
    return (ev_queue_sts_t)SUCCESS;
}


/*********************************************************************
 * @fn      ev_queue_push
 *
 * @brief   Push a data part of an element into a queue
 *
 * @param   q - The queue that a new element need to push to
 * @param   payload - The payload of the new element   
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_push( ev_queue_t* q, u8* payload )
{
    queue_item_t* newElement;
    newElement = (queue_item_t*)ev_buf_getHead(payload);
    return ev_queue_rawPush(q, newElement);
}


/*********************************************************************
 * @fn      ev_queue_pop
 *
 * @brief   Pop data part of the element from the specified queue.
 *
 * @param   q - The queue that element need to pop from
 *
 * @return  Pointer to data part of the @ev_bufItem_t
 */
u8* ev_queue_pop( ev_queue_t* q )
{
    queue_item_t* oneItem = ev_queue_rawPop(q);

    if (NULL==oneItem) {
        return NULL;
    } else {
        return ((ev_bufItem_t*)oneItem)->data;
    }
}

/*********************************************************************
 * @fn      ev_queue_delete
 *
 * @brief   Delete an element from the queue
 *
 * @param   q - The specified queue
 * @param   payload - Pointer to data part of the buffer that is to be deleted
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_delete( ev_queue_t* q, u8* payload )
{
    queue_item_t* delElement = (queue_item_t*)ev_buf_getHead(payload);
    return ev_queue_rawDelete(q, delElement);
}


/*********************************************************************
 * @fn      ev_queue_freeQ
 *
 * @brief   Free a queue. This also deallocates all buffers in the queue.
 *
 * @param   q - The specified queue to free
 *
 * @return  Status
 */
ev_queue_sts_t ev_queue_freeQ( ev_queue_t *q )
{
    u8 *buffer_ptr;
    if (NULL == q) { /* invalid q or newElement */
        return QUEUE_INVALID_PARAMETER;
    }

    while (NULL != (buffer_ptr = ev_queue_pop(q))) {
        ev_buf_free(buffer_ptr);
    }

    return (ev_queue_sts_t)SUCCESS;
}


