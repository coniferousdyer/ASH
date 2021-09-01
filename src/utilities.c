#include "header_files/utilities.h"
#include <string.h>

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString)
{
    int len = 0;

    for (int i = 0; inputString[i] != '\0'; i++)
    {
        // Checking if it's the first or the last character; if last character, there should not be a space before
        if (i == 0 || (i == strlen(inputString) - 1 && inputString[i - 1] != ' '))
        {
            // Converting tabs to spaces
            if (inputString[i] == '\t')
                inputString[i] = ' ';
            // If the current character is not a space, include it
            if (inputString[i] != ' ')
            {
                parsedString[len] = inputString[i];
                ++len;
            }
        }
        else
        {
            // Converting tabs to spaces
            if (inputString[i] == '\t')
                inputString[i] = ' ';
            // Ignoring consecutive spaces
            if (inputString[i] == ' ' && inputString[i - 1] == ' ')
                continue;
            else
            {
                parsedString[len] = inputString[i];
                ++len;
            }
        }
    }

    parsedString[len] = '\0';
}
