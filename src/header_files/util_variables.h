#ifndef UTIL_VARIABLES_H
#define UTIL_VARIABLES_H

// Defining a struct containing process information
typedef struct Process
{
    int jobNo;
    int pid;
    char *pName;
} Process;

// Defining important macros
#define MAX_COMMAND_LENGTH (int)1e5
#define MAX_PATH_LENGTH 4096
#define MAX_FILE_LENGTH 255
#define HOST_NAME_MAX 64
#define LOGIN_NAME_MAX 256
#define MAX_ARG_NO (int)1e4
#define MAX_CHILD_NO 512

// Important global variables
extern int SHELLPID;
extern char HOSTNAME[HOST_NAME_MAX + 1];
extern char USERNAME[LOGIN_NAME_MAX + 1];
extern char INPUTSTRING[MAX_COMMAND_LENGTH + 1];
extern char COMMANDSTRING[MAX_COMMAND_LENGTH + 1];
extern char PATH[MAX_PATH_LENGTH + 1];
extern char PREVIOUSPATH[MAX_PATH_LENGTH + 1];
extern char HOME[MAX_PATH_LENGTH + 1];
extern Process PROCESSLIST[MAX_CHILD_NO];
extern int CHILDNO;
extern char HISTORY[20][MAX_COMMAND_LENGTH + 1];
extern int HISTORYNO;
extern int REAR, FRONT;
extern struct termios ORIG_TERMIOS;
extern int FGPID;

#endif