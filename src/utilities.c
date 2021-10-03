#include "header_files/utilities.h"
#include "header_files/util_variables.h"
#include "header_files/commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

/*--------------------------------------------------------------------------------*
 * Function to check if a string can be converted to a number (before using atoi) *
 *--------------------------------------------------------------------------------*/
_Bool isInteger(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        if (str[i] > '9' || str[i] < '0')
            return 0;

    return 1;
}

/*-----------------------------------------------------*
 * Function to insert a process into the process array *
 *-----------------------------------------------------*/
void InsertProcess(int pid, char *pName)
{
    // Copying the pid and name to the array
    if (CHILDNO == 0)
        PROCESSLIST[CHILDNO].jobNo = 1;
    else
        PROCESSLIST[CHILDNO].jobNo = PROCESSLIST[CHILDNO - 1].jobNo + 1;
    PROCESSLIST[CHILDNO].pid = pid;
    PROCESSLIST[CHILDNO].pName = (char *)malloc(strlen(pName) + 1);
    strcpy(PROCESSLIST[CHILDNO].pName, pName);
    ++CHILDNO;
}

/*------------------------------------------------------*
 * Function to delete a process with given pid from the *
 * process array and returns the process name           *
 *------------------------------------------------------*/
void DeleteProcess(int pid, char nameString[])
{
    for (int i = 0; i < CHILDNO; i++)
        if (PROCESSLIST[i].pid == pid)
        {
            // Copying the name to nameString
            strcpy(nameString, PROCESSLIST[i].pName);
            free(PROCESSLIST[i].pName);

            // Shifting the array to delete the process from the array
            for (int j = i; j < CHILDNO - 1; j++)
                PROCESSLIST[j] = PROCESSLIST[j + 1];

            --CHILDNO;
            break;
        }
}

/*-------------------------------------------------------------------------*
 * Function to obtain input from user and format it to give us the command *
 *-------------------------------------------------------------------------*/
void parseInput(char *inputString, char *parsedString)
{
    int len = 0;
    _Bool doubleQuotes = 0, singleQuotes = 0;

    for (int i = 0; inputString[i] != '\0'; i++)
    {
        // If the current character is a double quote
        if (inputString[i] == '\"')
        {
            if (doubleQuotes)
                doubleQuotes = 0;
            else
                doubleQuotes = 1;
        }

        // If the current character is a single quote
        if (inputString[i] == '\'')
        {
            if (singleQuotes)
                singleQuotes = 0;
            else
                singleQuotes = 1;
        }

        // Checking if it's the first or the last character; if last character, there should not be a space before
        if (i == 0 || (i == strlen(inputString) - 1 && inputString[i - 1] != ' '))
        {
            // Converting tabs to spaces
            if (inputString[i] == '\t')
                inputString[i] = ' ';
            // If the current character is not a space, include it
            if (inputString[i] != ' ')
            {
                parsedString[len] = inputString[i];
                ++len;
            }
        }
        else
        {
            // Converting tabs to spaces
            if (inputString[i] == '\t')
                inputString[i] = ' ';
            // Ignoring consecutive spaces
            if (inputString[i] == ' ' && inputString[i - 1] == ' ')
                continue;
            else
            {
                // Account for spaces before and after double quotes
                if (inputString[i] == ' ' && doubleQuotes)
                {
                    // If preceding character was a double quote
                    if (inputString[i - 1] == '\"')
                        continue;
                    else
                    {
                        // Finding the index where " occurs
                        int j;
                        for (j = i + 1; inputString[j] == ' ' || inputString[j] == '\t'; j++)
                            if (inputString[j] == '\t')
                                inputString[j] = ' ';

                        if (inputString[j] == '\"' || inputString[j] == '\0')
                        {
                            doubleQuotes = 0;
                            i = j;
                        }
                    }
                }

                // Account for spaces before and after single quotes
                if (inputString[i] == ' ' && singleQuotes)
                {
                    // If preceding character was a single quote
                    if (inputString[i - 1] == '\'')
                        continue;
                    else
                    {
                        // Finding the index where ' occurs
                        int j;
                        for (j = i + 1; inputString[j] == ' ' || inputString[j] == '\t'; j++)
                            if (inputString[j] == '\t')
                                inputString[j] = ' ';

                        if (inputString[j] == '\'' || inputString[j] == '\0')
                        {
                            singleQuotes = 0;
                            i = j;
                        }
                    }
                }

                parsedString[len] = inputString[i];
                ++len;
            }
        }
    }

    parsedString[len] = '\0';
}

/*------------------------------------------*
 * Function to display a prompt to the user *
 *------------------------------------------*/
void displayPrompt()
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
        printf("<%s@%s:%s> ", USERNAME, HOSTNAME, PATH);

    printf("\033[0m");
}

/*---------------------------------------------------* 
 *Function to take input and store it in INPUTSTRING *
 *---------------------------------------------------*/
void takeInput()
{
    // Disabling buffering for stdout and enabling raw mode
    setbuf(stdout, NULL);
    enableRawMode();

    int len = 0;
    char c;
    int historyN = HISTORYNO;

    // Reading in characters from stdin
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            // newline character
            if (c == 10)
                break;
            // UP and DOWN arrow keys
            else if (c == 27)
            {
                char buffer[3];
                buffer[2] = '\0';
                if (read(STDIN_FILENO, buffer, 2) == 2)
                    // "UP" arrow key
                    if (strcmp(buffer, "[A") == 0)
                    {
                        if (historyN >= 1 && historyN <= HISTORYNO)
                        {
                            // If we are on the current command and something has been entered
                            if (historyN == HISTORYNO && len > 0)
                                continue;

                            --historyN;
                            len = recallCommand(historyN, len);
                        }
                    }
                    // "DOWN" arrow key
                    else if (strcmp(buffer, "[B") == 0)
                    {
                        // If we are on the last command in history
                        if (historyN == HISTORYNO - 1)
                        {
                            // Erasing the entire command and copying an empty string to INPUTSTRING
                            while (len--)
                            {
                                // If the character is a TAB character
                                if (INPUTSTRING[len] == 9)
                                    for (int i = 0; i < 8; i++)
                                        printf("\b");
                                else
                                    printf("\b \b");
                            }

                            ++historyN;
                            strcpy(INPUTSTRING, "");
                            len = 0;
                        }
                        else if (historyN >= 0 && historyN <= HISTORYNO - 1)
                        {
                            ++historyN;
                            len = recallCommand(historyN, len);
                        }
                    }
            }
            // TAB character
            else if (c == 9)
            {
                // TABS should be 8 spaces
                for (int i = 0; i < 8; i++)
                    printf(" ");

                INPUTSTRING[len++] = c;
            }
            // backspace
            else if (c == 127)
            {
                if (len > 0)
                {
                    // If the character is a TAB character
                    if (INPUTSTRING[len - 1] == 9)
                        for (int i = 0; i < 7; i++)
                            printf("\b");

                    INPUTSTRING[--len] = '\0';
                    printf("\b \b");
                }
            }
            // If Ctrl + D was pressed
            else if (c == 4)
            {
                printf("\n");
                exit(0);
            }
        }
        else
        {
            INPUTSTRING[len++] = c;
            printf("%c", c);
        }
    }

    // Terminating the string
    INPUTSTRING[len] = '\0';

    // Disabling raw mode
    disableRawMode();

    // To print the output on a new line
    printf("\n");
}

/*-------------------------------------------------------------*
 * Function to tokenize the input, account for backslashes and *
 * finally execute the command by calling execCommand          *
 *-------------------------------------------------------------*/
void tokenizeAndExec(char *args[])
{
    // Obtaining current path
    getcwd(PATH, MAX_PATH_LENGTH + 1);

    displayPrompt();

    // A character that accepts the residual newline at the end of a scanf
    char disposedCharacter;

    // Taking command as input
    takeInput();

    // Merging the arguments in order to compare with the last command in history
    _Bool repeated = 0;
    if (strcmp(INPUTSTRING, HISTORY[REAR]) == 0 || strcmp(INPUTSTRING, "") == 0)
        repeated = 1;

    // Adding the command to history
    if (!repeated)
    {
        if (HISTORYNO == 20)
            deleteFromHistory();
        addToHistory(INPUTSTRING);
        saveHistory();
    }

    // Splitting the command string into commands
    char *command = INPUTSTRING;
    // Marks the position of the next ; to replace it with null character
    int semicolonPos = 0;
    // Storing the length of the input string
    int len = strlen(INPUTSTRING);

    // The command loop
    while (semicolonPos < len)
    {
        // Looping through command string until ; is found or end of string is reached
        for (; semicolonPos != len; semicolonPos++)
            if (INPUTSTRING[semicolonPos] == ';')
                break;

        INPUTSTRING[semicolonPos] = '\0';
        ++semicolonPos;

        // Parsing the string entered to get a proper string that can be tokenized
        parseInput(command, COMMANDSTRING);

        // Counting the number of piped commands
        int pipeCount = 1;
        for (int i = 0; i < strlen(COMMANDSTRING); i++)
            if (COMMANDSTRING[i] == '|')
                pipeCount++;

        // The number of command-line arguments
        int p = 0;
        _Bool space = 0, Echo = 0;
        char pipedCommands[pipeCount][strlen(COMMANDSTRING) + 1];

        char *pipeSeparator = strtok(COMMANDSTRING, "|");
        while (pipeSeparator != NULL)
        {
            strcpy(pipedCommands[p], pipeSeparator);
            pipeSeparator = strtok(NULL, "|");
            p++;
        }

        // Array storing the virtual file descriptors to be written to for each piped command
        int pipeDescriptors[pipeCount - 1][2];
        for (int i = 0; i < pipeCount - 1; i++)
            if (pipe(pipeDescriptors[i]) < 0)
            {
                perror("Piping error");
                return;
            }

        p = 0;
        // Storing original file descriptors
        int origSTDIN = dup(STDIN_FILENO);
        int origSTDOUT = dup(STDOUT_FILENO);

        for (int i = 0; i < pipeCount; i++)
        {
            // Number of arguments in the command
            int argc = 0;

            // Tokenizing the command
            char *token = strtok(pipedCommands[i], " ");
            while (token != NULL)
            {
                args[argc] = token;
                ++argc;
                token = strtok(NULL, " ");

                // Checking if the number of arguments exceeds the maximum possible number
                if (argc >= MAX_ARG_NO)
                {
                    perror("Maximum number of arguments exceeded");
                    goto close_files;
                }
            }

            // Null-terminating the array
            args[argc] = NULL;

            // Setting the virtual files to be piped to
            if (p == pipeCount - 1)
                dup2(origSTDOUT, STDOUT_FILENO);
            else if (p < pipeCount - 1)
                if (dup2(pipeDescriptors[p][1], STDOUT_FILENO) < 0)
                {
                    perror("Piping error");
                    goto close_files;
                }

            if (p > 0)
                if (dup2(pipeDescriptors[p - 1][0], STDIN_FILENO) < 0)
                {
                    perror("Piping error");
                    goto close_files;
                }

            // Closing the dup'd pipes
            if (p > 0)
                close(pipeDescriptors[p - 1][0]);
            if (p < pipeCount - 1)
                close(pipeDescriptors[p][1]);

            // Executing the command
            execCommand(args, argc);
            p++;
        }

    close_files:
        // Resetting original file descriptors in case
        dup2(origSTDIN, STDIN_FILENO);
        dup2(origSTDOUT, STDOUT_FILENO);
        close(origSTDIN);
        close(origSTDOUT);

        // Moving command pointer to next command
        command = INPUTSTRING + semicolonPos;
    }
}