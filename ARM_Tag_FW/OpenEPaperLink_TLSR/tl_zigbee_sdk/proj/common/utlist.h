/********************************************************************************************************
 * @file    utlist.h
 *
 * @brief   This is the header file for utlist
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

#ifndef UTLIST_H
#define UTLIST_H

#define LIST_ADD(head, ele)							do{ \
														(ele)->next = head;         							\
														head = ele;                 							\
													}while(0)


#define LIST_DELETE(head, ele)						do{ \
														__typeof(head) lst;                                 	\
														if ((head) == (ele)) {                              	\
															(head) = (head)->next;                          	\
														} else {                                            	\
															lst = head;                                     	\
															while (lst->next && (lst->next != (ele))) {     	\
																lst = lst->next;                            	\
															}                                               	\
															if (lst->next) {                                	\
																lst->next = ((ele)->next);                  	\
															}                                               	\
														}                                                   	\
													}while(0)




#define LIST_EXIST(head, ele, result) 				do{	\
														for(result = head; result; result = (result)->next){	\
															if(result == ele){									\
																break;											\
															}													\
														} 			       						 				\
													}while(0)


#endif /* UTLIST_H */

