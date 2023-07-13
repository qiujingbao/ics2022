#include <common.h>
#include "syscall.h"
#include <proc.h>
#include <fs.h>
/* 0是系统调用号 1 2 3分别是第x个参数  */
static uintptr_t args[4];
extern int read_timeofday(void *);
extern void naive_uload(void *pcb, const char *filename);
extern int execve(const char *filename, char *const argv[], char *const envp[]);

int sys_gettimeofday()
{
  return read_timeofday((void *)args[1]);
}

int sys_exit()
{
  naive_uload(NULL, "/bin/menu");
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
int sys_open()
{
  const char *name = (const char *)(args[1]);
  return fs_open(name);
}
int sys_close()
{
  int fd = args[1];
  return fs_close(fd);
}
int sys_read()
{
  int fd = args[1];
  char *buf = (char *)args[2];
  size_t off = args[3];
  return fs_read(fd, buf, off);
}
int sys_lseek()
{
  int fd = args[1];
  size_t off = args[2];
  int whnece = args[3];
  return fs_lseek(fd, off, whnece);
}
/* 如果要完成启动 游戏必须修改下面的代码 将参数传入程序*/
/* 启动的时候根据elf文件找到 static区 将参数写入 */
int sys_execve()
{
  Log("%s",(char *)args[1]);
  return execve((char *)args[1],(char**)args[2],(char**)args[3]);
}
/* steal from xv6 os */
static int (*syscalls[])(void) = {
    [SYS_yield] sys_yield,
    [SYS_exit] sys_exit,
    [SYS_write] sys_write,
    [SYS_brk] sys_sbrk,
    [SYS_read] sys_read,
    [SYS_open] sys_open,
    [SYS_close] sys_close,
    [SYS_lseek] sys_lseek,
    [SYS_gettimeofday] sys_gettimeofday,
    [SYS_execve] sys_execve,
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
