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

#endif