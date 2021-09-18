#include "header_files/utilities.h"
#include "header_files/util_variables.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

// Important global strings
char HOSTNAME[HOST_NAME_MAX + 1];
char USERNAME[LOGIN_NAME_MAX + 1];
char INPUTSTRING[MAX_COMMAND_LENGTH + 1];
char COMMANDSTRING[MAX_COMMAND_LENGTH + 1];
char PATH[MAX_PATH_LENGTH + 1];
char PREVIOUSPATH[MAX_PATH_LENGTH + 1];
char HOME[MAX_PATH_LENGTH + 1];
Process PROCESSLIST[MAX_CHILD_NO];
int CHILDNO = 0;
char HISTORY[20][MAX_COMMAND_LENGTH + 1];
int HISTORYNO = 0;
int REAR = -1, FRONT = -1;
struct termios ORIG_TERMIOS;

int main(void)
{
    // Registering the signal handler for background processes
    signal(SIGCHLD, signalHandler);

    // The array containing the command-line arguments entered
    char *args[MAX_ARG_NO + 1];

    // Obtaning username, hostname and path of current working directory (the home directory for this shell)
    getlogin_r(USERNAME, LOGIN_NAME_MAX + 1);
    gethostname(HOSTNAME, HOST_NAME_MAX + 1);
    getcwd(PREVIOUSPATH, MAX_PATH_LENGTH + 1);
    getcwd(HOME, MAX_PATH_LENGTH + 1);

    // Initializing the history
    initHistory();

    // Printing intro
    printf("\033[0;34m\nWELCOME TO ASH!\nYour home path is %s\n\n\033[0m", HOME);

    // Core of the program - the infinite loop accepting commands as input
    while (1)
        tokenizeAndExec(args);

    return 0;
}