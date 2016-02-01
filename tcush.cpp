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

using namespace std;

//*********************************************************
//
// Command Object
//
//*********************************************************
class Command {
  private:
    string line;
    char **parts; //not sure how to declare this array

  public:
    Command();
    string getLine(void);
    char getParts(void);
    void setLine(string x);
    void setParts(char* x[]);
};

Command::Command () {
  //no code here necessary
}

string Command::getLine(void) {
  return line;
}
//not sure how to return this array
char Command::getParts(void) {
  return parts;
}
void Command::setLine(string x) {
  line = x;
}
void Command::setParts(char* x[]) {
  parts = x;
}

//*********************************************************
//
// Function Declarations
//
//*********************************************************

void displayPrompt();

void exitBlueShell();

// History feature functions
void recordCommand(char* toks[]);
void printQueue();
void historyCommand(char* toks[]);

// Internal Command Execution Functions
bool commandIsInternal(string command);
void executeInternalCommand(char* toks[]);

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
  extern const int NUMBER_OF_INTERNAL_COMMANDS = 6;
  string INTERNAL_COMMANDS [NUMBER_OF_INTERNAL_COMMANDS] = {"history", "!", "help", "quit", "exit", "cd"};
//}

//*********************************************************
//
// Global Variables
//
//*********************************************************

queue <Command> history;     // Queue to hold the command history in

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
  char **toks;
  int retval;

  // initialize local variables
  ii = 0;
  toks = NULL;
  retval = 0;

  // Clear terminal screen and print out welcome
  if (system("CLS")) system("clear");
  cout << blue_shell_ascii_art << endl;

  // main (infinite) loop
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

    	  if( !strcmp( toks[0], "quit" )) break;
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
    printQueue();
  } else if(command.compare("help") == 0){
    displayHelp();
  } else if( (command.compare("quit") == 0)  || (command.compare("exit") == 0) ){
    exitBlueShell();
  } else if(command.compare("cd") == 0){
    cd(toks);
  } else if (command.compare("!") == 0) {
    historyCommand(toks);
  }

}

void executeExternalCommand(char* toks[]){
  pid_t pid, child_pid;
  int child_status;

  pid = fork();

  if(pid == 0){
    // We are the child.

    // Exec and run the program specificied by user
    // Exec documentation at:
    // http://linux.die.net/man/3/execvp

    int exec = execvp(toks[0], toks);

    if(exec < 0) perror("ERROR: Execution of external command has failed. Check for typos.");

  } else {
    // We are the parent.

    // Wait on the child process to terminate
    do {
        child_pid = waitpid(child_pid, &child_status, WUNTRACED);
    } while (!WIFEXITED(child_status) && !WIFSIGNALED(child_status));
  }
}

void recordCommand(char* toks[]){
  // This function pushes commands onto the history queue.

  Command node;
  node.setParts(toks);

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
  node.setLine(command);
  // Push the command string into the front of the queue
  history.push(node);

  // If the size of the queue is over 10, pop the last command off
  if(history.size() > 10) history.pop();

}

void printQueue(){
  int size = history.size();
  int ii;

  for(ii = size; ii != 0; ii--){
      Command node = history.front();
      history.pop();
      cout << ii << node.getLine() << endl;
      history.push(node);
  }
}

//This function executes a command from the history queue
void historyCommand(char* toks[]){
  int size = history.size();
  string argument = toks[1];
  Command node;
  char **temp;

  //execute the most recent command
  if (argument.compare("!") == 0) {
    node = history.front();
    temp = node.getParts(); //how to get array from object??

    if(commandIsInternal(temp[0])){
      executeInternalCommand(temp);
    } else {
      executeExternalCommand(temp);
    }
  } else {
  //execute the Nth command
  int N = toks[1] - '0';

  //error should read, "No such command in history"
  }
}


void displayHelp(){
  string help = "BlueShell by Will Taylor & James Stewart\nThese commands are internal to the shell.\nFor help with external commands, type 'man X' where X is the command you wish to know more about.\n\nhelp - Displays a list of internal commands with their descriptions\nexit - Terminates the BlueShell application\nquit - Terminates the BlueShell application\ncd DIRECTORY - Switches the current working directory to DIRECTORY\nhistory - Displays a list of the last 10 executed user commands\n!! - Executes the most recent command in the history\n!N - Where N is a positive integer, the Nth command in the history queue is executed\n";
  cout << help;
}

// This function displays the prompt to the user.
void displayPrompt(){

  time_t t = time(0);   // get time
  struct tm * now = localtime( & t );
  cout << (now->tm_mon + 1) << '/' <<  now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << "$ ";

}

void cd(char* toks[]){
  // Wrapper of chdir function to implement the cd commandx

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

void exitBlueShell(){
  exit(0);
}
