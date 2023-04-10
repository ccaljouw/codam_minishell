/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/21 14:22:25 by ccaljouw      #+#    #+#                 */
/*   Updated: 2023/04/10 11:50:37 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

int	check_token_content(t_node *token, int type)
{
	char	*str;
	int		i;

	i = 0;
	str = token->content;
	if (str[0] == '#' && type != DQUOTE && type != SQUOTE)
		return (COMMENT);
	while (str[i])
	{
		if (str[i] == '\"' && type != SQUOTE)
			return (DQUOTE);
		else if (str[i] == '\'' && type != DQUOTE)
			return (SQUOTE);
		else if (str[i] == '$' && str[i + 1] != ' ' &&  \
					str[i + 1] != '\0' && type != SQUOTE && type != DQUOTE)
			return (EXPAND);
		else if (str[i] == '=' && type != DQUOTE && type != SQUOTE)
			return (ASSIGN);
		i++;
	}
	return (WORD);
}

int	parse_cmd(t_node **tokens, t_smpl_cmd **cmd)
{	
	int					state;
	static t_function	*parse[5];

	parse[WORD] = expander;
	parse[BLANK] = remove_node;
	parse[REDIRECT] = redirect;
	parse[PIPE] = set_cmd_end;
	parse[NEW_LINE] = set_cmd_end;
	state = 0;
	while (*tokens && !state)
		state = parse[(*tokens)->type](tokens, *cmd);
	state = set_cmd_end(tokens, *cmd);
	return (state);
}

int	parse_pipeline(t_node **tokens, t_node *env_list, t_pipe **pipeline)
{	
	t_smpl_cmd	*cmd;
	int			state;

	state = 0;
	while (*tokens && state != -1)
	{
		cmd = init_smpl_cmd(env_list);
		state = parse_cmd(tokens, &cmd);
		if (cmd)
			lstadd_back_pipe(&(*pipeline)->pipe_argv, cmd);
		if (state == -1)
			lstclear_pipe(*pipeline);
		if (*tokens && (*tokens)->type == NEW_LINE)
		{
			state = remove_node(tokens, NULL);
			break ;	
		}
	}
	printf("CREATED PIPLINE\n");
	print_pipeline(*pipeline);
	return (state);
}

t_list	*parse_list(t_node *tokens, t_node *env_list)
{
	// add check and set pipeline type?
	t_list	*lst;
	t_pipe	*pipeline;
	int		state;
	
	lst = NULL;
	while (tokens)
	{
		state = 0;
		pipeline = init_pipeline();
		state = parse_pipeline(&tokens, env_list, &pipeline);
		if (state == -1)
			printf("dont add to list?\n");
	}
	return (lst);
}
