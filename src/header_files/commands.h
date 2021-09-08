#ifndef COMMANDS_H
#define COMMANDS_H

// Implementation of cd
void changeDirectory(char *path, char *subpath);

// Implementation of pwd
void printWorkingDirectory(char *path, char *homePath);

// Implementation of echo
void echo(char *args[]);

#endif