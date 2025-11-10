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
 * build_argv - split command line into program name and arguments
 * @cmd: command line (modified in place)
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
 * find_in_path - locate command in PATH or check direct path
 * @cmd: command name (no spaces)
 * @envp: environment variables
 *
 * Return: malloc'd string with full path, or NULL if not found
 */
char *find_in_path(char *cmd, char **envp)
{
	int i;
	char *path_env = NULL, *path_copy, *dir, *full;
	size_t len_dir, len_cmd;

	if (cmd == NULL || *cmd == '\0')
		return (NULL);

	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
		{
			full = malloc(strlen(cmd) + 1);
			if (!full)
				return (NULL);
			strcpy(full, cmd);
			return (full);
		}
		return (NULL);
	}

	for (i = 0; envp && envp[i]; i++)
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_env = envp[i] + 5;
			break;
		}
	}
	if (!path_env || *path_env == '\0')
		return (NULL);

	path_copy = malloc(strlen(path_env) + 1);
	if (!path_copy)
		return (NULL);
	strcpy(path_copy, path_env);

	len_cmd = strlen(cmd);
	dir = strtok(path_copy, ":");
	while (dir)
	{
		len_dir = strlen(dir);
		full = malloc(len_dir + 1 + len_cmd + 1);
		if (!full)
		{
			free(path_copy);
			return (NULL);
		}
		strcpy(full, dir);
		full[len_dir] = '/';
		strcpy(full + len_dir + 1, cmd);
		if (access(full, X_OK) == 0)
		{
			free(path_copy);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}
	free(path_copy);
	return (NULL);
}

/**
 * execute_command - resolve path, fork and execute with execve
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
	char *path;

	if (build_argv(cmdline, argv, 64) == 0 || argv[0] == NULL)
		return;

	path = find_in_path(argv[0], envp);
	if (path == NULL)
	{
		perror(pname);
		return;
	}

	pid = fork();
	if (pid == -1)
	{
		perror(pname);
		free(path);
		return;
	}

	if (pid == 0)
	{
		if (execve(path, argv, envp) == -1)
		{
			perror(pname);
			free(path);
			free(line);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		wait(&status);
		free(path);
	}
}

/**
 * main - Simple UNIX command line interpreter (0.3 with PATH)
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
	char *cmd, *end;

	(void)ac;

	while (1)
	{
		if (read_command(&line, &len) == -1)
			break;

		cmd = line;
		while (*cmd == ' ' || *cmd == '\t')
			cmd++;

		if (*cmd == '\0')
			continue;

		end = cmd + strlen(cmd) - 1;
		while (end > cmd && (*end == ' ' || *end == '\t'))
		{
			*end = '\0';
			end--;
		}

		execute_command(cmd, av[0], envp, line);
	}

	free(line);
	return (0);
}
