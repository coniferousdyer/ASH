#include "../header_files/util_variables.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

// Function to bring a foreground process to the background
void bg(int jobNo)
{
     pid_t pid = -1;
    int STATUS;
    char nameString[MAX_FILE_LENGTH + 1];

    // Searching for the process
    for (int i = 0; i < CHILDNO; i++)
        if (PROCESSLIST[i].jobNo == jobNo)
        {
            pid = PROCESSLIST[i].pid;
            break;
        }

    // If background process does not exist
    if (pid == -1)
    {
        perror("Process not found");
        return;
    }

    // Sending a SIGCONT signal to make it continue
    kill(pid, SIGCONT);
}