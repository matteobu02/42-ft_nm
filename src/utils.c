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

static const char *skip_us(const char *s)
{
	while (s && *s && !ft_isalpha(*s))
		++s;
	return s;
}

static int8_t compare_sym_names(const char *s1, const char *s2)
{
	uint32_t i = 0;

	while (s1[i] && s2[i] && ft_tolower(s1[i]) == ft_tolower(s2[i]))
		++i;
	return (ft_tolower(s1[i]) - ft_tolower(s2[i]));
}

static void swap_sym(t_sym *s1, t_sym *s2)
{
	t_sym tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

void sort_alpha_symbols(t_sym *symbols, uint64_t size)
{
	for (uint64_t i = 0; i < size; ++i)
	{
		const char *tmp_name1 = skip_us(symbols[i].name);

		for (uint64_t j = i + 1; j < size; ++j)
		{
			const char *tmp_name2 = skip_us(symbols[j].name);

			if (compare_sym_names(tmp_name1, tmp_name2) > 0)
			{
				swap_sym(&symbols[i], &symbols[j]);
				tmp_name1 = skip_us(symbols[i].name);
			}
 		}
	}
}
