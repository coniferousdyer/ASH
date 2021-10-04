#include "../header_files/util_variables.h"
#include "../header_files/commands.h"
#include "../header_files/utilities.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Implementation of the replay command
void replay(char *args[], int argc)
{
    int commandPos = -1, periodPos = -1, intervalPos = -1;

    // Searching for the positions of the arguments
    for (int i = 1; i < argc - 1; i++)
        if (strcmp(args[i], "-command") == 0)
            commandPos = i + 1;
        else if (strcmp(args[i], "-interval") == 0)
            intervalPos = i + 1;
        else if (strcmp(args[i], "-period") == 0)
            periodPos = i + 1;

    // If one of the arguments was not found
    if (commandPos == -1)
    {
        perror("Command not entered");
        return;
    }
    else if (intervalPos == -1)
    {
        perror("Interval not entered");
        return;
    }
    else if (periodPos == -1)
    {
        perror("Period not entered");
        return;
    }

    // If whole numbers were not provided as arguments for period and interval
    if (!isInteger(args[periodPos]))
    {
        perror("Invalid period entered");
        return;
    }

    if (!isInteger(args[intervalPos]))
    {
        perror("Invalid interval entered");
        return;
    }

    // Storing period and interval
    int period, interval;
    period = atoi(args[periodPos]);
    interval = atoi(args[intervalPos]);

    // Counting number of command arguments
    int argCount = 0;
    for (int i = commandPos; i < argc; i++, argCount++)
        if (strcmp(args[i], "-command") == 0 || strcmp(args[i], "-interval") == 0 || strcmp(args[i], "-period") == 0)
            break;

    // If no command arguments were found
    if (argCount == 0)
    {
        perror("Command not entered");
        return;
    }

    // Storing the command arguments
    char *commandArgs[argCount + 1];
    commandArgs[argCount] = NULL;
    for (int i = commandPos; i < commandPos + argCount; i++)
    {
        commandArgs[i - commandPos] = (char *)malloc(strlen(args[i]) + 1);
        strcpy(commandArgs[i - commandPos], args[i]);
    }

    // Core logic
    int times = period / interval;
    int remainingTime = period - (times * interval);
    for (int i = 0; i < times; i++)
    {
        sleep(interval);
        execCommand(commandArgs, argCount);
    }

    // Sleeping for the time remaining
    sleep(remainingTime);

    // Freeing the malloc'd strings
    for (int i = 0; i < argCount; i++)
        free(commandArgs[i]);
}