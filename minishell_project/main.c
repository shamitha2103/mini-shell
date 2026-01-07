#include <stdio.h>
#include <string.h>
#include "main.h"


pid_t  foreground_pid = -1;
int job_count = 0; 
pid_t pid;
// Job structure for background processes
typedef struct JobNode {
    int job_id;
    pid_t pid;
    char command[200];
    struct JobNode *next;
} JobNode;
JobNode *head = NULL;

void insert_last(pid_t pid, char *command)
{
    JobNode *newNode = (JobNode *)malloc(sizeof(JobNode));
    newNode->job_id = ++job_count;
    newNode->pid = pid;
    strcpy(newNode->command, command);
    newNode->next = NULL;

    if (head == NULL) // Empty list
    {
        head = newNode;
    }
    else
    {
        JobNode *temp = head;
        while (temp->next != NULL) // Go to end
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    printf("\n[%d]+ Stopped                 %s\n", newNode->job_id,newNode->command);
}

// Print all jobs in the list
void print_list()
{
    JobNode *temp = head;
    while (temp != NULL)
    {
        printf("[%d]+  Stopped                 %s\n", temp->job_id, temp->command);
        temp = temp->next;
    }
}

// Get and remove last job's pid
int get_lastpid_list()
{
    if (head == NULL)
    {
        return -1; // No jobs
    }

    JobNode *temp = head;
    JobNode *prev = NULL;

    // Go to last job
    while (temp->next != NULL)
    {
        prev = temp;
        temp = temp->next;
    }

    int last_pid = temp->pid;

    if (prev == NULL) // Only one job
    {
        free(head);
        head = NULL;
        job_count--;
    }
    else
    {
        free(temp);
        prev->next = NULL;
        job_count--;
    }

    return last_pid;
}


char *external_commands[152]={NULL};//global declaration of 2d array
int child=1,status;
void execute_internal_commands(char *input_string)
{
    if(!strcmp(input_string,"exit"))                            // checking the command and exit the program
    {
        system("clear");
        printf("Exiting from Terminal\n");   
        exit(0);
    }
    else if(strstr(input_string,"cd"))
    {
        chdir((input_string+3));                                // going to the specific directory
    }
    else if(!strcmp(input_string,"pwd"))
    {
        printf("%s\n",getcwd(NULL,0));                           // getting the absolute path
    }
    else if(strstr(input_string,"echo"))
    {
        if(input_string[4]=='\0')
        {
            printf("\n");
        }
        else if(input_string[5]=='$')
        {
            if(strchr(input_string+6,'?'))
            {
                printf("Process exit status %d\n",status);                // getting the previous terminated process exit status 
            }
            else if(strchr(input_string+6,'$'))
            {
                printf("%d\n",getpid());                                        // printing the pid of the MINISHELL 
            }
            else if(strstr(input_string+6,"SHELL"))
            {
                printf("%s\n",getenv("SHELL"));                          // printing the environmental path
            }
        }
    }
}


void execute_external_commands(char *input_string) {
    char *commands[10];  // Hold up to 10 piped commands
    int pipe_count = 0;

    char input_copy[1024];
    strncpy(input_copy, input_string, sizeof(input_copy));
    input_copy[sizeof(input_copy) - 1] = '\0'; // Null-terminate safely

    // Split by '|'
    char *token = strtok(input_copy, "|");
    while (token != NULL && pipe_count < 10) {
        while (*token == ' ') token++;  // Trim leading spaces
        commands[pipe_count++] = token;
        token = strtok(NULL, "|");
    }

    int in_fd = 0;

    for (int i = 0; i < pipe_count; i++) {
        int pipefd[2];

        if (i < pipe_count - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid == 0) 
        {
            // Child

            if (in_fd != 0) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (i < pipe_count - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // Tokenize the command into arguments
            char *args[20];
            int argc = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL && argc < 19) {
                args[argc++] = arg;
                arg = strtok(NULL, " ");
            }
            args[argc] = NULL;

            execvp(args[0], args);
            perror("execvp failed");
            exit(127);
        } 
        else if (pid > 0) 
        {
            // Parent
            foreground_pid = pid;
            waitpid(pid, NULL, 0);
            foreground_pid = -1;

            if (in_fd != 0) close(in_fd);
            if (i < pipe_count - 1) {
                close(pipefd[1]);
                in_fd = pipefd[0];
            }
        } else {
            perror("fork failed");
            return;
        }
    }
}





void scan_input(char *prompt, char *input_string)
{
    char *cmd;
    while (1)
    {


        printf(ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET, prompt);

        scanf(" %[^\n]", input_string);
        //signals handler funtion 
        signal(SIGINT, signal_handler);   // Ctrl+C
        signal(SIGTSTP, signal_handler);  // Ctrl+Z
        cmd = get_command(input_string);


        
        if (strstr(cmd, "PS1=") == cmd)
        {
            if (cmd[4] != ' ') strcpy(prompt, cmd + 4);
        } else 
        {
            int ret = check_command_type(cmd);
            if (ret == BUILTIN) 
            execute_internal_commands(input_string);
            else if (ret == EXTERNAL)
             execute_external_commands(input_string);
        }
    }
}
char *get_command(char *input_string)
{
    static char new_array[100];
    int i = 0;

    // Skip leading spaces
    while (input_string[i] == ' ') i++;

    // Extract command until space or end of string
    int j = 0;
    while (input_string[i] != '\0' && input_string[i] != ' ' && j < 99) 
    {
        new_array[j++] = input_string[i++];
    }
    new_array[j] = '\0';

    return new_array;
}

int main()
{
    char prompt[100]="mini_shell$:";//prompt
    char input_string[100];
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTSTP, signal_handler);
    system("clear");//to clear the terminal screen

    extract_external_commands(external_commands);//funtion call
    scan_input(prompt,input_string);//funtion call
}
