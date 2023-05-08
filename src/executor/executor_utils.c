/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   executor_utils.c                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/11 13:22:26 by carlo         #+#    #+#                 */
/*   Updated: 2023/05/08 18:20:55 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

int	here_doc(t_pipe *pipeline, t_node *here_redirect, t_smpl_cmd *cmd)
{
	int		here_pipe[2];
	char	*line_read;
	char	*line;
	t_node	*tokens;

	line = NULL;
	if (pipe(here_pipe) == -1)
		exit_error("here_pipe fail", errno);
	// set_sigint_here();
	while (1)
	{
		line_read = get_input(pipeline->pipe_argv->env_list, "PS2", 0);
		if (!ft_strcmp(line_read, here_redirect->content) || line_read == NULL)
			break ;
		tokens = lexer(line_read, " \n");
		line = parse_heredoc(tokens, here_redirect, cmd);
		ft_putstr_fd(line, here_pipe[1]);
		free(line);
	}
	close(here_pipe[1]);
	// set_signals();
	return (here_pipe[0]);
}

char	**build_cmd_args(t_node **argv, int argc)
{
	t_node 	*temp;
	char	**cmd_args;
	int		i;

	i = 0;
	if (!argv || !argc)
		return (NULL);
	if (BONUS)
		argc = check_wildcars(argv);
	temp = *argv;
	cmd_args = malloc(sizeof(char *) * (argc + 1));
	while (i < argc)
	{
		cmd_args[i] = ft_strdup((*argv)->content);
		*argv = (*argv)->next;
		i++;
	}
	cmd_args[i] = NULL;
	*argv = temp;
	return (cmd_args);
}

/*
waitpid: wait for the child process with the specified PID to complete.
WIFEXITED macro: check if the child process exited normally
WEXITSTATUS macro: get the exit status of the child process.
*/
void	set_exit_st(int argc, pid_t *pid, int exit_set)
{
	int	waitstatus;
	int	i;

	i = 0;
	while (i < argc)
	{
		waitpid(pid[i], &waitstatus, 0);
		if (WIFEXITED(waitstatus) && exit_set == 0)
			g_exit_status = WEXITSTATUS(waitstatus);
		i++;
	}
	free(pid);
}

int		get_num_type(t_node *env_list)
{
	t_node	*curr;
	int		i;

	curr = env_list;
	i = 0;
	while (curr)
	{
		if (curr->type == 2)
			i++;
		curr = curr->next;
	}
	return (i);
}

char	**get_env(t_node *env_list)
{
	t_node	*curr;
	char	**arr;
	int		i;

	curr = env_list;
	if (!env_list)
		return (NULL);
	i = get_num_type(env_list);
	arr = malloc(sizeof(char *) * (i + 1));
	i = 0;
	curr = env_list;
	while (curr)
	{
		if (curr->type == 2)
			arr[i] = ft_strdup(curr->content);
		if (curr->type == 2)
			i++;
		curr = curr->next;
	}
	arr[i] = NULL;
	return (arr);
}

void	check_built(t_smpl_cmd *cmd)
{
	int		i;
	t_built	*built[7];
	char	**cmd_args;
	char	*builtins[7] =	{"echo", "cd", "pwd", "export", \
										"unset", "exit", "env"};
	built[0] = execute_echo;
	built[1] = execute_cd;
	built[2] = execute_pwd;
	built[3] = execute_export;
	built[4] = execute_unset;
	built[5] = execute_exit;
	built[6] = execute_env;
	i = 0;
	while (i < 7 && cmd->cmd_argc > 0)
	{
		if (ft_strcmp(cmd->cmd_argv->content, builtins[i]) == 0 || ft_strcmp_case(cmd->cmd_argv->content, builtins[0], &ft_tolower) == 0)
		{
			cmd_args = build_cmd_args(&cmd->cmd_argv, cmd->cmd_argc);
			g_exit_status = (built[i](cmd_args, cmd->env_list));
			ft_free_array(cmd_args);
			exit(g_exit_status);
		}
		i++;
	}
}

int	check_builtins_curr_env(t_smpl_cmd *cmd)
{
	t_built	*built[5];
	char	**cmd_args;
	char	*builtins[5] =	{"cd", "exit", "export", "unset", ".."};
	int		i;

	built[0] = execute_cd;
	built[1] = execute_exit;
	built[2] = execute_export;
	built[3] = execute_unset;
	built[4] = execute_cd;
	i = 0;

	while (i < 5 && cmd->cmd_argc > 0)
	{
		if (!ft_strcmp(cmd->cmd_argv->content, "export") && cmd->cmd_argc == 1)
			return (0);
		if (ft_strcmp(cmd->cmd_argv->content, builtins[i]) == 0)
		{
			cmd_args = build_cmd_args(&cmd->cmd_argv, cmd->cmd_argc);
			g_exit_status = built[i](cmd_args, cmd->env_list);
			ft_free_array(cmd_args);
			return (1);
		}
		i++;
	}
	return (0);
}
