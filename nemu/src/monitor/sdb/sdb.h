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
#include <elf.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define MAXBUF 10
#define MAXBTSIZE 2048


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
/*if meet jump instrctions ignore it. just show next n instrctions*/
typedef struct ringbuf
{
  //store addr
  uint64_t addrs[MAXBUF];
  uint8_t front, rear;
  uint8_t now_pc;
  uint8_t count;
} RB;

typedef struct {
    char func_name[64]; // 函数名
    paddr_t start;      // 起始地址
    size_t size;        // 函数体大小
}FuncInfo;              // [start, start+size)
typedef struct backtrace{
  FuncInfo * now_fun;
  uint8_t type;
  FuncInfo * next_fun;
  struct backtrace *next;
}BT;

/*break point api*/
int inser_bp(uint64_t addr);
word_t expr(char *e, bool *success);

/* watchpoint api */
void init_wp_pool();
void display_watchpoint();
void delete_watchpoint(int no);
void insert_watchpoint(char *exp);
WP * re_mut_watchpoint();


/*ring buff api*/
void init_rb();
void delete_instraction();
void insert_instrction(uint64_t addr);
void show_ringbuf_ins_riscv64();

/*brack trace api*/
void insert_bt(uint64_t addr,uint8_t type,uint64_t goal_pc,uint64_t instruction);
void show_bt_trace();

/*elf api*/
void init_ftrace(char *elf_file);
void read_elf_header64(int32_t fd, Elf64_Ehdr *elf_header);
bool is_ELF64(Elf64_Ehdr eh);
void read_section_header_table64(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[]);
char * read_section64(int32_t fd, Elf64_Shdr sh);
void print_symbol_table64(int32_t fd,Elf64_Ehdr eh,Elf64_Shdr sh_table[],uint32_t symbol_table);
void print_symbols64(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[]);
#endif
