#include "shell.h"

/**
 * free_env - free a copied environment array
 * @env: copied environment array
 *
 * Return: void
 */
void free_env(char **env)
{
	int i;

	if (!env)
		return;
	for (i = 0; env[i]; i++)
		free(env[i]);
	free(env);
}

/**
 * copy_env - duplicate envp into a new malloc'ed array
 * @envp: original environment array
 *
 * Return: newly allocated environment array or NULL
 */
char **copy_env(char **envp)
{
	int i, cnt = 0;
	char **res;

	if (!envp)
		return NULL;
	while (envp[cnt])
		cnt++;
	res = malloc((cnt + 1) * sizeof(char *));
	if (!res)
		return NULL;
	for (i = 0; i < cnt; i++)
	{
		res[i] = _strdup(envp[i]);
		if (!res[i])
		{
			/* cleanup on failure */
			int j;
			for (j = 0; j < i; j++)
				free(res[j]);
			free(res);
			return NULL;
		}
	}
	res[cnt] = NULL;
	return res;
}

/**
 * cleanup_and_exit - free resources then exit
 * @info: shell info structure
 * @exit_code: desired exit code
 *
 * Return: does not return (terminates process)
 */
void cleanup_and_exit(shell_info_t *info, int exit_code)
{
	if (!info)
		exit(exit_code);
	free_env(info->env);
	exit(exit_code);
}
