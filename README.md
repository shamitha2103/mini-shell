# Mini Shell (Linux)

A custom Linux shell implemented in C using system calls.

## Features
- Custom PS1 prompt
- Built-in commands (cd, exit, etc.)
- External command execution
- Piping (|)
- Background processes (&)
- Signal handling (SIGINT, SIGCHLD)

## Technologies
- C
- Linux System Calls
- fork(), execvp(), waitpid()
- Signals
- Pipes

## How to Run
gcc *.c -o project_name
./project_name

