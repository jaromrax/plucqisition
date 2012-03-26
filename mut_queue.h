
#include <stdio.h>  // printf
#include <iostream>
/* stdio = printf
 * stdlib = calloc, exit, free 
 */
#include <stdlib.h>
//#include <pthread.h> //  THIS COMPILES WITH g++
#include "TThread.h"   //  CINT?   //  -lThread
#include<sys/stat.h>  // struct stat



// this is from mk_plugin_man.C------evidently, with CINT we dont use dlfcn.h
#ifdef __CINT__
//#include "vmblock.cxx+"
#else
//#include "vmblock.cxx"
#include <iostream>
#include <dlfcn.h>
#include "TString.h"        //  -lCore

#endif
/*  
   .L mut_queue.C+ 
................maybe I do gSystem->Load("libThread"); in .rootlogon, but necessary?

   g++  -ggdb3  -lpthread  -fPIC  `root-config --libs --cflags --glibs`   -Wall -rdynamic -ldl mut_queue.c -o  mut_queue2

//    g++  -fPIC  `root-config --libs --cflags --glibs`   -Wall -rdynamic -ldl standalone.cpp -o standalone
*/
//******* * test plugins.so :
//  nm -g plugins.so 
#include <queue>   // STD LIB
using namespace std;
//              g++ queue.c -o queue 
//              g++ -lpthread mut_queue.c -o mut_queue
/*  LOGIKA
 *    - chanel     n-v,   def. -1
 *    - scaler     n-v-accv-t0
 *    - flag       n-v  (time, run, block, evt-number)
 */
//http://root.cern.ch/download/doc/ROOTUsersGuideHTML/ch12s07.html


/* TIMING - TThread:         848MB push/pop in 70 sec., main part was call pop
 *        65 mil  = 240MB buffer used
 * withou writeout -  848MB in  51 sec  ... 16MBps
 *        5  mil = 20MB max buffer used
 *
 * 75%CPU when similar
 * 34%CPU when 6.2 MBps
 *  7%    when 0.8MBps
 */





TCondition MyCond(0);  // this has an internal mutex --------




template<typename Data>
class concurrent_queue
{
private:
    std::queue<Data> the_queue;
public:
     ~concurrent_queue(){ printf("queue destructor called\n%s","");  }
    void push(Data const& data)
    {    
             TThread::Lock();
      //      printf("push - entered and locked with data %d\n", (int)data );
      the_queue.push( data );
      //      printf("push - pushed   empty=%d\n", (int)the_queue.empty() );
           TThread::UnLock();
    }

    
    Data front()
    {    
             TThread::Lock();
      //      printf("front -    empty=%d the queue=%d\n", (int)the_queue.empty(), (int)&the_queue  );
      Data aaa=the_queue.front();
           TThread::UnLock();
      return aaa;
    }

    /*
    Data back()
    {    
      pthread_mutex_lock( &mutex_queue );
      Data aaa=the_queue.back();
      pthread_mutex_unlock( &mutex_queue );
      return aaa;
    }
    void pop()
    {    
      pthread_mutex_lock( &mutex_queue );
      the_queue.pop();
      pthread_mutex_unlock( &mutex_queue );
    }
    */


    bool empty() const
    {
      //       pthread_mutex_lock( &mutex_queue );
        bool em=the_queue.empty();
	//       pthread_mutex_unlock( &mutex_queue );
       return em;
    }

    int size() const
    {
      //       pthread_mutex_lock( &mutex_queue );
       int size=the_queue.size();
       //       pthread_mutex_unlock( &mutex_queue );
        return size;
    }

    /*    bool try_pop(Data& popped_value)
    {

        if(the_queue.empty())
        {
            return false;
        }
        
       pthread_mutex_lock( &mutex_queue );
        popped_value=the_queue.front();
        the_queue.pop();
       pthread_mutex_unlock( &mutex_queue );
        return true;
    }
    */
    void wait_and_pop(Data& popped_value)
    {
        while(the_queue.empty())
        {
	  usleep(1000);//  1 ms wait
        }
            TThread::Lock();
        popped_value=the_queue.front();
        the_queue.pop();
	//	printf("poped %d, empty ==  %d\n",popped_value, the_queue.empty()  );
           TThread::UnLock();
    }
};










FILE* XTERM;


int fexists (char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;
       
}












//ps -ef |grep xterm | grep mut_terminal_log | awk '{print "/dev/"$6}'


// RUNS XTERM TO LOG
void logterm(){
  char devpts[80]="";
  //unused  char devpts2[80]="";
  //unused  int res=4;
  /*
   *  part I. tricky.  sort all /dev/pts/ *   by reverse number, first believe is xterm with cat shell
   *
   *  I search ps for "cat" (my xterm shell)....
   */

	std::string name;
	name = getenv("USER");
	char touchfile[200];
	char commandrm[200];
	sprintf(touchfile, "/tmp/mut_terminal_%s.log" , name.c_str() );
	sprintf(commandrm, "rm %s" , touchfile);
  system(commandrm);
      usleep(1000*100);
      //  system("ps -ef |grep xterm | grep mut_terminal_log >/tmp/mut_terminal_log");
      //  system("ps -ef |grep xterm | grep mut_terminal_log | awk '{print \"/dev/\"$6}' >/tmp/mut_terminal_log");
	char commandps[300];
	sprintf( commandps, "ps -ef |grep \"00 cat$\"|grep -v \"00 sh\"|grep -v grep|grep pts|sort -rn >%s2", touchfile );
	system( commandps );
	sprintf( commandps, "ps -ef |grep \"00 cat$\"|grep -v \"00 sh\"|grep -v grep|grep pts|sort -rn| awk '{print \"/dev/\"$6}' > %s", touchfile );
	system( commandps );
      usleep(1000*100);
  FILE *f=fopen( touchfile ,"r");
  if (f!=NULL){
    fscanf(f,"%s", devpts);
    printf("1Content of devpts == %s, \n",devpts);
    /*    if (!feof(f)){
    fscanf(f,"%s", devpts2);
    printf("2Content of devpts == %s, \n",devpts2);
    }
    */
    fclose(f);
    //  int res=fread(devpts,1,sizeof(devpts),f);
    if ( strlen(devpts)>0){
    printf("Content of devpts == %s, trying to open\n",devpts);
    XTERM=fopen(devpts,"a");
    printf("Content of devpts == %s\n",devpts);
    fprintf(XTERM,"AHOJAHOJ\n\n%s","");
    }//res>0 jsou dve, 1. je xterm....
  }
  /*
   *   If nothing works,  open directly now xterm
   *   and believe that new /dev/pts/ii  belongs to it
   *
   */
  if (XTERM==NULL){
    printf("file NOT readable\n%s","");
  for (int i=0;i<35;i++){
    sprintf(devpts,"/dev/pts/%d", i);
    //    printf( "file = %d\n", file_exists(devpts) );
    if ( fexists(devpts)!=0 ){
      //      printf("The %s exists\n", devpts);
    }else{
      //      printf("The %s doesnot exist\n", devpts);
      system("xterm -T mut_terminal_log  cat&");
      usleep(1000*1000);
      XTERM=fopen(devpts,"a");
      fprintf(XTERM,"AHOJAHOJ\n\n%s","");
      //      usleep(1000*1000*3);
      //      fclose(XTERM);
      break;
    }//if else fexists
  }//for
  }//XTERM stale NULL
}
