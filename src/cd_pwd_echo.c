#include "header_files/commands.h"
#include <stdio.h>
#include <string.h>

// Implementation of cd
void changeDirectory(char *path, char *subpath);

// Implementation of pwd
void printWorkingDirectory(char *path, char *homePath)
{
    if (strncmp(path, homePath, strlen(homePath)) == 0)
    {
        char *relativePath = &path[strlen(homePath)];
        printf("%s", relativePath);
    }
    else
        printf("%s", path);
}

// Implementation of echo
void echo(char *args[]);
