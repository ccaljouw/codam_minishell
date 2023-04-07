/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   pipe_utils.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/03/21 13:49:55 by ccaljouw      #+#    #+#                 */
/*   Updated: 2023/04/07 22:00:31 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	delete_cmd(void *smpl_cmd)
{
	t_smpl_cmd	*cmd;

	cmd = (t_smpl_cmd *)smpl_cmd;
	lstclear(&cmd->redirect, delete_content);
	lstclear(&cmd->assign, delete_content);
	lstclear(&cmd->cmd_argv, delete_content);
}

t_smpl_cmd	*lstlast_pipe(t_smpl_cmd *lst)
{
	while (lst && lst->next != NULL)
		lst = lst->next;
	return (lst);
}

void	lstadd_back_pipe(t_smpl_cmd **lst, t_smpl_cmd *new)
{
	t_smpl_cmd	*temp;

	if (new)
	{
		temp = lstlast_pipe(*lst);
		if (temp)
			temp->next = new;
		else
			*lst = new;
	}
}

void	lstdelone_pipe(t_smpl_cmd *lst, void (*del)(void *))
{
	if (lst && del)
	{
		del((void *)lst);
		free(lst);
	}
}

void	lstclear_pipe(t_smpl_cmd **lst, void (*del)(void *))
{
	t_smpl_cmd	*temp;

	if (del && lst)
	{
		while (lst && *lst)
		{
			temp = (*lst)->next;
			lstdelone_pipe(*lst, del);
			*lst = temp;
		}
	}
}