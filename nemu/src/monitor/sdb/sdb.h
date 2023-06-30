/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char exp[100];
  uint64_t val;
  uint64_t old_val;
} WP;
typedef struct braekpoint
{
  bool is_used;
  uint64_t addr;
}BP;

/*break point api*/
int inser_bp(uint64_t addr);
word_t expr(char *e, bool *success);

/* watchpoint api */
void init_wp_pool();
void display_watchpoint();
void delete_watchpoint(int no);
void insert_watchpoint(char *exp);
WP * re_mut_watchpoint();
#endif
