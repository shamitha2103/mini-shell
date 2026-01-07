#include <stdio.h>
#include <string.h>
#include "main.h"

void extract_external_commands(char **external_commands)
{
    int fd = open("command.txt", O_RDONLY); // Open the file
    if (fd == -1)
    {
        perror("Error opening external_commands.txt");
        exit(EXIT_FAILURE);
    }

    char ch;
    char temp[100];
    int idx = 0;
    int command_count = 0;

    while (read(fd, &ch, 1) == 1)
    {
        if (ch == '\n')
        {
            temp[idx] = '\0'; 
            external_commands[command_count] = malloc(strlen(temp) + 1);
            strcpy(external_commands[command_count], temp);
            command_count++;
            idx = 0; // Reset buffer
        }
        else
        {
            temp[idx++] = ch;
        }
    }

    // Handle case where file doesn't end with newline
    if (idx > 0)
    {
        temp[idx] = '\0';
        external_commands[command_count] = malloc(strlen(temp) + 1);
        strcpy(external_commands[command_count], temp);
        command_count++;
    }

    external_commands[command_count] = NULL; // Null-terminate the list
    close(fd);

}
