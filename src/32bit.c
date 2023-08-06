/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   32bit.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 14:41:43 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/06 15:06:04 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include "ft_nm.h"

static unsigned char get_letter(const Elf32_Sym *sym, const Elf32_Shdr *sects)
{
	uint32_t info = sym->st_info;
	uint32_t shndx = sym->st_shndx;
	uint32_t type = sects[shndx].sh_type;
	uint32_t flags = sects[shndx].sh_flags;
	char c = '?';

	if (ELF32_ST_BIND(info) == STB_GNU_UNIQUE)
		c = 'u';
	else if (ELF32_ST_BIND(info) == STB_WEAK)
	{
		c = 'W';
		if (shndx == SHN_UNDEF)
			c = 'w';
		if (ELF32_ST_TYPE(info) == STT_OBJECT) {
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
	else if (ELF32_ST_TYPE(info) == STT_GNU_IFUNC)
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

	if (ELF32_ST_BIND(info) == STB_LOCAL && c != '?')
		c += 32;

	return c;
}

uint8_t parse_32bit(const void *ptr, t_ctxt context)
{
	Elf32_Ehdr *elf_header = (Elf32_Ehdr *)ptr;
	Elf32_Shdr *sect_tab = (Elf32_Shdr *)(ptr + elf_header->e_shoff);
	Elf32_Shdr *sym_sect;
	Elf32_Sym *sym_tab;

	// locate symbol tab.
	for (uint16_t i = 0; i < elf_header->e_shnum; ++i)
	{
		if (sect_tab[i].sh_type == SHT_SYMTAB)
		{
			sym_tab = (Elf32_Sym *)(ptr + sect_tab[i].sh_offset);
			sym_sect = &sect_tab[i];
		}
	}

	// get string tab.
	const char *str_tab = (char *)(ptr + sect_tab[ sym_sect->sh_link ].sh_offset);

	// get actual number of symbols.
	uint32_t sym_num = 0;
	const uint32_t sym_tab_entries = sym_sect->sh_size / sym_sect->sh_entsize;
	for (uint32_t i = 0; i < sym_tab_entries; ++i)
	{
		uint32_t sym_type = ELF32_ST_TYPE(sym_tab[i].st_info);
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
	for (uint32_t i = 0; i < sym_tab_entries; ++i)
	{
		unsigned char sym_type = ELF32_ST_TYPE(sym_tab[i].st_info);
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
	print_symbols(symbols, sym_num + 1, context, 32);
	free(symbols);

	return 0;
}
