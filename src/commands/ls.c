#include <stdio.h>
#include <string.h>
#include <dirent.h>

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