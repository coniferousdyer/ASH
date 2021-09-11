#include "../header_files/commands.h"
#include "../header_files/util_variables.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void pinfo(pid_t pid, char *home)
{
    char fileName[25];
    sprintf(fileName, "/proc/%d/stat", pid);

    FILE *fp = fopen(fileName, "r");

    // If process does not exist
    if (fp == NULL)
    {
        printf("ERROR: Process with ID %d was not found.\n", pid);
        return;
    }

    char execPath[MAX_PATH_LENGTH + 1], state;
    unsigned long int vSize;

    // Getting state and virtual memory size
    fscanf(fp, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %lu", &state, &vSize);

    // Reading the process' executable path
    sprintf(fileName, "/proc/%d/exe", pid);
    int len = readlink(fileName, execPath, sizeof(execPath));
    execPath[len] = '\0';

    printf("PID -- %d\nProcess Status -- %c", pid, state);

    // Checking if process is a foreground or background process
    

    printf("Memory -- %lu\nExecutable Path -- ", vSize);

    // Deciding whether relative or absolute path is to be used by comparing the first strlen(HOME) characters
    if (strncmp(execPath, home, strlen(home)) == 0)
    {
        char *relativePath = &execPath[strlen(home)];
        printf("~%s\n", relativePath);
    }
    else
        printf("%s\n", execPath);

    fclose(fp);
}