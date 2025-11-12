#include "shell.h"

/**
 * read_line - read a line from input stream using getline
 * @stream: input stream (usually stdin)
 *
 * Return: pointer to malloc'ed string or NULL on EOF
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
		return (NULL);
	}
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';
	return (line);
}

/**
 * count_tokens - count tokens separated by space or tab
 * @line: input string
 *
 * Return: number of tokens
 */
int count_tokens(const char *line)
{
	int count = 0;
	int in_token = 0;
	const char *p = line;

	while (p && *p)
	{
		if (*p == ' ' || *p == '\t')
			in_token = 0;
		else if (!in_token)
		{
			in_token = 1;
			count++;
		}
		p++;
	}
	return (count);
}

/**
 * copy_token - create malloc'ed copy of substring
 * @start: start of token
 * @len: length of token
 *
 * Return: malloc'ed string
 */
char *copy_token(const char *start, size_t len)
{
	char *token;

	token = malloc(len + 1);
	if (!token)
		return (NULL);
	memcpy(token, start, len);
	token[len] = '\0';
	return (token);
}

/**
 * tokenize - split the line into tokens (space/tab separated)
 * @line: input string
 *
 * Return: malloc'ed array of strings terminated by NULL
 */
char **tokenize(char *line)
{
	int ntokens, idx;
	char **argv;
	char *p;
	char *start;

	if (!line)
		return (NULL);

	ntokens = count_tokens(line);
	argv = malloc((ntokens + 1) * sizeof(char *));
	if (!argv)
		return (NULL);

	p = line;
	idx = 0;
	start = NULL;

	while (*p && idx < ntokens)
	{
		while (*p == ' ' || *p == '\t')
			p++;
		if (!*p)
			break;

		start = p;

		while (*p && *p != ' ' && *p != '\t')
			p++;

		argv[idx] = copy_token(start, (size_t)(p - start));
		if (!argv[idx])
		{
			free_tokens(argv);
			return (NULL);
		}
		idx++;
	}
	argv[idx] = NULL;
	return (argv);
}

/**
 * free_tokens - free array of strings
 * @tokens: tokenized array
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
