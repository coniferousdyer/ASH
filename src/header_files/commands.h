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

// Function to count the digits of a number
int countDigits(long int n);

// Function to check if a file/directory was last modified 6 or more months ago
_Bool moreThanSixMonths(long modTime);

// Function to split the path into directory path and file name
void splitPath(char *path, char *fileName);

// Function to compute size and column lengths
void totalSize(char *path, int numFlag, int colLengths[]);

// Function to list the contents of a directory
void ls(int numFlag, char *path);

// Function to list the contents of a directory with permissions and other information
void lsl(int numFlag, char *path);

// The control function for ls
void lsHandler(char *args[], int argc);

// Function to add command to history
void addToHistory(char *command);

// Function to delete earliest command from history
void deleteFromHistory();

// Function to print the history of commands
void history(int n);

// Function that displays an error message and exits
void die(const char *s);

// Function that restores the original terminal attributes
void disableRawMode();

// Function that enables raw mode for the terminal
void enableRawMode();

// Function to recall the previous/next command from history on pressing UP/DOWN arrow key
int recallCommand(int n, int len);

// The signal handler which reaps background processes
void sigchldHandler(int sig);

// The signal handler which interrupts the foreground process
void sigintHandler(int sig);

// The signal handler which shifts the foreground process to background
void sigtstpHandler(int sig);

// Implementation of the sig command
void sig(int jobNo, int sig);

// Sets up the required signal handler functions
void installSignals();

// Function to read previous session commands from external file
void initHistory();

// Function to save current session's history to external file
void saveHistory();

// Displays the current background processes
void jobs(int param);

// Handles the input to jobs() and controls the output displayed by jobs()
void jobsHandler(char *args[], int argc);

// Function to bring a background process to the foreground
void fg(int jobNo);

// Function to bring a foreground process to the background
void bg(int jobNo);

// Implementation of the replay command
void replay(char *args[], int argc);

// Function to print the most recently created PID
void bw_newborn(int interval);

// Function to print the number of times the CPUs have been interrupted by the keyboard controller (i8042 with IRQ 1)
void bw_interrupt(int interval);

// Function to print the size of dirty memory
void bw_dirty(int interval);

// Implementation of the baywatch command
void baywatch(char *args[], int argc);

// Function to execute the corresponding command
void execCommand(char *args[], int argc);

#endif