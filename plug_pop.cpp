#define POPDEBUG 0
#include "xml_attr.h"    // bude xml
#include "log_term.h"    // bude xml
#include "mut_queue.h"
#include "acq_core.h"
#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......
#include "ZH_data.h"

#include <stdlib.h>     /* atof */


//#include <iostream>  //#include <math.h>
#include <stdio.h>
#include "TROOT.h"    // Main INCLUDE !!!!
#include "TSystem.h" 
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"  //i cannot draw in standalone???
#include "TRandom.h"
#include "TVector3.h"
//#include <pthread.h>
// I DONT WANT THIS ANYMORE   #include "nano_acquis_pureconvert.C" 

#include "TSocket.h"   //net thread
//---------------------------try server (for a client with txt...)
#include "TServerSocket.h"
#include "TMonitor.h"

//----------------------------with   mmap ------------------------
#include <err.h>
#include <sys/mman.h>




/****
IF C++ => care about namemangling....
// because of mixing  c and c++, namemangling.....
 */

#ifdef  __cplusplus
extern "C" {
#endif




//                                                                         ===
// ========= Here I have something for   mmap=============================MMAP
//                                                                         ===

    int mmapfd;      //  =-1       file handle for mmap
    char *mmap_file; // pointer to     mmap











  /*****************************************************************ZH DATA
   *  I USE====================================
   *   ZH_data.C      
   *                 void ZH_data(int events);
   *
   *
   */
  int* PLUG(int* par, int* par2){// POP ... nanot ZD data 4*int system

   int datum=0;// one word from the concurent queue
   char chL[500];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   sprintf(chL,"POP: pop_ZH  : buff==%lld",(Long64_t)buffer);table_log(1,chL);

   //=============
   //HERE I DEFINE NEXT QUEUE
   concurrent_queue<int> *BUFANAL=(concurrent_queue<int>*)par2;  // empty queue(..,chan,..)
    sprintf(chL,"POP: pop_ZH  :buff2==%lld",(Long64_t)BUFANAL);table_log(1,chL);
  // send it by       (int*)&buffer 
   //=============

      //STANDARD  XML  READ-------------
      FILE *outfile;  char fname[400];
      int buffer4;
      long long int cnt=0;
      size_t result;   

//-------- here I will control with    control.mmap    file------   
    if ((mmapfd = open("control.mmap", O_RDWR, 0)) == -1) err(1, "open");
    mmap_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, mmapfd, 0);
    if (mmap_file == MAP_FAILED) errx(1, "either mmap");

    char mmap_result[100];
//-------- here I will control with    control.mmap    file------   

  char  acqxml2[100];
  char pushis[100];
  char definitions[2000];// channel definitions
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap
  double respush=1.0;// PUSH is running   "push="  token.....
  TSmallish_xml xml(    acqxml2   );
  xml.DisplayTele( xml.mainnode, 0, "plugins","poper","definitions" );
  sprintf( definitions  ,"%s", xml.output  );

  // things from 20130820
  //  OverrideFileOUT=NULL;// NO filename out  override
  sprintf( OverrideFileOUT, "%s", "" );
   ZHbuffer = (int*) malloc ( 10000 );


  OEBmax=1000;// ONE EVENT LIMIT !!!!!!!!!!
  DataRead=0; // HowMuch was read to buffer
  cnt_evt=0; // event number
  // I dont understand.....this should be in *.o 
 cTIME=0.0; // current data TIME (always>0)
 bTIME=0.0; // buffered time (mostly 0)
 sTIME=0.0; // startup time
 dTIME=0.0; // difference

  reset_chan_table();
  sprintf(chL,"POP_ZH: loading the channel table%s", "");table_log(1,chL);

  load_chan_table( definitions  );

  //  load_chan_table("circtree=100000,c001=1,c002=2,c003=3,c004=4,c005=5,c006=6,c007=7,c008=8,c017=17,c018=18,c019=19,c020=20,c021=21,c022=22,c023=23,c024=24,c032=32,c1024=t1,c1025=t2,c1026=t3,c1027=t4,c033=s001,c035=s002,c037=s003,c039=s004" );



  // int pos=0;//  this is position in the buffer...
  int c=0;//   position in OEbuffer..............

  while (respush>=1.0){//run while push is running........
      if (POPDEBUG!=0){sprintf(chL,"POP:  entering while/buffer c=%d", c);table_log(1,chL);}
      
      //      usleep(15000000);
    while( !buffer->empty() ){// concurent queue "buffer" is an object HERE

      buffer->wait_and_pop(datum);
      if (cnt %100 == 0){usleep(10);} // no delay@572k
      if (POPDEBUG!=0){sprintf(chL,"POP:  datum; cnt==%lld   /c=%d", cnt,c);table_log(1,chL);}

      if ((cnt%25000)==0){
	sprintf(chL,"POP:%7.1f MB %9lld evts; %d",
		4.*cnt/1000000, cnt_evt_data, buffer->size() );table_log(1,chL);
      } //printout every MB

      cnt++; 
      OEbuf[c] = datum;
      if (POPDEBUG!=0){sprintf(chL,"POP:oebuf[c]=%08x        %4d", OEbuf[c]    ,c);table_log(1,chL);}

      //buffer [c] is either  EOE  or  NOT
      if (OEbuf[c]==EOE){//END OF EVENT
      if (POPDEBUG!=0){sprintf(chL,"POP: proc ONE cnt==%lld", cnt);table_log(1,chL);}
      if (POPDEBUG!=0){sprintf(chL,"POP:oebuf=%08x %08x %08x", OEbuf[0],OEbuf[1],OEbuf[2]);table_log(1,chL);}
      if (POPDEBUG!=0){sprintf(chL,"POP:oebuf=%08x %08x %08x", OEbuf[c-2],OEbuf[c-1],OEbuf[c]);table_log(1,chL);}
	process_ONE_EVENT(OEbuf, (int*)BUFANAL  );
      if (POPDEBUG!=0){sprintf(chL,"POP: proc ONE cnt==%lld ok", cnt);table_log(1,chL);}
	c=0;// reset position in the buffer
      }//END OF EVENT
      else{
	c++;
      }
    }//BUFFER NOT EMPTY
    
    usleep(1000*10); // wait 100ms and retry again..
    respush=TokenGet( "run=" , mmap_file , pushis ); //takes a value
    //    if ( 0.0==TokenGet( "run=" , mmap_file , pushis ) ){
    //      respush=0.0;
    //    } //takes a value
    if (respush<1.0){break;}
    if (POPDEBUG!=0){sprintf(chL,"POP:respush!=0  c=%d size==%d",c,buffer->size() );table_log(1,chL);} 



  }//WHILE respush==0....push running...
  //  sprintf(chL,"Ending: respush==%1.0f; /%s/ ... \n/%s/", respush, pushis,mmap_file);table_log(1,chL);
  // if (respush<1.0){break;}


  if (ftree!=NULL){
    //  ZH_tree->Write();
    ftree->Write();
    ftree->Close();
  }
  sprintf(chL,"EXITING POP-ZH (bytes=%lld)", 4*cnt );table_log(1,chL);
 if (ZHbuffer != NULL){ 
    printf( "%s\n","...freeing the memory  /ZHbuffer/ (pop)\n");
    //    printf( "%s\n","...freeing the memory\n");
    //    printf( "%s\n","...freeing the memory\n");
    free (ZHbuffer); 
  }

  sprintf(chL,"EXITING POP-ZH (evnts=%lld)",cnt_evt );table_log(1,chL);
  sprintf(chL,"EXITING POP-ZH (data =%lld)",cnt_evt_data );table_log(1,chL);

}//pop_ZH_____________________________________________________________END___








  //============================== EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif
