//*********************************************************
//
// James Stewart and Will Taylor
// Operating Systems
// Project #2: Writing Your Own Shell: BlueShell
//
//*********************************************************

Time spent: Maybe 10 hours each

Three internal commands implemented for grade:

$help - Displays a list of internal commands with their descriptions

$cd DIRECTORY - Switches the current working directory to DIRECTORY. We used
                  the chdir() function to accomplish this.

$alarm N - Where in is a positive integer, it sets an alarm that will go off
            in N seconds. Only one alarm can be set at once.
            If N=0, then any previously set alarm will be canceled.
            The feature uses the alarm() system call, and then the function
            alarmHandler() receives the signal sent from the kernel and
            displays the alarm message.

Other internal commands:

$exit & $quit - Terminates the BlueShell application

$history - Displays a list of the last 10 executed user commands

$!! - Executes the most recent command in the history

$!N - Where N is a positive integer, the Nth command in the history queue is
        executed

Difficulties:
    - James had a lot of difficulty in implementing the history features due

    - While working on the project, James and I both learned how to use C++ and
    GitHub. While they both took a good bit of reading to get used to, they
    made things much easier in the long run. We found C++ to be more useful
    than C due to classes and the ability to use previously defined structures
    like the queue class. Once we both got used to it, GitHub was an excellent
    tool in managing the code between the two of us and prevented many headaches
    that would have been caused by trying to manage different versions of code
    on Dropbox or Google Drive. 

Problems: Piping was never fully implemented.
