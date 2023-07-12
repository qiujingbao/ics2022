#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
/* 中断处理函数 判断后跳转到回调函数 回调函数由操作系统指定 */
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    printf("irq_handler:%lx\n",c->mepc);
    printf("irq_handler:%lx\n",c->mcause);
    switch (c->mcause) {
      default: ev.event = EVENT_ERROR; break;
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

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
