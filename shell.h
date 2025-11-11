#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

extern char **environ;

#define MAX_ARGS 128
#define PROMPT "$ "
#define UNUSED(x) (void)(x)

typedef struct shell_info
{
    char *prog_name;
    char **env;
    int interactive;
    int status;
} shell_info_t;

/* prototypes */
char *read_line(FILE *stream);
char **tokenize(char *line);
void free_tokens(char **tokens);

char *find_in_path(const char *cmd, char **envp);
char *get_env_value(const char *name, char **envp);
char *join_path(const char *dir, const char *file);

void execute_command(char **argv, const char *pname, char **envp, char *line);
int wait_for_child(pid_t pid);

int is_builtin(char **argv);
int handle_builtin(char **argv, shell_info_t *info);
int builtin_exit(char **argv, shell_info_t *info);
int builtin_env(shell_info_t *info);

char *_strdup(const char *s);
size_t _strlen(const char *s);
void safe_free(void **ptr);

void setup_signals(void);
void sigint_handler(int signo);

void free_env(char **env);
char **copy_env(char **envp);

void cleanup_and_exit(shell_info_t *info, int exit_code);

#endif /* SHELL_H */
