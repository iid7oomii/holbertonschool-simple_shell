#include "shell.h"

/**
 * get_env_value - get environment variable value from envp as strdup
 * @name: variable name without '=' (e.g., "PATH")
 * @envp: environment array
 *
 * Return: malloc'ed value string or NULL
 */
char *get_env_value(const char *name, char **envp)
{
	size_t n = strlen(name);
	int i;

	if (!envp)
		return NULL;
	for (i = 0; envp[i]; i++)
	{
		if (strncmp(envp[i], name, n) == 0 && envp[i][n] == '=')
			return _strdup(envp[i] + n + 1);
	}
	return NULL;
}

/**
 * join_path - build dir/file path and return malloc'ed string
 * @dir: directory path
 * @file: filename or command
 *
 * Return: malloc'ed full path or NULL
 */
char *join_path(const char *dir, const char *file)
{
	size_t ldir = strlen(dir);
	size_t lfile = strlen(file);
	char *res = malloc(ldir + 1 + lfile + 1);

	if (!res)
		return NULL;
	memcpy(res, dir, ldir);
	res[ldir] = '/';
	memcpy(res + ldir + 1, file, lfile);
	res[ldir + 1 + lfile] = '\0';
	return res;
}

/**
 * find_in_path - return executable path or NULL without forking
 * @cmd: command name (may contain '/')
 * @envp: environment array
 *
 * Return: malloc'ed full path if found, or NULL
 */
char *find_in_path(const char *cmd, char **envp)
{
	char *path_env, *copy, *dir;
	char *full;

	if (!cmd)
		return NULL;

	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return _strdup(cmd);
		return NULL;
	}

	path_env = get_env_value("PATH", envp);
	if (!path_env)
		return NULL;

	copy = path_env;
	dir = strtok(copy, ":");
	while (dir)
	{
		full = join_path(dir, cmd);
		if (!full)
		{
			free(path_env);
			return NULL;
		}
		if (access(full, X_OK) == 0)
		{
			free(path_env);
			return full;
		}
		free(full);
		dir = strtok(NULL, ":");
	}

	free(path_env);
	return NULL;
}
