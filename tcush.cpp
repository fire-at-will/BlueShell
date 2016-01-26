//*********************************************************
//
// PUT YOUR NAME HERE!!!!
// Operating Systems
// Project #2: Writing Your Own Shell: myish
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

using namespace std;

//*********************************************************
//
// Function Declarations
//
//*********************************************************

void displayPrompt();

// History feature functions
void recordCommand(char* toks[]);
void printQueue(queue <string> queueToPrint);

// Internal Command Execution Functions
bool commandIsInternal(string command);
void executeInternalCommand(char* toks[]);


//*********************************************************
//
// Extern Declarations
//
//*********************************************************
using namespace std;
//extern "C"
//{
  extern char **gettoks();
  extern const int NUMBER_OF_INTERNAL_COMMANDS = 3;
  string INTERNAL_COMMANDS [NUMBER_OF_INTERNAL_COMMANDS] = {"history", "!!", "!%d"};
//}

//*********************************************************
//
// Global Variables
//
//*********************************************************

queue <string> history;     // Queue to hold the command history in
string shell = "                                              `.\n                                          `.-/o:\n                                       ``..-:+sy`\n                                    ``..--:+oys-.\n                                 ``...-://+os++oo     ----- Blue Shell -----\n    `.           :            ``...-:::::/+o+oy/`     |                    |\n    `oo.        /o/        ``...-:::--:/oooooo+.      |By Will Taylor      |\n     +ss-`     //:+:      `..-:::--:/+oo++++oss`      |and                 |\n     :o+os:/osyhyyyhyo:` `.-://::///++//+osys:`       |James Stewart       |\n     :+/.-/shhddhhhdddhy..--//::////+oooooooo         |                    |\n  ``./so:``:yhdhhhyo:.`-.-:://://+++ooooosy+`         |--------------------|\n  `+oymhs/+hhdmdhdy-  .:-::://////++osyys:`\n    ommddddmmmhhdddy+:/o:::::///+++osysy`\n    ydhyyyyhdddhhmmmmmmdo//////++ooooos:\n `/+/:::-....--:/+osssyyhyooooooooss+:`\n soooosyss+/::----::::://///+++oyhd.\n +hyhdNMMMN/:+hdhhyysooooossssysso-                   Look out, first place.\n  -/symMMm+.-+NMMMMmh++ssyhdmm/\n    `+so+/++//odmmho//oosyhdms\n       omhyso++oo//:/ossyhdm+\n        -ydhhyyso+oosyyyhho.\n          ./syhddddhhys+:`\n               ````` ";

//*********************************************************
//
// Main Function
//
//*********************************************************
int main( int argc, char *argv[] )
{
  cout << shell << endl;
  // local variables
  int ii;
  char **toks;
  int retval;

  // initialize local variables
  ii = 0;
  toks = NULL;
  retval = 0;

  // main (infinite) loop
  while( true ){
      // get arguments
      displayPrompt();
      toks = gettoks();
      cout << "TOKEN 1: " << toks[0] << "\n";

      if( toks[0] != NULL ){
	  // simple loop to echo all arguments
	  // for( ii=0; toks[ii] != NULL; ii++ )
	  //   {
	  //     cout << "Argument " << ii << ": " << toks[ii] << endl;
	  //   }

    if(commandIsInternal(toks[0])){
      cout << "Internal command\n";
      executeInternalCommand(toks);

    }

	   if( !strcmp( toks[0], "myexit" ))
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

  if( (command.compare("history") ) == 0){
    printQueue(history);
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

// This function displays the prompt to the user.
void displayPrompt(){

  time_t t = time(0);   // get time
  struct tm * now = localtime( & t );
  cout << (now->tm_mon + 1) << '/' <<  now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << "$ ";

}
