#ifndef acq_core_H
#define acq_core_H

#include "TThread.h"   //  CINT?   //  -lThread


const char acq_default[4096]="file=acq_setup.xml\nrun=1\ntest=123\nargument=654321";


/*************************************************
 *  THREAD definitions - 
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
{  ifstream ifile(filename);  return ifile;}
*/

 struct thread_info *tinfo;// create n threads ..... array. calloc:

  /*****************************
   *  I start dlopen rather here....................... PUSHER PART
   */
  void* dl_handle_push;
  void* dl_handle_pop;
  void* dl_handle_analyze;

void *xml_masterthread(void* arg);// gets xml from mmap file

#endif
