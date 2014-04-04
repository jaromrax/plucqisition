#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>   //  pthread_create ,   pthread_join
// usleep  in g++ needs  <unistd.h>
#include <unistd.h>

#include <string.h>    // strncat
#include <sys/time.h>   //   time

#include "commands.h"   
#include "arguments.h"   

#include "vme_calls.h"

/////////////////////////////////////////// THREADS 
static const int NTHREADS=1;
int  global_flag_quit=0; //QUIT == -1

int filexists(const char* filename){
FILE *fp = fopen( filename ,"r");
 if( fp ) {fclose(fp); return 1; } else {   return 0; }//else
}//funciton
//////////////////////////////////////////////////////

typedef struct{    /* Used as argument to thread_start() */
     pthread_t thread_id;        /* ID returned by pthread_create() */
     int       thread_num;       /* Application-defined thread # */
 } thread_info;
thread_info *tinfo;
////////////////////////////////////////////////////////
//  copied from c04_lib
////////////////////////////////////////////////////////
void  *loop_thread( void *arg ){
  thread_info *tinfo2 = (thread_info *) arg;
  char ch[20]="===";
  printf("thread number from tinfo2: %d\n", tinfo2->thread_num );
  int loopnum=0;
  do{
    loopnum++;
    if (global_flag_quit==-1){ //---------------QUIT
      printf("%sGot break demand\n",ch);
      break;
    }
     usleep(1000);//now it is ok, i dont know - maybe print was blocking...
      break;
  }while(1); // infinite loop
  printf("%sThread %d ended\n", ch,tinfo2->thread_num );
  return NULL;
}// ====================================================+END OF LOOP





/*   this will be a    function table
 *        key to interpretter that will call the compiled functions
 *2 test :  behavior on 32bit  system
 */

typedef int  (*functiontype)();
typedef int  (*functiontype_i)(int);
typedef int  (*functiontype_ii)(int,int);


struct TCompile_Line{
  char     name[32] ; // set_register_franta
  functiontype_i addr;      // real address of the function
} compile_table[40];  // 4kB size +-



// here is one function ------ to call by a reference in ct.addr
int vme_get_stat_reg(int a){
  printf("%d/%s\n" , a, "... in the vme_stat_reg function");
  return 0;
}
int vme_get_version(int a){
  printf("%d/%s\n" , a, "... in the vme_get_version function");
  return 0;
}



//////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  //  thread_info *tinfo;
/*************************************************************
 *   MAIN PROGRAM -  INIT
 */
  if (argc<=1){printf("%s\n","  some argument neessary....just for fun");exit(0);}


///////////////////////////////////// lockfile from argument (.net)
  char lockfile[180]="";
  parse_arguments(argc,argv, &args);//in main=normal var, outsourced with 
  if (args.rvalue!=NULL){strncpy( lockfile ,  args.rvalue,  180 );}
  printf( "lockfile is ...  %s\n", lockfile  );



  // FILL THE TABLE OF FUNCTIONS ===========================TABLE
  int ic=0;
  sprintf( compile_table[ic].name, "%s", "get_status" );
      compile_table[ic].addr=(functiontype_i)&vme_get_stat_reg;
      ic++;
  sprintf( compile_table[ic].name, "%s", "get_version" );
      compile_table[ic].addr=(functiontype_i)&vme_get_version;
      ic++;
  int icc;
  printf("list of known functions\n%s" , "");
  for (icc=0;icc<ic;icc++){
      printf( "  %s\n", compile_table[icc].name  );
  }
  // END OF TABLE ==========================================TABLE




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


   //==========================================CORE
   //////   command_loop();
 int cmd_param[100];   //set of parameters// minimum 10!
 int i=0; // CMD counter
 int res;
 cmd_print_help();
 while(1==1){
   i++; usleep(1000);
   cmd_line( i );                   // shows PROMPT; reads INPUT to cmd_buffer;
   res=get_cmd_name(    cmd_param );// 
   if (res != 0 ){ 
     if (res == 1 ){ break;} // .q
     if (res == 2 ){ ;} // .l
   } // was an internal command
   else{
     //     printf("cmd_name=%s\n", cmd_name);
     for (icc=0;icc<ic;icc++){
       if ( strcmp(cmd_name,compile_table[icc].name)==0){
	 //	 printf("  matches =%d\n", icc);
	 compile_table[0].addr( cmd_param[0] );
       }//match
     }
     //PERFORM ON YOUR OWN
   }// real command
 }// INF.WHILE.
   //==========================================CORE



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
     if ( filexists(lockfile)==1  ) {
       printf("File %s exist\n", lockfile );fflush(stdin);
       if( remove( lockfile ) != 0 ){
	 perror( "Error deleting file" );
       }else{
	 puts( "File successfully deleted" );
       }
     }

     exit(0);
}//===================================================================MAIN

