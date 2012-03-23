/*
 *  chybi:   dlclose a uvolneni   .so               --OK
 *           ovladani zarizeni ??                   
 *           testy counters                         
 *           test  v zivote - zatez -circular..... 
 *           scalers - nekde se vali gPads....      
 *           contury - online conturovani           --~OK
 *
 */
#include "mut_queue.h"
#include "xml_attr.h"    
#include "TTimeStamp.h"
#include <iostream>
#include <fstream>    
using namespace std;
/*************************************************
 *  this is defined once here , not in .h
 */
  concurrent_queue<int> buffer;            // empty queue



/*************************************************
 *  this is THREAD definitions - 
 */
const int MAXTHREADS=10; //
TThread *shspe_threads[MAXTHREADS];

//------------- thread functions names predefined--------------------
void *evt_pusher( void *arg ); // loads the queue 
void *evt_poper( void *arg ); // reads the queue (pop)
void *evt_analyze( void *arg ); // reads the queue (pop)

struct thread_info {       /* Used as argument to thread_start() */ //tinfo
  int       thread_num;       /* Application-defined thread # */
  int       running;         // 1 yes 0 no
  int       callnumber;      // 1 1st call in program  etc. (file w/a)
  char      file[200];       // can be filename to  push or flush...
  thread_info* parent;        // poper can see parent....
 };






/********************************************
 *   these functions are located in plugin.so or  plug_queue.so
 */
typedef int* (*plugin_tint)( int* );
plugin_tint evt_pusher_remote ;
plugin_tint evt_poper_remote ;
plugin_tint evt_analyze_remote ;

/*
bool fexists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}
*/



TTimeStamp t_start;



 struct thread_info *tinfo;// create n threads ..... array. calloc:




/*********************************************************************
*********************************************************************
*********************************************************************
  *                         MAIN     MAIN       MAIN
*********************************************************************
*********************************************************************
  */
  /*****************************
   *  I start dlopen rather here....................... PUSHER PART
   */
  void* dl_handle;
  void* dl_handle_analyze;


void *mut_queue_masterthread(void* arg){

  TThread *t;

  //unused     int res;

  printf("mut_queue_masterthread is launched\n%s", "");

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
   * collect:  name of plugin.so,  function names, files (in/out)
   * 
   */
   TSmallish_xml xml("mut_queue.xml");
   xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","so" );
   sprintf( so_push,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","poper","so" );//must be the same *.so
   sprintf( so_pop,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","so" );//
   sprintf( so_analyze,"%s", xml.output  );


   xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","func" );
   sprintf( func_push,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","poper","func" );
   sprintf( func_pop,"%s", xml.output  );
   xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","func" );
   sprintf( func_analyze,"%s", xml.output  );


  xml.DisplayTele( xml.mainnode, 0, "files","pusher","file" );
   sprintf( file_push,"%s", xml.output  );
  xml.DisplayTele( xml.mainnode, 0, "files","poper","file" );
   sprintf( file_pop,"%s", xml.output  );

   if (fexists(file_pop)==1){printf("File %s already exists\n",file_pop);return NULL;}

  /*****************************
   *  I start dlopen rather here....................... PUSHER PART
   */
   //  void* dl_handle;
   //  void* dl_handle_analyze;



  const char *dlsym_error;


    printf("I want   to OPEN PUSH/POP   library==\"%s\"\n", so_push);
    dl_handle = dlopen( so_push , RTLD_GLOBAL |  RTLD_LAZY);
    printf("I wanted to OPEN the dl_handle %d\n", (int)dl_handle);
    if (!dl_handle) {printf("Cannot open library: %s; %s\n", so_push,dlerror() );
	 return 0;//can return 0
    }//if (dl_handle) 

    /*****************************
     *  I start  here....................... PUSH PART
     */
    dlerror();
    // global...   plugin_tint 
      evt_pusher_remote = (plugin_tint) dlsym(dl_handle, func_push  );
    dlsym_error = dlerror();
    if (dlsym_error) {
      printf("Cannot load symbol: %s; %s\n", func_push,dlsym_error );
        dlclose(dl_handle);
        return 0;//can return 0
    }// if (dlsym_error) {.....


    /******************************************************
   *  I start  here....................... POP PART
   */
    dlerror();
    // global...   plugin_tint 
      evt_poper_remote = (plugin_tint) dlsym(dl_handle,func_pop );
    dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol: %s; %s\n", func_pop,dlsym_error );
        dlclose(dl_handle);
        return 0;//can return 0
    }// if (dlsym_error) {.....



    printf("I want   to OPEN ANALYZE   library==\"%s\"\n", so_analyze);
    dl_handle_analyze = dlopen( so_analyze , RTLD_GLOBAL |  RTLD_LAZY);
    printf("I wanted to OPEN the dl_handle_analyze %d\n", (int)dl_handle_analyze);
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


   if(XTERM!=NULL)fprintf(XTERM,"creating pusher_thread = %d\n",(int)TThread::GetThread("pusher_thread") );
   t=TThread::GetThread("pusher_thread");
   if (t!=NULL){
     if (t->GetState()==6){ t->Delete();}//canceled
   }

   if (TThread::GetThread("pusher_thread")==0){
    shspe_threads[1] = new TThread( "pusher_thread" , evt_pusher, (void*) &tinfo[1] );
    if (shspe_threads[1]==NULL){ printf("exiting, thread 1 not running\n%s","");return NULL;}
    shspe_threads[1]->Run();
    //   TThread::Ps();
   // if ( TThread::Exists()>0){printf("(%d) threads exist\n",TThread::Exists());}
   // TO CLEAN PUHERS TThread::GetThre
   //ad("pusher_thread")->Delete() .......
   }



   if(XTERM!=NULL)fprintf(XTERM,"creating poper_thread = %d\n",(int)TThread::GetThread("poper_thread") );
   if (TThread::GetThread("poper_thread")==0){
    shspe_threads[2] = new TThread( "poper_thread"  , evt_poper, (void*) &tinfo[2] );
    shspe_threads[2]->Run();
    //   TThread::Ps();
   //    printf( "poper_thread === %d\n", TThread::GetThread("poper_thread")>0 );
   }



   if(XTERM!=NULL)fprintf(XTERM,"creating analyze_thread = %d\n",(int)TThread::GetThread("analyze_thread") );
   TThread *thq=(TThread*)TThread::GetThread("analyze_thread");
   if (thq!=NULL){
     if (thq->GetState()==6){printf("Killing analyze_thread\n");thq->Delete();}
   }
   if (TThread::GetThread("analyze_thread")==0){
    shspe_threads[3] = new TThread( "analyze_thread"  , evt_analyze, (void*) &tinfo[3] );
    shspe_threads[3]->Run();
    //   TThread::Ps();
   //    printf( "poper_thread === %d\n", TThread::GetThread("poper_thread")>0 );
   }



   // ***************************   PREVIOUS JOIN DELETE ETC.............
    printf("MASTER: I wait to JOIN ALL T. The dl_handle %d\n", (int)dl_handle);
    while (   (TThread::GetThread("pusher_thread")!=0)|| 
	      (TThread::GetThread("poper_thread")!=0)||
	      (TThread::GetThread("analyze_thread")!=0)
	      ){
      usleep(1000*600);  //  MAIN  WAIT IN MASTER
      if(XTERM!=NULL)fprintf(XTERM,"%s", "" );
    }
  if(XTERM!=NULL)fprintf(XTERM,"MASTER: ALL Threads OVER. The dl_handle %d\n",(int)dl_handle);


    /*   NO JOINS, al lare to be killed..........................
    shspe_threads[1]->Join();
    shspe_threads[2]->Join();
    shspe_threads[3]->Join();
    delete shspe_threads[1]->;
    delete shspe_threads[2]->;
    delete shspe_threads[3]->;
    */
  TTimeStamp t_stop;
  printf("In the MASTER:\n%s\n%s\n%d\n",
	 t_start.AsString("l"),  
	 t_stop.AsString("l"), 
	 (int)(t_stop.GetSec()-t_start.GetSec() ) );

  usleep(1000*1000*1);  //doesnot change much

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
/*****************************************************************END OF FUNCTION
 *****************************************************************MADSTER THREAD
 */














int mut_queue(const char * startstop="start")
{
  TThread *t;

    if (XTERM==NULL){logterm(); }//ALLOCATE OR FIND XTERMINAL






  //===================================================================STOP========
  if ( strcmp(startstop,"stop")==0){//==============================IF STOP=========
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
      not_deads=0;
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "broadcasting ALL _threads");
    printf("%sbreaodcasting to ALL threads\n","");
    MyCond.Broadcast();
    usleep(1000*500);
    //    TThread::Ps();
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "3");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "2");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "1");



    //-----------------------------
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the pusher_thread");
    t=TThread::GetThread("pusher_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","Thread state==canceled=>DELETIN'");t->Delete();}
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
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the analyze_thread");
    t=TThread::GetThread("analyze_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","Thread state==canceled=>DELETIN'");t->Delete();}
      usleep(1000*100);
    }
    t=TThread::GetThread("analyze_thread");
   if (t!=NULL){
         if(XTERM!=NULL)fprintf(XTERM,"%s\n", "analyze_thread NOT DEAD, TRY AGAIN, SORRY ");
         printf("%s\n", "analyze_thread NOT DEAD, TRY AGAIN, SORRY ");
	 not_deads++;

   }
   //=================================================================================END


   //=================================================================================BEGIN
   //   if(XTERM!=NULL)fprintf(XTERM,"%s\n", "broadcasting pusher_thread");
   //    printf("%sbreaodcasting to pusher_thread\n","");
    /*    MyCond.Broadcast();
    usleep(1000*500);
    TThread::Ps();
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "3");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "2");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "1");
    */

   //=================================================================================BEGIN
   //    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "broadcasting poper_thread");
   //    printf("%sbreaodcasting to poper_thread\n","");
    /*    MyCond.Broadcast();
    usleep(1000*500);
    TThread::Ps();
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "3");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "2");
    usleep(1000*500);
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "1");
    */
    //----------------this is very fragile thread--------------
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "checking the existence of the poper_thread");
    t=TThread::GetThread("poper_thread");
    if (t!=NULL){
      if (t->GetState()==6){if(XTERM!=NULL)fprintf(XTERM,"%s\n","Thread state==canceled=>DELETIN'");t->Delete();}
      usleep(1000*100);
    }
    t=TThread::GetThread("poper_thread");
   if (t!=NULL){
         if(XTERM!=NULL)fprintf(XTERM,"%s\n", "poper_thread NOT DEAD, TRY AGAIN, SORRY ");
         printf("%s\n", "poper_thread NOT DEAD, TRY AGAIN, SORRY ");
	 not_deads++;

   }
   //=================================================================================END

   /*
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "killing poper_thread");
    t=TThread::GetThread("poper_thread");
    if (t!=NULL){
      t->Kill();
      t->Delete();
    usleep(1000*100);
    }
    if(XTERM!=NULL)fprintf(XTERM,"%s\n", "killing pusher_thread");
    t=TThread::GetThread("pusher_thread");
    if (t!=NULL){
      t->Kill();
      t->Delete();
    usleep(1000*100);
    }
   */


    TThread::Ps(); 
    if (kill_retries>2){ break;}
    kill_retries++;
    }while (not_deads>0);


    if (not_deads>0){ 	 return 0; }

    while(TThread::GetThread("master_thread")!=0){
      printf("--------------------after the tthread masacre wait for MASTER\n%s", "");
      usleep(1000*200);
      if (TThread::GetThread("master_thread")->GetState()==6){
	TThread::GetThread("master_thread")->Delete();
      }//state 6
      usleep(1000*200);
    }//master exists.....................
    //    printf("--------------------after the tthread masacre:\n\n%s", "");
    //    TThread::Ps();
    //    printf("\n%s", "");  



      usleep(1000*1000);
    int res;

    if(XTERM!=NULL)fprintf(XTERM,"%s","MASTER:SECOND ................. to dl close!\n" );
    dlerror();
    res=dlclose(dl_handle_analyze); // THIS IS DIFFICULT!!!!!!!!!!!!!!!!!
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


    TTimeStamp t_last;
    printf("This is in stop section (t_last):\n%s\n%s\n%d\n",
	 t_start.AsString("l"),  
	 t_last.AsString("l"), 
	 (int)(t_last.GetSec()-t_start.GetSec() ) );
    return 0;
    printf("--------------------after the return\n\n%s", "");
  }//=========================== STOP====================================
//============================== STOP====================================END
//============================== STOP====================================









  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START=====================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START=====================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START=====================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START=====================\n");
  if(XTERM!=NULL)fprintf(XTERM,"%s\n", "===================MAKING START=====================\n");
  if (TThread::GetThread("master_thread")!=0){
    if (TThread::GetThread("master_thread")->GetState()==6){
	TThread::GetThread("master_thread")->Delete();
      }//state 6
  }//master exists
  usleep(1000);
  if (TThread::GetThread("master_thread")==0){




    tinfo = (thread_info*)calloc( 5, sizeof(struct thread_info));// 5threads

    tinfo[0].thread_num=0;//============ THREAD DEFINITION=========
    tinfo[0].running=1; tinfo[0].callnumber=1;tinfo[0].parent=NULL;
    sprintf(tinfo[0].file, "%s",""); 

    shspe_threads[0] = new TThread( "master_thread" , mut_queue_masterthread, (void*) &tinfo[0] );
    if (shspe_threads[0]==NULL){ printf("exiting, MASTER thread ) NOT launched!!\n%s","");return NULL;}
    shspe_threads[0]->Run();
    //NOT HERE IN 0... if (dl_handle!=NULL)dlclose(dl_handle); // can be closed after all threads down

  }else{
    printf("MASTER THREAD ALREADY RUNS, TRY TO STOP IT FIRST%s....\nremaining processes:\n","");
    //    TThread::Ps();
    printf("MASTER THREAD ALREADY RUNS, TRY TO STOP IT FIRST%s....\nremaining processes:\n","");
  }

  return 0;// non void function 
}//=================================================######################### MAIN MAIN END







/**************************************************
 * muze se stat ze by byl empty a rekl non empty? pak pop je ale v mutexu
 *    
 */
void *evt_pusher( void *arg )  // loads the queue 
{
 struct thread_info *tinfo = (struct thread_info *) arg;
 int nt=tinfo->thread_num;
     evt_pusher_remote( (int*)&buffer ); //#########EVENT#########
  tinfo->running=0;// say stop
 printf("%s","evt_pusher - stop \n");
  return NULL;
}//--------void *evt_pusher( void *arg )  // loads the queue 







/************************************************************************
 *   very difficult - once there exists a ttree, we cannot return to TTree
 */
void *evt_poper( void *arg )  // reads the queue (pop)
{
 struct thread_info *tinfo = (struct thread_info *) arg;
 int nt=tinfo->thread_num;
 int call=tinfo->callnumber; tinfo->callnumber++;// to keep track opened file
 // printf("starting thread %d poper for the %dth time\n", nt, call );
 //     printf("removing ........bufsize==%10d   empty==%d \n", buffer.size(), buffer.empty() );
     evt_poper_remote(  (int*)&buffer  );
 printf("%s","evt_remote - stop \n");
  tinfo->running=0;// say stop
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
 struct thread_info *tinfo = (struct thread_info *) arg;
 int nt=tinfo->thread_num;
 int call=tinfo->callnumber; tinfo->callnumber++;// to keep track opened file
   evt_analyze_remote(  (int*)&buffer  ); //#########EVENT#########
 printf("%s","evt_analyze - stop \n");
  tinfo->running=0;// say stop
     return NULL;
}
