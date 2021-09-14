#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>

// Function to add command to history
void addToHistory(char *args[], int argc)
{
    char command[MAX_COMMAND_LENGTH + 1] = "";
    for (int i = 0; i < argc; i++)
    {
        strcat(command, args[i]);
        if (i < argc - 1)
            strcat(command, " ");
    }

    // If less than 20 commands present
    if (HISTORYNO < 20)
    {
        ++REAR;
        strcpy(HISTORY[REAR], command);
        ++HISTORYNO;
    }
    else
    {
        if (REAR == 19 && FRONT != 0)
        {
            REAR = 0;
            strcpy(HISTORY[REAR], command);
        }
        else
        {
            ++REAR;
            strcpy(HISTORY[REAR], command);
        }
    }
}

// Function to delete earliest command from history
void deleteFromHistory()
{
    if (FRONT == 19)
        FRONT = 0;
    else
        FRONT++;
}

// Function to print the history of commands
void history(int n)
{
    if (HISTORYNO == 0)
    {
        printf("No history.\n");
        return;
    }

    if (REAR >= FRONT)
        for (int i = FRONT; i <= REAR; i++)
            printf("%s\n", HISTORY[i]);
    else
    {
        for (int i = FRONT; i < 20; i++)
            printf("%s\n", HISTORY[i]);
        for (int i = 0; i <= REAR; i++)
            printf("%s\n", HISTORY[i]);
    }
}