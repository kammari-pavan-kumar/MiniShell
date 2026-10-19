#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 
//Structure used to maintain the list of stopped jobs
struct data
{
	int pid;
	char cmd[100];
	struct data *link;
};

//Macros used to identify the type of command.
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

// These macros can be used to print colored text in the terminal.
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//Reads the command entered by the user and processes the command continuously.
void scan_input(char *prompt, char *input_string);

//Extracts the first word from the complete input string.
char *get_command(char *input_string);

// Changes the shell prompt when the user enters a valid PS1 command.
void copy_change(char *prompt, char *input_string);

//Checks whether the command is BUILTIN, EXTERNAL or NO_COMMAND.
int check_command_type(char *command);

//Handles special echo commands
void echo(char *input_string);

//Executes internal commands
void execute_internal_commands(char *input_string);

//Executes external commands
void execute_external_commands(char *input_string);

//Handles signals - SIGINT,SIGTSTP,SIGCHLD
void signal_handler(int sig_num);

//Reads external commands from the external_command.txt file and stores them 
// in the external command array
void extract_external_commands(char **external_commands);

//Executes commands connected using one or more pipe operators.
void N_pipe(char *ext_argv[21], int pipe_count);

//Displays all stopped jobs stored in the linked list.
void jobs(struct data *head);

//Brings the last stopped job to the foreground
void fg(struct data **head);

//Continues the last stopped job in the background.
void bg(struct data **head);

//Deletes the last job from the stopped-job linked list.
void delete_last(struct data **head);

//Inserts a new stopped job at the end of the linked list.
void insert_last(struct data **head, int pid, char *cmd);

#endif