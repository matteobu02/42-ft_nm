/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:12:35 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/12 01:11:16 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include "../libft/include/libft.h"

typedef struct s_sym
{
	char			*name;
	uint64_t		addr;
	unsigned char	letter;
}	t_sym;

typedef struct s_context
{
	int				ac;
	const char		*filename;
	unsigned long	fsize;
}	t_ctxt;

/* ft_nm.c */
void print_symbols(const t_sym *symbols, uint64_t size, t_ctxt context, int arch);
void nm_wrapper(t_ctxt context);

/* 64bit.c */
uint8_t parse_64bit(const void *ptr, t_ctxt context);

/* 32bit.c */
uint8_t parse_32bit(const void *ptr, t_ctxt context);

/* utils.c */
void write_error(const char *filename, const char *msg);
void write_warning(const char *filename, const char *msg);
void sort_symbols(t_sym *symbols, uint64_t size);
int isnullterm(const void *ptr, int fsize, int offset);

