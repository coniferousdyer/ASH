#include "../header_files/commands.h"
#include "../header_files/util_variables.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

// Function to modify the path
void modifyPath(char *path, char *subpath, char *home)
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
        // Accounting for cd and cd ~
        else if (strcmp(directory, "~") == 0)
            strcpy(path, home);
        // Accounting for cd .
        else if (strcmp(directory, ".") == 0)
        {
            directory = strtok(NULL, "/");
            continue;
        }
        else
        {
            // Checking if the last character is already a forward slash, i.e. the home directory
            if (path[strlen(path) - 1] != '/')
                strcat(path, "/");
            strcat(path, directory);
        }

        directory = strtok(NULL, "/");
    }
}

// Implementation of cd
void changeDirectory(char *path, char *subpath, char *home, char *prevPath)
{
    char tempPath[MAX_PATH_LENGTH + 1];
    strcpy(tempPath, path);

    // Accounting for cd - here to avoid stack allocation overhead
    if (strcmp(subpath, "-") == 0)
    {
        strcpy(path, prevPath);
        strcpy(prevPath, tempPath);

        return;
    }

    // Modifying the path
    modifyPath(path, subpath, home);

    struct stat test;

    // If path does not exist
    if (stat(path, &test) < 0)
    {
        printf("ERROR: Invalid path specified. Please try again.\n");
        strcpy(path, tempPath);
        return;
    }
    // If path exists but not directory
    else if (!S_ISDIR(test.st_mode))
    {
        printf("ERROR: Not a directory. Please try again.\n");
        strcpy(path, tempPath);
        return;
    }

    // Copying the previous path to prevPath
    strcpy(prevPath, tempPath);
}