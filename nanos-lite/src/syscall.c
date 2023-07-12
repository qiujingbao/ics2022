#include <common.h>
#include "syscall.h"
#include <proc.h>
/* 0是系统调用号 1 2 3分别是第x个参数  */
static uintptr_t args[4];
extern size_t fs_write(int fd, const void *buf, size_t count);
int sys_exit()
{
  halt((int)args[1]);
  return 0;
}
int sys_yield()
{
  yield();
  return 0;
}
int sys_write()
{
  int fd = args[1];
  char *buf = (char *)args[2];
  size_t off = args[3];
  return fs_write(fd, buf, off);
}
int sys_sbrk()
{
  return 0;
}
/* steal from xv6 os */
static int (*syscalls[])(void) = {
    [SYS_yield] sys_yield,
    [SYS_exit] sys_exit,
    [SYS_write] sys_write,
    [SYS_brk] sys_sbrk,
};
void do_syscall(Context *c)
{
  args[0] = c->GPR1;
  args[1] = c->GPR2;
  args[2] = c->GPR3;
  args[3] = c->GPR4;
  if (0 <= args[0] && args[0] <= 20 && syscalls[args[0]])
  {
    /* do sys call*/
    c->GPRx = syscalls[args[0]]();
  }
  else
  {
    panic("Unhandled syscall ID = %d", args[0]);
  }
}
