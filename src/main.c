#include "header_files/utilities.h"
#include "header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Important global strings
char HOSTNAME[HOST_NAME_MAX + 1];
char USERNAME[LOGIN_NAME_MAX + 1];
char INPUTSTRING[MAX_COMMAND_LENGTH + 1];
char COMMANDSTRING[MAX_COMMAND_LENGTH + 1];
char PATH[MAX_PATH_LENGTH + 1];
char HOME[MAX_PATH_LENGTH + 1];

int main(void)
{
    // A character that accepts the residual newline at the end of a scanf
    char disposedCharacter;

    // The array containing the command-line arguments entered
    char *args[MAX_ARG_NO + 1];

    // Obtaning username, hostname and path of current working directory (the home directory for this shell)
    getlogin_r(USERNAME, LOGIN_NAME_MAX + 1);
    gethostname(HOSTNAME, HOST_NAME_MAX + 1);
    getcwd(PATH, MAX_PATH_LENGTH + 1);
    getcwd(HOME, MAX_PATH_LENGTH + 1);

    // Printing intro
    printf("\nWELCOME TO A-SHELL!\n\n");

    // Core of the program - the infinite loop accepting commands as input
    while (1)
    {
        // Printing username, hostname and path in color red
        printf("\033[1;31m");

        // Deciding whether relative or absolute path is to be used by comparing the first strlen(HOME) characters
        if (strncmp(PATH, HOME, strlen(HOME)) == 0)
        {
            char *relativePath = &PATH[strlen(HOME)];
            printf("<%s@%s:~%s> ", USERNAME, HOSTNAME, relativePath);
        }
        else
            printf("<%s@%s:~%s> ", USERNAME, HOSTNAME, PATH);

        printf("\033[0m");

        // Taking command as input
        scanf("%[^\n]s", INPUTSTRING);
        scanf("%c", &disposedCharacter);

        // Parsing the string entered to get a proper string that can be tokenized
        parseInput(INPUTSTRING, COMMANDSTRING);

        // The number of command-line arguments
        int argc = 0;
        _Bool space = 0;

        // Tokenizing the string
        char *token = strtok(COMMANDSTRING, " ");
        while (token != NULL)
        {
            // If previous token ended in a backslash, concatenate current token with previous token
            if (space == 1)
            {
                // Creating a temporary string to store the concatenataed data
                char tempString[strlen(args[argc - 1]) + strlen(token) + 2];
                strcpy(tempString, args[argc - 1]);
                strcat(tempString, " ");
                strcat(tempString, token);
                strcpy(args[argc - 1], tempString);

                space = 0;
            }
            else
            {
                args[argc] = token;
                ++argc;
            }

            // Accounting for backslashes
            if (args[argc - 1][strlen(args[argc - 1]) - 1] == '\\')
            {
                space = 1;
                args[argc - 1][strlen(args[argc - 1]) - 1] = '\0';
            }

            token = strtok(NULL, " ");
        }

        // Null-terminating the array
        args[argc] = NULL;

        // Executing the command
        execCommand(args, argc, PATH, HOME);
    }

    return 0;
}