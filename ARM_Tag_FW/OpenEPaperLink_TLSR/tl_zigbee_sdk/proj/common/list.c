/********************************************************************************************************
 * @file    list.c
 *
 * @brief   This is the source file for list
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

#include "types.h"
#include "list.h"

struct list{
	struct list *next;
};

void listInit(list_t list){
	*list = NULL;
}

void *listHead(list_t list){
	return *list;
}

void listCopy(list_t dest, list_t src){
	*dest = *src;
}

void *listTail(list_t list){
	struct list *l;

	if(*list == NULL){
		return NULL;
	}

	for(l = *list; l->next != NULL; l = l->next);

	return l;
}

void listAdd(list_t list, void *item){
	struct list *l;

	((struct list *)item)->next = NULL;

	l = listTail(list);
	if(l == NULL){
		*list = item;
	}else{
		l->next = item;
	}
}

void listPush(list_t list, void *item){
	((struct list *)item)->next = *list;
	*list = item;
}

void *listChop(list_t list){
	struct list *l, *p;

	if(*list == NULL){
		return NULL;
	}

	if(((struct list *)*list)->next == NULL){
		l = *list;
		*list = NULL;
		return l;
	}

	for(l = *list; l->next->next != NULL; l = l->next);

	p = l->next;
	l->next = NULL;

	return p;
}

void *listPop(list_t list){
	if(*list != NULL){
		*list = ((struct list *)*list)->next;
	}

	return *list;
}

void listRemove(list_t list, void *item){
	struct list *l, *p;

	if(*list == NULL){
		return;
	}

	p = NULL;
	for(l = *list; l != NULL; l = l->next){
		if(l == item){
			if(p == NULL){
				*list = l->next;
			}else{
				p->next = l->next;
			}
			l->next = NULL;
			return;
		}
		p = l;
	}
}

int listLength(list_t list){
	struct list *l;
	int num = 0;

	for(l = *list; l != NULL; l = l->next){
		++num;
	}

	return num;
}

void listInsert(list_t list, void *prevItem, void *newItem){
	if(prevItem == NULL){
		listPush(list, newItem);
	}else{
		((struct list *)newItem)->next = ((struct list *)prevItem)->next;
		((struct list *)prevItem)->next = newItem;
	}
}
