#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
/* 中断处理函数 判断后跳转到回调函数 回调函数由操作系统指定 */
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    if (c->mcause >= 0 && c->mcause < 20)
    {
      ev.event = EVENT_SYSCALL;
    }
    else if (c->mcause == -1)
    {
      ev.event = EVENT_YIELD;
    }
    else
    {
      ev.event = EVENT_ERROR;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);
//注册do_event作为回调处理函数
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  // 正如前文所说设置中断处理函数
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  /*从栈尾找到一个context大小的位置，然后转换为conext给cp复制*/
  Context *c = (Context *)((uint8_t *)(kstack.end) - sizeof(Context));
  /*设置context的各个寄存器*/
  /*除此之外全部为0*/
  /* 配置寄存器即可 其余寄存器为0 */
  c->mepc = (uintptr_t)entry;
  c->mstatus = 0x1800;
  c->gpr[10] = (uintptr_t)arg;
  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
