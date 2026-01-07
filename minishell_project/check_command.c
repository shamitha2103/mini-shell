#include "main.h"
extern char *external_commands[152];
int check_command_type(char *command)
{
    // List builtin commands
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                       "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                       "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

    // Check builtin commands
    for (int i = 0; builtins[i] != NULL; i++) 
    {
        if (strcmp(builtins[i], command) == 0) 
        {
            return BUILTIN;
        }
    }
    // Check external commands
    for (int s = 0; external_commands[s] != NULL; s++) 
    {
        if (strcmp(external_commands[s], command) == 0) 
        {
            return EXTERNAL;
        }
    }

    return NO_COMMAND;
}