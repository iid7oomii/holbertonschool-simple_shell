# Simple Shell Project



## 🧠 What’s This About?

This is a minimalist UNIX command interpreter written in C — yes, **that** kind of shell.  
It’s built from scratch, obeying the same logic as `/bin/sh`, but with more caffeine and less sanity.

---

## ⚙️ Features

- Displays a prompt and waits for a command.  
- Executes simple commands found in the PATH.  
- Handles built-in commands like `exit` and `env`.  
- Prints appropriate error messages when your command doesn’t exist (don’t cry, just debug).  
- Manages permissions gracefully — “Permission denied” isn’t a bug, it’s a lifestyle.

---

## 🔩 How to Compile

bash
make
This will build the binary hsh.

---

## To clean up compiled files (because clutter is evil):
<pre> ``` make clean ``` </pre>

---

## How to Run
Interactive mode:
<pre> ``` ./hsh
$ echo "Hello, Shell!"
Hello, Shell!
$ exit
 ``` </pre>

 Non-interactive mode:
<pre> ``` echo "ls -l" | ./hsh ``` </pre>

---

## Example Session

<pre> ``` $ ls
AUTHORS  main.c  path.c  builtins.c
$ echo "I feel powerful"
I feel powerful
$ exit
 ``` </pre>

 ---

 ## Team Philosophy

 "If it compiles, it works."
– A confident programmer, moments before segfault.
We built this project to understand what’s really happening when you type a command in your terminal.
Spoiler: it involves forks, execve, and a healthy amount of pain.

---

## Testing

Want to test it like a pro? Try:
<pre> ``` echo "ls; echo hello; pwd" | ./hsh ``` </pre>
If you see output and no crashes — congratulations, you have achieved temporary happiness.

