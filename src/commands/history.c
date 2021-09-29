#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Function to read previous session commands from external file
void initHistory()
{
    // Opening file if it exists in the directory of the executable
    char path[MAX_PATH_LENGTH + 101] = "";
    readlink("/proc/self/exe", path, MAX_PATH_LENGTH + 101);
    int pos = strlen(path) - 1;
    for (; pos >= 0 && path[pos] != '/'; pos--)
        ;
    path[pos + 1] = '\0';
    strcat(path, "history.txt");

    FILE *fp = fopen(path, "r");

    if (fp == NULL)
        return;

    // Reading each line (command) from the line and storing it in a circular queue
    while (fgets(HISTORY[HISTORYNO], sizeof(HISTORY[HISTORYNO]), fp) != NULL)
    {
        HISTORY[HISTORYNO][strlen(HISTORY[HISTORYNO]) - 1] = '\0';
        ++HISTORYNO;

        if (FRONT == -1)
            FRONT = REAR = 0;
        else
            ++REAR;
    }

    fclose(fp);
}

// Function to save current session's history to external file
void saveHistory()
{
    // Opening file if it exists, else creating it
    char path[MAX_PATH_LENGTH + 101] = "";
    readlink("/proc/self/exe", path, MAX_PATH_LENGTH + 101);
    int pos = strlen(path) - 1;
    for (; pos >= 0 && path[pos] != '/'; pos--)
        ;
    path[pos + 1] = '\0';
    strcat(path, "history.txt");

    FILE *fp = fopen(path, "w");

    // Writing each command to the file
    if (REAR >= FRONT)
        for (int i = FRONT; i <= REAR; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
    else
    {
        for (int i = FRONT; i < 20; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
        for (int i = 0; i <= REAR; i++)
            fprintf(fp, "%s\n", HISTORY[i]);
    }

    fclose(fp);
}

// Function to add command to history
void addToHistory(char *command)
{
    // If less than 20 commands present
    if (HISTORYNO < 20)
    {
        if (FRONT == -1)
            FRONT = 0;

        ++REAR;
        strcpy(HISTORY[REAR], command);
        ++HISTORYNO;
    }
    else
    {
        if (FRONT == -1)
        {
            FRONT = REAR = 0;
            strcpy(HISTORY[REAR], command);
        }
        else if (REAR == 19 && FRONT != 0)
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
        perror("No history");
        return;
    }

    int count = 0;
    char *tempArray[20];

    // Storing in a temporary array for printing convenience
    if (REAR >= FRONT)
        for (int i = FRONT; i <= REAR; i++, count++)
            tempArray[count] = HISTORY[i];
    else
    {
        for (int i = FRONT; i < 20; i++, count++)
            tempArray[count] = HISTORY[i];
        for (int i = 0; i <= REAR; i++, count++)
            tempArray[count] = HISTORY[i];
    }

    for (int i = count - n; i < count; i++)
        printf("%s\n", tempArray[i]);
}