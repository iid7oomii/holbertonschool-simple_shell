#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

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
 * trim_spaces - remove leading and trailing spaces/tabs from a line
 * @line: input line
 *
 * Return: pointer to first non-space/non-tab character
 */
char *trim_spaces(char *line)
{
	char *start = line;
	char *end;

	if (line == NULL)
		return (NULL);

	while (*start == ' ' || *start == '\t')
		start++;

	if (*start == '\0')
		return (start);

	end = start + strlen(start) - 1;
	while (end > start && (*end == ' ' || *end == '\t'))
	{
		*end = '\0';
		end--;
	}

	return (start);
}

/**
 * build_argv - split command line into program name and arguments
 * @cmd: trimmed command line (modified in place)
 * @argv: array of pointers to fill
 * @size: maximum number of entries in argv
 *
 * Return: number of arguments stored (argc)
 */
int build_argv(char *cmd, char **argv, int size)
{
	int i = 0;
	char *token;

	token = strtok(cmd, " \t");
	while (token != NULL && i < size - 1)
	{
		argv[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	argv[i] = NULL;

	return (i);
}

/**
 * execute_command - fork and execute a command with execve
 * @cmdline: command line to execute (program + optional arguments)
 * @pname: program name (from argv[0]) for error messages
 * @envp: environment variables
 * @line: input buffer to free in the child on execve error
 */
void execute_command(char *cmdline, char *pname, char **envp, char *line)
{
	pid_t pid;
	int status;
	char *argv[64];

	if (build_argv(cmdline, argv, 64) == 0 || argv[0] == NULL)
		return;

	pid = fork();
	if (pid == -1)
	{
		perror(pname);
		return;
	}

	if (pid == 0)
	{
		if (execve(argv[0], argv, envp) == -1)
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

		cmd = trim_spaces(line);
		if (*cmd == '\0')
			continue;

		/* No PATH: command must be given as a path (e.g. /bin/ls, ./hbtn_ls) */
		execute_command(cmd, av[0], envp, line);
	}

	free(line);
	return (0);
}
