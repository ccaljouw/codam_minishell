/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   lexer.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: cwesseli <cwesseli@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/21 10:03:07 by cwesseli      #+#    #+#                 */
/*   Updated: 2023/03/21 16:05:51 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H
# include "minisell.h"

typedef enum e_lextype
{
	WORD = 0,
	GREAT,
	LESS,
	G_GREAT,
	L_LESS,
	PIPE,
	VAR,
	OPERATOR,
	ASSIGN,
	D_QUOTE,
	S_QUOTE,
	WILD_CARD,
	AND_AND,
	OR_OR,
}	t_lextype;


#endif

/*error handeling:
1.

*/