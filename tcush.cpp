//*********************************************************
//
// James Stewart and Will Taylor
// Operating Systems
// Project #2: Writing Your Own Shell: BlueShell
//
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <ctime>
#include <string>
#include <cerrno>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

//*********************************************************
//
// Function Declarations
//
//*********************************************************

void displayPrompt();

void exitBlueShell();
int lengthOfTokenArray();
void fixTokArray();

// Signal Handlers
void alarmHandler(int s);
void reapZombieChild(int s);

// History feature functions
void recordCommand(char* toks[]);
void printQueue(queue <string> queueToPrint);
void historyCommand(char* toks[], queue <string> commandsList);

// Internal Command Execution Functions
bool commandIsInternal(string command);
void executeInternalCommand(char* toks[]);
void setAlarm(char* toks[]);

// External Command Execution Functions
void executeExternalCommand(char* toks[]);

// Other Internal Command Functions
void displayHelp();
void cd(char* toks[]);

//*********************************************************
//
// Extern Declarations
//
//*********************************************************
using namespace std;
//extern "C"
//{
  extern char **gettoks();
  extern const int NUMBER_OF_INTERNAL_COMMANDS = 7;
  string INTERNAL_COMMANDS [NUMBER_OF_INTERNAL_COMMANDS] = {"history", "!", "help", "quit", "exit", "cd", "alarm"};
//}

//*********************************************************
//
// Global Variables
//
//*********************************************************

queue <string> history;     // Queue to hold the command history in
char **toks;
char **command1;
char **command2;

// Welcome message
string blue_shell_ascii_art = "                                              `.\n                                          `.-/o:\n                                       ``..-:+sy`\n                                    ``..--:+oys-.\n                                 ``...-://+os++oo     ----- Blue Shell -----\n    `.           :            ``...-:::::/+o+oy/`     |                    |\n    `oo.        /o/        ``...-:::--:/oooooo+.      |By Will Taylor      |\n     +ss-`     //:+:      `..-:::--:/+oo++++oss`      |and                 |\n     :o+os:/osyhyyyhyo:` `.-://::///++//+osys:`       |James Stewart       |\n     :+/.-/shhddhhhdddhy..--//::////+oooooooo         |                    |\n  ``./so:``:yhdhhhyo:.`-.-:://://+++ooooosy+`         |--------------------|\n  `+oymhs/+hhdmdhdy-  .:-::://////++osyys:`\n    ommddddmmmhhdddy+:/o:::::///+++osysy`\n    ydhyyyyhdddhhmmmmmmdo//////++ooooos:\n `/+/:::-....--:/+osssyyhyooooooooss+:`\n soooosyss+/::----::::://///+++oyhd.\n +hyhdNMMMN/:+hdhhyysooooossssysso-                   Look out, first place.\n  -/symMMm+.-+NMMMMmh++ssyhdmm/\n    `+so+/++//odmmho//oosyhdms\n       omhyso++oo//:/ossyhdm+\n        -ydhhyyso+oosyyyhho.                         Type \"help\" for a list\n          ./syhddddhhys+:`                      of available internal commands.\n               ````` ";

//*********************************************************
//
// Main Function
//
//*********************************************************
int main( int argc, char *argv[] ){

  // local variables
  int ii;
  int retval;

  // initialize local variables
  ii = 0;
  toks = NULL;
  retval = 0;

  // Register signal handlers
  signal(SIGCHLD, reapZombieChild);
  signal(SIGALRM, alarmHandler);

  // Clear terminal screen and print out welcome
  if (system("CLS")) system("clear");
  cout << blue_shell_ascii_art << endl;

  while( true ){
      // get arguments
      displayPrompt();
      toks = gettoks();

      if( toks[0] != NULL ){

    if(commandIsInternal(toks[0])){
      executeInternalCommand(toks);
    } else {
      executeExternalCommand(toks);
    }

	   if( !strcmp( toks[0], "quit" ))
	    break;
	   }

     recordCommand(toks);

    }

  // return to calling environment
  return( retval );
}

bool commandIsInternal(string command){

  // Function takes in tok[0] of the tokens and if
  // the command is an internal shell command, returns
  // true, else returns false.

  int ii = 0;

  // If the command exists in the array of internal commands, return true.
  // bool exists =  std::find(INTERNAL_COMMANDS.begin(), INTERNAL_COMMANDS.end(), command) != INTERNAL_COMMANDS.end();
  // cout << exists
  // return exists

  for(ii; ii < NUMBER_OF_INTERNAL_COMMANDS; ii++){

    string this_command = INTERNAL_COMMANDS[ii];
    if( !strcmp(this_command.c_str(), command.c_str() ) ){
     return true;
    }

  }
  return false;
}

void executeInternalCommand(char* toks[]){

  string command = toks[0];

  if(command.compare("history") == 0){
    printQueue(history);
  } else if(command.compare("help") == 0){
    displayHelp();
  } else if( (command.compare("quit") == 0)  || (command.compare("exit") == 0) ){
    exitBlueShell();
  } else if(command.compare("cd") == 0){
    cd(toks);
  } else if (command.compare("!") == 0) {
    historyCommand(toks, history);
  } else if (command.compare("alarm") == 0) {
    setAlarm(toks);
  }

}

void executeExternalCommand(char* toks[]){
  pid_t pid, child_pid;
  int child_status;

  // Check to see if we need to let the child run in the background or not
  int length = lengthOfTokenArray();
  string lastTok = toks[length - 1];
  bool programShouldRunInBackground = (lastTok.compare("&") == 0);
  if(programShouldRunInBackground){
    // Remove the & at the end of the tok array
    toks[length - 1] = NULL;
  }

  // Determine if we need to do any piping.
  bool needPiping = false;
  int pipingCharAt = -1;
  for(int ii=0; toks[ii] != NULL; ii++ ){
      string command = toks[ii];

      if(command.compare("|") == 0){
        needPiping = true;
        pipingCharAt = ii;
      }
  }

  int pipefd[2];
  if(needPiping){
    // Make a pipe.
    // file descripters go in pipefd[0] & pipefd[1]
    pipe(pipefd);

  }

  pid = fork();

  if(pid == 0){
    // We are the child.

    if(programShouldRunInBackground){
      setpgid(0, 0);
    }

    // Pipe handling
    if(needPiping){
      // Child handles second part of the command
      dup2(pipefd[0], 0);  // Replace stdin with input side of pipe
      close(pipefd[1]);    // Close output side of pipe
    }

    // I/O Redirection
    // Check for I/O redirection

    bool changedIO = false;


    int ii;
    for( ii=0; toks[ii] != NULL; ii++ ){
        string command = toks[ii];

        if(command.compare(">") == 0){
          // Redirect output
          int out = open(toks[ii + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

          dup2(out, 1);     // Stdout
          close(out);

          changedIO = true;

        }

        if(command.compare("<") == 0){
          // Redirect input
          int in = open(toks[ii + 1], O_RDONLY);

          dup2(in, 0);     // Stdin
          close(in);

          changedIO = true;
        }
    }

    if(changedIO){
      fixTokArray();
    }
    // Exec and run the program specificied by user
    // Exec documentation at:
    // http://linux.die.net/man/3/execvp

    int exec = execvp(toks[0], toks);

    if(exec < 0) perror("ERROR: Execution of external command has failed. Check for typos.");

  } else {
    // We are the parent.

    if(needPiping){
      // Parent handles first part of command

      // Replace stdout with output on the pipe
      dup2(pipefd[1], 1);

      // Close input part of pipe
      close(pipefd[0]);
    }

    if(!programShouldRunInBackground){
      // Run in foreground
      // Wait on the child process to terminate
      do {
          child_pid = waitpid(child_pid, &child_status, WUNTRACED);
      } while (!WIFEXITED(child_status) && !WIFSIGNALED(child_status));
    } else {
      cout << "Child PID: " << pid << "\n";
    }

  }
}

void recordCommand(char* toks[]){
  // This function pushes commands onto the history queue.

  // Local variables
  int ii;
  string command = "";

  // Combine all tokens into one string to push into history queue
  for(ii = 0; toks[ii] != NULL; ii++){
      command = command + " " + toks[ii];

      //if the command is "!", properly concatenate string
      if (command.compare(" !") == 0) {
        command = command + toks[ii+1];
        break;
      }
  }

  // Push the command string into the front of the queue
  history.push(command);

  // If the size of the queue is over 10, pop the last command off
  if(history.size() > 10) history.pop();

}

void printQueue(queue <string> queueToPrint){
  int size = queueToPrint.size();
  int ii;

  for(ii = size; ii != 0; ii--){
      string item = queueToPrint.front();
      queueToPrint.pop();
      cout << ii << item << endl;
      queueToPrint.push(item);
  }
}

//This function executes a command from the history queue
void historyCommand(char* toks[], queue <string> commandsList){
  int size = commandsList.size();
  string argument = toks[1];

//execute a command from history and throw an error if the command cannot be found
  if (argument.compare("!") == 0) {
    //execute most recent command
    //error should read, "No commands in history"
  } else {
    //execute the Nth command
    //error should read, "No such command in history"
  }
}


void displayHelp(){
  string help = "BlueShell by Will Taylor & James Stewart\nThese commands are internal to the shell.\n\nhelp - Displays a list of internal commands with their descriptions\nexit - Terminates the BlueShell application\nquit - Terminates the BlueShell application\ncd DIRECTORY - Switches the current working directory to DIRECTORY\nhistory - Displays a list of the last 10 executed user commands\n!! - Executes the most recent command in the history\n!N - Where N is a positive integer, the Nth command in the history queue is executed\nalarm N - Where in is a positive integer, it sets an alarm that will go off in N seconds. Only one alarm can be set at once. If N=0, then any previously set alarm will be canceled.\n";
  cout << help;
}

// This function displays the prompt to the user.
void displayPrompt(){
  time_t t = time(0);   // get time
  struct tm * now = localtime( & t );
  cout << (now->tm_mon + 1) << '/' <<  now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << "$ ";
  std::cout.flush();
}

void cd(char* toks[]){
  // Wrapper of chdir function to implement the cd command

  int status = 1024;

  // Get current working directory
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  // Add '/' to the cwd
  strcat(cwd, "/");
  const char* directory = toks[1];

  // Add new directory
  strcat(cwd, directory);

  // Change directory
  status = chdir(cwd);

  if(status != 0) perror("Directory change failed.\n");
}

int lengthOfTokenArray(){

  int ii;
  int answer = 0;
  for( ii=0; toks[ii] != NULL; ii++ )
    {
      answer = answer + 1;
    }

  return answer;
}

void reapZombieChild(int s){
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

void alarmHandler(int s){
  cout << "BEEP BEEP! Your alarm is going off.\n";
  displayPrompt();
}

void setAlarm(char* toks[]){
  int seconds = atoi(toks[1]);

  if(seconds < 0){
    perror("Error: Number of seconds must be greater than or equal to 0.\n");
    return;
  }

  int status = alarm(seconds);

  if(seconds == 0){
    cout << "Canceled any previously set alarms.\n";
    return;
  }

  if(status != 0){
    // Check to make sure there isn't already an alarm set
    cout << "Error: Failed to set alarm due to another scheduled alarm. To cancel the currently alarm that will go off in " << status << " seconds, please use the command $alarm 0.\n";
  } else {
    cout << "Alarm set to go off in " << seconds << ".\n";
  }

}

void fixTokArray(){

  // Any time we find < or > in the token array, delete it and
  // the proceeding token, then shift the rest of the array to
  // the left 2 to fill in the gaps

  bool solved = false;

  while(!solved){
    int ii;

    for( ii=0; toks[ii] != NULL; ii++ ){

        if( !strcmp(toks[ii], ">") || !strcmp(toks[ii], "<")){
          int length = lengthOfTokenArray();
          std::copy( (toks + ii + 2), (toks + length), (toks + ii) );

          toks[length - 1] = NULL;
          toks[length - 2] = NULL;
        }

    }

    bool foundOne = false;
    for( ii=0; toks[ii] != NULL; ii++ ){
        if( !strcmp(toks[ii], ">") || !strcmp(toks[ii], "<")){
          foundOne = true;
        }
    }

    if(foundOne){
      solved = false;
    } else {
      solved = true;
    }
  }
}

void exitBlueShell(){
  exit(0);
}
