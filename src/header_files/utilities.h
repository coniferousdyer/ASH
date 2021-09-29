#ifndef INPUT_H
#define INPUT_H

// Function to check if a string can be converted to a number (before using atoi)
_Bool isInteger(char *str);

// Function to insert a process into the process array
void InsertProcess(int pid, char *name);

// Function to delete a process with given pid from the process array and returns the process name
void DeleteProcess(int pid, char nameString[]);

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString);

// Function to execute the corresponding command
void execCommand(char *args[], int argc);

// Function to display a prompt to the user
void displayPrompt();

// Function to take input and store it in INPUTSTRING
void takeInput();

// Function to tokenize the input, account for backslashes and finally execute the command by calling execCommand
void tokenizeAndExec(char *args[]);

#endif