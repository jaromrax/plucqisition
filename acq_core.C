/*MAJOR REORGANIZATION: classical *h *C
 *
 * NEW PROBLEM - before compile we need (@ 5.32)
 *               gSystem->Load("/usr/lib/libdl.so")
 */

#include "acq_core.h"     



#include "mut_queue.h"   //  QUEUE  and  MUTTEX DEFINED
TCondition MyCond(0);//this has an internal mutex-LINK TO cpp plugins....
#include "xml_attr.h"     
#include "log_term.h"
//FILE* XTERM; I must put it into log_term.C....


//==================================== FROM PREVIOUS HEADER=====
#include <unistd.h>   //  stat (fexists) file size
#include <stdio.h>  // printf
#include <iostream>
/* stdio = printf
 * stdlib = calloc, exit, free 
 */
#include <stdlib.h>
//#include <pthread.h> //  THIS COMPILES WITH g++
#include "TThread.h"   //  CINT?   //  -lThread
//  2013/06/25 - 15:51 I had to uncoment this line on ubuntu 1204!!!
//       while mojzis was ok
#include<sys/stat.h>  // struct stat

//----------------------------with   mmap ------------------------
#include <err.h>
#include <sys/mman.h>


// this is from mk_plugin_man.C------evidently, with CINT we dont use dlfcn.h
#ifdef __CINT__
//#include "vmblock.cxx+"
#else
//#include "vmblock.cxx"
#include <iostream>
#include <dlfcn.h>
#include "TString.h"        //  -lCore

#endif




#include "TTimeStamp.h"  // root own time stamping
#include <iostream>
#include <fstream>    
using namespace std;
/*************************************************
 *  this is defined once here , not in .h
 */
         // this BUFFER is passed to all plugins
concurrent_queue<int> buffer;   // empty queue

TTimeStamp t_start;


/*********************************************************************
*********************************************************************
*                         MAIN     MAIN       MAIN
*********************************************************************
*/

int mmapfd;        //  =-1           file handle for mmap
char* mmap_file ;  // pointer to     mmap



void *xml_masterthread(void* arg){

  TThread *t;
  printf("..........................masterthread is launched\n%s", "");
  /******************************
   * I do xml readout
   *  so-lib and function.....
   */
  char so_push[1000];
  char so_pop[1000];
  char so_analyze[1000];
  char func_push[1000];
  char func_pop[1000];
  char func_analyze[1000];
  char file_push[1000];
  char file_pop[1000];
  /*
   * collect:  name of plugin.so,  function names ; files (in/out),ip,port
   */
  //   TSmallish_xml xml("mut_queue.xml");

  char  acqxml2[100];
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap and ...
  printf("...       gettinf XML    /%s/\n",  acqxml2 );
   TSmallish_xml xml(    acqxml2      );
   xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","so" );
   sprintf( so_push,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","poper","so" );
   sprintf( so_pop,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","so" );
   sprintf( so_analyze,"%s", xml.output  );


   xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","func" );
   sprintf( func_push,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","poper","func" );
   sprintf( func_pop,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","func" );
   sprintf( func_analyze,"%s", xml.output  );


  xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","file" );
   sprintf( file_push,"%s", xml.output  );
  xml.DisplayTele( xml.mainnode, 0, "plugins","poper","file" );
   sprintf( file_pop,"%s", xml.output  );

   if (fexists(file_pop)==1){ printf("File %s already exists, STOPPING\n",file_pop);return NULL;}
   if ( (strlen(file_push)>0)&&(fexists(file_push)!=1) ){printf("File %s DOESNOT exist,  STOPPING\n",file_push);return NULL;}



 
   //------------------------------------dl------------------
  const char *dlsym_error;

  //    printf("I want   to OPEN PUSH/POP   library==\"%s\"\n", so_push);
    dl_handle_push = dlopen( so_push , RTLD_GLOBAL |  RTLD_LAZY);
    //    printf("I wanted to OPEN the dl_handle %d\n", (int)dl_handle);
    if (!dl_handle_push) {printf("Cannot open library: %s; %s\n", so_push,dlerror() );
	 return 0;//can return 0
    }//if (dl_handle_push) 



    /*****************************
     *  I start  here....................... PUSH PART
     */
    dlerror();
    // global...   plugin_tint 
      evt_pusher_remote = (plugin_tint) dlsym(dl_handle_push, func_push  );
    dlsym_error = dlerror();
    if (dlsym_error) {
      printf("Cannot load symbol: %s; %s\n", func_push,dlsym_error );
        dlclose(dl_handle_push);
        return 0;//can return 0
    }// if (dlsym_error) {.....




  //    printf("I want   to OPEN PUSH/POP   library==\"%s\"\n", so_push);
    dl_handle_pop = dlopen( so_pop , RTLD_GLOBAL |  RTLD_LAZY);
    //    printf("I wanted to OPEN the dl_handle %d\n", (int)dl_handle);
    if (!dl_handle_pop) {printf("Cannot open library: %s; %s\n", so_pop,dlerror() );
	 return 0;//can return 0
    }//if (dl_handle_pop) 
    /******************************************************
   *  I start  here....................... POP PART
   */
    dlerror();
    // global...   plugin_tint 
      evt_poper_remote = (plugin_tint) dlsym(dl_handle_pop,func_pop );
    dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol: %s; %s\n", func_pop,dlsym_error );
        dlclose(dl_handle_pop);
        return 0;//can return 0
    }// if (dlsym_error) {.....



    //    printf("I want   to OPEN ANALYZE   library==\"%s\"\n", so_analyze);
    dl_handle_analyze = dlopen( so_analyze , RTLD_GLOBAL |  RTLD_LAZY);
    //    printf("I wanted to OPEN the dl_handle_analyze %d\n", (int)dl_handle_analyze);
    if (!dl_handle_analyze) {printf("Cannot open library: %s; %s\n", so_push,dlerror() );
	 return 0;//can return 0
    }//if (dl_handle_analyze) 
    /*****************************
     *  I start dlopen rather here....................... ANALYZE PART
     */
    dlerror();
    // global...   plugin_tint 
      evt_analyze_remote = (plugin_tint) dlsym(dl_handle_analyze, func_analyze  );
    dlsym_error = dlerror();
    if (dlsym_error) {
      printf("Cannot load symbol: %s; %s\n", func_analyze,dlsym_error );
        dlclose(dl_handle_analyze);
        return 0;//can return 0
    }// if (dlsym_error) {.....





    //NONSENSE HERE
    //    TThread::SetCancelOn();//allows to set TThread::CancelPoint() and use dereffered cancel(->Kill)
    /************************************************************
     *
     *         create  threads  here   --------------------------
     *
     *GetState():   2 ... running
     *              5 ... canceling
     *              6 ... canceled
     */
  //   tinfo is allocated in MAIN----------------------------------------------
   tinfo[1].thread_num=1;//============ THREAD DEFINITION=========PUSH
   tinfo[1].running=1; tinfo[1].callnumber=1;tinfo[1].parent=NULL;
   sprintf(tinfo[1].file, "%s",file_push); // send also filename
   tinfo[2].thread_num=2;//============ THREAD DEFINITION=========POP
   tinfo[2].running=1;tinfo[2].callnumber=1;tinfo[2].parent=&tinfo[0];
   sprintf(tinfo[2].file, "%s",file_pop);// send also filename
   tinfo[3].thread_num=3;//============ THREAD DEFINITION=========ANALYZE
   tinfo[3].running=1;tinfo[3].callnumber=1;tinfo[3].parent=&tinfo[0];
   sprintf(tinfo[3].file, "%s","temporary.root");// send also filename



   if(XTERM!=NULL)fprintf(XTERM,"creating pusher_thread = %ld\n",(int64_t)TThread::GetThread("pusher_thread") );
   t=TThread::GetThread("pusher_thread");
   if (t!=NULL){
     if (t->GetState()==6){ t->Delete();}//canceled
   }

   if (TThread::GetThread("pusher_thread")==0){
    shspe_threads[1] = new TThread( "pusher_thread" , evt_pusher, (void*) &tinfo[1] );
    if (shspe_threads[1]==NULL){ printf("exiting, thread 1 not running\n%s","");return NULL;}
    shspe_threads[1]->Run();
   }



   if(XTERM!=NULL)fprintf(XTERM,"creating poper_thread = %ld\n",(int64_t)TThread::GetThread("poper_thread") );
   if (TThread::GetThread("poper_thread")==0){
    shspe_threads[2] = new TThread( "poper_thread"  , evt_poper, (void*) &tinfo[2] );
    if (shspe_threads[2]==NULL){ printf("exiting, thread 2 not running\n%s","");return NULL;}    
    shspe_threads[2]->Run();
   }



   if(XTERM!=NULL)fprintf(XTERM,"creating analyze_thread = %ld\n",(int64_t)TThread::GetThread("analyze_thread") );
   TThread *thq=(TThread*)TThread::GetThread("analyze_thread");
   if (thq!=NULL){
     if (thq->GetState()==6){printf("Killing analyze_thread\n");thq->Delete();}
   }
   if (TThread::GetThread("analyze_thread")==0){
    shspe_threads[3] = new TThread( "analyze_thread"  , evt_analyze, (void*) &tinfo[3] );
    if (shspe_threads[3]==NULL){ printf("exiting, thread 3 not running\n%s","");return NULL;}    
    shspe_threads[3]->Run();
   }


   /*******************************************
    *         W A I T for chidren            **
    */


   // ***************************   PREVIOUS JOIN DELETE ETC.............
   printf("\nMASTER: waiting to JOIN ALL T. dl_handleS==%ld/%ld\n",(int64_t)dl_handle_push,(int64_t)dl_handle_pop);
    while (   (TThread::GetThread("pusher_thread")!=0)|| 
	      (TThread::GetThread("poper_thread")!=0)||
	      (TThread::GetThread("analyze_thread")!=0)
	      ){
      usleep(1000*600);  //  MAIN  WAIT IN MASTER   0.6 sec
      //if(XTERM!=NULL)fprintf(XTERM,"%s", "" );
    }
    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER: ALL Threads OVER. The dl_handle \n");


    /*   NO JOINS, al lare to be killed..........................
    shspe_threads[1]->Join();
    shspe_threads[2]->Join();
    shspe_threads[3]->Join();
    delete shspe_threads[1]->;
    delete shspe_threads[2]->;
    delete shspe_threads[3]->;
    */
    TTimeStamp t_stop;  t_stop.Set();
  printf("In the MASTER:\n  start %s\n  stop  %s\n  seconds = %d\n",
	 t_start.AsString("l"),  
	 t_stop.AsString("l"), 
	 (int)(t_stop.GetSec()-t_start.GetSec() ) );

  // usleep(1000*1000*1);  //doesnot change much

  /*

    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:ONE  ....................to dl close:\n" );
    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:SECOND ................. to dl close:\n" );

    dlerror();
  if (!dl_handle){
    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:ONE dl_handle just closing now\n" );
    res=dlclose(dl_handle); // THIS IS DIFFICULT!!!!!!!!!!!!!!!!!
    if(XTERM!=NULL)fprintf(XTERM,"MASTER:Result of dlclosing ==%d (should be 0); Err::%s\n", 
			   res, dlerror() );
    //    dl_handle=NULL;
  } // can be closed after all threads down


    dlerror();
  if (!dl_handle_analyze){
    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:ONE dl_handle_analyze just closing now\n" );
    res=dlclose(dl_handle_analyze); // THIS IS DIFFICULT!!!!!!!!!!!!!!!!!
    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:ONE  dl_handle_analyze now DONE\n" );
    if(XTERM!=NULL)fprintf(XTERM,"MASTER:Result of dlclosing_analyze ==%d (should be 0); Err::%s\n", 
			   res, dlerror() );
    //    dl_handle_analyze=NULL;
  } // can be closed after all threads down


  TThread::Ps();
  */

  return NULL;   // void* mut_master_thread()
}
/************************************************END OF FUNCTION
 ************************************************MASTER THREAD
 */













/*                                                                ====
  ==== THE REAL  MAIN===============================================
 *                                                                ====
 */

int acq(const char * startstop="start")
{
  //  TThread *t;

    //===========================XTERM INIT============================
  if (XTERM==NULL){
    if (logterm()!=0 )usleep(1000*800);
  }//ALLOCATE OR FIND XTERMINAL


    //===========================MMAP INIT============================
     if (mmapfd==0){
    //   char str1[] = "acq_setup.xml\nrun==1\n";    // this means "running"
     //  char  *cnt_file;  //pointer
    system("dd if=/dev/zero of=control.mmap  bs=4096  count=1");
    if ((mmapfd = open("control.mmap", O_RDWR, 0)) == -1) err(1, "open");
    mmap_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, mmapfd, 0);
    if (mmap_file == MAP_FAILED) errx(1, "either mmap");
    strcpy(mmap_file, acq_default   ); // "acq_setup.xml\nrun=1\n";
    printf("...default  XML file setup:  %s\n", acq_default  );
    //  munmap(mmap_file, 4096);        
    //  close(mmapfd);
    //I WILL NEVER  DEALOCATE THIS ..... CAN IT BE A PROBLEM?
     }// CREATE MMAP:   in the first RUN


     //==========================introduce optional XML file=========
     if ( strstr(startstop,".xml") - startstop ==  strlen(startstop)-4 ){
       printf("...new XML file demanded:  %s\n", startstop );
       char newline[100];
       sprintf( newline, "%s\nrun=1\n" , startstop ); 
       strcpy(mmap_file, newline  ); // "acq_setup.xml\nrun=1\n";
     }else{
       //       printf("...new XML file NOT demanded:  %s \n", startstop );
     }



  if ( strcmp(startstop,"stop")==0){//=====================IF STOP=========
    printf(".....go stop.\n%s", "" );
  }else{
    printf(".....go START.\n%s", "" );    
    t_start.Set();
  }






  //==========================================================STOP========
  if ( strcmp(startstop,"stop")==0){//====================IF STOP=========
    int not_deads=0;
    int kill_retries=0;
    //    TThread::Ps();
    /*    First thing will be to try few time to send broadcast.....
     *    check if analyze got it --------- else exit without success...
     */
   //=================================================================================BEGIN

    if(TThread::GetThread("master_thread")==0){
          if(XTERM!=NULL)fprintf(XTERM,"%s\n", "master thread seems not to exist, acq not started????");
	  printf("%s master thread seems not to exist, acq not started???? \n","");
	  return 0;
    }


    do{ //while not deads

      TokenReplace( "run=",  "run=0",  mmap_file );
      usleep(1000*100); // why not to wait.....a while....
      not_deads=0;
    TThread::Ps();
    //    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "...broadcasting ALL _threads");
    //    printf("%s...broadcasting to ALL threads\n","");
    MyCond.GetMutex();
    MyCond.Broadcast();
    usleep(1000*100);



    TThread *t;

    //-----------------------------
    //    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the pusher_thread");
    t=TThread::GetThread("pusher_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","pusher_thread==canceled=>DELETIN'");t->Delete();}
      usleep(1000*100);
    }
    t=TThread::GetThread("pusher_thread");
   if (t!=NULL){
         if(XTERM!=NULL)fprintf(XTERM,"%s\n", "pusher_thread NOT DEAD, TRY AGAIN, SORRY ");
         printf("%s\n", "pusher_thread NOT DEAD, TRY AGAIN, SORRY ");
	 not_deads++;
   }
   //=================================================================================END


    //----------------this is very fragile thread------------------------------------------------------
   //    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the analyze_thread");
    t=TThread::GetThread("analyze_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","analyze_thread==canceled=>DELETIN'");t->Delete();}
      usleep(1000*100);
    }
    t=TThread::GetThread("analyze_thread");
   if (t!=NULL){
         if(XTERM!=NULL)fprintf(XTERM,"%s\n", "analyze_thread NOT DEAD, TRY AGAIN, SORRY ");
         printf("%s\n", "analyze_thread NOT DEAD, TRY AGAIN, SORRY ");
	 not_deads++;

   }
   //=================================================================================END


    //----------------this is very fragile thread--------------
   //    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the poper_thread");
    t=TThread::GetThread("poper_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","poper_thread==canceled=>DELETIN'");t->Delete();}
      usleep(1000*100);
    }
    t=TThread::GetThread("poper_thread");
   if (t!=NULL){
         if(XTERM!=NULL)fprintf(XTERM,"%s\n", "poper_thread NOT DEAD, TRY AGAIN, SORRY ");
         printf("%s\n", "poper_thread NOT DEAD, TRY AGAIN, SORRY ");
	 not_deads++;

   }
   //=================================================================================END



    TThread::Ps(); 
    if (kill_retries>=   0   ){ break;}
    kill_retries++;
    }while (not_deads>0);

    if (not_deads>0){ 	 return 0; } // MUST EXIT HERE, SOME ZOMBIE IS THERE######################





    int cdown=3;// approximately this 2 seconds it takes
    while(TThread::GetThread("master_thread")!=0){
      printf("--------------------------- wait for MASTER ... %d sec\n", cdown-- );
      usleep(1000*500);
      if (TThread::GetThread("master_thread")->GetState()==6){
	TThread::GetThread("master_thread")->Delete();
      }//state 6
    }//master exists.....................



    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:SECOND ................. to dl close!\n" );
    dlerror();
    int res=dlclose(dl_handle_analyze); // THIS IS DIFFICULT!!!!!!!!!!!!!!!!!
    // there is a decrement....
    // http://stackoverflow.com/questions/8792363/c-dlclose-doesnt-unload-the-shared-library
    // maybe one (analyze) dlclose is fine, maybe it make both?
    /*
      usleep(1000*1000*3);

    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:ONE  ....................to dl close!\n" );
    dlerror();
    //    res=dlclose(dl_handle); // THIS IS DIFFICULT!!!!!!!!!!!!!!!!!
    printf("RESULT==%d Err:%s\n", res,  dlerror()  );
      usleep(1000*1000*4);
    */


    /*    TTimeStamp t_last;
        printf("This is in stop section (t_last):\n%s\n%s\n%d\n",
	 t_start.AsString("l"),  
	 t_last.AsString("l"), 
	 (int)(t_last.GetSec()-t_start.GetSec() ) );
    */
    return 0;
    printf("--------------------after the return\n\n%s", "");
  }//=========================== STOP====================================
//============================== STOP====================================END
//============================== STOP====================================





//if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START================\n");
  if (TThread::GetThread("master_thread")!=0){
    if (TThread::GetThread("master_thread")->GetState()==6){
	TThread::GetThread("master_thread")->Delete();
      }//state 6
  }//master exists
  usleep(1000);
  if (TThread::GetThread("master_thread")==0){

    //======START======

    TokenReplace( "run=" , "run=2", mmap_file );


    tinfo = (thread_info*)calloc( 5, sizeof(struct thread_info));// 5threads

    tinfo[0].thread_num=0;//============ THREAD DEFINITION=========
    tinfo[0].running=1; tinfo[0].callnumber=1;tinfo[0].parent=NULL;
    sprintf(tinfo[0].file, "%s",""); 

    shspe_threads[0] = new TThread( "master_thread" , xml_masterthread, (void*) &tinfo[0] );
    if (shspe_threads[0]==NULL){ printf("exiting, MASTER thread ) NOT launched!!\n%s","");return 0;}
    shspe_threads[0]->Run();
    //NOT HERE IN 0... if (dl_handle!=NULL)dlclose(dl_handle); // can be closed after all threads down

  }else{
    //    TThread::Ps();
    printf("MASTER THREAD ALREADY RUNS, TRY acq(\"stop\") %s\n","");
  }

  return 0;// non void function 
}//=================================================######################### MAIN MAIN END







/**************************************************
 * muze se stat ze by byl empty a rekl non empty? pak pop je ale v mutexu
 *    
 */
void *evt_pusher( void *arg )  // loads the queue 
{
 struct thread_info *mtinfo = (struct thread_info *) arg;
 int nt=mtinfo->thread_num;
     evt_pusher_remote( (int*)&buffer ); //#########EVENT#########
  mtinfo->running=0;// say stop
 printf("%s","pusher function is stoped \n");
  return NULL;
}//--------void *evt_pusher( void *arg )  // loads the queue 




/************************************************************************
 *   very difficult - once there exists a ttree, we cannot return to TTree
 */
void *evt_poper( void *arg )  // reads the queue (pop)
{
 struct thread_info *mtinfo = (struct thread_info *) arg;
 int nt=mtinfo->thread_num;
 int call=mtinfo->callnumber; mtinfo->callnumber++;// to keep track opened file
 // printf("starting thread %d poper for the %dth time\n", nt, call );
 //     printf("removing ........bufsize==%10d   empty==%d \n", buffer.size(), buffer.empty() );
     evt_poper_remote(  (int*)&buffer  );
     printf("%s","poper function is stoped\n");
  mtinfo->running=0;// say stop
     return NULL;
//jak to budu analyzovat? konec eventu je kde????
//1/  ffff efff run    0000 e0**   hodnota,kanal.    0000 f000 ... ffff ffff
//2/  prosta radka textu: \n
//3/  keithley - svuj system 123:45:67:78\n
//  3.2MB file,  100k eventu...............
//          .od -h -A n  -w2  infin  | grep e004 | wc
 return NULL;
}//--------void *evt_poper( void *arg )  // reads the queue (pop)





/************************************************************************
 *   
 */
void *evt_analyze( void *arg )  // 
{
 struct thread_info *mtinfo = (struct thread_info *) arg;
 int nt=mtinfo->thread_num;
 int call=mtinfo->callnumber; mtinfo->callnumber++;// to keep track opened file
   evt_analyze_remote(  (int*)&buffer  ); //#########EVENT#########
 printf("%s","analyze function is stoped \n");
  mtinfo->running=0;// say stop
     return NULL;
}