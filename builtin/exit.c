/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   exit.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: cwesseli <cwesseli@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/17 19:37:37 by cwesseli      #+#    #+#                 */
/*   Updated: 2023/04/26 14:49:12 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_exit(char **cmd_vector, t_node *env_list)
{
	int		exit_code;
	int		i;

	i = 0;
	exit_code = 0;
	while (cmd_vector[i])
		i++;
	if (i > 2)
		exit_error("ccs: too many arguments\n", 1);
	if (cmd_vector[1])
		exit_code = ft_atoi_long(cmd_vector[1]);
	else
		exit_code = g_exit_status;
	ft_free_array(cmd_vector);
	lstclear(&env_list, delete_content);
	exit(exit_code);
}
