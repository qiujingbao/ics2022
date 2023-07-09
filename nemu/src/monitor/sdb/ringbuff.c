#include "sdb.h"
RB rb;
extern word_t vaddr_ifetch(vaddr_t addr, int len);
void init_rb()
{
	rb.rear = 0;
	rb.front = 0;
	rb.now_pc = -1;
	rb.count = 0;
}
void insert_instrction(uint64_t addr)
{
	if ((rb.rear + 1) % MAXBUF == rb.front)
	{
		delete_instraction();
	}
	rb.addrs[rb.rear] = addr;
	rb.now_pc = rb.rear;
	rb.rear = (rb.rear + 1) % MAXBUF;
	rb.count++;
}
void delete_instraction()
{
	if (rb.front == rb.rear)
	{
		return;
	}
	rb.front = (rb.front + 1) % MAXBUF;
	rb.count--;
}

void show_ringbuf_ins_riscv64()
{
  printf("\t地址\t\t代码\n");
  size_t i = rb.front;
  size_t c = 0;
  for (; i < MAXBUF && c < rb.count; i++, c++)
  {
    i = (i) % MAXBUF;
    if (i == rb.now_pc)
      printf("--->\t%lx\t%08x\n", rb.addrs[i], vaddr_ifetch((vaddr_t)rb.addrs[i], 4));
    else
      printf("\t%lx\t%08x\n", rb.addrs[i], vaddr_ifetch((vaddr_t)rb.addrs[i], 4));
  }
  uint64_t x = rb.addrs[rb.now_pc];
  i = 1;
  for (; i < MAXBUF; i++)
  {
    printf("\t%lx\t%08x\n", x + 4 * i, vaddr_ifetch((vaddr_t)(x + i * 4), 4));
  }
}