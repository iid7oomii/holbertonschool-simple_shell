#include "shell.h"

/**
 * is_builtin - check if argv[0] is a builtin command
 * @argv: argument vector
 *
 * Return: 1 if builtin, 0 otherwise
 */
int is_builtin(char **argv)
{
	if (!argv || !argv[0])
		return 0;
	if (strcmp(argv[0], "exit") == 0)
		return 1;
	if (strcmp(argv[0], "env") == 0)
		return 1;
	return 0;
}

/**
 * builtin_env - print environment variables
 * @info: shell info structure containing env
 *
 * Return: 0 always (success)
 */
int builtin_env(shell_info_t *info)
{
	int i;

	if (!info || !info->env)
		return 0;
	for (i = 0; info->env[i]; i++)
	{
		write(STDOUT_FILENO, info->env[i], strlen(info->env[i]));
		write(STDOUT_FILENO, "\n", 1);
	}
	return 0;
}

/**
 * builtin_exit - perform exit [n] with cleanup
 * @argv: argument vector (argv[1] may be exit code)
 * @info: shell info structure
 *
 * Return: normally does not return (cleanup_and_exit); or returns error code on invalid number
 */
int builtin_exit(char **argv, shell_info_t *info)
{
	int code = 0;

	if (argv[1])
	{
		char *endptr;
		long v = strtol(argv[1], &endptr, 10);
		if (*endptr != '\0')
		{
			dprintf(STDERR_FILENO, "%s: exit: Illegal number: %s\n",
					info->prog_name, argv[1]);
			return 2;
		}
		code = (int)v;
	}
	cleanup_and_exit(info, code);
	return code;
}

/**
 * handle_builtin - dispatch to the appropriate builtin handler
 * @argv: argument vector
 * @info: shell info structure
 *
 * Return: exit code from the builtin
 */
int handle_builtin(char **argv, shell_info_t *info)
{
	if (strcmp(argv[0], "exit") == 0)
		return builtin_exit(argv, info);
	if (strcmp(argv[0], "env") == 0)
		return builtin_env(info);
	return 0;
}
