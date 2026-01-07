#include "main.h"



void signal_handler(int sig_num)
{
    if (sig_num == SIGINT)
    {
        if (foreground_pid > 0)
        {
            // Send signal to foreground process group
            kill(foreground_pid, SIGINT);
            foreground_pid = 0;  
            printf("\n");       
            fflush(stdout);

        }
        else
        {
            printf(ANSI_COLOR_GREEN"\nminishell$:"ANSI_COLOR_RESET);
        
            fflush(stdout);
        }
    }
    else if (sig_num == SIGTSTP)
    {
        if (foreground_pid > 0)
        {
            kill(foreground_pid, SIGTSTP);
            printf("\n[PID %d stopped]", foreground_pid);
            foreground_pid = 0;
            printf("\n");
            fflush(stdout);

        }
        else
        {
            printf(ANSI_COLOR_GREEN"\nminishell$:"ANSI_COLOR_RESET);
            fflush(stdout);
        }
    }
    
}