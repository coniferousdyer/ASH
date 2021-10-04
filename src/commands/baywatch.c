#include "../header_files/utilities.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Function to print the most recently created PID
void bw_newborn(int interval)
{
    FILE *fp = fopen("/proc/loadavg", "r");

    if (fp == NULL)
    {
        perror("Error in newborn");
        return;
    }

    // Reading the 5th value in the file
    int pid;
    fscanf(fp, "%*f %*f %*f %*s %d", &pid);
    printf("%d\n", pid);

    fclose(fp);
}

// Function to print the number of times the CPUs have been interrupted by the keyboard controller (i8042 with IRQ 1)
void bw_interrupt(int interval)
{
    FILE *fp = fopen("/proc/interrupts", "r");

    if (fp == NULL)
    {
        perror("Error in interrupt");
        return;
    }

    char line[200];

    // Reading the heading
    fgets(line, sizeof(line), fp);
    line[strlen(line) - 1] = '\0';
    printf("%s\n", line + 12);

    // Reading the 3rd line in the file
    for (int i = 0; i < 2; i++)
        fgets(line, sizeof(line), fp);

    line[95] = '\0';
    printf("%s\n", line + 12);

    fclose(fp);
}

// Implementation of the baywatch command
void baywatch(char *args[], int argc)
{
    int intervalPos = -1;
    _Bool interrupt = 0, newborn = 0, dirty = 0;

    // Searching for the positions of the arguments
    for (int i = 1; i < argc; i++)
        if (strcmp(args[i], "-n") == 0)
            intervalPos = i + 1;
        else if (strcmp(args[i], "interrupt") == 0)
            interrupt = 1;
        else if (strcmp(args[i], "newborn") == 0)
            newborn = 1;
        else if (strcmp(args[i], "dirty") == 0)
            dirty = 1;

    // If interval was not provided
    if (intervalPos == -1 || intervalPos == argc)
    {
        perror("Interval not entered");
        return;
    }

    // If none of the three commands allowed was entered
    if (!(interrupt || newborn || dirty))
    {
        perror("Interrupt/newborn/dirty command not entered");
        return;
    }

    // If interval was not a whole number
    if (!isInteger(args[intervalPos]))
    {
        perror("Invalid interval entered");
        return;
    }

    // Storing interval
    int interval = atoi(args[intervalPos]);

    //////////////////////////////

    
}