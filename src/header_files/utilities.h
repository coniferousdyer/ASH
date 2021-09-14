#ifndef INPUT_H
#define INPUT_H

// Function to insert a process into the process array
void InsertProcess(int pid, char *name);

// Function to delete a process with given pid from the process array and returns the process name
void DeleteProcess(int pid, char nameString[]);

// Function to read previous session commands from external file
void initHistory();

// Function to save current session's history to external file
void saveHistory();

// The signal handler which reaps background processes
void signalHandler(int signal);

// Function to obtain input from user and format it to give us the command
void parseInput(char *inputString, char *parsedString);

// Function to execute the corresponding command
void execCommand(char *args[], int argc, _Bool flag);

// Function to display a prompt to the user
void displayPrompt();

// Function to tokenize the input, account for backslashes and finally execute the command by calling execCommand
void tokenizeAndExec(char *args[]);

#endif