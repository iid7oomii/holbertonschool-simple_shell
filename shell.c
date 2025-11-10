#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * read_command - display prompt (if tty) and read one line from stdin
 * @line: address of buffer pointer used by getline
 * @len: address of buffer size used by getline
 *
 * Return: 0 on success, -1 on EOF or error
 */
int read_command(char **line, size_t *len)
{
	ssize_t nread;

	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);

	nread = getline(line, len, stdin);
	if (nread == -1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "\n", 1);
		return (-1);
	}

	if (nread > 0 && (*line)[nread - 1] == '\n')
		(*line)[nread - 1] = '\0';

	return (0);
}

/**
 * skip_spaces - skip leading spaces and tabs in a line
 * @line: input line
 *
 * Return: pointer to first non-space/non-tab character
 */
char *skip_spaces(char *line)
{
	char *p = line;

	while (*p == ' ' || *p == '\t')
		p++;

	return (p);
}

/**
 * execute_command - fork and execute a command with execve
 * @cmd: command to execute (absolute path expected in this task)
 * @pname: program name (from argv[0]) for error messages
 * @envp: environment variables
 * @line: input buffer to free in the child on execve error
 */
void execute_command(char *cmd, char *pname, char **envp, char *line)
{
	pid_t pid;
	int status;
	char *argv[2];

	pid = fork();
	if (pid == -1)
	{
		perror(pname);
		return;
	}

	if (pid == 0)
	{
		argv[0] = cmd;
		argv[1] = NULL;

		if (execve(cmd, argv, envp) == -1)
		{
			perror(pname);
			free(line);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		wait(&status);
	}
}

/**
 * main - Simple UNIX command line interpreter (Simple shell 0.1)
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
	char *cmd;

	(void)ac;

	while (1)
	{
		if (read_command(&line, &len) == -1)
			break;

		cmd = skip_spaces(line);
		if (*cmd == '\0')
			continue;

		/* No PATH, no arguments: command must be a single word path */
		execute_command(cmd, av[0], envp, line);
	}

	free(line);
	return (0);
}
