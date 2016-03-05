# BlueShell
A simple Unix shell written in C++.

Written by [Will Taylor](https://github.com/fire-at-will) and [James Stewart](https://github.com/stewratking) for our Operating Systems course at Texas Christian University.


Features
-----------------
* Internal commands:
  * `help` Displays a list of internal commands with their descriptions
  * `cd DIRECTORY` Switches the current working directory to DIRECTORY.
  * `history` Displays a list of the last 10 executed user commands
  * `!!` Executes the most recent command in the history
  * `!N` If N is a positive integer, the Nth command in the history queue is executed
  * `alarm N` Where N is a positive integer, an alarm is set that will go off in N seconds. Only one alarm may be set at one time. If N=0, then any previously set alarm will be canceled.
  * `exit` or `quit` Terminates the BlueShell application
* Processes are initiated by forking a child process using the `fork` syscall.
* Backgrounding of processes is allowed by placing `&` after the command.
* I/O redirection: I/O redirection is implemented with the `dup2` syscalls (`cmd0 >> cmd1`) or (`cmd0 << cmd1`).
* Piping: single piping is allowed by using (`cmd0 | cmd1`). Implemented with `dup2` and `pipe` syscalls.
