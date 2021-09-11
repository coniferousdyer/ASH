#include <stdio.h>

// Implementation of echo
void echo(char *args[], int argc)
{
    for (int i = 1; i < argc; i++)
        printf("%s ", args[i]);
    printf("\n");
}