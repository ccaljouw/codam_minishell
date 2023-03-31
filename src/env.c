/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   env.c                                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/24 09:52:22 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/03/31 10:51:45 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_node	*env_to_list(char **envp)
{
	t_node	*env_list;
	int		i;
	
	i = 0;
	while (envp[i])
	{
		lstadd_back(&env_list, new_node(2,ft_strdup(envp[i])));
		i++;
	}
	return (env_list);
}

//flags: 1 = local (default) 2 = external 3 = all
void	print_env(t_node *env_list, int flag)
{
	while (env_list)
	{
		if (flag == 1)
		{
			if (env_list->type == 1 && check_env_content(env_list->content))
				printf("%s\n", env_list->content);
		}
		if (flag == 2)
		{
			if (env_list->type == 2 && check_env_content(env_list->content))
				printf("%s\n", env_list->content);
		}
		if (flag == 3)
			if ((env_list->type == 1 || env_list->type == 2) && check_env_content(env_list->content))
				printf("%s\n", env_list->content);		
		env_list = env_list->next;
	}
}

//type default = 1 (local)
void	add_variable(t_node *env_list, char *content, int type)
{
	if (!type)
		type = 1;
	if (type < 1 || type > 2)
	{
		printf("Invalid type. Use type 1=local type 2=external\n");
		return ;
	}
	if (env_list && content)
		lstadd_back(&env_list, new_node(type, content));
}

char	*get_variable(t_node *env_list, char *name)
{
	char	*ret;
	char	*temp;
		
	while (env_list && name)
	{
		if (ft_strncmp(env_list->content, name, ft_strlen(name)) == 0)
		{
			temp = ft_substr(env_list->content, ft_strlen(name) + 1, 
			 	ft_strlen(env_list->content) - (ft_strlen(name) + 2));
			if (!temp)
			 	exit_error(errno);
		}
		env_list = env_list->next;
	}
	if (temp)
	{
		ret = temp;
		free (temp);
		return (ret);
	}
	return (NULL);
}

int	check_env_content(char *str)
{
	int	i;
	
	i = 0;
	while (str[i] != '=' && str[i])
		i++;
	if (str[i] && str[i + 1])
		return (1);
	return (0);
}

void	init_variables(t_node *env_list)
{
	add_variable(env_list, "PS1=CC_PROMPT:>", 1);
	add_variable(env_list, "PS2=", 1);
}