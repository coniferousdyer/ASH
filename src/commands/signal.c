#include "../header_files/util_variables.h"
#include "../header_files/utilities.h"
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

// The signal handler which reaps background processes
void sigchldHandler(int sig)
{
    int STATUS, pid;

    // Waiting for background processes to terminate
    while ((pid = waitpid(-1, &STATUS, WNOHANG)) > 0)
    {
        if (pid > 0)
        {
            // Deleting the process from the process list and obtaining its name
            char name[MAX_FILE_LENGTH + 1];
            DeleteProcess(pid, name);

            if (WIFEXITED(STATUS))
                fprintf(stderr, "\n%s with pid %d exited normally.\n", name, pid);
            else if (WIFSIGNALED(STATUS))
                fprintf(stderr, "\n%s with pid %d exited abnormally.\n", name, pid);

            // Displaying the prompt
            displayPrompt();
            fflush(stdout);
        }
    }

    signal(SIGCHLD, sigchldHandler);
}

// The signal handler which interrupts the foreground process
void sigintHandler(int sig)
{
    // Checking if no foreground process is running
    if (FGPID == -2)
    {
        printf("^C\n");
        displayPrompt();
        return;
    }

    kill(FGPID, SIGINT);
    FGPID = -2;
    signal(SIGINT, sigintHandler);
}

// The signal handler which shifts the foreground process to background
void sigtstpHandler(int sig)
{
    // Checking if no foreground process is running
    if (FGPID == -2)
        return;

    // Obtaining the process name
    char processName[MAX_PATH_LENGTH + 1];
    sprintf(processName, "/proc/%d/cmdline", FGPID);

    FILE *fp = fopen(processName, "r");
    fgets(processName, MAX_PATH_LENGTH, fp);
    fclose(fp);

    // Adding process to background process array
    InsertProcess(FGPID, processName);

    // Sending the foreground process a SIGTSTP signal
    kill(FGPID, SIGTSTP);

    FGPID = -2;
    signal(SIGTSTP, sigtstpHandler);
}

// Implementation of the sig command
void sig(int jobNo, int sig)
{
    // Searching for the process
    pid_t pid = -1;
    for (int i = 0; i < CHILDNO; i++)
        if (PROCESSLIST[i].jobNo == jobNo)
        {
            pid = PROCESSLIST[i].pid;
            break;
        }

    // Sending the signal to the process
    if (pid == -1)
        perror("Process not found");
    else
    {
        char state;

        char fileName[25];
        sprintf(fileName, "/proc/%d/stat", pid);

        FILE *fp = fopen(fileName, "r");

        // Getting state
        fscanf(fp, "%*d %*s %c", &state);

        // Sending signal only if process is running or suspended
        if (state == 'R' || state == 'S')
            kill(pid, sig);
        else
            perror("Process not running/suspended");

        fclose(fp);
    }
}

// Sets up the required signal handler functions
void installSignals()
{
    signal(SIGCHLD, sigchldHandler);
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigtstpHandler);
}