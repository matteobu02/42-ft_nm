/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/12 00:07:50 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "ft_nm.h"

void print_symbols(const t_sym *symbols, uint64_t size, t_ctxt context, int arch)
{
	if (context.ac > 2)
		printf("\n%s:\n", context.filename);

	for (uint64_t i = 0; i < size; ++i)
	{
		if (!*symbols[i].name)
			continue;

		if (arch == 32)
		{
			if (symbols[i].letter == 'U' || symbols[i].letter == 'w' || symbols[i].letter == 'V')
				printf("%8c %c %s", ' ', symbols[i].letter, symbols[i].name);
			else
				printf("%08lx %c %s", symbols[i].addr, symbols[i].letter, symbols[i].name);
		}
		else
		{
			if (symbols[i].letter == 'U' || symbols[i].letter == 'w' || symbols[i].letter == 'V')
				printf("%16c %c %s", ' ', symbols[i].letter, symbols[i].name);
			else
				printf("%016lx %c %s", symbols[i].addr, symbols[i].letter, symbols[i].name);
		}
		printf("\n");
	}
}

#define ELF_MAGIC 0x464c457f

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

#undef ELF_MAGIC

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
	context.fsize = buff.st_size;
	ft_nm(ptr, context);

	// unmap file.
	if (munmap((void *)ptr, buff.st_size) == -1)
		return write_error(context.filename, NULL);
}
