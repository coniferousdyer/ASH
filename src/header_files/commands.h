#ifndef COMMANDS_H
#define COMMANDS_H

// Implementation of cd
void changeDirectory(char *subpath);

// Implementation of pwd
void printWorkingDirectory();

// Implementation of echo
void echo(char *args[], int argc);

// Implementation of pinfo
void pinfo(int pid);

// Function to list the contents of a directory
void ls(int numFlag, char *path);

// Function to list the contents of a directory with permissions and other information
void lsl(int numFlag, char *path);

// The control function for ls
void lsHandler(char *args[], int argc);

#endif