// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        ::::::::            */
// /*   expander_new.c                                     :+:    :+:            */
// /*                                                     +:+                    */
// /*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
// /*                                                   +#+                      */
// /*   Created: 2023/04/07 21:51:28 by cariencaljo   #+#    #+#                 */
// /*   Updated: 2023/05/02 07:10:41 by cariencaljo   ########   odam.nl         */
// /*                                                                            */
// /* ************************************************************************** */

// #include "minishell.h"
// #include "parser.h"

// int	expander(t_node **token, t_smpl_cmd *cmd, t_list *list)  //expand word (make separate expander for redirects)
// {
// 	static t_function	*parse[16];
// 	int					state;

// 	(void)list;
// 	parse[COMMENT] = remove_comment;
// 	parse[SQUOTE] = merge_quoted;
// 	parse[DQUOTE] = merge_quoted;
// 	parse[EXPAND] = expand;
// 	parse[ASSIGN] = parser_assign;
// 	parse[TILDE] = expand_tilde;
// 	state = 0;
// 	while (*token && ((*token)->type == WORD || (*token)->type > 8))
// 	{
// 		// printf ("1. in expander, content: %s, type: %d\n", (*token)->content, (*token)->type);
// 		(*token)->type = check_token_content(*token, (*token)->type);
// 		// printf ("2. in expander, content: %s, type: %d\n", (*token)->content, (*token)->type);
// 		if ((*token)->type && (*token)->type > 8)
// 			state = parse[(*token)->type](token, cmd);
// 		if (state == -1)
// 			return (-1);
// 		else
// 			(*token)->type = state;
// 		// print_tokens(*token, "in expander after parse\n");
// 		if (*token && (*token)->type == WORD)
// 		{
// 			// printf("add to cmd: %s\n", (*token)->content);
// 			add_word_to_cmd(token, cmd);
// 			// print_cmd(cmd);
// 		}
// 	}
// 	return (0);
// }