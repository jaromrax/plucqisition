#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
// usleep  in g++ needs  <unistd.h>
#include <unistd.h>
// strncat
#include <string.h>
//   time
#include <sys/time.h>

#include "arguments.h"   

static const int NTHREADS=1;

typedef struct{    /* Used as argument to thread_start() */
     pthread_t thread_id;        /* ID returned by pthread_create() */
     int       thread_num;       /* Application-defined thread # */
 } thread_info;
// void *functionC( void *arg );
// mutex    -  locking the memory
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
// global data accesible by all threads
 int  counter = 0;
int  global_flag_stop=0;
int  global_flag_info=0;


int filexists(const char* filename){
FILE *fp = fopen( filename ,"r");
 if( fp ) {
// exists
//   printf("file exists  %s\n", filename );
fclose(fp);
 return 1;
 } else {
   //   printf("file NOT exists  %s\n", filename );
   return 0;
// doesnt exist
 }//else
}//funciton




int main(int argc, char **argv)
{
  thread_info *tinfo;
   // commandline part
  int i=0; //UNUSED , imax=1000000;
  char buffer[10000];
  //  char buffer2[10000];
  //  char vartoch[100]; // variable to change
  //  double valoch;     // value to set 

/*************************************************************
 *   MAIN PROGRAM -  INIT
 */

/**************
 *  look for default  tty  (e.g.   /dev/pts/2)
 */

  if (argc<=1){
  printf("%s\n","  some argument neessary....just for fun");
    exit(0);
  }


  char lockfile[180]="";
  parse_arguments(argc,argv, &args);//in main=normal var, outsourced with 
  if (args.rvalue!=NULL){
      strncpy( lockfile ,  args.rvalue,  180 );
  }
  printf( "lockflie is ...  %s\n", lockfile  );




/*   //  initialize  threads......................................... */
/*   //  immediately - loop starts -  */


/*    tinfo = (thread_info *)calloc(NTHREADS , sizeof(thread_info) ); */
/*    if (tinfo == NULL) {printf("calloc failed%s\n", "");return 1;} */

/* for (tnum = 0; tnum < NTHREADS; tnum++) { */
/*                tinfo[tnum].thread_num = tnum + 1; */
/*                /\* The pthread_create() call stores the thread ID into */
/*                   corresponding element of tinfo[] *\/ */
/*                s = pthread_create(&tinfo[tnum].thread_id, NULL, */
/*                                   &functionC,  &tinfo[tnum] ); */
/*                if (s != 0){printf("pthread_create failed%s\n","");return 1;} */
/*  }//for all threads */



int command=0;// recognized command TYPE
 /* command: 
  *   0 no 
  *   1 ok
  *   3 BASIC   (better #2???)
  *   4 fullfill a command
  */
// I DO cmd in two passes: 1 - input, 2 - execute
  while(1==1){
   i++;
   usleep(1000);
   //   if (i==1){printf("%s\n","pid=/tmp/gregory1234");}
   if (command!=4){  // execute a command==bypass the cmdline
     command=0;// recognized command
     printf ("%04d>", i ); fflush( stdout);
     fgets (buffer, 10000, stdin);
     memset( &buffer[strlen(buffer) - 1] , 0 , 1);//remove \n, put \0
   }else{
     //sprintf( buffer,  "%s \0", args.xvalue  );
     command=0;
   }//else.....................

   // **********  reset  *********************

   if ((strcmp (buffer,"reset")==0)||(strcmp (buffer,"r")==0)){// exact match==0
     //printf("%s\n" ,  "" );
     pthread_mutex_lock( &mutex1 );
     global_flag_info=4;  ///GLOBAL RESET
     pthread_mutex_unlock( &mutex1 );
     command=1;
   }
   // **********  list   *********************
   if ((strcmp (buffer,"list")==0)||(strcmp (buffer,"l")==0)){
     //     list_phidget_values(); //into basic 
     command=1;
   }

   // **********  quit   *********************
   if ((strcmp (buffer,"q")==0)||(strcmp (buffer,"quit")==0)||(strcmp (buffer,"exit")==0)){// exact match==0
     printf("quiting........\n");fflush(stdin);
     break;
     command=1;
  }



 }//       WHILE (1==1) INTERACTIVE COMMAND infnite loop#####################
     printf("quiting........\n");fflush(stdin);




/* // ************************************************************ */
/* // *   MAIN PROGRAM -  END SECTION */
/* // */
/* // / * WAIT UNTIL THREADS END *\/ */
/*   printf("%s\n","---------------------------------------Thread-close-begin"); */
/*   for (tnum = 0; tnum < NTHREADS; tnum++) { */
/*                s = pthread_join(tinfo[tnum].thread_id, NULL ); */
/*                if (s != 0){printf("pthread_join failed%s'\n","");exit(1);} */
/*                printf("Master was joined by the thread %d\n", */
/*                        tinfo[tnum].thread_num); */
/* 	       printf("thread %d/%d", tnum+1, NTHREADS); */
/*            } */
/*            free(tinfo); */
/*   printf("%s\n","---------------------------------------Thread-close-end"); */




// REMOVE LOCKFILE IS EXISTT ********************************
  //  if ( (strlen(lockfile)>4) ){
    if ( filexists(lockfile)==1  ) {
    printf("File %s exist\n", lockfile );

    if( remove( lockfile ) != 0 ){
    perror( "Error deleting file" );
    }else{
    puts( "File successfully deleted" );
    }
    }else{//file exists

    //}else{
    printf("File %s doesnot exist\n", lockfile );
    }//else
  //}//lenght is reasonable


     exit(0);
}//===================================================================MAIN

