#include "shell.h"

static void print_prompt(void);

/**
 * main - entry point and shell loop
 * @argc: argument count
 * @argv: argument vector
 * @envp: environment variables
 *
 * Return: exit status (int)
 */
int main(int argc, char **argv, char **envp)
{
	shell_info_t info;
	char *line;
	char **args;

	UNUSED(argc);
	info.prog_name = argv[0];
	info.env = copy_env(envp);
	info.interactive = isatty(STDIN_FILENO);
	info.status = 0;

	setup_signals();

	while (1)
	{
		if (info.interactive)
			print_prompt();

		line = read_line(stdin);
		if (!line)
			break;

		args = tokenize(line);
		free(line);
		if (!args)
			continue;

		if (args[0] == NULL)
		{
			free_tokens(args);
			continue;
		}

		if (is_builtin(args))
		{
			info.status = handle_builtin(args, &info);
			free_tokens(args);
			continue;
		}

		execute_command(args, info.prog_name, info.env, NULL);
		free_tokens(args);
	}

	cleanup_and_exit(&info, info.status);
	return info.status;
}

/**
 * print_prompt - write the interactive prompt
 *
 * Return: void
 */
static void print_prompt(void)
{
	write(STDOUT_FILENO, PROMPT, sizeof(PROMPT) - 1);
}
