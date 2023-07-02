/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:13:19 by mbucci            #+#    #+#             */
/*   Updated: 2023/07/02 00:56:34 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"

#define DFLT_TARGET "a.out"

int main(int ac, char **av)
{
	if (ac < 2)
		nm_wrapper(DFLT_TARGET);
	else
	{
		for (int i = 1; i < ac; ++i)
			nm_wrapper(av[i]);
	}
	return 0;
}
