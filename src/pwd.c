#include <stdio.h>
#include <string.h>

// Implementation of pwd
void printWorkingDirectory(char *path, char *homePath)
{
    // if (strncmp(path, homePath, strlen(homePath)) == 0)
    // {
    //     char *relativePath = &path[strlen(homePath)];
    //     printf("%s\n", relativePath);
    // }
    // else
    printf("%s\n", path);
}