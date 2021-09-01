#include "header_files/utilities.h"
#include "header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// String storing hostname
char HOSTNAME[HOST_NAME_MAX + 1];
char USERNAME[LOGIN_NAME_MAX + 1];
char INPUTSTRING[MAX_COMMAND_LENGTH + 1];
char COMMANDSTRING[MAX_COMMAND_LENGTH + 1];

int main(void)
{
    // A character that accepts the residual newline at the end of a scanf
    char disposedCharacter;

    // The array containing the command-line arguments entered
    char *args[MAX_ARG_NO];

    // Obtaning username and hostname
    getlogin_r(USERNAME, LOGIN_NAME_MAX + 1);
    gethostname(HOSTNAME, HOST_NAME_MAX + 1);

    // Printing intro
    printf("\nWELCOME TO A-SHELL!\n\n");

    // Core of the program - the infinite loop accepting commands as input
    while (1)
    {
        // Printing username, hostname and path in color red
        printf("\033[1;31m");
        printf("<%s@%s:~> ", USERNAME, HOSTNAME);
        printf("\033[0m"); 

        // Taking command as input
        scanf("%[^\n]s", INPUTSTRING);
        scanf("%c", &disposedCharacter);

        // Parsing the string entered to get a proper string that can be tokenized
        parseInput(INPUTSTRING, COMMANDSTRING);

        // The number of command-line arguments
        int argc = 0;

        // Tokenizing the string
        char *token = strtok(COMMANDSTRING, " ");
        while (token != NULL)
        {
            args[argc] = token;
            token = strtok(NULL, " ");
            ++argc;
        }
    }

    return 0;
}