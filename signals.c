#include "shell.h"

static volatile sig_atomic_t got_sigint = 0;

/**
 * sigint_handler - SIGINT handler to print newline and prompt instead of exiting
 * @signo: signal number
 *
 * Return: void
 */
void sigint_handler(int signo)
{
	UNUSED(signo);
	got_sigint = 1;
	write(STDOUT_FILENO, "\n", 1);
	write(STDOUT_FILENO, PROMPT, sizeof(PROMPT) - 1);
}

/**
 * setup_signals - install signal handlers in parent
 *
 * Return: void
 */
void setup_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
