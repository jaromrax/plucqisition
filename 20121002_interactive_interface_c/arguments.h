#define DEBUGCMD 1
#ifndef arguments_H
#define arguments_H
 
/*   c04_arguments.h
 *
 *   handle the startup arguments  -  keep  main clean
 */
// http://www.gnu.org/s/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
#include <stdio.h> // printf
#include <stdlib.h> //isprint   abort

#include <unistd.h> //    opterr
#include <ctype.h>  //isprint


//separated from main :  PARSE ARGUMENT S
typedef struct{
  int hflag ;  //help
  int bflag ;  // batch == not-interactive
  char *cvalue ; // config - variable-to-phidget
  char *rvalue ; //  remote command line for cprint
} Targs;
Targs args;

int parse_arguments( int argc, char **argv, Targs *args);


#endif
