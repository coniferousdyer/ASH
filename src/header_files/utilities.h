#ifndef INPUT_H
#define INPUT_H

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString);

// Function to modify the path
void modifyPath(char *path, char *subpath);

// Function to execute the corresponding command
void execCommand(char *args[], int argc);

#endif