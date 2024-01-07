/********************************************************************************************************
 * @file    mempool.c
 *
 * @brief   This is the source file for mempool
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

#include "mempool.h"
#include "utility.h"

mem_pool_t *mempool_init(mem_pool_t *pool, void *mem, int itemsize, int itemcount)
{
    if(!pool || !mem)
		return (0);
	
	pool->free_list = (mem_block_t *)mem;

	unsigned int block_size = (unsigned int)(MEMPOOL_ITEMSIZE_2_BLOCKSIZE(itemsize));
	mem_block_t *tmp = (mem_block_t *)mem;
	int i;
	for(i = 0; i < itemcount - 1; ++i){
		tmp = tmp->next_block = (mem_block_t *)(((unsigned int)tmp) + block_size);
	}
	tmp->next_block = 0;
	return pool;
}

mem_block_t *mempool_header(char *pd)
{
	return (mem_block_t *)(pd - OFFSETOF(mem_block_t, data));
}

void *mempool_alloc(mem_pool_t *pool)
{
	if(!pool->free_list)
		return 0;

	mem_block_t *tmp = pool->free_list;
	pool->free_list = tmp->next_block;
	return &tmp->data;
}

void mempool_free(mem_pool_t *pool, void *p)
{
	mem_block_t *tmp = mempool_header((char *)p);
	tmp->next_block = pool->free_list;
	pool->free_list = tmp;
}
