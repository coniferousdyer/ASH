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
char PREVIOUSPATH[MAX_PATH_LENGTH + 1];
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
    getcwd(PREVIOUSPATH, MAX_PATH_LENGTH + 1);
    getcwd(HOME, MAX_PATH_LENGTH + 1);

    // Printing intro
    printf("\nWELCOME TO A-SHELL!\nYour home path is %s\n\n", HOME);

    // Core of the program - the infinite loop accepting commands as input
    while (1)
    {
        // Obtaining current path
        getcwd(PATH, MAX_PATH_LENGTH + 1);

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

        // Splitting the command string into commands
        char *command = INPUTSTRING;
        // Marks the position of the next ; to replace it with null character
        int semicolonPos = 0;
        // Storing the length of the input string
        int len = strlen(INPUTSTRING);

        // The command loop
        while (semicolonPos < len)
        {
            _Bool doubleQuotes = 0;

            // Looping through command string until ; which is not within double quotes is found or end of string is reached
            for (; semicolonPos != len; semicolonPos++)
            {
                if (INPUTSTRING[semicolonPos] == '\"' && !doubleQuotes)
                    doubleQuotes = 1;
                else if (INPUTSTRING[semicolonPos] == '\"' && doubleQuotes)
                    doubleQuotes = 0;

                if (INPUTSTRING[semicolonPos] == ';' && !doubleQuotes)
                    break;
            }

            INPUTSTRING[semicolonPos] = '\0';
            ++semicolonPos;

            // Parsing the string entered to get a proper string that can be tokenized
            parseInput(command, COMMANDSTRING);

            // The number of command-line arguments
            int argc = 0;
            _Bool space = 0, Echo = 0;

            // Checking if the command entered was "echo"
            if (strncmp(COMMANDSTRING, "echo", 4) == 0)
                Echo = 1;

            // Tokenizing the command
            char *token = strtok(COMMANDSTRING, " ");
            while (token != NULL)
            {
                // If previous token ended in a backslash and command is not echo, concatenate current token with previous token
                if (!Echo && space == 1)
                {
                    // Creating a temporary string to store the concatenated data
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
                if (!Echo && args[argc - 1][strlen(args[argc - 1]) - 1] == '\\')
                {
                    space = 1;
                    args[argc - 1][strlen(args[argc - 1]) - 1] = '\0';
                }

                token = strtok(NULL, " ");
            }

            // Null-terminating the array
            args[argc] = NULL;

            // Executing the command
            execCommand(args, argc, HOME, PREVIOUSPATH);

            // Moving command pointer to next command
            command = INPUTSTRING + semicolonPos;
        }
    }

    return 0;
}