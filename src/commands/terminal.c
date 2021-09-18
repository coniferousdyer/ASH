#include "../header_files/util_variables.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Function that displays an error message and exits
void die(const char *s)
{
    perror(s);
    exit(1);
}

// Function that restores the original terminal attributes
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIG_TERMIOS) == -1)
        die("tcsetattr");
}

// Function that enables raw mode for the terminal
void enableRawMode()
{
    // Storing the current attributes of the terminal
    if (tcgetattr(STDIN_FILENO, &ORIG_TERMIOS) == -1)
        die("tcgetattr");
    
    // Registering disableRawMode to be called before exiting
    atexit(disableRawMode);

    struct termios raw = ORIG_TERMIOS;

    // Disabling ECHO and making input non-canonical
    raw.c_lflag &= ~(ICANON | ECHO);

    // TCSAFLUSH waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}