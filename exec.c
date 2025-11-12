#include "shell.h"

/**
 * run_external - perform execve in child process
 * @path: full program path
 * @argv: argument vector terminated by NULL
 * @envp: environment array
 *
 * Return: does not return on success, -1 on failure
 */
int run_external(char *path, char **argv, char **envp)
{
	execve(path, argv, envp);
	return -1;
}

/**
 * wait_for_child - wait for child and interpret exit status
 * @pid: child pid
 *
 * Return: child's exit status or -1 on error
 */
int wait_for_child(pid_t pid)
{
	int status;

	if (waitpid(pid, &status, 0) == -1)
		return -1;
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return -1;
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
void execute_command(char **argv, const char *pname, char **envp, char *line)
{
	char *path;
	pid_t pid;
	int status;

	if (!argv || !argv[0])
		return;

	path = find_in_path(argv[0], envp);
	if (!path)
	{
		dprintf(STDERR_FILENO, "%s: 1: %s: not found\n", pname, argv[0]);
		return;
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		free(path);
		return;
	}

	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		if (line)
			free(line);
		execve(path, argv, envp);
		dprintf(STDERR_FILENO, "%s: 1: %s: %s\n", pname, argv[0], strerror(errno));
		free(path);
		_exit(126);
	}

	free(path);
	if (waitpid(pid, &status, 0) == -1)
		perror("waitpid");
	(void)status;
	return;
}
