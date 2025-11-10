# Simple Shell

This repository contains a simple UNIX command line interpreter
implemented in C for the Holberton/ALX simple_shell project.

## Features (Task 2: Simple shell 0.1)

- Displays a prompt `#cisfun$ ` in interactive mode
- Reads a line from standard input using `getline`
- Executes a single-word command using `fork` and `execve`
- Does not use the PATH (absolute paths only, e.g. `/bin/ls`)
- Prints an error message if the executable cannot be found
- Handles the end-of-file (Ctrl+D)
- Works in both interactive and non-interactive modes

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
