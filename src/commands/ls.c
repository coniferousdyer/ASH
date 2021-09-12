#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

// Function to list the contents of a directory
void ls(int numFlag, char *path)
{
    DIR *directory;
    struct dirent *entry;
    directory = opendir(path);

    if (directory)
    {
        // If -a was provided
        if (numFlag == 1)
            while ((entry = readdir(directory)) != NULL)
                printf("%s\n", entry->d_name);

        // If no -a was provided
        else if (numFlag == 0)
            while ((entry = readdir(directory)) != NULL)
                if (entry->d_name[0] != '.')
                    printf("%s\n", entry->d_name);

        closedir(directory);
    }
    else
    {
        printf("ERROR: Invalid path specified. Please try again.\n");
        return;
    }
}

// Function to list the contents of a directory with permissions and other information
void lsl(int numFlag, char *path)
{
    printf("CALLED lsl\n");
}

// The control function for ls
void lsHandler(char *args[], int argc)
{
    // If only ls was provided
    if (argc == 1)
    {
        ls(0, ".");
        return;
    }

    /* combination = 0 -> no flags
         * combination = 1 -> only -a
         * combination = 2 -> only -l
         * combination = 3 -> both -l and -a */
    int opt, combination = 0;
    char path[MAX_PATH_LENGTH + 1];

    // Resetting optind to 0 for getopts to work
    optind = 0;

    while ((opt = getopt(argc, args, "al")) != -1)
        switch (opt)
        {
        case 'a':
            if (combination != 1 && combination != 3)
                combination += 1;
            break;
        case 'l':
            if (combination != 2 && combination != 3)
                combination += 2;
            break;
        case ':':
            break;
        case '?':
            printf("ERROR: Invalid flag entered. Please try again.\n");
            return;
        }

    // Number of extra arguments
    int extraArgs = argc - optind;
    int extraArgsRemaining = extraArgs;

    // If no paths were provided
    if (extraArgs == 0)
    {
        if (combination <= 1)
            ls(combination, ".");
        else
            lsl(combination, ".");
    }

    // Checking for extra arguments, i.e. the paths, supplied
    for (; optind < argc; optind++)
    {
        if (strcmp(args[optind], "~") == 0)
            args[optind] = HOME;

        // Checking the appropriate function to call, with correct arguments
        if (combination <= 1)
        {
            // If more than 2 paths were provided
            if (extraArgs >= 2)
            {
                printf("%s:\n", args[optind]);
                ls(combination, args[optind]);
                if (extraArgsRemaining > 1)
                    printf("\n");
            }
            else
                ls(combination, args[optind]);
        }
        else
        {
            // If more than 2 paths were provided
            if (extraArgs >= 2)
            {
                printf("%s:\n", args[optind]);
                lsl(combination - 2, args[optind]);
                if (extraArgsRemaining > 1)
                    printf("\n");
            }
            else
                lsl(combination - 2, args[optind]);
        }

        --extraArgsRemaining;
    }
}