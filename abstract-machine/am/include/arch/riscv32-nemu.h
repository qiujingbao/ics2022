#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  /*
    MAP(REGS, PUSH) 通用寄存器
    csrr t0, mcause 
    csrr t1, mstatus
    csrr t2, mepc
  */
 /*如果这个顺序不对程序不会立即崩溃，例如cause的顺序乱了后面 asm_irq_handler读不到正确的编号*/
  uintptr_t gpr[32];
  uintptr_t mcause, mstatus, mepc;
  void *pdir;
};
/*# define ARGS_ARRAY ("ecall", "a7", "a0", "a1", "a2", "a0")*/
/*
const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
*/
#define GPR1 gpr[17] // a7
#define GPR2 gpr[10] // a0
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]


#endif
