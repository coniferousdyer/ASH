#include "../header_files/utilities.h"
#include "../header_files/commands.h"
#include "../header_files/util_variables.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// Function to disable raw mode
void bw_raw_mode_off()
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ORIG_TERMIOS) == -1)
        die("tcsetattr");
}

// Function to enable raw mode for the nightswatch command specifically
void bw_raw_mode_on()
{
    // Storing the current attributes of the terminal
    if (tcgetattr(STDIN_FILENO, &ORIG_TERMIOS) == -1)
        die("tcgetattr");

    // Registering bw_raw_mode_off to be called before exiting
    atexit(bw_raw_mode_off);
    struct termios raw = ORIG_TERMIOS;

    // Disabling ECHO and making input non-canonical
    raw.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1)
        die("tcsetattr");
}

// Function to print the most recently created PID
void bw_newborn(int interval)
{
    // Enabling raw mode and setting descriptor of stdin to non-blocking mode
    int block = fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    enableRawMode();

    int key = ' ';

    while (key != 'q' && key != 'Q')
    {
        FILE *fp = fopen("/proc/loadavg", "r");

        if (fp == NULL)
        {
            perror("Error in newborn");
            return;
        }

        for (int i = 0; i < interval; i++)
        {
            sleep(1);
            key = getchar();
            if (key == 'q' || key == 'Q')
                break;
        }

        // Reading the 5th value in the file
        int pid;
        fscanf(fp, "%*f %*f %*f %*s %d", &pid);
        printf("%d\n", pid);
        fclose(fp);
    }

    // Disabling raw mode and non-blocking mode
    disableRawMode();
    block &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, block);
}

// Function to print the number of times the CPUs have been interrupted by the keyboard controller (i8042 with IRQ 1)
void bw_interrupt(int interval)
{
    FILE *fp = fopen("/proc/interrupts", "r");

    if (fp == NULL)
    {
        perror("Error in interrupt");
        return;
    }

    // Enabling raw mode and setting descriptor of stdin to non-blocking mode
    int block = fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    enableRawMode();

    int key = ' ';
    char line[200];

    // Reading the heading
    fgets(line, sizeof(line), fp);
    line[strlen(line) - 1] = '\0';
    printf("%s\n", line + 12);
    fclose(fp);

    while (key != 'q' && key != 'Q')
    {
        FILE *fp = fopen("/proc/interrupts", "r");

        if (fp == NULL)
        {
            perror("Error in interrupt");
            return;
        }

        for (int i = 0; i < interval; i++)
        {
            sleep(1);
            key = getchar();
            if (key == 'q' || key == 'Q')
                break;
        }

        // Reading the 3rd line in the file
        for (int i = 0; i < 3; i++)
            fgets(line, sizeof(line), fp);

        line[95] = '\0';
        printf("%s\n", line + 12);
        fclose(fp);
    }

    // Disabling raw mode and non-blocking mode
    disableRawMode();
    block &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, block);
}

// Function to print the size of dirty memory
void bw_dirty(int interval)
{
    char line[100], unit[5];
    int memory;

    // Enabling raw mode and setting descriptor of stdin to non-blocking mode
    int block = fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    enableRawMode();

    int key = ' ';

    while (key != 'q' && key != 'Q')
    {
        FILE *fp = fopen("/proc/meminfo", "r");

        if (fp == NULL)
        {
            perror("Error in dirty");
            return;
        }

        for (int i = 0; i < interval; i++)
        {
            sleep(1);
            key = getchar();
            if (key == 'q' || key == 'Q')
                break;
        }

        // Reading each line
        for (int i = 0; i < 17; i++)
            fscanf(fp, "%*s %d %s", &memory, unit);

        printf("%d %s\n", memory, unit);
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
    }

    // Disabling raw mode and non-blocking mode
    disableRawMode();
    block &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, block);
}

// Implementation of the baywatch command
void baywatch(char *args[], int argc)
{
    int intervalPos = -1;
    _Bool interrupt = 0, newborn = 0, dirty = 0;

    // Searching for the positions of the arguments
    for (int i = 1; i < argc; i++)
        if (strcmp(args[i], "-n") == 0)
            intervalPos = i + 1;
        else if (strcmp(args[i], "interrupt") == 0)
            interrupt = 1;
        else if (strcmp(args[i], "newborn") == 0)
            newborn = 1;
        else if (strcmp(args[i], "dirty") == 0)
            dirty = 1;

    // If interval was not provided
    if (intervalPos == -1 || intervalPos == argc)
    {
        perror("Interval not entered");
        return;
    }

    // If none of the three commands allowed was entered
    if (!(interrupt || newborn || dirty))
    {
        perror("Interrupt/newborn/dirty command not entered");
        return;
    }

    // If interval was not a whole number
    if (!isInteger(args[intervalPos]))
    {
        perror("Invalid interval entered");
        return;
    }

    // Storing interval
    int interval = atoi(args[intervalPos]);

    if (interrupt)
        bw_interrupt(interval);
    else if (newborn)
        bw_newborn(interval);
    else if (dirty)
        bw_dirty(interval);
}