#ifndef COMMANDS_H
#define COMMANDS_H

// Function to modify the path
void modifyPath(char *path, char *subpath, char *home);

// Implementation of cd
void changeDirectory(char *path, char *subpath, char *home, char *prevPath);

// Implementation of pwd
void printWorkingDirectory(char *path, char *homePath);

// Implementation of echo
void echo(char *args[], int argc);

#endif