/********************************************************************************************************
 * @file    mempool.h
 *
 * @brief   This is the header file for mempool
 *
 * @author  Driver & Zigbee Group
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


typedef struct mem_block_t{
	struct mem_block_t	*next_block;
    char        		data[4];		// must 4 or 8 aligned, padding
}mem_block_t;

typedef struct mem_pool_t{
    mem_block_t	*free_list;
}mem_pool_t;

#define MEMPOOL_ALIGNMENT 	4
#define MEMPOOL_ITEMSIZE_2_BLOCKSIZE(s)		((s + (MEMPOOL_ALIGNMENT - 1)) & ~(MEMPOOL_ALIGNMENT-1))

#define MEMPOOL_DECLARE(pool_name, pool_mem, itemsize, itemcount)	\
	mem_pool_t pool_name;											\
	u8 pool_mem[MEMPOOL_ITEMSIZE_2_BLOCKSIZE(itemsize) * itemcount] _attribute_aligned_(4);

mem_pool_t *mempool_init(mem_pool_t *pool, void *mem, int itemsize, int itemcount);
void *mempool_alloc(mem_pool_t *pool);
void mempool_free(mem_pool_t *pool, void *p);
mem_block_t* mempool_header(char *pd);


