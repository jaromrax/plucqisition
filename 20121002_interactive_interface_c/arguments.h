/*   c04_arguments.h
 *
 *   handle the startup arguments  -  keep  main clean
 */
// http://www.gnu.org/s/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt



//separated from main :  PARSE ARGUMENT S
typedef struct{
  int hflag ;  //help
  int bflag ;  // batch == not-interactive
  char *cvalue ; // config - variable-to-phidget
  char *rvalue ; //  remote command line for cprint
} Targs;
Targs args;
/*
typedef struct{
  int hflag ;  //help
  char *cvalue ;
  char *fvalue ; // file - basic code.bas
  char *tvalue ; // time tick in seconds
} Targs;
Targs args;
*/
int parse_arguments( int argc, char **argv, Targs *args){ 
 int c;
opterr = 0;

 args->hflag=0;
 args->bflag=0;  //not batch normaly

 args->rvalue=NULL;
 args->cvalue=NULL;


     
       while ((c = getopt (argc, argv, "hbt:f:r:c:p:x:r:")) != -1)
         switch (c)
           {
           case 'h':     args->hflag = 1;             break;
           case 'b':     args->bflag = 1;             break;
           case 'c':     args->cvalue = optarg;       break;
           case 'r':     args->rvalue = optarg;       break;
           case '?':
	     //return 1;
             if ( (optopt == 'c')||(optopt == 'r')  )
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             else
               fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
             return 1;
           default:
             abort ();
           }

      printf ("COMMANDLINE PARAMETERS:\n\
 -c = %s \
 -r = %s \
 \n",
	      args->cvalue ,
	      args->rvalue 
	      );

 return 0;
}//parse_arguments
