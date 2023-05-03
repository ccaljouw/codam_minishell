/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   executor.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/06 15:16:07 by carlo         #+#    #+#                 */
/*   Updated: 2023/05/03 12:03:25 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

void	check_cmd(char *cmd)
{
	struct stat	file_stat;

	if (stat(cmd, &file_stat) == -1)
		return ;
	if (S_ISDIR(file_stat.st_mode))
		exit_error("minishell: is a directory", 126);
	else if (access(cmd, F_OK) == -1)
		exit_error("minishell: No such file or directory", 127);
	else if (access(cmd, X_OK) == -1)
		exit_error("minishell: Permission denied", 126);
	return ;
}

int	exec_default(char **cmd_args, t_smpl_cmd *pipe_argv, t_node *env_list, char **env)
{
	char	*path;
	char	**my_directories;
	int		i;

	i = 0;
	path = get_variable(env_list, "PATH");
	if (!path)
		path = ft_strdup(cmd_args[0]);
	my_directories = ft_split(path, ':');
	lstclear(&env_list, delete_content);
	delete_cmd(pipe_argv);
	ft_free(path);
	while (my_directories[i])
	{
		path = ft_strjoin(my_directories[i], "/");
		path = ft_strjoin_free_s1(path, cmd_args[0]);
		execve(path, cmd_args, env);
		ft_free(path);
		i++;
	}
	ft_free_array(my_directories);
	return (execve(cmd_args[0], cmd_args, env));
}

void	exec_cmd(t_smpl_cmd *pipe_argv, t_node *env_list)
{
	char	**cmd_args;
	char	**env;

	check_built(pipe_argv);
	cmd_args = build_cmd_args(&pipe_argv->cmd_argv, pipe_argv->cmd_argc);
	env = get_env(env_list);
	if (cmd_args[0][0] == '.' && !cmd_args[0][1])
		exit_error("minishell: filename argument required", 2);
	if (cmd_args[0][0] == '/' || cmd_args[0][0] == '.')
	{
		check_cmd(cmd_args[0]);
		execve(cmd_args[0], cmd_args, env);
	}
	else
		exec_default(cmd_args, pipe_argv, env_list, env);
	exit_error("minishell: executer", 127);
}

void	assignments(t_smpl_cmd *pipe_argv, pid_t pid)
{
	t_node	*temp;

	temp = pipe_argv->assign;
	if (pid == 0)
	{
		while (temp)
		{
			add_variable(pipe_argv->env_list, \
						ft_strdup(temp->content), 1); //moet dit niet 2 zijn?
			temp = temp->next;
		}
	}
}

int		set_out(int *fd_pipe, t_node *temp)
{
	if (temp->type == OUTPUT)
		fd_pipe[1] = open(temp->content, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (temp->type == APPEND)
		fd_pipe[1] = open(temp->content, O_CREAT | O_WRONLY | O_APPEND, 0644);	
	return (1);
}

void	set_in(int *keep, t_node *temp)
{
	close (*keep);
	*keep = open(temp->content, O_RDONLY);
}

/*
Redirects to exisiting files in MacOs are not allowed default. 
In other Linus systems this is not the case. We choose to take
the Linux approach and allow overwriting existing files.
To modify the behaviour in bash to meet our version change the
noclobber setting: 'set +o noclobber'.*/
int	set_fd(t_smpl_cmd *smpl_cmd, int *keep, int *fd_pipe)
{
	int		trigger;
	t_node	*temp;

	trigger = 0;
	temp = smpl_cmd->redirect;
	while (temp)
	{
		if (temp->type == OUTPUT || temp->type == APPEND)
			trigger = set_out(fd_pipe, temp);
		else if (temp->type == INPUT)
			set_in(keep, temp);
		else if (temp->type == HEREDOC || temp->type == HEREDOCQ)
		{
			dup2(smpl_cmd->here_doc, *keep);
			if (!smpl_cmd->here_doc)
				exit_error("dup fail", -1);
		}
		if (*keep == -1 || fd_pipe[0] == -1 || fd_pipe[1] == -1)
			return (-1);
		temp = temp->next;
	}
	return (trigger);
}

void	redirect(t_smpl_cmd *cmd, pid_t pid, int keep, int *fd_pipe)
{
	if (pid == 0)
	{
		close(fd_pipe[0]);
		if (set_fd(cmd, &keep, fd_pipe) == -1)
			exit_error("minishell: redirect", 1);
		dup2(keep, STDIN_FILENO);
		if (!keep)
			exit_error("dup fail", 1); //change
		if (!(!cmd->next && !set_fd(cmd, &keep, fd_pipe)))
			dup2(fd_pipe[1], STDOUT_FILENO);
		if (!fd_pipe[1])
			exit_error("dup fail", 1);
	}
	else
	{	
		close(keep);
		if (cmd->next)
			keep = dup(fd_pipe[0]);
		close(fd_pipe[0]);
		close(fd_pipe[1]);
		if (cmd->here_doc)
			close(cmd->here_doc);
	}
}

void	read_heredocs(t_pipe *pipeline)
{
	t_smpl_cmd	*tcmd;
	t_node		*tredirect;

	tcmd = pipeline->pipe_argv;
	while (tcmd)
	{
		tredirect = tcmd->redirect;
		while (tredirect)
		{
			if (tredirect->type == HEREDOC || tredirect->type == HEREDOCQ)
			{
				if (tcmd->here_doc)
					close(tcmd->here_doc);
				tcmd->here_doc = here_doc(pipeline, tredirect, tcmd);
			}
			tredirect = tredirect->next;
		}
		tcmd = tcmd->next;
	}
}


int	assign_one(t_pipe *pipeline)
{
	if (pipeline->pipe_argv->cmd_argc == 0)
		assignments(pipeline->pipe_argv, 0);
	if (check_builtins_curr_env(pipeline->pipe_argv))
		return (1);
	return (0);
}

void		executor(t_pipe *pipeline)
{
	pid_t		*pid;
	int			fd_pipe[2];
	int			keep;
	int			i;
	int			exit_set;

	exit_set = 0;
	i = 0;
	keep = dup(STDIN_FILENO);
	if (!keep)
		exit_error("dup fail", 1);
	read_heredocs(pipeline);
	pid = malloc(sizeof(pid_t) * (pipeline->pipe_argc + 1));
	while (pipeline && pipeline->pipe_argv)
	{
		if (pipeline->pipe_argc == 1)
			exit_set = assign_one(pipeline);
		if (exit_set == 1)
			break ;
		if (pipe(fd_pipe) == -1)
			exit_error("pipe fail", errno);
		pid[i] = fork();
		if (pid[i] == -1)
		{
			perror("minishell: fork");
			g_exit_status = 128;
			exit_set = 1;
			break ;
		}
		redirect(pipeline->pipe_argv, pid[i], keep, fd_pipe);
		assignments(pipeline->pipe_argv, pid[i]);
		if (pid[i] == 0)
		{
			if (pipeline->pipe_argv->cmd_argc > 0)
				exec_cmd(pipeline->pipe_argv, pipeline->pipe_argv->env_list);
			else
				execute_exit(NULL, pipeline->pipe_argv->env_list);
		}
		pipeline->pipe_argv = pipeline->pipe_argv->next;
		i++;
	}
	set_exit_st(pipeline->pipe_argc, pid, exit_set);
}
