#include "header_files/utilities.h"
#include "header_files/commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
                // If enclosed within double quotes, insert a backslash to account for spaces - essentially converting the double quotes problem to a backslash problem
                if (inputString[i] == ' ' && doubleQuotes)
                {
                    // If preceding character was a double quote
                    if (inputString[i - 1] == '\"')
                        continue;

                    if (i != strlen(inputString) - 1 && inputString[i + 1] == '\"')
                        continue;

                    parsedString[len] = '\\';
                    parsedString[len + 1] = ' ';
                    len += 2;
                }

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

    // Exiting the shell
    if (strcmp(args[0], "exit") == 0)
    {
        if (argc != 1)
        {
            printf("ERROR: Too many arguments specified. Please try again.\n");
            return;
        }

        printf("Exited with status 0.\n");
        exit(0);
    }
    // Checking if cd was entered
    else if (strcmp(args[0], "cd") == 0)
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
        echo(args, argc);
        return;
    }
    // Checking if pinfo was entered
    else if (strcmp(args[0], "pinfo") == 0)
    {
        if (argc == 1)
        {
            pid_t pid = getpid();
            pinfo(pid, home);
        }
        else if (argc == 2)
        {
            pid_t pid = atoi(args[1]);
            pinfo(pid, home);
        }
        else
            printf("ERROR: Too many arguments specified. Please try again.\n");

        return;
    }
    // Checking if repeat was entered
    else if (strcmp(args[0], "repeat") == 0)
    {
        if (argc <= 2)
        {
            printf("ERROR: Too few arguments specified. Please try again.\n");
            return;
        }

        // Storinging the number of times the command must be repeated
        int times = atoi(args[1]);

        // Shifting the args array back by 2 to replace "repeat n"
        for (int i = 2; i <= argc; i++)
            args[i - 2] = args[i];
        argc -= 2;

        // Calling execCommand to execute the command the required number of times
        for (int i = 0; i < times; i++)
            execCommand(args, argc, home, prevPath);

        return;
    }

    /*-----------SYSTEM COMMANDS-----------*/

    // Create a new process
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("ERROR: A new process could not be created.\n");
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
            printf("ERROR: The command does not exist. Please try again.\n");
    }
    else
    {
        int STATUS;

        // Wait for the child process to terminate if not a background process
        if (strcmp(args[argc - 1], "&") != 0)
            waitpid(pid, &STATUS, 0);
        else
            printf("%d\n", pid);
    }
}