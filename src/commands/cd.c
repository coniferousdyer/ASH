#include "../header_files/commands.h"
#include "../header_files/util_variables.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

// Implementation of cd
void changeDirectory(char *subpath)
{
    char tempPath[MAX_PATH_LENGTH + 1];
    getcwd(tempPath, MAX_PATH_LENGTH + 1);

    // Accounting for cd -
    if (strcmp(subpath, "-") == 0)
    {
        chdir(PREVIOUSPATH);
        strcpy(PREVIOUSPATH, tempPath);
        return;
    }
    // Accounting for cd ~
    else if (strcmp(subpath, "~") == 0)
    {
        chdir(HOME);
        return;
    }

    // Accounting for ~ in the middle of the path provided
    for (int i = strlen(subpath) - 1; i >= 0; i--)
        if (subpath[i] == '~')
        {
            // Validation of the preceding path
            char precedingPath[strlen(subpath) + 1];
            strncpy(precedingPath, subpath, i);
            precedingPath[i] = '\0';
            if (strcmp(precedingPath, "") != 0)
                if (chdir(precedingPath) != 0)
                {
                    perror("Invalid path specified");
                    return;
                }

            // After validation, concatenating home path and the part of provided path after ~
            chdir(HOME);
            if (i == strlen(subpath) - 1 || (i == strlen(subpath) - 2 && subpath[i + 1] == '/'))
                return;
            strcpy(subpath, &subpath[i + 2]);

            break;
        }

    if (chdir(subpath) != 0)
    {
        perror("Invalid path specified");
        return;
    }

    // Copying the previous path to prevPath
    strcpy(PREVIOUSPATH, tempPath);
}