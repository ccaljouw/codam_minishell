/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   minishell.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: cwesseli <cwesseli@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/21 09:48:38 by cwesseli      #+#    #+#                 */
/*   Updated: 2023/04/02 16:27:26 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **envp) //remove arguments and return type?
{
	t_node		*tokens;
	t_node		*env_list;
	char		*line_read;

	(void)argc;
	(void)argv;
	atexit(leaks);
	line_read = NULL;
	env_list = env_to_list(envp);
	init_variables(env_list);
	set_sig_term();
	while (1)
	{
		if (line_read)
		{
		  	free(line_read);
		  	line_read = NULL;
		}
		line_read = readline(get_variable(env_list, "PS1"));
		if (!ft_strcmp(line_read, "exit"))
			break ;
		if (line_read && *line_read)
		 	add_history(line_read);

	// tests
	//	run_tests(line_read, tokens, env_list);
		if (tokens)
			lstclear(&tokens, delete_content);
	}
	rl_clear_history();
	exit(EXIT_SUCCESS);
}
