/* file: shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * main - Simple UNIX command line interpreter (Simple shell 0.1)
 *
 * @ac: argument count (unused)
 * @av: argument vector (used for program name in error messages)
 * @envp: environment variables (passed to execve)
 *
 * Return: 0 on success, or EXIT_FAILURE on fatal error
 */
int main(int ac, char **av, char **envp)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status;
	char *cmd;
	char *argv[2];

	(void)ac;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		cmd = line;
		while (*cmd == ' ' || *cmd == '\t')
			cmd++;

		if (*cmd == '\0')
			continue;

		pid = fork();
		if (pid == -1)
		{
			perror(av[0]);
			continue;
		}

		if (pid == 0)
		{
			argv[0] = cmd;
			argv[1] = NULL;

			if (execve(cmd, argv, envp) == -1)
			{
				perror(av[0]);
				free(line);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			wait(&status);
		}
	}

	free(line);
	return (0);
}
