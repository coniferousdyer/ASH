#include "../header_files/util_variables.h"
#include "../header_files/utilities.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>

// Function to bring a background process to the foreground
void fg(int jobNo)
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

    // Ignoring the SIGTTOU signal
    signal(SIGTTOU, SIG_IGN);

    // Changing the foreground process group to that of the background process
    tcsetpgrp(STDIN_FILENO, getpgid(pid));

    FGPID = pid;

    // Deleting the process from the background process array
    DeleteProcess(pid, nameString);

    // Sending the SIGCONT signal to the process in case it is stopped
    kill(pid, SIGCONT);

    // Waiting for the process to end
    waitpid(pid, &STATUS, 0);
    FGPID = -2;

    // Changing the foreground process group back to that of the shell
    tcsetpgrp(STDIN_FILENO, getpgid(SHELLPID));

    // Restoring the default action to SIGTTOU
    signal(SIGTTOU, SIG_DFL);
}