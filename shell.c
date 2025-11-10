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
	char *start;
	char *end;

	start = line;
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
 * @cmd: command line (modified in place)
 * @argv: array of pointers to fill
 * @size: maximum number of entries in argv
 *
 * Return: number of arguments stored (argc)
 */
int build_argv(char *cmd, char **argv, int size)
{
	int i;
	char *token;

	i = 0;
	if (cmd == NULL)
	{
		if (argv)
			argv[0] = NULL;
		return (0);
	}
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
 * handle_builtin - handle built-in commands: exit, env
 * @argv: argument vector (argv[0] is command)
 * @envp: environment variables
 * @line: input buffer to free before exiting
 *
 * Return: 1 if a built-in was executed, 0 otherwise
 */
int handle_builtin(char **argv, char **envp, char *line)
{
	int i;

	if (argv == NULL || argv[0] == NULL)
		return (0);
	if (strcmp(argv[0], "exit") == 0)
	{
		free(line);
		exit(0);
	}
	if (strcmp(argv[0], "env") == 0)
	{
		i = 0;
		while (envp && envp[i])
		{
			write(STDOUT_FILENO, envp[i], strlen(envp[i]));
			write(STDOUT_FILENO, "\n", 1);
			i++;
		}
		return (1);
	}
	return (0);
}

/**
 * path_from_env - search PATH directories for a command
 * @cmd: command name (no slash)
 * @envp: environment variables
 *
 * Return: malloc'd string with full path, or NULL if not found
 */
char *path_from_env(char *cmd, char **envp)
{
	char *path_env;
	char *copy;
	char *dir;
	char *full;
	size_t len_cmd;
	size_t len_dir;
	int i;

	path_env = NULL;
	copy = NULL;
	dir = NULL;
	full = NULL;
	len_cmd = 0;
	len_dir = 0;
	if (!cmd || !envp)
		return (NULL);
	for (i = 0; envp[i]; i++)
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_env = envp[i] + 5;
			break;
		}
	}
	if (!path_env || *path_env == '\0')
		return (NULL);
	copy = malloc(strlen(path_env) + 1);
	if (!copy)
		return (NULL);
	strcpy(copy, path_env);
	len_cmd = strlen(cmd);
	dir = strtok(copy, ":");
	while (dir)
	{
		len_dir = strlen(dir);
		full = malloc(len_dir + 1 + len_cmd + 1);
		if (!full)
		{
			free(copy);
			return (NULL);
		}
		strcpy(full, dir);
		full[len_dir] = '/';
		strcpy(full + len_dir + 1, cmd);
		if (access(full, X_OK) == 0)
		{
			free(copy);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}
	free(copy);
	return (NULL);
}

/**
 * resolve_path - resolve a command to an absolute path
 * @cmd: command name (may contain slash)
 * @envp: environment variables
 *
 * Return: malloc'd string with full path, or NULL if not found
 */
char *resolve_path(char *cmd, char **envp)
{
	char *full;

	full = NULL;
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
	return (path_from_env(cmd, envp));
}

/**
 * execute_command - resolve path and execute command with execve
 * @argv: argument vector (argv[0] is command name)
 * @pname: program name (from argv[0] of main) for error messages
 * @envp: environment variables
 * @line: input buffer to free in the child on execve error
 *
 * Return: void
 */
void execute_command(char **argv, char *pname, char **envp, char *line)
{
	pid_t pid;
	int status;
	char *full;

	full = resolve_path(argv[0], envp);
	if (full == NULL)
	{
		perror(pname);
		return;
	}
	pid = fork();
	if (pid == -1)
	{
		perror(pname);
		free(full);
		return;
	}
	if (pid == 0)
	{
		if (execve(full, argv, envp) == -1)
		{
			perror(pname);
			free(full);
			free(line);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		wait(&status);
		free(full);
	}
}

/**
 * main - Simple UNIX command line interpreter with PATH and built-ins
 * @ac: argument count (unused)
 * @av: argument vector (used for program name in error messages)
 * @envp: environment variables (passed to execve and env builtin)
 *
 * Return: 0 on success, or EXIT_FAILURE on fatal error
 */
int main(int ac, char **av, char **envp)
{
	char *line;
	size_t len;
	char *cmd;
	char *argv[64];

	(void)ac;
	line = NULL;
	len = 0;
	while (1)
	{
		if (read_command(&line, &len) == -1)
			break;
		cmd = trim_spaces(line);
		if (*cmd == '\0')
			continue;
		if (build_argv(cmd, argv, 64) == 0 || argv[0] == NULL)
			continue;
		if (handle_builtin(argv, envp, line))
			continue;
		execute_command(argv, av[0], envp, line);
	}
	free(line);
	return (0);
}
