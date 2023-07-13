#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename)
{
  Log("loader:%s",filename);
  Elf_Ehdr elf;
  int fd = fs_open(filename);
  assert(fd != -1);
  fs_lseek(fd, 0, SEEK_SET);
  fs_read(fd, &elf, sizeof(Elf_Ehdr));
  Elf_Phdr Phdr;
  assert(*(uint32_t *)elf.e_ident == 0x464c457f);
  for (int i = 0; i < elf.e_phnum; i++)
  {
    fs_lseek(fd, elf.e_phoff + i * elf.e_phentsize, SEEK_SET);
    fs_read(fd, &Phdr, sizeof(Phdr));
    if (Phdr.p_type == 1)
    {
      fs_lseek(fd, Phdr.p_offset, SEEK_SET);
      fs_read(fd, (void *)Phdr.p_vaddr, Phdr.p_filesz);
      for (unsigned int i = Phdr.p_filesz; i < Phdr.p_memsz; i++)
      {
        ((char *)Phdr.p_vaddr)[i] = 0;
      }
    }
  }
  return elf.e_entry;
  /* 加载第一个文件 */
  /*Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));

  Elf_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;*/
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}

int get_count_ptr(char *const argv[])
{
  int len = 0;
  while (argv[len] != NULL)
  {
    Log("%s",argv[len]);
    len++;
  }
  return len;
}
/*
两个栈一个用户栈一个内核栈
内核栈即为在PCB中声明的，用户栈是整个系统的尾部，通过GPRx也就是a0寄存器传递过去
*/
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
  Log("context uload %p %p",argv,envp);
  /* 分配8页也就是32KB 并把指针移动到末端 */
  char *sp = (char *)new_page(8) + 8 * PGSIZE;
  int argc = (argv == NULL) ? 0 : get_count_ptr(argv);
  int envc = (envp == NULL) ? 0 : get_count_ptr(envp);
  Log("%d %d ",argc,envc);
  // malloc temp argv
  char *args[argc];
  char *envs[envc];

  // fill the argc string to string segment
  for (int i = 0; i < argc; i++)
  {
    sp -= (strlen(argv[i]) + 1);
    strcpy(sp, argv[i]);
    args[i] = sp;
  }

  // fill the envp string to string segment
  for (int i = 0; i < envc; i++)
  {
    sp -= (strlen(envp[i]) + 1);
    strcpy(sp, envp[i]);
    envs[i] = sp;
  }
  // fill the point to string in stack
  char **spp = (char **)sp;

  spp--;
  *spp = NULL;
  for (int i = envc - 1; i >= 0; i--)
  {
    spp--;
    *spp = envs[i];
  }

  spp--;
  *spp = NULL;
  for (int i = argc - 1; i >= 0; i--)
  {
    spp--;
    *spp = args[i];
  }

  spp--;
  *((int *)spp) = argc;
  uintptr_t entry = loader(pcb, filename);
  Area stack = {pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = ucontext(&pcb->as, stack, (void *)entry);
  pcb->cp->GPRx = (uintptr_t)spp;
  /* 因为不同的用户进程不能都是用操作系统的尾部作为栈顶所以统一使用new_page的分配方式 */
  // pcb->cp->GPRx = (uintptr_t)heap.end;
   Log("entry:%p gprx:%p cp:%p",entry,pcb->cp->GPRx,pcb->cp);
}