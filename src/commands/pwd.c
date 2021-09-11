#include "../header_files/util_variables.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// Implementation of pwd
void printWorkingDirectory()
{
    char path[MAX_PATH_LENGTH + 1];
    printf("%s\n", getcwd(path, MAX_PATH_LENGTH + 1));
}