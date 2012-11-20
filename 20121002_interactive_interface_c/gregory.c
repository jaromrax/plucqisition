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
#include "gregory_mutex.h"   


/////////////////////////////////////////// THREADS 
static const int NTHREADS=1;








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

  /////////////////////////////////////////// lockfile from argument (.net)
  char lockfile[180]="";
  parse_arguments(argc,argv, &args);//in main=normal var, outsourced with 
  if (args.rvalue!=NULL){
      strncpy( lockfile ,  args.rvalue,  180 );
  }
  printf( "lockflie is ...  %s\n", lockfile  );




  //  initialize  threads.........................................
  //  immediately - loop starts -


   tinfo = (thread_info *)calloc(NTHREADS , sizeof(thread_info) );
   if (tinfo == NULL) {printf("calloc failed%s\n", "");return 1;}
   int tnum;
   int pth_res;
for (tnum = 0; tnum < NTHREADS; tnum++) {
               tinfo[tnum].thread_num = tnum + 1;
               /* The pthread_create() call stores the thread ID into
                  corresponding element of tinfo[] */
               pth_res = pthread_create(&tinfo[tnum].thread_id, NULL,
                                  &loop_thread,  &tinfo[tnum] );
               if ( pth_res != 0){printf("pthread_create failed%s\n","");return 1;}
 }//for all threads













int command=0;// recognized command TYPE
 /* command:  ===============================================================+COMMANDS
  *   0 no 
  *   1 ok
  *   3 BASIC   (better #2???)
  *   4 fullfill a command
  *  -1  QUIT
  */
// I DO cmd in two passes: 1 - input, 2 - execute
 char ch[180]=">>>";
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
     command=0; // BYPASS ... designed for batch schedule.......
   }//else.....................




   // **********  reset  ********************* 4
   if ((strcmp (buffer,"reset")==0)||(strcmp (buffer,"r")==0)){// exact match==0
     //printf("%s\n" ,  "" );
     pthread_mutex_lock( &mutex1 );
     global_flag_info=4;  ///GLOBAL RESET
     global_flag_info_status=4;  //
     pthread_mutex_unlock( &mutex1 );
     command=1;
   }



   // **********  list   *********************
   if ((strcmp (buffer,"list")==0)||(strcmp (buffer,"l")==0)){
     printf("%s %s info_status:%d ; \n", ch,".. list", global_flag_info_status);
     if (global_flag_info_status==1){
       printf("%s %s STARTED          \n", ch,"       "  );
     }
     if (global_flag_info_status==2){
       printf("%s %s STOPPED          \n", ch,"       "  );
     }
     if (global_flag_file==1){
       printf("%s %s file is ON  !    \n", ch,"       "  );
     }
     if (global_flag_file==0){
       printf("%s %s file is OFF      \n", ch,"       "  );
     }
     pthread_mutex_lock( &mutex1 );
       global_flag_info=3;
     pthread_mutex_unlock( &mutex1 );
     command=1;
   }


   // **********  start   ********************* 1
   if ((strcmp (buffer,"start")==0)||(strcmp (buffer,"s")==0)){
     //     printf("%s\n","started");
     command=1;
     pthread_mutex_lock( &mutex1 );
     if (global_flag_info_status!=1){
       global_flag_info=1;
       global_flag_info_status=1;
     }else{
       printf("%s %s\n",ch,".. already started !");
     }
     pthread_mutex_unlock( &mutex1 );
   }



   // **********  stop   ********************* 2
   if ((strcmp (buffer,"stop")==0)||(strcmp (buffer,"t")==0)){
     //     printf("%s\n","stopped");
     command=1;
     pthread_mutex_lock( &mutex1 );
     if (global_flag_info_status==1){
       global_flag_info=2;
       global_flag_info_status=2;  
     }else{
       printf("%s %s\n",ch,".. not started !");
     }
     pthread_mutex_unlock( &mutex1 );
   }



   // **********  file   ********************* 2
   if ((strcmp (buffer,"file")==0)||(strcmp (buffer,"f")==0)){
     command=1;
     pthread_mutex_lock( &mutex1 );
     if (global_flag_file==0){
       global_flag_file=1;
       printf("%s %s\n",ch,".. file ON !");
     }else{
       global_flag_file=0;
       printf("%s %s\n",ch,".. file OFF !");
     }
     pthread_mutex_unlock( &mutex1 );
   }





   // **********  quit   *********************
   if ((strcmp (buffer,"q")==0)||(strcmp (buffer,"quit")==0)||(strcmp (buffer,"exit")==0)){// exact match==0
     printf("       ........while loop\n");fflush(stdin);
     //     printf("quiting........while loop\n");fflush(stdin);
     //     printf("quiting........while loop\n");fflush(stdin);
     global_flag_quit=-1; // earlier trick to break the loop

     command= -1;
     break; //doesnot work....
     command= -1;
   }

  } while (command!=-1);// INTERACTIVE COMMAND infnite loop#####################


  //     printf("quiting........waiting threads\n");fflush(stdin);
  //     printf("quiting........waiting threads\n");fflush(stdin);
     printf("       ........waiting threads\n");fflush(stdin);


  printf("%s\n","------------------------------Thread-close-begin");fflush(stdin);
  for (tnum = 0; tnum < NTHREADS; tnum++) {
  printf("thread %d\n", tnum+1);fflush(stdin);
               pth_res = pthread_join(tinfo[tnum].thread_id, NULL );
               if (pth_res != 0){printf("pthread_join failed%s'\n","");exit(1);}
               printf(" master was joined by the thread %d (out of %d)\n",
		      tinfo[tnum].thread_num, NTHREADS);fflush(stdin);
	       //	       printf("thread %d/%d\n", tnum+1, NTHREADS);fflush(stdin);
           }
           free(tinfo);
  printf("%s\n","------------------------------Thread-close-end");fflush(stdin);











// REMOVE LOCKFILE IS EXISTT ********************************
  //  if ( (strlen(lockfile)>4) ){
    if ( filexists(lockfile)==1  ) {
    printf("File %s exist\n", lockfile );fflush(stdin);

    if( remove( lockfile ) != 0 ){
    perror( "Error deleting file" );
    }else{
    puts( "File successfully deleted" );
    }
    }else{//file exists
      //    printf("File <%s> to delete doesnot exist\n", lockfile );fflush(stdin);
    }//else
  //}//lenght is reasonable


     exit(0);
}//===================================================================MAIN

