#include "header_files/utilities.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString)
{
    int len = 0;

    for (int i = 0; inputString[i] != '\0'; i++)
    {
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

// Function to modify the path
void modifyPath(char *path, char *subpath)
{
    // Splitting the subpath into the respective directories
    char *directory = strtok(subpath, "/");
    while (directory != NULL)
    {
        // Checking if we're going back or front
        if (strcmp(directory, "..") == 0)
        {
            // Checking if the absolute path is currently the root directory
            if (strcmp("path", "/") == 0)
                continue;

            // i stores the index of the last '/' in the path
            int i;
            for (i = strlen(path) - 1; path[i] != '/'; i--)
                ;

            // Checking if we are in the home directory and going to root directory
            if (i == 0)
                path[i + 1] = '\0';
            else
                path[i] = '\0';
        }
        else
        {
            if (path[strlen(path) - 1] != '/')
                strcat(path, "/");
            strcat(path, directory);
        }

        directory = strtok(NULL, "/");
    }
}

// Function to execute the corresponding command
void execCommand(char *args[], int argc)
{
    // If nothing was entered
    if (argc == 0)
        return;

    // Create a new process
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("ERROR: An error occurred while executing the command. Please try again.");
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