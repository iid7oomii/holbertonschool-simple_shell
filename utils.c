#include "shell.h"

/**
 * _strdup - duplicate a string using malloc
 * @s: source string
 *
 * Return: newly allocated string or NULL
 */
char *_strdup(const char *s)
{
	size_t n;
	char *p;

	if (!s)
		return NULL;
	n = strlen(s);
	p = malloc(n + 1);
	if (!p)
		return NULL;
	memcpy(p, s, n + 1);
	return p;
}

/**
 * _strlen - compute string length
 * @s: input string
 *
 * Return: length as size_t
 */
size_t _strlen(const char *s)
{
	const char *p = s;
	if (!s)
		return 0;
	while (*p)
		p++;
	return (size_t)(p - s);
}

/**
 * safe_free - free pointer and set it to NULL
 * @ptr: address of the pointer to free
 *
 * Return: void
 */
void safe_free(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}
