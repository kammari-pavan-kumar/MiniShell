# MiniShell 

A MiniShell implemented using C to demonstrate Linux system programming, command execution, process management, inter-process communication, signal handling, and job control.

## Features

- Built-in Commands - Supports internal commands such as `echo`, `cd`, `pwd`, and `exit`.
- External Commands - Executes external Linux commands using process creation and execution system calls.
- Pipe Handling - Supports pipes to pass the output of one command as input to another.
- Foreground Execution - Executes commands in the foreground and waits for their completion.
- Background Execution - Supports background execution of processes.
- Signal Handling - Handles signals such as `SIGINT`, `SIGTSTP`, and `SIGCHLD`.
- Job Control - Supports `jobs`, `fg`, and `bg` commands to manage stopped and background processes.
- Shell Variables - Supports shell-related variables such as `$$`, `$?`, and `PS1`.

## Technologies Used

- Programming Language: C
- Operating System: Linux
- Compiler: GCC
- Concepts: System Calls, Process Creation, `fork()`, `execvp()`, `waitpid()`, Pipes, Signal Handling, Process Synchronization, and Job Control

---

## Installation

### Prerequisites

- A Linux operating system or a Linux environment such as Ubuntu or WSL.
- GCC compiler.
- Basic knowledge of Linux commands and C programming.

### Get the Project

1. Download the project from GitHub as a ZIP file.
2. Extract the downloaded ZIP file.
3. Open the extracted project folder in a Linux terminal.

---

## How to Compile and Run

### Using GCC

Compile the source code using the following command:

    gcc main.c -o minishell

Run the program:

    ./minishell

Note: Make sure `main.c`, `main.h`, and `external_command.txt` are available in the project directory. If your source code requires additional source files, include them in the compilation command.

---

## Project Structure

    MiniShell-in-C/
    |
    |-- main.c
    |-- main.h
    |-- external_command.txt
    |-- documentation.txt
    |-- README.md

---

## File Responsibilities

- main.c: Contains the main function and implements the MiniShell functionality.
- main.h: Contains header declarations, shared definitions, and function prototypes.
- external_command.txt: Contains the list of supported external commands.
- documentation.txt: Contains project documentation, sample executions, and the project conclusion.
- README.md: Describes the project, its features, and instructions for compiling and running it.

---

## Sample Execution

### Executing Commands

    minishell$: pwd
    /home/pavan/Linux_Internal/PROJECT1

    minishell$: echo $$
    1089

    minishell$: echo $?
    0

### Pipe Handling

    minishell$: ls -l | wc -l
    7

    minishell$: ls | wc
          6       6      64

    minishell$: ls -l | grep .c | wc
          3      27     178

### Job Control

    minishell$: sleep 20
    ^Z

    minishell$: jobs
    [1]+  Stopped 763 sleep 20

    minishell$: bg
    [763
