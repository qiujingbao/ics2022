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

#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}
enum {
  mepc = 0,
  mstatus = 1,
  mcause = 2,
  mtvec = 3
};
static inline int convet_csr_index(int idx)
{
  switch (idx)
  {
  case 773:
    return mtvec;
  case 768:
    return mstatus;
  case 833:
    return mepc;
  case 834:
    return mcause;
  default:
    assert(0);
  }
  return idx;
}
#define gpr(idx) cpu.gpr[check_reg_idx(idx)]
#define CSR_Register(imm) cpu.csr[convet_csr_index(imm)]
static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

#endif
