#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// A special struct containing the state variable in addition to Process members
typedef struct bgProcess
{
    int jobNo;
    char *pName;
    int pid;
    char state;
} bgProcess;

// Function to compare two Process structs based on command names
int compare(const void *A, const void *B)
{
    const bgProcess *pA = (bgProcess *)A;
    const bgProcess *pB = (bgProcess *)B;
    return strcmp(pA->pName, pB->pName);
}

// Displays the current background processes
void jobs(int param)
{
    int count = 0;

    // Copying the PIDs from PROCESSLIST to processList
    bgProcess processList[CHILDNO];
    for (int i = 0; i < CHILDNO; i++)
    {
        char state;

        // Checking /proc/pid/stat to check process state
        char fileName[25];
        sprintf(fileName, "/proc/%d/stat", PROCESSLIST[i].pid);
        FILE *fp1 = fopen(fileName, "r");
        fscanf(fp1, "%*d %*s %c", &state);

        // Checking flags
        if (param == 0 || (param == 1 && (state == 'S' || state == 'R')) || (param == 2 && state != 'R' && state != 'S'))
        {
            // Checking /proc/pid/cmdline to check command name
            char processName[MAX_PATH_LENGTH + 1];
            sprintf(processName, "/proc/%d/cmdline", PROCESSLIST[i].pid);
            FILE *fp2 = fopen(processName, "r");
            fgets(processName, MAX_PATH_LENGTH, fp2);

            // Copying job number, state, command name and pid to processList
            processList[count].jobNo = PROCESSLIST[i].jobNo;
            processList[count].pName = (char *)malloc(strlen(processName) + 1);
            strcpy(processList[count].pName, processName);
            processList[count].pid = PROCESSLIST[i].pid;
            processList[count].state = state;
            ++count;

            fclose(fp2);
        }

        fclose(fp1);
    }

    // Sorting the list according to command name in alphabetical order
    qsort(processList, count, sizeof(bgProcess), compare);

    // Printing the jobs
    for (int i = 0; i < count; i++)
    {
        printf("[%d] %s %s [%d]\n", processList[i].jobNo, (processList[i].state == 'T' ? "Stopped" : "Running"), processList[i].pName, processList[i].pid);
        free(processList[i].pName);
    }
}

// Handles the input to jobs() and controls the output displayed by jobs()
void jobsHandler(char *args[], int argc)
{
    /* combination = 0 -> no flags
     * combination = 1 -> -r
     * combination = 2 -> -s
     * combination = 3 -> both -r and -s */
    int opt, combination = 0;

    // Resetting optind to 0
    optind = 0;

    while ((opt = getopt(argc, args, "rs")) != -1)
        switch (opt)
        {
        case 'r':
            if (combination != 1 && combination != 3)
                combination += 1;
            break;
        case 's':
            if (combination != 2 && combination != 3)
                combination += 2;
            break;
        case ':':
            break;
        case '?':
            perror("Invalid flag");
            return;
        }

    // Number of extra arguments
    if (argc - optind > 0)
    {
        perror("Too many arguments specified");
        return;
    }

    // Calling jobs(). jobs(3) = jobs(0)
    jobs(combination % 3);
}