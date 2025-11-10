#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

/**
 * print_prompt - print prompt if interactive
 *
 * Return: void
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * read_line - read a line from stdin using getline
 * @line: address of buffer pointer used by getline
 * @len: address of buffer size used by getline
 *
 * Return: 0 on success, -1 on EOF/error
 */
int read_line(char **line, size_t *len)
{
	ssize_t nread;

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
 * trim_edges - trim leading and trailing spaces/tabs
 * @s: string to trim (modified in place)
 *
 * Return: pointer to trimmed string (start)
 */
char *trim_edges(char *s)
{
	char *p;
	char *end;

	p = s;
	if (!p)
		return (NULL);
	while (*p && (*p == ' ' || *p == '\t'))
		p++;
	if (*p == '\0')
		return (p);
	end = p + strlen(p) - 1;
	while (end > p && (*end == ' ' || *end == '\t'))
	{
		*end = '\0';
		end--;
	}
	return (p);
}

/**
 * build_argv - split a command line into argv (modifies input)
 * @cmd: command line to split (modified)
 * @argv: array to fill with pointers
 * @max: maximum entries in argv
 *
 * Return: argc (number of args)
 */
int build_argv(char *cmd, char **argv, int max)
{
	int argc;
	char *token;
	char *save;

	argc = 0;
	if (!cmd)
		return (0);
	token = strtok(cmd, " \t");
	while (token && argc < max - 1)
	{
		argv[argc++] = token;
		token = strtok(NULL, " \t");
	}
	argv[argc] = NULL;
	return (argc);
}

/**
 * path_from_env - search PATH for cmd and return malloc'd path
 * @cmd: command name (no slash)
 * @envp: environment variables
 *
 * Return: malloc'd full path or NULL
 */
char *path_from_env(char *cmd, char **envp)
{
	char *path_env;
	char *copy;
	char *dir;
	char *full;
	size_t cmdlen;
	size_t dlen;
	int i;

	path_env = NULL;
	copy = NULL;
	dir = NULL;
	full = NULL;
	cmdlen = 0;
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
	copy = strdup(path_env);
	if (!copy)
		return (NULL);
	cmdlen = strlen(cmd);
	dir = strtok(copy, ":");
	while (dir)
	{
		dlen = strlen(dir);
		full = malloc(dlen + 1 + cmdlen + 1);
		if (!full)
		{
			free(copy);
			return (NULL);
		}
		memcpy(full, dir, dlen);
		full[dlen] = '/';
		memcpy(full + dlen + 1, cmd, cmdlen + 1);
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
 * @cmd: command name (may contain '/')
 * @envp: environment variables
 *
 * Return: malloc'd string with full path, or NULL if not found
 */
char *resolve_path(char *cmd, char **envp)
{
	char *ret;

	ret = NULL;
	if (!cmd || *cmd == '\0')
		return (NULL);
	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}
	return (path_from_env(cmd, envp));
}

/**
 * fork_and_exec - fork and exec path with argv, return child's exit status
 * @path: resolved executable path
 * @argv: argument vector
 * @envp: environment variables
 * @pname: program name for error messages
 * @line_no: input line number for error messages
 *
 * Return: child's exit status or -1 on fork/wait error
 */
int fork_and_exec(char *path, char **argv, char **envp, char *pname, int line_no)
{
	pid_t pid;
	int status;
	int ret;

	pid = fork();
	if (pid == -1)
	{
		perror(pname);
		return (-1);
	}
	if (pid == 0)
	{
		execve(path, argv, envp);
		dprintf(STDERR_FILENO, "%s: %d: %s: not found\n", pname, line_no, argv[0]);
		_exit(127);
	}
	ret = waitpid(pid, &status, 0);
	if (ret == -1)
	{
		perror(pname);
		return (-1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (status);
}

/**
 * handle_builtin - handle built-in commands: exit, env
 * @argv: argument vector (argv[0] is command)
 * @envp: environment variables
 * @line: input buffer to free before exiting
 * @last_status: last command exit status
 * @pname: program name for error messages
 *
 * Return: 1 if a built-in was executed, 0 otherwise
 */
int handle_builtin(char **argv, char **envp, char *line, int last_status, char *pname)
{
	int i;

	(void)pname;
	if (!argv || !argv[0])
		return (0);
	if (strcmp(argv[0], "exit") == 0)
	{
		if (argv[1] == NULL)
		{
			free(line);
			exit(last_status);
		}
		free(line);
		exit(atoi(argv[1]));
	}
	if (strcmp(argv[0], "env") == 0)
	{
		for (i = 0; envp && envp[i]; i++)
		{
			write(STDOUT_FILENO, envp[i], strlen(envp[i]));
			write(STDOUT_FILENO, "\n", 1);
		}
		return (1);
	}
	return (0);
}

/**
 * execute_command - resolve path and execute command with execve
 * @linebuf: command line to execute (program + optional arguments)
 * @pname: program name (from argv[0] of main) for error messages
 * @envp: environment variables
 * @line_no: input line number for error messages
 *
 * Return: child exit status or 127 if not found
 */
int execute_command(char *linebuf, char *pname, char **envp, int line_no)
{
	char *dup;
	char *argv[64];
	char *path;
	int argc;
	int status;

	dup = NULL;
	path = NULL;
	argc = 0;
	status = 0;
	if (!linebuf)
		return (0);
	dup = strdup(linebuf);
	if (!dup)
	{
		perror(pname);
		return (0);
	}
	argc = build_argv(dup, argv, 64);
	if (argc == 0)
	{
		free(dup);
		return (0);
	}
	path = resolve_path(argv[0], envp);
	if (!path)
	{
		dprintf(STDERR_FILENO, "%s: %d: %s: not found\n", pname, line_no, argv[0]);
		free(dup);
		if (!isatty(STDIN_FILENO))
			exit(127);
		return (127);
	}
	status = fork_and_exec(path, argv, envp, pname, line_no);
	free(path);
	free(dup);
	return (status);
}

/**
 * main - Simple UNIX command line interpreter (Simple shell)
 * @ac: argument count (unused)
 * @av: argument vector (used for program name in error messages)
 * @envp: environment variables (passed to execve and env builtin)
 *
 * Return: 0 on success
 */
int main(int ac, char **av, char **envp)
{
	char *line;
	size_t len;
	int line_no;
	int last_status;
	char *cmd;
	char *argv_full[64];
	int argc;

	(void)ac;
	line = NULL;
	len = 0;
	line_no = 0;
	last_status = 0;
	argc = 0;

	while (1)
	{
		print_prompt();
		if (read_line(&line, &len) == -1)
			break;
		line_no++;
		cmd = trim_edges(line);
		if (!cmd || *cmd == '\0')
			continue;
		argc = build_argv(cmd, argv_full, 64);
		if (argc == 0)
			continue;
		if (handle_builtin(argv_full, envp, line, last_status, av[0]))
			continue;
		last_status = execute_command(cmd, av[0], envp, line_no);
	}
	free(line);
	return (0);
}
