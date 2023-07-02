/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:14:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/02 02:37:10 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "ft_nm.h"

void write_error(const char *filename)
{
	ft_putstr_fd("ft_nm: ", STDERR_FILENO);
	perror(filename);
}

void ft_nm(char *ptr)
{
	int magic_number = *(int *)ptr;
	printf("magic: %x\n", magic_number);
	magic_number = *(int*)(ptr+1);
	printf("ffsdfgic: %x\n", magic_number);
	printf("target: %d\n", ELFCLASS64);
	if (magic_number == ELFCLASS64)
	{
		printf("64->[%x]\n", ELFCLASS64);
	}
	else if (magic_number == ELFCLASS32)
	{
		printf("32->[%x]\n", ELFCLASS32);
	}
	else
		printf("nope\n");
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
	char *ptr = (char *)mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// no need to keep the fd open
	// after loading file in memory.
	close(fd);

	if (ptr == MAP_FAILED)
	{
		write_error(filename);
		return;
	}

	// do the thing.
	ft_nm(ptr);

	// unmap file.
	if (munmap(ptr, buff.st_size) == -1)
	{
		write_error(filename);
		return;
	}
}
