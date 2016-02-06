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
// Command Object
//
//*********************************************************
class Command {
  private:
    string line;
    char** parts = (char**) malloc(128 * sizeof(char*));

  public:
    Command();
    string getLine(void);
    char** getParts(void);
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
char** Command::getParts(void) {
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
int lengthOfTokenArray(char* toks[]);
char* fixTokArray(char* toks[]);

// Signal Handlers
void alarmHandler(int s);
void reapZombieChild(int s);

// History feature functions
void recordCommand(char* toks[]);
void printQueue();
void historyCommand(char* toks[]);

// Internal Command Execution Functions
bool commandIsInternal(string command);
void executeInternalCommand(char* toks[]);
void setAlarm(char* toks[]);

// External Command Execution Functions
void executeExternalCommand(char* toks[]);

// Other Internal Command Functions
void displayHelp(char* toks[]);
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

queue <Command> history;     // Queue to hold the command history in
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

  char **toks;

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
    fixTokArray(toks);
  }

  if(command.compare("history") == 0){
    printQueue();
  } else if(command.compare("help") == 0){
    displayHelp(toks);
  } else if( (command.compare("quit") == 0)  || (command.compare("exit") == 0) ){
    exitBlueShell();
  } else if(command.compare("cd") == 0){
    cd(toks);
  } else if (command.compare("!") == 0) {
    historyCommand(toks);
  } else if (command.compare("alarm") == 0) {
    setAlarm(toks);
  }

  if(changedIO){
    // Revert to standard I/O
    dup2(0, 0);
    dup2(1, 1);
  }

}

void executeExternalCommand(char* toks[]){
  pid_t pid, child_pid;
  int child_status;

  // Check to see if we need to let the child run in the background or not
  int length = lengthOfTokenArray(toks);
  string lastTok = toks[length - 1];

  bool programShouldRunInBackground = (lastTok.compare("&") == 0);
  if(programShouldRunInBackground){
    // Remove the & at the end of the tok array
    toks[length - 1] = NULL;
  }

  pid = fork();

  if(pid == 0){
    // We are the child.

    if(programShouldRunInBackground){
      setpgid(0, 0);
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
      fixTokArray(toks);
    }
    // Exec and run the program specificied by user
    // Exec documentation at:
    // http://linux.die.net/man/3/execvp

    int exec = execvp(toks[0], toks);

    if(exec < 0) perror("ERROR: Execution of external command has failed. Check for typos.");

  } else {
    // We are the parent.

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
  char** temp = (char**) malloc(128 * sizeof(char*));

  // Combine all tokens into one string to push into history queue
  for(ii = 0; toks[ii] != NULL; ii++){
      command = command + " " + toks[ii];

      //if the command is "!", properly concatenate string
      if (command.compare(" !") == 0) {
        command = command + toks[ii+1];
        break;
      }
  }
  //Create the Command object
  Command node;
  node.setLine(command);
  std::copy(toks, toks+128, temp);
  node.setParts(temp);
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
    if (history.size() != 0) { //if there is history
      node = history.back();
      string comp = (node.getParts())[0]; //string used for comparison

      if(comp.compare("!") != 0) { //if previous command is not "!!" or "!X"
        temp = node.getParts();

        //pass the command
        if(commandIsInternal(temp[0])){
          executeInternalCommand(temp);
        } else {
          executeExternalCommand(temp);
        }
      }
      else { //previous command is !!
        cout << "The previous command is \"!!\" or \"!X\"" << endl;
      }
    }
    else { //there is no history
      cout << "There is no commands in history." << endl;
    }
  }
  else {
    //execute the Nth command

    //get the X from !X
    string convert = toks[1];

    // null = convert.empty();
    // cout << "historyCommand convert null: " << null << endl;

    int n = atoi(convert.c_str());

    //test if the history queue is even as long as the int
    if (n < history.size()) {

      //loop through queue by popping and pushing and grab Nth command
      int size = history.size();
      int ii;
      for(ii = 0; ii < size; ii++){
        Command xx = history.front();
        history.pop();
        history.push(xx);
        if (n == (size-ii)) {
          node = xx;
        }
      } //end for loop

      temp = node.getParts();
      string comp = temp[0];

      if (comp.compare("!") == 0) {
        cout << "The command you are asking for is \"!!\" or \"!X\"" << endl;
      }
      else{

        //pass the command
        if(commandIsInternal(temp[0])){
          executeInternalCommand(temp);
        }
        else {
          executeExternalCommand(temp);
        }
      }
    }
    else {
      //Command does not exist
      cout << "No such command in history" << endl;
    }
  } //end else
} //end historyCommand()


void displayHelp(char* toks[]){
  int length = lengthOfTokenArray(toks);
  if(length != 1){
    cout << "Usage: $help\n";
    return;
  }
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

  int length = lengthOfTokenArray(toks);
  if(length != 2){
    cout << "Usage: $cd dir, where dir is a string representing a relative directory.\n";
    return;
  }

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

int lengthOfTokenArray(char* toks[]){

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

  int length = lengthOfTokenArray(toks);
  if(length != 2){
    perror("Usage: $alarm N, where N is an integer >= 0.");
    return;
  }

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
    cout << "Alarm set to go off in " << seconds << " seconds.\n";
  }

}

char* fixTokArray(char* toks[]){

  // Any time we find < or > in the token array, delete it and
  // the proceeding token, then shift the rest of the array to
  // the left 2 to fill in the gaps

  bool solved = false;

  while(!solved){
    int ii;

    for( ii=0; toks[ii] != NULL; ii++ ){

        if( !strcmp(toks[ii], ">") || !strcmp(toks[ii], "<")){
          int length = lengthOfTokenArray(toks);
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

  return *toks;
}

void exitBlueShell(){
  exit(0);
}
