/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:12:35 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/08 15:26:26 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdint.h>
#include "../libft/include/libft.h"

typedef struct s_sym
{
	char			*name;
	uint64_t		addr;
	unsigned char	letter;
}	t_sym;

void nm_wrapper(const char *filename);
void write_error(const char *filename, const char *msg);
void write_warning(const char *filename, const char *msg);
void sort_alpha_symbols(t_sym *symbols, uint64_t size);
