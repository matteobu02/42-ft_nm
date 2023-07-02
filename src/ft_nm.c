/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/02 17:33:19 by mbucci           ###   ########.fr       */
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

void _64_bit_handler(const void *ptr, const char *filename)
{
	struct Elf64_Ehdr *elf_header;
	elf_header = (struct Elf64_Ehdr *)ptr;

	// section header table: ptr + elf_header->e_shoff;
	// size of a section header: elf_header->e_shnum * elf_header->e_shentsize;
}

void _32_bit_handler(const void *ptr, const char *filename)
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
	// is 32_bit or 64_bit.
	int file_class = *(int *)(ptr + 1) >> 24;
	if (file_class == ELFCLASS64)
	{
		// handle 64_bit files
		_64_bit_handler(ptr, filename);
	}
	else if (file_class == ELFCLASS32)
	{
		// handle 32_bit files
		_32_bit_handler(ptr, filename);
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
	// open file.
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		write_error(filename);
		return;
	}

	// get file info.
	struct stat buff;
	int stat = fstat(fd, &buff);
	if (stat == -1)
	{
		write_error(filename);
		return;
	}

	// load file in memory.
	const void *ptr = mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// no need to keep the fd open
	// after loading file in memory.
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
