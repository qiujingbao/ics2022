#include "sdb.h"
FuncInfo elf_func[1024];

int elf_index=0;
void init_ftrace(char *elf_file)
{
	int32_t fd;
	fd = open(elf_file, O_RDONLY | O_SYNC);
	if (fd < 0)
	{
		printf("Error %d Unable to open %s\n", fd, elf_file);
		return;
	}

	Elf32_Ehdr eh32;	/* elf-header is fixed size */
	Elf32_Shdr *sh_tbl; /* section-header table is variable size */

	read_elf_header32(fd, &eh32);

	/* Section header table :  */
	sh_tbl = malloc(eh32.e_shentsize * eh32.e_shnum+1);
	if (!sh_tbl)
	{
		printf("Failed to allocate %d bytes\n",
			   (eh32.e_shentsize * eh32.e_shnum));
	}
	read_section_header_table32(fd, eh32, sh_tbl);
	//print_section_headers32(fd, eh32, sh_tbl);

	print_symbols32(fd, eh32, sh_tbl);
	free(sh_tbl);
	for (size_t i = 0; i < elf_index; i++)
	{
		printf("%08x\t%02ld\t%s\n",elf_func[i].start,elf_func[i].size,elf_func[i].func_name);
	}
	return;
}

void read_elf_header32(int32_t fd, Elf32_Ehdr *elf_header)
{
	assert(elf_header != NULL);
	assert(lseek(fd, (off_t)0, SEEK_SET) == (off_t)0);
	assert(read(fd, (void *)elf_header, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr));
}

bool is_ELF32(Elf32_Ehdr eh)
{
	/* ELF magic bytes are 0x7f,'E','L','F'
	 * Using  octal escape sequence to represent 0x7f
	 */
	if (!strncmp((char *)eh.e_ident, "\177ELF", 4))
	{
		printf("ELFMAGIC \t= ELF\n");
		/* IS a ELF file */
		return 1;
	}
	else
	{
		printf("ELFMAGIC mismatch!\n");
		/* Not ELF file */
		return 0;
	}
}

void read_section_header_table32(int32_t fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32_t i;

	assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

	for (i = 0; i < eh.e_shnum; i++)
	{
		assert(read(fd, (void *)&sh_table[i], eh.e_shentsize) == eh.e_shentsize);
	}
}

char *read_section32(int32_t fd, Elf32_Shdr sh)
{
	char *buff = malloc(sh.sh_size);
	if (!buff)
	{
		printf("%s:Failed to allocate %dbytes\n","read_section32",sh.sh_size);
	}

	assert(buff != NULL);
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
	assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

	return buff;
}

void print_symbol_table32(int32_t fd,
						  Elf32_Ehdr eh,
						  Elf32_Shdr sh_table[],
						  uint32_t symbol_table)
{

	char *str_tbl;
	Elf32_Sym *sym_tbl;
	uint32_t i, symbol_count;

	sym_tbl = (Elf32_Sym *)read_section32(fd, sh_table[symbol_table]);

	/* Read linked string-table
	 * Section containing the string table having names of
	 * symbols of this section
	 */
	uint32_t str_tbl_ndx = sh_table[symbol_table].sh_link;
	str_tbl = read_section32(fd, sh_table[str_tbl_ndx]);

	symbol_count = (sh_table[symbol_table].sh_size / sizeof(Elf32_Sym));

	for (i = 0; i < symbol_count; i++)
	{
		if(ELF32_ST_TYPE(sym_tbl[i].st_info)==0x02)
		{
			
			strcpy(elf_func[elf_index].func_name,(str_tbl + sym_tbl[i].st_name));
			if(strcmp(elf_func[elf_index].func_name,"_start")==0)
			{
			elf_func[elf_index].size=16;
			}
			else
			{
			elf_func[elf_index].size=sym_tbl[i].st_size;
			}
			elf_func[elf_index].start= sym_tbl[i].st_value;
			elf_index++;
		}	
	}
	free(str_tbl);
	free(sym_tbl);
}

void print_symbols32(int32_t fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[])
{
	uint32_t i;

	for (i = 0; i < eh.e_shnum; i++)
	{
		if ((sh_table[i].sh_type == SHT_SYMTAB) || (sh_table[i].sh_type == SHT_DYNSYM))
		{
			printf("\n[Section %03d]", i);
			print_symbol_table32(fd, eh, sh_table, i);
		}
	}
}
