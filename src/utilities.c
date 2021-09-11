#include "header_files/utilities.h"
#include "header_files/commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString)
{
    int len = 0;
    _Bool doubleQuotes = 0;

    for (int i = 0; inputString[i] != '\0'; i++)
    {
        // If the current character is a double quote
        if (inputString[i] == '\"')
        {
            if (doubleQuotes)
                doubleQuotes = 0;
            else
                doubleQuotes = 1;

            continue;
        }

        // If enclosed within double quotes, insert a backslash to account for spaces - essentially converting the double quotes problem to a backslash problem
        if (inputString[i] == ' ' && doubleQuotes)
        {
            parsedString[len] = '\\';
            parsedString[len + 1] = ' ';
            len += 2;
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
                parsedString[len] = inputString[i];
                ++len;
            }
        }
    }

    parsedString[len] = '\0';
}

// Function to execute the corresponding command
void execCommand(char *args[], int argc, char *home, char *prevPath)
{
    /*-----------BUILTIN COMMANDS-----------*/

    // If nothing was entered
    if (argc == 0)
        return;

    // Checking if cd was entered
    if (strcmp(args[0], "cd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 2)
        {
            printf("ERROR: Too many arguments specified. Please try again.\n");
            return;
        }

        // Accounting for cd only
        if (argc == 1)
        {
            chdir(home);
            return;
        }

        changeDirectory(args[1], home, prevPath);
        return;
    }
    // Checking if pwd was entered
    else if (strcmp(args[0], "pwd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 1)
        {
            printf("ERROR: Too many arguments specified. Please try again.\n");
            return;
        }

        printWorkingDirectory();
        return;
    }
    // Checking if echo was entered
    else if (strcmp(args[0], "echo") == 0)
    {
        // If no argument except command was provided
        if (argc == 1)
            return;
    }

    /*-----------SYSTEM COMMANDS-----------*/

    // Create a new process
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("ERROR: An error occurred while executing the command. Please try again.\n");
        return;
    }

    if (pid == 0)
    {
        // Executing the given command, replacing the address space of the child process
        if (execvp(args[0], args))
            printf("ERROR: The command does not exist. Please try again.\n");
    }
    else
    {
        int STATUS;

        // Wait for the child process to terminate
        wait(&STATUS);
    }
}