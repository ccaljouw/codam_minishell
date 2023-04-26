/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cd_utils.c                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/16 11:03:39 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/04/26 17:51:58 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_env(t_node *env_list, char *cmd_arg)
{
	char	buf[PATH_MAX];
	char	*cur_dir;
	char	*pwd;

	pwd = get_variable(env_list, "PWD");
	add_variable(env_list, ft_strjoin("OLDPWD=", pwd), 2);
	cur_dir = ft_strdup(getcwd(buf, PATH_MAX));
	if (!cur_dir)
	{
		if (pwd[ft_strlen(pwd) - 1] != '/')
			pwd = ft_strjoin_free_s1(pwd, "/");
		cur_dir = ft_strjoin(pwd, cmd_arg);
	}
	add_variable(env_list, ft_strjoin("PWD=", cur_dir), 2);
	free(cur_dir);
	if (pwd)
		free(pwd);
}

int	change_dir(char	*str, char *arg)
{
	int		ret;

	if (str[ft_strlen(str) - 1] != '/')
		str = ft_strjoin_free_s1(str, "/");
	if (arg)
		str = ft_strjoin_free_s1(str, arg);
	ret = chdir(str);
	if (!ret)
		printf("%s\n", str);
	free(str);
	return (ret);
}

char	*get_curr_dir(char *cmd_arg, t_node *env_list)
{
	char	*cur_dir;
	char	buf[PATH_MAX];

	cur_dir = getcwd(buf, PATH_MAX);
	if (!cur_dir)
	{
		if (!ft_strcmp(cmd_arg, ".."))
			cur_dir = get_variable(env_list, "PWD");
		if (!ft_strcmp(cmd_arg, "."))
		{
			ft_putstr_fd("minishell: cd: error retrieving current dir\n", 2);
			return (NULL);
		}
	}
	else
		cur_dir = ft_strdup(cur_dir);
	return (cur_dir);
}

char	*get_back(char *cmd_arg, char *pwd)
{
	int		i;
	int		j;
	char	*back;

	i = 0;
	j = ft_strlen(pwd);
	while (cmd_arg[i] == '.' && cmd_arg[i + 1] == '.' && i < (int)ft_strlen(cmd_arg))
	{
		j--;
		while (pwd[j] != '/' && j > 0)
			j--;
		i += 3;	
	}
	if (cmd_arg[i - 1] == '/')
		i++;
	if (j > 0)
		back = ft_strjoin_free_s1(ft_substr(pwd, 0, j + 1), &cmd_arg[i - 1]);
	else
		back = ft_strjoin("/", &cmd_arg[i - 1]);
	free(pwd);
	return (back);
}

char	**get_path_arr(t_node *env_list, char *pwd)
{
	char	*path;
	char	**path_arr;

	path = get_variable(env_list, "CDPATH");
	if (!path)
		path = ft_strdup(pwd);
	else
	{
		path = ft_strjoin_free_s1(path, ":");
		path = ft_strjoin_free_s1(path, pwd);
	}
	path_arr = ft_split(path, ':');
	free(path);
	return (path_arr);
}
