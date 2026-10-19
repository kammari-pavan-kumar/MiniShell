#include "main.h"

//bult in commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs", "fg", "bg", NULL};

		
//Array to store external commands
char *external_commands[155];

//declare varabiles pid for process id, status to get exit status of last process, cmd to store command name
pid_t pid;
int status = 0;
char *cmd = NULL;

//declare signum varaible to store the signal number received by signal handler
int signum;

//default shell prompt
char prompt[100] = "minishell$";

//head pointer for linked list
struct data *head = NULL;


int main()
{
	system("clear");

	/* Register signal handlers. */
	signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

	//declare Buffer to store command
    char input_string[100];

	//reading and processing of shell commands
    scan_input(prompt,input_string);

	return 0;
}

void scan_input(char *prompt,char *input_string)
{

	//call extract_external_commands function to read external commands from external_command.txt file
	//and to store in to external_commands array.
	extract_external_commands(external_commands);
	while(1)
	{
		
		/* Display the shell prompt*/
		printf("%s: ",prompt);
		memset(input_string, 0, sizeof(input_string));

		/* Read the command */
		scanf("%[^\n]",input_string);

		getchar();
	
		//ignore empty input
		if(input_string[0]=='\0')
		continue;

		//check user entered command is PS1 or not
		if(strncmp("PS1=",input_string,4)==0)
		{
			//change the shell prompt
			copy_change(prompt,input_string);

			// clear the input buffer
			memset(input_string,0,sizeof(input_string));

			//back to prompt again
			continue;
		}
		else
		{
			//Extract only the first word from the input
			cmd = get_command(input_string);

			// check wheather command is BUILTIN or EXTERNAL or NOCOMMAND
			int ret =  check_command_type(cmd);


			if(ret == BUILTIN)
			{
				// Execute the built-in command.
				execute_internal_commands(input_string);

			}
			else if(ret == EXTERNAL)
			{
				//Execute external commands in a child process.
				strcpy(cmd, input_string);
				pid = fork();
				if(pid == 0)
				{
					// Set default signal handling for the child
					signal(SIGINT,SIG_DFL);
					signal(SIGTSTP,SIG_DFL);

					/* Execute the external command. */
					execute_external_commands(input_string);
					
					/* Exit the child process. */
					exit(0);
				}
				else if(pid > 0)
				{
				
					// Wait for the child process to complete or stop
					waitpid(pid,&status,WUNTRACED);

					if(WIFSTOPPED(status))
                    {
						// Store the stopped process in the job list
                        insert_last(&head, pid, cmd);
                        printf("\n");
                    }
                    else if(WIFSIGNALED(status))
                    {
                        printf("\n");
                    }
					//Reset pid
					pid = 0;
				}
				else
				{
					printf("no command\n");
				}

			}  
	        else
			{
				printf("NO_COMMAND\n");
			}
			//Free command memory 
			free(cmd); 
			cmd = NULL;
		}
		
		
	}
	printf("\n");

}


/* Changes the shell prompt when the user PS1=userprompt without space*/
void copy_change(char *prompt, char *input_string)
{
	int flag = 0;

	//Check whether the input contains a space or not
	//if space contains then it is invalid
	for(int i=0;input_string[i];i++)
	{
		if(input_string[i]==' ')
		{
			printf("invalid\n");
			flag = 1;
			break;
		}
	}

	//if it is valid then copy input_string to prompt
	if(flag == 0)
	strcpy(prompt,&input_string[4]);
	

}

//Extracts the first word from the complete input string.
char *get_command(char *input_string)
{

	/* Allocate memory to store the command name. */
	char *str = malloc(strlen(input_string) + 1);
	int i=0;

	//get the first word from input string
	for(i=0;input_string[i];i++)
	{
		if(input_string[i]==' ')
		{
			str[i] = '\0';
			return str;
		}
		str[i] = input_string[i];
	}
	
	str[i] = '\0';
	return str;

}


// Checks whether the given command is BUILTIN or EXTERNAL or NO_COMMAND 
int check_command_type(char *command)
{
    // chcek the cmd is bultin or not
	for (int i = 0; builtins[i] != NULL; i++)
	{
		
		if(strcmp(builtins[i],command)==0)
		{
			return BUILTIN;
		}
	}
	
	//check command is external command or not
	for (int j = 0; external_commands[j] != NULL; j++)
	{
		
		if(strcmp(external_commands[j],command)==0)
		{
			return EXTERNAL;
		}
	}
	return NO_COMMAND;

}

//Reads external commands from the file: external_command.txt
// Each command is stored separately in the external_commands array.
void extract_external_commands(char **external_commands)
{
	// Open the external command file in read-only mode. 
	int fd = open("external_command.txt",O_RDONLY);
	if(fd == -1)
	{
		perror("open");
		return;
	}
	char ch;
	int i=0,j=0;

	/* buffer to store command */
	char buffer[100];

	//Read the file one character at a time
	while(read(fd,&ch,1)>0)
	{
		// Ignore carriage return character.
		if(ch == '\r')
		continue;

		//store command in array if newline found
		else if(ch == '\n')
		{
			/* end the command string. */
			buffer[j] = '\0';

			//Allocate memory and store the command.
			external_commands[i] = malloc(j+1);
			strcpy(external_commands[i],buffer);
			
			i++;
			j = 0;
		}
		else
		{
			// store character in ch
			buffer[j] = ch;
			j++;
		}
		
	}

	//store the last command
	buffer[j] = '\0';
	external_commands[i] = malloc(j + 1);
	strcpy(external_commands[i],buffer);
	i++;

	//indicate the end of external command array
	external_commands[i] = NULL;

	// close the command file
	close(fd);
}

// Executes internal commands 
void execute_internal_commands(char *input_string)
{
	//check input string is pwd or not 
	if(strcmp("pwd",input_string) == 0)
	{
		//getcwd() returns the current working directory.
		char *cwd_str = getcwd(NULL,0);
		//print output which is current working directory
		printf("%s\n",cwd_str);
	}
	// Display the list of stopped jobs.
	else if(strcmp("jobs", input_string) == 0)
    {
        jobs(head);
    }
	//Bring the last stopped job to the foreground.
    else if(strcmp("fg", input_string) == 0)
    {
        fg(&head);
    }
	//Continue the last stopped job in the background.
    else if(strcmp("bg", input_string) == 0)
    {
        bg(&head);
    }
	else
	{
		int i=0,j=0;
		char backup[100];

		//remove spaces in cd command input 
		for(i=0;input_string[i];i++)
		{
			char ch = input_string[i];
			if(ch==' ')
			continue;
			else
			backup[j++] = ch;
		}
		backup[j] = '\0';

		//check wheather the command is cd 
		if(strncmp("cd",backup,2)==0)
		{
			//If only "cd" is given, change to HOME directory.
			if(backup[2] == '\0')
			chdir(getenv("HOME")); 
			//Otherwise change to the specified directory.
			else
			chdir(&backup[2]);
		}
		else
		{
			// Check for exit command. 
			if(strcmp("exit",input_string)==0)
			{
				//terminate the shell
				exit(0);
			}
			else
			{
				// Check whether the command is echo.
				if(strncmp("echo",input_string,4)==0)
				{
					//call echo function
					echo(input_string);
				}
				else
				{
					//print command not found
					printf("command not found\n");
				}
			}
		}
		
	}

}

//exceutes echo commands - echo $$, echo $?,echo $SHELL 
void echo(char *input_string)
{	
	//check the input_string is echo$$ 
	if(strcmp("echo $$",input_string)==0)
	{
		//get process id and print it
		printf("%d",getpid());
	}
	else
	{
		//check input string is echo $? 
		if(strcmp("echo $?",input_string)==0)
		{
			// get last command exit status
			printf("%d",WEXITSTATUS(status));
		}
		else
		{
			// check input_string is echo $SHELL
			// Print the value of the SHELL environment variable.
			if(strcmp("echo $SHELL",input_string)==0)
			{
				char* env_str = getenv("SHELL");
				printf("%s",env_str);
			}
			else
			{
				// Unsupported echo argument
				printf("Command not found");
			}
		}
	}
	printf("\n");
}

//Exceute external commands

void execute_external_commands(char *input_string)
{

	// Array used to store command arguments
	char argv[20][20];
	char buf[100];
	int j=0,k=0;

	//Start position of the current argument
	int start = 0;

	//Number of pipe symbols present in the command
	int pipe_count = 0;

	//check the input string character by character
	for(int i=0;input_string[i];i++)
	{
		char ch = input_string[i];

		//Count the number of pipe operators
		if(ch == '|')
		{
			pipe_count++;
		}
		
		//if space found it indicates end of one argument
		if(ch == ' ')
		{
			buf[j] = '\0';
			strcpy(argv[k++],buf+start);

			//Start position of the next argument
			start = j+1;
		}
		else
		{
		/* Store the current character. */
		    buf[j] = ch;
		}
		j++;
	}
	buf[j] = '\0';
	strcpy(argv[k++],buf+start);

	//Convert the 2D character array into a char * array required by execvp().
	char *ext_argv[21];
	for(int i=0;i<k;i++)
	{
		ext_argv[i] = argv[i];
	}
	//execvp() requires the argument list to end with NULL
	ext_argv[k] = NULL;
	//if one or more pipes present execute n pipes
	if(pipe_count > 0)
	{
		N_pipe(ext_argv, pipe_count);
	}
	else
	{
		//Execute a normal external command.
		execvp(ext_argv[0], ext_argv);
	}
	    
}

//Executes commands connected using pipes.
void N_pipe(char *ext_argv[21], int pipe_count)
{
	
	int index[pipe_count+1];
	int j = 1;
	int fd[2];

	//store pipe index as NULL
	index[0] = 0;
		
	//Search for pipe symbols.
    //Replace each "|" with NULL so that each command
    //becomes a separate argument list for execvp().
	for(int i = 0; ext_argv[i] != NULL; i++)
    {
        if(strcmp(ext_argv[i], "|") == 0)
        {
			// The command after the pipe starts at i + 1.
            index[j++] = i + 1;
            ext_argv[i] = NULL;
        }
    }

	//Create and execute each command in the pipes.
	for(int i=0;i<pipe_count+1;i++)
	{
		//Create a pipe for every command except the last one.
		if (i < pipe_count)
        pipe(fd);
		
		// Create a child process.
		int ret1 = fork();
		if(ret1 > 0)
		{
			//If this is not the last command, connect the read end of the pipe to stdin.
			if(i<pipe_count)
			{
				close(fd[1]);

				//Redirect standard input to pipe read end
			    dup2(fd[0],0);
			}
		}
		else if(ret1 == 0)
		{
			if(i < pipe_count)
			{
				//close the read end of pipe
			    close(fd[0]);
				//redirect the stdout to write end of pipe 
			    dup2(fd[1],1);
			}
			//Execute the current command.
			execvp(ext_argv[index[i]],&ext_argv[index[i]]);
		}
	}

	//Save the current stdin and stdout fd.
	int backupstdin = dup(0);
	int backupstdout = dup(1);

	//close fd
	close(fd[0]);
	close(fd[1]);

	//Wait for all child processes created for the pipes.
	for(int i=0;i<pipe_count+1;i++)
	{
		wait(NULL);
	}

	//Restore the original stdin and stdout.
	dup(backupstdin);
	dup(backupstdout);


}

// Handles signals received by the shell - SIGINT,SIGTSTP,SIGCHLD
void signal_handler(int sig_num)
{
	//SIGINT is generated when the user presses Ctrl+C.
	if(sig_num==SIGINT)
	{
		// If no foreground child is running, display new line
		if(pid == 0)
		{
			printf("\n");
			fflush(stdout);
			
		}
	}
	else if(sig_num == SIGTSTP)
    {
		// Handle Ctrl+Z when no foreground process is running
        if(pid == 0)
        {
            printf("\n");
            printf("%s: ",prompt);
            fflush(stdout);
        }
    }
	//SIGCHLD is generated when a child process terminates.
	else if(sig_num == SIGCHLD)
	{
		//Check for a terminated child without blocking.
		waitpid(-1,&status,WNOHANG);
	}
	

}

// Adds a stopped process to the end of the linked list
void insert_last(struct data **head, int pid, char *cmd)
{
	//Allocate memory for a new node.
	struct data *new = malloc(sizeof(struct data));

	//Store process information
	new->pid = pid;
	strcpy(new->cmd,cmd);
	new->link = NULL;

	//if new node is empty make new node as first node
	if(*head == NULL)
	{
		*head = new;
		return;
	}
	//store head in temp 
	struct data *temp = *head;
	//traverse until NULL
	while(temp->link != NULL)
	{
		//move node to next
		temp = temp->link;
	}
	//add new node at last
	temp->link = new; 

}

//Deletes the last stopped job from the linked list
void delete_last(struct data **head)
{
	struct data *temp = *head;

	//if node present return
	if (*head == NULL)
        return;

	//if one node is present delete it and make head as NULL
	if ((*head)->link == NULL)
    {
        free(*head);
        *head = NULL;
        return;
    }
	//traverse until the second last node
	while(temp->link->link!=NULL)
	{
		temp = temp->link;
	}

	//free the last node
	free(temp->link);
	//assign NULL to temp link which is second last node
	temp->link = NULL;
	
}

//Displays all stopped jobs stored in the linked list.
void jobs(struct data *head)
{
	int count = 0;
	//store head to temp
	struct data *temp = head;
	//traverse till end
	while(temp != NULL)
	{
		count++;
		//print job number and command
		printf("[%d]+  Stopped %d %s\n",count,temp->pid,temp->cmd);
		//move to next node
		temp = temp->link;
	}
}

//Brings the last stopped job to the foreground.
void fg(struct data **head)
{
	struct data *temp = *head;
	int last_pid;

	//if no node is present return
	if (*head == NULL)
        return;
	
	//traverse till end
	while(temp->link!= NULL)
	{
		// move to next node
		temp = temp->link;
	}
	//store pid of last job
	last_pid = temp->pid;
	//continue the stopped process
	kill(last_pid,SIGCONT);

	//Wait until the process terminates or gets stopped again.
	waitpid(last_pid,&status,WUNTRACED);

	//Remove the job from the list
	delete_last(head);

}

// Continues the last stopped job in the background.
void bg(struct data **head)
{
	struct data *temp = *head;
	int last_pid;

	// if no nodes present return
	if (*head == NULL)
        return;

	// Find the last stopped job,traverse till end
	while(temp->link!= NULL)
	{
		//move to next node
		temp = temp->link;
	}

	// Store the PID of the last job
	last_pid = temp->pid;
	// Continue the process in the background
	kill(last_pid,SIGCONT);

	// Display the status of the background job
	printf("[%d]+ Running %s &\n",last_pid,temp->cmd);
	if(!WIFSTOPPED(status))
    {
        delete_last(head);
    }
}