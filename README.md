# Simple Shell

This repository contains a simple UNIX command line interpreter
implemented in C for the Holberton/ALX simple_shell project.

## Project scope (Task 2: Simple shell 0.1)

The current implementation provides:

- A prompt `#cisfun$ ` in interactive mode
- Reading a line from standard input using `getline`
- Support for single-word commands **without arguments**
- Execution of commands using `fork` and `execve`
- No use of the `PATH` (absolute path is required, e.g. `/bin/ls`)
- Error handling when the executable is not found
- Proper handling of end-of-file (Ctrl+D)
- Support for both interactive and non-interactive modes

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
