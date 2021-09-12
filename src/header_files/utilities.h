#ifndef INPUT_H
#define INPUT_H

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString);

// Function to execute the corresponding command
void execCommand(char *args[], int argc);

// Function to display a prompt to the user
void displayPrompt();

// Function to tokenize the input, account for backslashes and finally execute the command by calling execCommand
void tokenizeAndExec(char *args[]);

#endif