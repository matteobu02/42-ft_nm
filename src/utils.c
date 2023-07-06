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
		char *tmp_name1 = symbols[i].name;
		while (tmp_name1 && !ft_isalpha(*tmp_name1))
			++tmp_name1;

		for (uint64_t j = i + 1; j < size; ++j)
		{
			char *tmp_name2 = symbols[j].name;

			while (tmp_name2 && !ft_isalpha(*tmp_name2))
				++tmp_name2;
			if (ft_strcmp(tmp_name1, tmp_name2) < 0)
				swap_sym(&symbols[i], &symbols[j]);
 		}
	}
}
