#ifndef INPUT_H
#define INPUT_H

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString);

// Function to execute the corresponding command
void execCommand(char *args[], int argc, char *path, char *home, char *prevPath);

#endif