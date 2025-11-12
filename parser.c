#include "shell.h"

/**
 * read_line - read a full line using getline and return malloc'ed buffer
 * @stream: input stream (usually stdin)
 *
 * Return: pointer to line (malloc) or NULL on EOF/error
 */
char *read_line(FILE *stream)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	nread = getline(&line, &len, stream);
	if (nread == -1)
	{
		free(line);
		return NULL;
	}
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';
	return line;
}

/**
 * copy_token - copy a substring into a new malloc'ed string
 * @start: start pointer of token
 * @len: length of token
 *
 * Return: newly allocated string or NULL
 */
char *copy_token(const char *start, size_t len)
{
	char *tok = malloc(len + 1);
	if (!tok)
		return NULL;
	memcpy(tok, start, len);
	tok[len] = '\0';
	return tok;
}

/**
 * count_tokens - count simple space-separated tokens in a line
 * @line: input line
 *
 * Return: number of tokens
 */
int count_tokens(const char *line)
{
	int count = 0;
	const char *p = line;
	int in_tok = 0;

	while (*p)
	{
		if (*p == ' ' || *p == '\t')
			in_tok = 0;
		else if (!in_tok)
		{
			in_tok = 1;
			count++;
		}
		p++;
	}
	return count;
}

/**
 * tokenize - split the line into a simple argv array (no full shell grammar)
 * @line: input line (modified in process)
 *
 * Return: malloc'ed argv array terminated by NULL or NULL on failure
 */
char **tokenize(char *line)
{
	int ntokens = count_tokens(line);
	char **argv = malloc((ntokens + 1) * sizeof(char *));
	char *p = line;
	int idx = 0;

	if (!argv)
		return NULL;

	while (*p && idx < ntokens)
	{
		char *start = p;
		while (*p == ' ' || *p == '\t')
			p++;
		if (!*p)
			break;
		while (*p && *p != ' ' && *p != '\t')
			p++;
		argv[idx] = copy_token(start, p - start);
		if (!argv[idx])
		{
			free_tokens(argv);
			return NULL;
		}
		idx++;
	}
	argv[idx] = NULL;
	return argv;
}

/**
 * free_tokens - free argv array and its elements
 * @tokens: argv array terminated by NULL
 *
 * Return: void
 */
void free_tokens(char **tokens)
{
	int i;

	if (!tokens)
		return;
	for (i = 0; tokens[i]; i++)
		free(tokens[i]);
	free(tokens);
}
