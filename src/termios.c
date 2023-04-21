/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   termios.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: cwesseli <cwesseli@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/04/03 15:52:32 by cwesseli      #+#    #+#                 */
/*   Updated: 2023/04/21 15:08:31 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "sigterm.h"

//swap ctrl-d (eof) and ctrl-c (sigint))
//https://stackoverflow.com/questions/1516122/how-to-capture-controld-signal

//global variable for now
struct termios	g_old_termios;

// add more masks to signals?
// VEOF: End-of-file character (usually ^D).
// VERASE: Erase character (usually ^H).
// VINTR: Interrupt character (usually ^C).
// VKILL: Kill line character (usually ^U).
// VSUSP: Suspend character (usually ^Z). (ignored)

void	set_termios(void)
{
// struct termios	old_termios;
	struct termios	new_termios;

	if (tcgetattr(STDIN_FILENO, &g_old_termios) == -1)
		exit(errno);
	new_termios = g_old_termios;
	new_termios.c_cc[VQUIT] = g_old_termios.c_cc[VINTR];
	new_termios.c_cc[VINTR] = g_old_termios.c_cc[VEOF];
	new_termios.c_lflag &= ~ECHOK;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1)
		exit(errno);
	if (atexit(restore_terminos) != 0)
		exit(errno);
}

void	restore_terminos()
{
	if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &g_old_termios) == -1)
		exit(errno);
}
