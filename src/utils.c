/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 15:28:34 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/06 16:18:52 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "ft_nm.h"

void write_error(const char *filename, const char *msg)
{
	ft_putstr_fd("ft_nm: ", STDERR_FILENO);
	if (msg)
	{
		ft_putstr_fd(filename, STDERR_FILENO);
		ft_putendl_fd(msg, STDERR_FILENO);
	}
	else
		perror(filename);
}

static int8_t compare_sym_names(const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		while (*s1 && !ft_isalnum(*s1))
			++s1;
		while (*s2 && !ft_isalnum(*s2))
			++s2;
		if (ft_tolower(*s1) != ft_tolower(*s2))
			break;
		++s1;
		++s2;
	}
	return (ft_tolower(*s1) - ft_tolower(*s2));
}

static void swap_sym(t_sym *s1, t_sym *s2)
{
	t_sym tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

void sort_symbols(t_sym *symbols, uint64_t size)
{
	for (uint64_t i = 0; i < size; ++i)
	{
		const char *tmp_name1 = symbols[i].name;

		for (uint64_t j = i + 1; j < size; ++j)
		{
			const char *tmp_name2 = symbols[j].name;
			int8_t cmp = compare_sym_names(tmp_name1, tmp_name2);

			if (cmp > 0 || (!cmp && symbols[i].addr > symbols[j].addr)
				|| (!cmp && symbols[i].addr == symbols[j].addr && symbols[i].letter > symbols[j].letter))
			{
				swap_sym(&symbols[i], &symbols[j]);
				tmp_name1 = symbols[i].name;
			}
 		}
	}
}
