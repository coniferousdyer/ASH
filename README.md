# ASH - The A-SHell

<b>ASH (or the A-SHell)</b> is a simple shell written in C, similar to bash. It supports several functionalities, including many bash commands.

## Commands

* `cd`: Changes the working directory of the shell.
```bash
cd
cd - # To go to the previous working directory
cd ~
cd ../..
cd dir_1/dir_2/dir_3
```

* `echo`: Prints the text following "echo" on the terminal.
```bash
echo "Hello World"
echo Welcome to ASH
```

* `history`: Displays a list of the commands previously used (at most the 20 latest commands).
```bash
history
history <n> # To display last n commands used
```

* `ls`: Lists the contents of a particular directory.
```bash
ls
ls -a
ls -l
ls -al
ls -la
ls -a -l
ls -l -a
ls ../../dir_1/dir_2
ls ~
ls dir_1/file_name
```

* `pinfo`: Displays information about a particular process.
```bash
pinfo # Displays information about the shell process itself
pinfo <pid>
```

* `pwd`: Prints the absolute path of the current working directory.
```bash
pwd
```

* `repeat`: Executes a given command n times.
```bash
repeat <n> <command>
```

* <b>System Commands: </b>ASH should run many of the processes that bash can, including `gedit`, `vim`, `clear`, etc.
```bash
gedit
vim
clear
ps
```

* <b>Background Processes: </b>Add `&` at the end of the command to run it as a background process. This however only works for system commands, and not shell built-ins.
```bash
gedit &
```

* <b>Arrow Keys: </b>Acts as a shortcut to easily view previous commands, and execute them, just like in bash.<br>
<kbd>UP</kbd> - View earlier commands<br>
<kbd>DOWN</kbd> - View more recent commands

## Setup

* Run the following to generate the executable from the makefile.
```bash
make
```
* To start the shell, run the following command.
```bash
./shell
```

* To exit the shell, run the following command within the shell.
```bash
exit
```

## Directory Structure

<pre>
<b>ASH</b>
|______README.md
|______makefile
|______src
       |______main.c
       |______utilities.c
       |______header_files
       |      |______commands.h
       |      |______util_variables.h
       |      |______utilities.h
       |
       |______commands
              |______arrow.c
              |______cd.c
              |______echo.c
              |______history.c
              |______ls.c
              |______pinfo.c
              |______pwd.c
              |______terminal.c
</pre>

## Code Division

* `main.c`: Contains the main() function where execution starts.
* `utilities.c`: Contains utility functions that are either used for input processing or performing general tasks. Also contains the implementation of the `repeat` command.

### Header Files

* `commands.h`: Contains the declarations of all the functions present in the `commands` folder.
* `util_variables.h`: Contains macros and important global variables used throughout the program.
* `utilities.h`: Contains the declarations of all the functions present in `utilities.c`.

### Commands

* `arrow.c`: Contains the implementation of the arrow key handling, which shows previous commands.

* `cd.c`: Contains the functions required to change the working directory.

* `echo.c`: Contains the implementation of the echo command.

* `history.c`: Contains the functions required to display the history of commands used.

* `ls.c`: Contains the functions required to list the contents of a directory.

* `pinfo.c`: Contains the functions required to display information about a particular process.

* `pwd.c`: Contains the implementation for the pwd command.

* `terminal.c`: Contains the functions required to modify and obtain terminal attributes.

## Assumptions

* 6 months is exactly equal to 15778463 seconds.
* By the process name supposed to be printed when a background process exits abnormally, we mean the name of the command used to run that background process.
