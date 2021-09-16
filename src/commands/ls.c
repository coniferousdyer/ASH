#include "../header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

// Function to count the digits of a number
int countDigits(long int n)
{
    int count = 0;
    while (n != 0)
    {
        n = n / 10;
        ++count;
    }

    return count;
}

// Function to check if a file/directory was last modified 6 or more months ago
_Bool moreThanSixMonths(time_t modTime)
{
    // Obtaining current time
    time_t currentTime;
    time(&currentTime);

    // Calculating difference in seconds between current and last modification time
    double diff = difftime(currentTime, modTime);

    // 6 months = 15780000 seconds
    if (diff >= (double)15780000)
        return 1;
    else
        return 0;
}

// Function to split the path into directory path and file name
void splitPath(char *path, char *fileName)
{
    int i;
    for (i = strlen(path) - 1; i >= 0 && path[i] != '/'; i--)
        ;

    strcpy(fileName, path + i + 1);

    if (i == -1)
        strcpy(path, ".");
    else
        path[i + 1] = '\0';
}

// Function to compute size and column lengths
int totalSize(char *path, int numFlag, int colLengths[])
{
    int size = 0;
    DIR *directory;
    struct dirent *entry;

    directory = opendir(path);

    if (directory)
    {
        // Looping through the contents of the directory
        while ((entry = readdir(directory)) != NULL)
        {
            // If it is a hidden file/directory, i.e. when no -a was provided
            if (numFlag == 0 && entry->d_name[0] == '.')
                continue;

            struct stat s;
            char fullPath[MAX_PATH_LENGTH + 1];

            // Obtaining the path of the file/directory
            sprintf(fullPath, "%s/%s", path, entry->d_name);

            stat(fullPath, &s);

            // Core of the function
            size += (s.st_blocks / 2);
            colLengths[0] = (colLengths[0] >= countDigits(s.st_nlink)) ? colLengths[0] : countDigits(s.st_nlink);
            colLengths[1] = (colLengths[1] >= strlen(getpwuid(s.st_uid)->pw_name)) ? colLengths[1] : strlen(getpwuid(s.st_uid)->pw_name);
            colLengths[2] = (colLengths[2] >= strlen(getgrgid(s.st_gid)->gr_name)) ? colLengths[2] : strlen(getgrgid(s.st_gid)->gr_name);
            colLengths[3] = (colLengths[3] >= countDigits(s.st_size)) ? colLengths[3] : countDigits(s.st_size);
        }

        closedir(directory);
        return size;
    }
    else
        return -1;
}

// Function to list the contents of a directory
void ls(int numFlag, char *path)
{
    // Checking if path points to a file
    _Bool fileFlag = 0;
    char fileName[MAX_FILE_LENGTH + 1];
    struct stat f;
    stat(path, &f);
    if (S_ISREG(f.st_mode))
        fileFlag = 1;

    DIR *directory;
    int total;
    struct dirent **entry;
    if (fileFlag)
        splitPath(path, fileName);
    total = scandir(path, &entry, NULL, alphasort);

    if (total >= 0)
    {
        // If -a was provided
        if (numFlag == 1)
            for (int i = 0; i < total; i++)
            {
                // If we are searching only for a particular file
                if (fileFlag && strcmp(entry[i]->d_name, fileName) != 0)
                {
                    if (i + 1 == total)
                        perror("Non-existent or hidden file");
                    continue;
                }

                printf("%s\n", entry[i]->d_name);

                if (fileFlag)
                    break;
            }

        // If no -a was provided
        else if (numFlag == 0)
            for (int i = 0; i < total; i++)
            {
                // If we are searching only for a particular file
                if (fileFlag && strcmp(entry[i]->d_name, fileName) != 0)
                {
                    if (i + 1 == total)
                        perror("Non-existent or hidden file");
                    continue;
                }

                if (entry[i]->d_name[0] != '.')
                    printf("%s\n", entry[i]->d_name);

                if (fileFlag)
                    break;
            }
    }
    else
    {
        perror("Invalid path specified");
        return;
    }
}

// Function to list the contents of a directory with permissions and other information
void lsl(int numFlag, char *path)
{
    // Checking if path points to a file
    _Bool fileFlag = 0;
    char fileName[MAX_FILE_LENGTH + 1];
    struct stat f;
    stat(path, &f);
    if (S_ISREG(f.st_mode))
        fileFlag = 1;

    DIR *directory;
    int total;
    struct dirent **entry;
    if (fileFlag)
        splitPath(path, fileName);
    total = scandir(path, &entry, NULL, alphasort);

    if (total >= 0)
    {
        // Computing the total size and column lengths
        int colLengths[4] = {};
        int tSize = totalSize(path, numFlag, colLengths);
        if (tSize == -1)
        {
            perror("Invalid path specified");
            return;
        }

        if (!fileFlag)
            printf("total %d\n", tSize);

        // Looping through the contents of the directory
        for (int i = 0; i < total; i++)
        {
            // If we are searching only for a particular file
            if (fileFlag && strcmp(entry[i]->d_name, fileName) != 0)
            {
                if (i + 1 == total)
                    perror("Non-existent or hidden file");
                continue;
            }

            // If it is a hidden file/directory, i.e. when no -a was provided
            if (numFlag == 0 && entry[i]->d_name[0] == '.')
                continue;

            struct stat s;
            char fullPath[MAX_PATH_LENGTH + 1];

            // Obtaining the path of the file/directory
            sprintf(fullPath, "%s/%s", path, entry[i]->d_name);

            stat(fullPath, &s);

            // Obtaining last modification time of file/directory
            struct tm *modTime = localtime(&(s.st_mtime));
            char timeString[30];

            // Checking if file was modified more than 6 months ago
            if (moreThanSixMonths(s.st_mtime))
                strftime(timeString, 24, "%b %d %Y", modTime);
            else
                strftime(timeString, 24, "%b %d %H:%M", modTime);

            // String to store the permissions
            char permissionString[11];
            strcpy(permissionString, "");

            // Storing file permissions in the permissions string
            strcat(permissionString, (S_ISDIR(s.st_mode)) ? "d" : "-");
            strcat(permissionString, (s.st_mode & S_IRUSR) ? "r" : "-");
            strcat(permissionString, (s.st_mode & S_IWUSR) ? "w" : "-");
            strcat(permissionString, (s.st_mode & S_IXUSR) ? "x" : "-");
            strcat(permissionString, (s.st_mode & S_IRGRP) ? "r" : "-");
            strcat(permissionString, (s.st_mode & S_IWGRP) ? "w" : "-");
            strcat(permissionString, (s.st_mode & S_IXGRP) ? "x" : "-");
            strcat(permissionString, (s.st_mode & S_IROTH) ? "r" : "-");
            strcat(permissionString, (s.st_mode & S_IWOTH) ? "w" : "-");
            strcat(permissionString, (s.st_mode & S_IXOTH) ? "x" : "-");

            printf("%s %*ld %*s %*s %*ld %-12s %s\n", permissionString, colLengths[0], s.st_nlink, colLengths[1], getpwuid(s.st_uid)->pw_name, colLengths[2], getgrgid(s.st_gid)->gr_name, colLengths[3], s.st_size, timeString, entry[i]->d_name);

            if (fileFlag)
                break;
        }
    }
    else
    {
        perror("Invalid path specified");
        return;
    }
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

    // Resetting optind to 0 for getopt to work
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
            perror("Invalid flag");
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
            lsl(combination - 2, ".");
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