/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/06 14:05:11 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "ft_nm.h"

#define ELF_MAGIC 0x464c457f

static unsigned char get_letter(const Elf64_Sym *sym, const Elf64_Shdr *sects)
{
	uint64_t info = sym->st_info;
	uint64_t shndx = sym->st_shndx;
	uint64_t type = sects[shndx].sh_type;
	uint64_t flags = sects[shndx].sh_flags;
	char c = '?';

	if (ELF64_ST_BIND(info) == STB_GNU_UNIQUE)
		c = 'u';
	else if (ELF64_ST_BIND(info) == STB_WEAK)
	{
		c = 'W';
		if (shndx == SHN_UNDEF)
			c = 'w';
		if (ELF64_ST_TYPE(info) == STT_OBJECT) {
			c = 'V';
			if (shndx == SHN_UNDEF)
				c = 'v';
		}
	}
	else if (shndx == SHN_UNDEF)
		c = 'U';
	else if (shndx == SHN_ABS)
		c = 'A';
	else if (shndx == SHN_COMMON)
		c = 'C';
	else if (ELF64_ST_TYPE(info) == STT_GNU_IFUNC)
		c = 'i';

	// .bss || .tbss
	else if (type == SHT_NOBITS && (
			 flags == (SHF_ALLOC | SHF_WRITE)
		  || flags == (SHF_ALLOC | SHF_WRITE | SHF_TLS)
		  ))
		c = 'B';

	// .rodata || .rodata str
	else if (type == SHT_PROGBITS && (
			 flags == (SHF_ALLOC)
		  || flags == (SHF_ALLOC | SHF_MERGE)
		  || flags == (SHF_ALLOC | SHF_MERGE | SHF_STRINGS)
		  ))
		c = 'R';

	// .data || .tdata
	else if (type == SHT_PROGBITS && (
			 flags == (SHF_ALLOC | SHF_WRITE)
		  || flags == (SHF_ALLOC | SHF_WRITE | SHF_TLS)
		  ))
		c = 'D';

	// .init_array || .fini_array || .preinit_array
	else if (flags == (SHF_ALLOC | SHF_WRITE) && (
			 type == SHT_INIT_ARRAY
		  || type == SHT_FINI_ARRAY
		  || type == SHT_PREINIT_ARRAY
		  ))
		c = 'D';

	// .text || .text with group || .plt
	else if (type == SHT_PROGBITS && (
			 flags == (SHF_ALLOC | SHF_EXECINSTR)
		  || flags == (SHF_ALLOC | SHF_EXECINSTR | SHF_GROUP)
		  || flags == (SHF_WRITE | SHF_EXECINSTR | SHF_ALLOC)
		  ))
		c = 'T';

	else if (type == SHT_GROUP && !flags) //.group
		c = 'N';
	else if (type == SHT_DYNAMIC) //.dynamic
		c = 'D';

	if (ELF64_ST_BIND(info) == STB_LOCAL && c != '?')
		c += 32;

	return c;
}

static void print_symbols(t_sym *symbols, uint64_t size, t_ctxt context)
{
	if (context.ac > 2)
	{
		ft_putchar_fd('\n', 1);
		ft_putstr_fd(context.filename, 1);
		ft_putendl_fd(":", 1);
	}
	for (uint64_t i = 0; i < size; ++i)
	{
		if (!*symbols[i].name)
			continue;

		if (symbols[i].letter == 'U' || symbols[i].letter == 'w' || symbols[i].letter == 'V')
			printf("%16c %c %s", ' ', symbols[i].letter, symbols[i].name);
		else
			printf("%016lx %c %s", symbols[i].addr, symbols[i].letter, symbols[i].name);
		printf("\n");
	}
}

static uint8_t parse_64bit(const void *ptr, t_ctxt context)
{
	Elf64_Ehdr *elf_header = (Elf64_Ehdr *)ptr;
	Elf64_Shdr *sect_tab = (Elf64_Shdr *)(ptr + elf_header->e_shoff);
	Elf64_Shdr *sym_sect;
	Elf64_Sym *sym_tab;

	// locate symbol tab.
	for (uint16_t i = 0; i < elf_header->e_shnum; ++i)
	{
		if (sect_tab[i].sh_type == SHT_SYMTAB)
		{
			sym_tab = (Elf64_Sym *)(ptr + sect_tab[i].sh_offset);
			sym_sect = &sect_tab[i];
		}
	}

	// get string tab.
	const char *str_tab = (char *)(ptr + sect_tab[ sym_sect->sh_link ].sh_offset);

	// get actual number of symbols.
	uint64_t sym_num = 0;
	const uint64_t sym_tab_entries = sym_sect->sh_size / sym_sect->sh_entsize;
	for (uint64_t i = 0; i < sym_tab_entries; ++i)
	{
		uint32_t sym_type = ELF64_ST_TYPE(sym_tab[i].st_info);
		if (sym_type == STT_FUNC || sym_type == STT_OBJECT || sym_type == STT_NOTYPE)
			++sym_num;
	}

	// parse symbol tab.
	t_sym *symbols = (t_sym *)malloc(sizeof(t_sym) * sym_num);
	if (!symbols)
	{
		write_error(context.filename, "malloc failed");
		return 1;
	}

	sym_num = -1;
	for (uint64_t i = 0; i < sym_tab_entries; ++i)
	{
		unsigned char sym_type = ELF64_ST_TYPE(sym_tab[i].st_info);
		if (sym_type == STT_FUNC || sym_type == STT_OBJECT || sym_type == STT_NOTYPE)
		{
			symbols[++sym_num].name = (char *)(str_tab + sym_tab[i].st_name);
			symbols[sym_num].addr = sym_tab[i].st_value;
			symbols[sym_num].letter = get_letter(&sym_tab[i], sect_tab);
			if (!ft_strcmp(symbols[sym_num].name, "__abi_tag"))
				symbols[sym_num].letter = 'r';
		}
	}

	// print symbols
	sort_alpha_symbols(symbols, sym_num + 1);
	print_symbols(symbols, sym_num + 1, context);

	return 0;
}

static uint8_t parse_32bit(const void *ptr, t_ctxt args)
{
	(void)ptr;
	(void)args;
	return 0;
}

static void ft_nm(const void *ptr, t_ctxt context)
{
	uint32_t magic = *(uint32_t *)ptr;
	if (magic != ELF_MAGIC)
		return write_error(context.filename, ": file format not recognized");

	// a file's class indicates if the file
	// is 32 or 64 bit. The class is stored
	// in the file's 5th byte.
	uint32_t file_class = *(uint32_t *)(ptr + 1) >> 24;
	if (file_class == ELFCLASS64)
	{
		if (parse_64bit(ptr, context))
			return write_error(context.filename, ": file corrupted");
	}
	else if (file_class == ELFCLASS32)
	{
		if (parse_32bit(ptr, context))
			return write_error(context.filename, ": file corrupted");
	}
	else
		return write_error(context.filename, ": file format not recognized");
}

void nm_wrapper(t_ctxt context)
{
	int32_t fd = open(context.filename, O_RDONLY);
	if (fd == -1)
		return write_error(context.filename, NULL);

	// get and check file info.
	struct stat buff;
	if (fstat(fd, &buff) == -1)
		return write_error(context.filename, NULL);
	if (S_ISDIR(buff.st_mode))
		return write_error(context.filename, ": is a directory");
	if (!S_ISREG(buff.st_mode))
		return write_error(context.filename, ": is not a regular file");

	// load file in memory.
	const void *ptr = mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// no need to keep the fd open
	// after loading file into memory.
	close(fd);

	if (ptr == MAP_FAILED)
		return write_error(context.filename, NULL);

	// do the thing.
	ft_nm(ptr, context);

	// unmap file.
	if (munmap((void *)ptr, buff.st_size) == -1)
		return write_error(context.filename, NULL);
}
