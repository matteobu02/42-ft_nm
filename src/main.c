/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:13:19 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/06 14:04:49 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"

#define DFLT_TARGET "a.out"

int main(int ac, char **av)
{
	t_ctxt context;
	context.ac = ac;

	if (ac < 2)
	{
		context.filename = DFLT_TARGET;
		nm_wrapper(context);
	}
	else
	{
		for (int i = 1; i < ac; ++i)
		{
			context.filename = av[i];
			nm_wrapper(context);
		}
	}
	return 0;
}
