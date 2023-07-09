#include "sdb.h"
BT bt_pool[MAXBTSIZE] = {};
extern FuncInfo elf_func[1024];
extern int elf_index;
int current_bt = 0;
void insert_bt(uint64_t addr, uint8_t type, uint64_t goal_pc, uint64_t instruction)
{
    #ifdef CONFIG_FTRACE_DEBUG
	if (current_bt > MAXBTSIZE)
	{
		printf("bracktrace buff is too small!\n");
		return;
	}
	if (instruction == 0x00008067) // ret
	{
		bt_pool[current_bt].type = 0;
	}
	else
	{
		bt_pool[current_bt].type = 1;
	}
	for (size_t i = 0; i < elf_index; i++)
	{
		if (addr >= elf_func[i].start && addr < elf_func[i].start + elf_func[i].size)
		{
			bt_pool[current_bt].now_fun = &elf_func[i];
		}
		if (goal_pc >= elf_func[i].start && goal_pc < elf_func[i].start + elf_func[i].size)
		{
			bt_pool[current_bt].next_fun = &elf_func[i];
		}
	}
	current_bt++;
    #endif
	return;
}
void show_bt_trace()
{
	int call_n = 0;
	for (size_t i = 0; i < current_bt; i++)
	{
		for (size_t j = 0; j < call_n; j++)
		{
			printf(" ");
		}
		if (bt_pool[i].type == 1) // call
		{
			call_n++;
			printf("%s-->%s\n", bt_pool[i].now_fun->func_name, bt_pool[i].next_fun->func_name);
		}
		else // ret
		{
			call_n--;
			printf("%s<--%s\n", bt_pool[i].next_fun->func_name, bt_pool[i].now_fun->func_name);
		}
	}
}