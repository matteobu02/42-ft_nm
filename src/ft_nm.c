/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/06 18:06:36 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "ft_nm.h"

#define ELF_MAGIC 0x464c457f

static uint8_t handle_64bit(const void *ptr)
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
	t_sym symbols[sym_num];
	sym_num = -1;
	for (uint64_t i = 0; i < sym_tab_entries; ++i)
	{
		uint32_t sym_type = ELF64_ST_TYPE(sym_tab[i].st_info);
		if (sym_type == STT_FUNC || sym_type == STT_OBJECT || sym_type == STT_NOTYPE)
		{
			symbols[++sym_num].name = (char *)(str_tab + sym_tab[i].st_name);
			symbols[sym_num].addr = sym_tab[i].st_value;
		}
	}
	sort_alpha_symbols(symbols, sym_num + 1);
	for (uint64_t i = 0; i < sym_num + 1; ++i)
		printf("%lx %s\n", symbols[i].addr, symbols[i].name);

	return 0;
}

static uint8_t handle_32bit(const void *ptr)
{
	(void)ptr;
	return 0;
}

static void ft_nm(const void *ptr, const char *filename)
{
	uint32_t magic = *(uint32_t *)ptr;
	if (magic != ELF_MAGIC)
		return write_error(filename, ": file format not recognized");

	// a file's class indicates if the file
	// is 32 or 64 bit. The class is stored
	// in the file's 5th byte.
	uint32_t file_class = *(uint32_t *)(ptr + 1) >> 24;
	if (file_class == ELFCLASS64)
	{
		if (handle_64bit(ptr))
			return write_error(filename, ": file corrupted");
	}
	else if (file_class == ELFCLASS32)
	{
		if (handle_32bit(ptr))
			return write_error(filename, ": file corrupted");
	}
	else
		return write_error(filename, ": file format not recognized");
}

void nm_wrapper(const char *filename)
{
	int32_t fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		write_error(filename, NULL);
		return;
	}

	// get file info.
	struct stat buff;
	if (fstat(fd, &buff) == -1)
	{
		write_error(filename, NULL);
		return;
	}

	// load file in memory.
	const void *ptr = mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// no need to keep the fd open
	// after loading file into memory.
	close(fd);

	if (ptr == MAP_FAILED)
	{
		write_error(filename, NULL);
		return;
	}

	// do the thing.
	ft_nm(ptr, filename);

	// unmap file.
	if (munmap((void *)ptr, buff.st_size) == -1)
	{
		write_error(filename, NULL);
		return;
	}
}
