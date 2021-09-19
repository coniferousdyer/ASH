#include "header_files/utilities.h"
#include "header_files/util_variables.h"
#include "header_files/commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
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

/*---------------------------------------------------------------*
 * Function to read previous session commands from external file *
 *---------------------------------------------------------------*/
void initHistory()
{
    // Opening file if it exists in the directory of the executable
    char path[MAX_PATH_LENGTH + 101] = "";
    readlink("/proc/self/exe", path, MAX_PATH_LENGTH + 101);
    int pos = strlen(path) - 1;
    for (; pos >= 0 && path[pos] != '/'; pos--)
        ;
    path[pos + 1] = '\0';
    strcat(path, "history.txt");

    FILE *fp = fopen(path, "r");

    if (fp == NULL)
        return;

    // Reading each line (command) from the line and storing it in a circular queue
    while (fgets(HISTORY[HISTORYNO], sizeof(HISTORY[HISTORYNO]), fp) != NULL)
    {
        HISTORY[HISTORYNO][strlen(HISTORY[HISTORYNO]) - 1] = '\0';
        ++HISTORYNO;

        if (FRONT == -1)
            FRONT = REAR = 0;
        else
            ++REAR;
    }

    fclose(fp);
}

/*-------------------------------------------------------------*
 * Function to save current session's history to external file *
 *-------------------------------------------------------------*/
void saveHistory()
{
    // Opening file if it exists, else creating it
    char path[MAX_PATH_LENGTH + 101] = "";
    readlink("/proc/self/exe", path, MAX_PATH_LENGTH + 101);
    int pos = strlen(path) - 1;
    for (; pos >= 0 && path[pos] != '/'; pos--)
        ;
    path[pos + 1] = '\0';
    strcat(path, "history.txt");

    FILE *fp = fopen(path, "w");

    // Writing each command to the file
    if (REAR >= FRONT)
        for (int i = FRONT; i <= REAR; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
    else
    {
        for (int i = FRONT; i < 20; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
        for (int i = 0; i <= REAR; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
    }

    fclose(fp);
}

/*-----------------------------------------------------*
 * The signal handler which reaps background processes *
 *-----------------------------------------------------*/
void signalHandler(int signal)
{
    int STATUS, pid;

    // Waiting for background processes to terminate
    while ((pid = waitpid(-1, &STATUS, WNOHANG)) > 0)
    {
        if (pid > 0)
        {
            // Deleting the process from the process list and obtaining its name
            char name[MAX_FILE_LENGTH + 1];
            DeleteProcess(pid, name);

            if (WIFEXITED(STATUS))
                fprintf(stderr, "\n%s with pid %d exited normally.\n", name, pid);
            else if (WIFSIGNALED(STATUS))
                fprintf(stderr, "\n%s with pid %d exited abnormally.\n", name, pid);

            // Displaying the prompt
            displayPrompt();
            fflush(stdout);
        }
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

/*-----------------------------------------------*
 * Function to execute the corresponding command *
 *-----------------------------------------------*/
void execCommand(char *args[], int argc)
{
    /*-----------BUILTIN COMMANDS-----------*/

    // If nothing was entered
    if (argc == 0)
        return;

    // Exiting the shell
    if (strcmp(args[0], "exit") == 0)
    {
        if (argc != 1)
        {
            perror("Too many arguments specified");
            return;
        }

        printf("Exited with status 0.\n");
        saveHistory();
        exit(0);
    }
    // Checking if cd was entered
    else if (strcmp(args[0], "cd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 2)
        {
            perror("Too many arguments specified");
            return;
        }

        // Accounting for cd only
        if (argc == 1)
        {
            chdir(HOME);
            return;
        }

        changeDirectory(args[1]);
        return;
    }
    // Checking if pwd was entered
    else if (strcmp(args[0], "pwd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 1)
        {
            perror("Too many arguments specified");
            return;
        }

        printWorkingDirectory();
        return;
    }
    // Checking if echo was entered
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args, argc);
        return;
    }
    // Checking if pinfo was entered
    else if (strcmp(args[0], "pinfo") == 0)
    {
        if (argc == 1)
        {
            pid_t pid = getpid();
            pinfo(pid);
        }
        else if (argc == 2)
        {
            if (!isInteger(args[1]))
            {
                perror("Invalid argument specified");
                return;
            }

            pid_t pid = atoi(args[1]);
            pinfo(pid);
        }
        else
            perror("Too many arguments specified");

        return;
    }
    // Checking if repeat was entered
    else if (strcmp(args[0], "repeat") == 0)
    {
        if (argc <= 2)
        {
            perror("Too few arguments specified");
            return;
        }

        if (!isInteger(args[1]))
        {
            perror("Invalid argument specified");
            return;
        }

        // Storinging the number of times the command must be repeated
        int times = atoi(args[1]);

        // Creating a copy of args
        char *argsCopy[argc + 1];

        // Shifting the args array back by 2 to replace "repeat n"
        for (int i = 2; i <= argc; i++)
            argsCopy[i - 2] = args[i];
        argc -= 2;

        // Calling execCommand to execute the command the required number of times
        for (int i = 0; i < times; i++)
            execCommand(argsCopy, argc);

        return;
    }
    // Checking if ls was entered
    else if (strcmp(args[0], "ls") == 0)
    {
        lsHandler(args, argc);
        return;
    }
    // Checking if history was entered
    else if (strcmp(args[0], "history") == 0)
    {
        if (argc == 1)
        {
            if (HISTORYNO < 10)
                history(HISTORYNO);
            else
                history(10);
        }
        else if (argc == 2)
        {
            if (!isInteger(args[1]))
            {
                perror("Invalid argument specified");
                return;
            }

            int n = atoi(args[1]);

            if (n > 20)
                perror("Not more than 20 commands can be listed");
            else if (n < 0)
            {
                perror("Invalid argument specified");
                return;
            }

            if (HISTORYNO < n)
                history(HISTORYNO);
            else
                history(n);
        }
        else
            perror("Too few arguments specified");

        return;
    }

    /*-----------SYSTEM COMMANDS-----------*/

    // Create a new process
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Could not create new process");
        return;
    }

    if (pid == 0)
    {
        // Checking if the last argument was &
        if (strcmp(args[argc - 1], "&") == 0)
        {
            // Setting the child process group ID to 0 to identify it as a background process
            setpgid(0, 0);
            args[argc - 1] = NULL;
            argc--;
        }

        // Executing the given command, replacing the address space of the child process
        if (execvp(args[0], args))
        {
            perror("Non-existent command");

            // To exit the new child process created
            exit(0);
        }
    }
    else
    {
        int STATUS;

        // Wait for the child process to terminate if not a background process
        if (strcmp(args[argc - 1], "&") != 0)
            waitpid(pid, &STATUS, 0);
        else
        {
            if (CHILDNO == MAX_CHILD_NO)
            {
                perror("Maximum number of background processes reached");
                fprintf(stderr, "The process is being executed as a foreground process.\n");
                waitpid(pid, &STATUS, 0);
                return;
            }

            printf("%d\n", pid);

            char path[MAX_FILE_LENGTH + 12];

            // Inserting the process into the process list
            InsertProcess(pid, args[0]);
        }
    }
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
    if (strcmp(INPUTSTRING, HISTORY[REAR]) == 0)
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

        // The number of command-line arguments
        int argc = 0;
        _Bool space = 0, Echo = 0;
        char CommandStringCopy[strlen(COMMANDSTRING) + 1];

        // Tokenizing the command
        char *token = strtok(COMMANDSTRING, " ");
        while (token != NULL)
        {
            args[argc] = token;
            ++argc;
            token = strtok(NULL, " ");

            // Checking if the number of arguments exceeds the maximum possible number
            if (argc >= MAX_ARG_NO)
            {
                perror("Maximum number of arguments exceeded");
                return;
            }
        }

        // Null-terminating the array
        args[argc] = NULL;

        // Executing the command
        execCommand(args, argc);

        // Moving command pointer to next command
        command = INPUTSTRING + semicolonPos;
    }
}