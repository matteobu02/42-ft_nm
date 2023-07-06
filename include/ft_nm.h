/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:12:35 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/06 16:06:37 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "../libft/include/libft.h"

typedef struct s_sym
{
	char		*name;
	uint64_t	addr;
}	t_sym;

void nm_wrapper(const char *filename);
void write_error(const char *filename, const char *msg);
void sort_alpha_symbols(t_sym *symbols, uint64_t size);
