/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cd.c                                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/12 19:40:16 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/04/17 19:54:19 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtin.h"
#include <unistd.h>

// cd []
// Change the current directory to dir. The variable HOME is the default dir. 
// The variable CDPATH defines the search path for the directory containing dir
// Alternative directory names in CDPATH are separated by a colon (:). 
// A null directory name in CDPATH is the same as the current directory, 
// i.e., ''.''. 
// If a non-empty directory name from CDPATH is used, or if - is the first 
// argument, and the directory change is successful, the absolute pathname of 
// the new working directory is written to the standard output. 
// An argument of - is equivalent to $OLDPWD.
// The return value is true if the directory was successfully changed; 
// false otherwise.
int	change_dir(char	*str, char *arg, int print)
{
	int		ret;
	char	buf[PATH_MAX];

	if (print)
		print = ft_strcmp(str, getcwd(buf, PATH_MAX));
	if (str[ft_strlen(str) - 1] != '/')
		str = ft_strjoin_free_s1(str, "/");
	if (arg)
		str = ft_strjoin_free_s1(str, arg);
	ret = chdir(str);
	if (!ret && print)
		printf("%s\n", str);
	free(str);
	return (ret);
}

int	go_home(t_node *env_list, char *arg)
{
	char	*home;
	
	home = get_variable(env_list, "HOME");
	if (!home)
	{
		write(2, "minishell: cd: HOME not set\n", 28);
		return (1);
	}
	if (change_dir(home, NULL, 0) == -1)
		return (return_error("minishell: cd", 1));
	else
	{
		free(arg);
		return (0);
	}
}

int	go_oldpwd(t_node *env_list, char *arg)
{
	char	*oldpwd;
	
	oldpwd = get_variable(env_list, "OLDPWD");
	if (!oldpwd)
	{
		write(2, "minishell: cd: OLDPWD not set\n", 30);
		return (1);
	}
	if (change_dir(oldpwd, NULL, 1) == -1)
		return (return_error("minishell: cd", 1));
	else
	{
		free(arg);
		return (0);
	}
}

int	cd_absolute(int i, char *arg, t_node *env_list)
{

	if (i == 1)
		return (go_home(env_list, arg));
	if (!ft_strcmp(arg, "-"))
		return (go_oldpwd(env_list, arg));
	if (arg[0] == '/' || arg[0] == '-')
	{
		if (arg[0] == '-' && arg[0] != '\0')
			perror("minishell: cd: invalid option");
		else if (chdir(arg) == -1)
			perror("minishell: cd");
		else
		{
			free(arg);
			return (0);
		}
	}
	return (2);
}

int	cd_relative(t_node *env_list, char *arg)
{
	char	**path_arr;
	char	buf[PATH_MAX];
	char	*pwd;
	int		i;

	pwd = getcwd(buf, PATH_MAX);
	path_arr = get_path_arr(env_list, pwd);
	if (!path_arr)
		exit_error("cd", 1);
	i = 0;
	while (path_arr[i])
	{
		if (change_dir(ft_strdup(path_arr[i]), arg, 1) == -1)
			i++;
		else
		{
			ft_free_array(path_arr);
			return (0);
		}
	}
	ft_free_array(path_arr);
	return (return_error("minishell: cd", 1));
}

int	execute_cd(char **cmd_vector, t_node *env_list)
{
	int		i;
	char	buf[PATH_MAX];
	char	*pwd;
	char	*arg;

	i = 0;
	while (cmd_vector[i])
		i++;
	if (i > 2)
	{
		write(2, "minishell: cd: too many arguments\n", 34);
		return (1);
	}
	arg = get_arg(cmd_vector[i - 1]);
	if (!arg)
		return (1);
	if (cd_absolute(i, arg, env_list) == 2)
	{
		if (cd_relative(env_list, arg))
			return (1);
	}
	pwd = get_variable(env_list, "PWD");
	add_variable(env_list, ft_strjoin("OLDPWD=", pwd), 2);
	add_variable(env_list, ft_strjoin("PWD=", getcwd(buf, PATH_MAX)), 2);
	free(pwd);
	return (0);
}
