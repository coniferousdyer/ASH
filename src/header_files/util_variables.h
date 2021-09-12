#ifndef UTIL_VARIABLES_H
#define UTIL_VARIABLES_H

// Defining important macros
#define MAX_COMMAND_LENGTH (int)1e5
#define MAX_PATH_LENGTH 4096
#define MAX_FILE_LENGTH 255
#define HOST_NAME_MAX 64
#define LOGIN_NAME_MAX 256
#define MAX_ARG_NO (int)1e4

// Important global strings
extern char HOSTNAME[HOST_NAME_MAX + 1];
extern char USERNAME[LOGIN_NAME_MAX + 1];
extern char INPUTSTRING[MAX_COMMAND_LENGTH + 1];
extern char COMMANDSTRING[MAX_COMMAND_LENGTH + 1];
extern char PATH[MAX_PATH_LENGTH + 1];
extern char PREVIOUSPATH[MAX_PATH_LENGTH + 1];
extern char HOME[MAX_PATH_LENGTH + 1];

#endif