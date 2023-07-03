/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/03 01:31:27 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "ft_nm.h"

#define ELF_MAGIC 0x464c457f

void write_error(const char *filename)
{
	ft_putstr_fd("ft_nm: ", STDERR_FILENO);
	perror(filename);
}

void handle_64bit(const void *ptr, const char *filename)
{
	Elf64_Ehdr *elf_header = (Elf64_Ehdr *)ptr;

	// section header table: ptr + elf_header->e_shoff;
	// size of a section header: elf_header->e_shnum * elf_header->e_shentsize;
	Elf64_Shdr *sect_tab = (Elf64_Shdr *)(ptr + elf_header->e_shoff);

	for (uint16_t i = 0; i < elf_header->e_shnum; ++i)
	{
		if (sect_tab[i].sh_type == SHT_SYMTAB)
		{
			;
		}
	}
	(void)filename;
}

void handle_32bit(const void *ptr, const char *filename)
{
	(void)ptr;
	(void)filename;
}

void ft_nm(const void *ptr, const char *filename)
{
	int magic = *(int *)ptr;
	if (magic != ELF_MAGIC)
	{
		ft_putstr_fd("ft_nm: ", STDERR_FILENO);
		ft_putstr_fd(filename, STDERR_FILENO);
		ft_putendl_fd(": file format not recognized", STDERR_FILENO);
		return;
	}

	// a file's class indicates if the file
	// is 32 or 64 bit. The class is stored
	// in the file's 5th byte.
	int file_class = *(int *)(ptr + 1) >> 24;
	if (file_class == ELFCLASS64)
	{
		handle_64bit(ptr, filename);
	}
	else if (file_class == ELFCLASS32)
	{
		handle_32bit(ptr, filename);
	}
	else
	{
		ft_putstr_fd("ft_nm: ", STDERR_FILENO);
		ft_putstr_fd(filename, STDERR_FILENO);
		ft_putendl_fd(": file format not recognized", STDERR_FILENO);
		return;
	}
}

void nm_wrapper(const char *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		write_error(filename);
		return;
	}

	// get file info.
	struct stat buff;
	if (fstat(fd, &buff) == -1)
	{
		write_error(filename);
		return;
	}

	// load file in memory.
	const void *ptr = mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// no need to keep the fd open
	// after loading file into memory.
	close(fd);

	if (ptr == MAP_FAILED)
	{
		write_error(filename);
		return;
	}

	// do the thing.
	ft_nm(ptr, filename);

	// unmap file.
	if (munmap((void *)ptr, buff.st_size) == -1)
	{
		write_error(filename);
		return;
	}
}
