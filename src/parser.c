/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/21 14:22:25 by ccaljouw      #+#    #+#                 */
/*   Updated: 2023/05/03 12:41:20 by ccaljouw      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "builtin.h"

char	*parse_heredoc(t_node *token, t_node *here_redirect, t_smpl_cmd *cmd)
{
	int					type;
	char				*input;

	type = here_redirect->type;
	if (type == HEREDOC)
		type = INPUT;
	if (type == HEREDOCQ)
		type = HEREDOC;
	input = ft_strdup("");
	while (token)
	{
		token->type = check_token_content(token, token->type);
		if ((token->type == SQUOTE || token->type == DQUOTE) && type != HEREDOC)
			token->type = merge_quoted_heredoc(&token, cmd);
		else if (token->type == EXPAND && type != HEREDOC)
			token->type = expand(&token, cmd);
		if (token->content)
			input = ft_strjoin_free_s1(input, token->content);
		remove_node(&token, NULL);
	}
	input = ft_strjoin_free_s1(input, "\n");
	return (input);
}

int	parse_cmd(t_node **tokens, t_smpl_cmd **cmd, t_list *list)
{	
	int					state;
	static t_parser		*parse[9];

	parse[WORD] = expander;
	parse[BLANK] = remove_node_parser;
	parse[REDIRECT] = redirect_tokens;
	parse[PIPE] = set_cmd_end;
	parse[AND] = check_and;
	parse[OR] = check_or;
	parse[BRACE_O] = set_brace;
	parse[BRACE_C] = set_brace;
	parse[PIPE_END] = set_cmd_end;
	state = 0;
	while (*tokens && !state)
	{
		// print_tokens(*tokens, "1. in parse cmd\n");
		state = parse[(*tokens)->type](tokens, *cmd, list);
	}
	if (state != -1)
		state = set_cmd_end(tokens, *cmd, list);
	check_env(*cmd);
	return (state);
}

t_pipe	*parse_pipeline(t_node **tokens, t_node *env_list, t_list *list)
{	
	t_pipe		*pipeline;
	t_smpl_cmd	*cmd;
	int			state;

	state = 0;
	pipeline = init_pipeline();
	while (*tokens && state != -1)
	{
		cmd = init_smpl_cmd(env_list);
		state = parse_cmd(tokens, &cmd, list);
		if (cmd)
		{
			lstadd_back_cmd(&pipeline->pipe_argv, cmd);
			pipeline->pipe_argc++;
		}
		if (state == -1)
		{
			lstclear_cmdlst(&pipeline->pipe_argv, delete_cmd);
			free (pipeline);
			return (NULL);
		}
		if (*tokens && (*tokens)->type == PIPE_END)
			break ;
	}
	return (pipeline);
}

void	parse_and_execute(t_node *tokens, t_node *env_list)
{
	t_list	*list;
	t_pipe	*pipeline;

	if (tokens && count_braces(&tokens))
	{
		syntax_error(&tokens, NULL, "unclosed braces\n", 1);
		return ;
	}
	list = init_list();
	while (tokens)
	{
		pipeline = parse_pipeline(&tokens, env_list, list);
		if (pipeline)
		{
			// print_pipeline(pipeline);
			executor(pipeline);
			// printf("exit status: %d\n", g_exit_status);
			delete_pipe(pipeline);
		}
		if (tokens)
			check_list(&tokens, list);
	}
	free(list);
}
