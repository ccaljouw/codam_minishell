/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   executor.h                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/06 15:16:31 by carlo         #+#    #+#                 */
/*   Updated: 2023/05/15 16:09:48 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "minishell.h"
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include "parser.h"
# include "builtin.h"

typedef int	t_built(char **, t_node *);

void	executor(t_pipe *pipeline);
char	**build_cmd_args(t_node **argv, int argc);
void	check_cmd(char *cmd);
void	assignments(t_smpl_cmd *pipe_argv, pid_t pid);
int		set_out(int *fd_pipe, t_node *temp);
void	set_in(int *keep, t_node *temp);
int		set_fd(t_smpl_cmd *smpl_cmd, int *keep, int *fd_pipe);
void	set_exit_st(int argc, pid_t *pid);
void	check_built(t_smpl_cmd *cmd);
int		check_builtins_curr_env(t_smpl_cmd *cmd);
char	**get_env(t_node *env_list);
int		check_sorted_argv(t_node *argv);
t_node	*sort_argv(t_node *argv);
int		check_and_pipe(t_pipe *pipeline, pid_t *pid, int *fd_pipe);
pid_t	*init_pid(int argc);
int		init_keep(void);
void	fork_error(void);

#endif
