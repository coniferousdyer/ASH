#include "../header_files/utilities.h"
#include "../header_files/util_variables.h"
#include "../header_files/commands.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

// Function to execute the corresponding command
void execCommand(char *args[], int argc)
{
    // I/O redirection handling
    int read = 0, write = 0;
    int iFilePos = -1, oFilePos = -1, iOpPos = -1, oOpPos = -1;

    // Checking if redirecting operators are present
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(args[i], ">") == 0)
        {
            write = 1;
            oOpPos = i;
            oFilePos = i + 1;

            // Finding the last file name after >
            for (; oFilePos < argc; oFilePos++)
                if (strcmp(args[oFilePos], ">") == 0 || strcmp(args[oFilePos], ">>") == 0 || strcmp(args[oFilePos], "<") == 0)
                    break;

            oFilePos--;
        }
        else if (strcmp(args[i], "<") == 0)
        {
            read = 1;
            iOpPos = i;
            iFilePos = i + 1;

            // Finding the last file name after <
            for (; iFilePos < argc; iFilePos++)
                if (strcmp(args[iFilePos], ">") == 0 || strcmp(args[iFilePos], ">>") == 0 || strcmp(args[iFilePos], "<") == 0)
                    break;

            iFilePos--;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            write = 2;
            oOpPos = i;
            oFilePos = i + 1;

            // Finding the last file name after >>
            for (; oFilePos < argc; oFilePos++)
                if (strcmp(args[oFilePos], ">") == 0 || strcmp(args[oFilePos], ">>") == 0 || strcmp(args[oFilePos], "<") == 0)
                    break;

            oFilePos--;
        }
    }

    // If no redirecting operators are present
    if (read + write == 0)
        goto skip;

    int origSTDIN, origSTDOUT, iFD, oFD;

    // If input redirector is present
    if (read > 0)
    {
        // Opening a read-only file
        origSTDIN = dup(STDIN_FILENO);
        iFD = open(args[iFilePos], O_RDONLY);

        if (iFD < 0)
        {
            perror("Redirection failed");
            return;
        }

        // Saving the stdin file descriptor
        if (dup2(iFD, STDIN_FILENO) < 0)
        {
            perror("Redirection failed");
            return;
        }
    }

    // If output redirector is present
    if (write > 0)
    {
        origSTDOUT = dup(STDOUT_FILENO);

        // Opening after creating a new file if needed
        if (write == 2)
            oFD = open(args[oFilePos], O_CREAT | O_WRONLY | O_APPEND, 0644);
        else
            oFD = open(args[oFilePos], O_CREAT | O_WRONLY | O_TRUNC, 0644);

        if (oFD < 0)
        {
            perror("Redirection failed");
            return;
        }

        // Saving the stdout file descriptor
        if (dup2(oFD, STDOUT_FILENO) < 0)
        {
            perror("Redirection failed");
            return;
        }
    }

    // If input redirector is present
    if (read > 0)
    {
        args[iOpPos] = NULL;
        if (argc > iOpPos)
            argc = iOpPos;
    }

    // If output redirector is present
    if (write > 0)
    {
        args[oOpPos] = NULL;
        if (argc > oOpPos)
            argc = oOpPos;
    }
skip:;

    /*-----------BUILTIN COMMANDS-----------*/

    // If nothing was entered
    if (argc == 0)
        return;

    // Exiting the shell
    if (strcmp(args[0], "exit") == 0)
    {
        if (argc != 1)
        {
            perror("Too many arguments specified");
            goto close_files;
        }

        printf("Exited with status 0.\n");
        saveHistory();
        exit(0);
    }
    // Checking if cd was entered
    else if (strcmp(args[0], "cd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 2)
        {
            perror("Too many arguments specified");
            goto close_files;
        }

        // Accounting for cd only
        if (argc == 1)
        {
            char tempPath[MAX_PATH_LENGTH + 1];
            getcwd(tempPath, MAX_PATH_LENGTH + 1);
            strcpy(PREVIOUSPATH, tempPath);
            chdir(HOME);
            goto close_files;
        }

        changeDirectory(args[1]);
        goto close_files;
    }
    // Checking if pwd was entered
    else if (strcmp(args[0], "pwd") == 0)
    {
        // Checking for correct number of arguments
        if (argc > 1)
        {
            perror("Too many arguments specified");
            goto close_files;
        }

        printWorkingDirectory();
        goto close_files;
    }
    // Checking if echo was entered
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args, argc);
        goto close_files;
    }
    // Checking if pinfo was entered
    else if (strcmp(args[0], "pinfo") == 0)
    {
        if (argc == 1)
        {
            pid_t pid = getpid();
            pinfo(pid);
        }
        else if (argc == 2)
        {
            if (!isInteger(args[1]))
            {
                perror("Invalid argument specified");
                goto close_files;
            }

            pid_t pid = atoi(args[1]);
            pinfo(pid);
        }
        else
            perror("Too many arguments specified");

        goto close_files;
    }
    // Checking if repeat was entered
    else if (strcmp(args[0], "repeat") == 0)
    {
        if (argc <= 2)
        {
            perror("Too few arguments specified");
            goto close_files;
        }

        if (!isInteger(args[1]))
        {
            perror("Invalid argument specified");
            goto close_files;
        }

        // Storinging the number of times the command must be repeated
        int times = atoi(args[1]);

        // Creating a copy of args
        char *argsCopy[argc + 1];

        // Shifting the args array back by 2 to replace "repeat n"
        for (int i = 2; i <= argc; i++)
            argsCopy[i - 2] = args[i];
        argc -= 2;

        // Calling execCommand to execute the command the required number of times
        for (int i = 0; i < times; i++)
            execCommand(argsCopy, argc);

        goto close_files;
    }
    // Checking if ls was entered
    else if (strcmp(args[0], "ls") == 0)
    {
        lsHandler(args, argc);
        goto close_files;
    }
    // Checking if history was entered
    else if (strcmp(args[0], "history") == 0)
    {
        if (argc == 1)
        {
            if (HISTORYNO < 10)
                history(HISTORYNO);
            else
                history(10);
        }
        else if (argc == 2)
        {
            if (!isInteger(args[1]))
            {
                perror("Invalid argument specified");
                goto close_files;
            }

            int n = atoi(args[1]);

            if (n > 20)
                perror("Not more than 20 commands can be listed");
            else if (n < 0)
            {
                perror("Invalid argument specified");
                goto close_files;
            }

            if (HISTORYNO < n)
                history(HISTORYNO);
            else
                history(n);
        }
        else
            perror("Too few arguments specified");

        goto close_files;
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        jobsHandler(args, argc);
        goto close_files;
    }
    else if (strcmp(args[0], "sig") == 0)
    {
        if (argc > 3)
        {
            perror("Too many arguments specified");
            goto close_files;
        }
        else if (argc < 3)
        {
            perror("Too less arguments specified");
            goto close_files;
        }

        if (!isInteger(args[1]))
        {
            perror("Invalid argument specified");
            goto close_files;
        }

        if (!isInteger(args[2]))
        {
            perror("Invalid argument specified");
            goto close_files;
        }

        int jobNo = atoi(args[1]);
        int signal = atoi(args[2]);

        sig(jobNo, signal);

        goto close_files;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        if (argc > 2)
        {
            perror("Too many arguments specified");
            goto close_files;
        }
        else if (argc < 2)
        {
            perror("Too less arguments specified");
            goto close_files;
        }

        if (!isInteger(args[1]))
        {
            perror("Invalid argument specified");
            goto close_files;
        }

        int jobNo = atoi(args[1]);
        fg(jobNo);

        goto close_files;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        if (argc > 2)
        {
            perror("Too many arguments specified");
            goto close_files;
        }
        else if (argc < 2)
        {
            perror("Too less arguments specified");
            goto close_files;
        }

        if (!isInteger(args[1]))
        {
            perror("Invalid argument specified");
            goto close_files;
        }

        int jobNo = atoi(args[1]);
        bg(jobNo);

        goto close_files;
    }
    else if (strcmp(args[0], "replay") == 0)
    {
        if (argc == 1)
        {
            perror("Too less arguments specified");
            goto close_files;
        }

        replay(args, argc);

        goto close_files;
    }

    /*-----------SYSTEM COMMANDS-----------*/

    // Create a new process
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Could not create new process");
        goto close_files;
    }

    if (pid == 0)
    {
        // Checking if the last argument was &
        if (strcmp(args[argc - 1], "&") == 0)
        {
            // Setting the child process group ID to 0 to identify it as a background process
            setpgid(0, 0);
            args[argc - 1] = NULL;
            argc--;
        }

        // Executing the given command, replacing the address space of the child process
        if (execvp(args[0], args))
        {
            perror("Non-existent command");

            // To exit the new child process created
            exit(0);
        }
    }
    else
    {
        int STATUS;

        // Wait for the child process to terminate if not a background process
        if (strcmp(args[argc - 1], "&") != 0)
        {
            FGPID = pid;
            waitpid(pid, &STATUS, 0);
            FGPID = -2;
        }
        else
        {
            if (CHILDNO == MAX_CHILD_NO)
            {
                perror("Maximum number of background processes reached");
                fprintf(stderr, "The process is being executed as a foreground process.\n");
                waitpid(pid, &STATUS, 0);
                goto close_files;
            }

            printf("%d\n", pid);

            char path[MAX_FILE_LENGTH + 12];

            // Inserting the process into the process list
            InsertProcess(pid, args[0]);
        }
    }

close_files:;
    // Restoring the original file descriptors if needed
    if (read > 0)
    {
        dup2(origSTDIN, STDIN_FILENO);
        close(origSTDIN);
        close(iFD);
    }
    if (write > 0)
    {
        dup2(origSTDOUT, STDOUT_FILENO);
        close(origSTDOUT);
        close(oFD);
    }
}