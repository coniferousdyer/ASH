#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>

// Function to recall the previous command from history
int recallCommand(int n, int len)
{
    if (HISTORYNO == 0)
        return len;

    int count = 0;
    char *tempArray[20];

    // Storing in a temporary array for convenience
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

    // Printing backspace repeatedly to remove the current command
    while (len--)
    {
        // If the character is a TAB character
        if (INPUTSTRING[len] == 9)
            for (int i = 0; i < 8; i++)
                printf("\b");
        else
            printf("\b \b");
    }

    // Copying the previous command to INPUTSTRING so that it can be manipulated
    strcpy(INPUTSTRING, HISTORY[n]);

    // Printing INPUTSTRING - there was an issue with printing tabs properly
    for (int i = 0; INPUTSTRING[i] != '\0'; i++)
        // If the character was a tab
        if (INPUTSTRING[i] == 9)
            for (int i = 0; i < 8; i++)
                printf(" ");
        else
            printf("%c", INPUTSTRING[i]);

    // Returning the new length of INPUTSTRING
    return strlen(INPUTSTRING);
}