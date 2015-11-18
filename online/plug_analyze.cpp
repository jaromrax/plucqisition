#define ANADEBUG 1
#include "xml_attr.h"     
#include "log_term.h"     
#include "mut_queue.h"
//#include "acq_core.h"  // 
#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......
//---- I include ZH_data.h -------
//#include "ZH_data.h"

#include "TDirectory.h"

//----------------------------with   mmap ------------------------
#include <err.h>
#include <sys/mman.h>

//#include "nanot.h"   // HERE I PUT THE TTREE CLASS +======================
//                                                                         ===
// ========= Here I have something for   mmap=============================MMAP
//                                                                         ===
int mmapfd;      //  =-1       file handle for mmap
char *mmap_file; // pointer to     mmap
char repla[4096];


//  I make the variable  "extern" in   *.h  and againI define in *.C
//  then - when I include here (so) and I try to link *.o that was created before
//         it works: var is allocated in *.o, reffered from *.h....................




  // I should define the variables that are declared (extern) in header
// I cannot compile on DEBIAN/Aron      
//         TCondition MyCond(0);
// FILE* XTERM; When it is declared in logtermh, defined in log_term.C, no prob. here
//------------------------------------------ My sources finished-----

#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

//#include <iostream>  //#include <math.h>
#include "TROOT.h"    // Main INCLUDE !!!!
#include "TSystem.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"  //i cannot draw in standalone???
#include "TRandom.h"
#include "TVector3.h"
//#include <pthread.h>
#include "TSocket.h"   //net thread
#include <math.h>   //net thread
#include "TGraphErrors.h"   //net thread

//---------------------------- with mpad and tcounter comes::::::
//#include "TText.h"
//----------------------------with   mmap ------------------------
#include <err.h>
#include <sys/mman.h>

/**********compile command:
g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o 
IF C++ => care about namemangling....
 */

// because of mixing  c and c++, namemangling.....
#ifdef  __cplusplus
extern "C" {
#endif





  int* PLUG(int* par, int* par2){

   char ch[400];
   int firsttime=0;
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   int datum=0;
   sprintf(ch,"evt_analyze EMPTY2...........%s","");table_log(2,ch);

   sprintf(ch,"EXITING NOW %s" , "" );table_log(2,ch);

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
  char treename[2000];// channel definitions
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap


  double respush=1.0;// PUSH is running   "push="  token.....
  TSmallish_xml xml(    acqxml2   );
  xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","treename" );
  sprintf( treename  ,"%s", xml.output  );
  outfile=NULL;
  // outfile=fopen( "analyze.empty2","wb" );//-OPEN FILE append and write

  int cnt2=0;
  int cnt3=0;
  Long64_t events=0;
  const int MAXCHANA=100;
  int chan[MAXCHANA];
  int datum2=0;
  for (int i=0;i<MAXCHANA;i++){ chan[i]=0; }
 //================================================DEFINE MATRICES====

  usleep(1000*100);
#include "plug_analyze_definitions.cpp"


  sprintf(ch,"ANA: after definitions%s","");table_log(2,ch);

  printf("ANA %s\n"," start");
  //============================================== MAIN WHILE LOOP =========
  while (respush>=1.0){//run while push is running........
    /*
      HERE I USE CONCURENT QUEUE BUFFER but this time special and easy:
      one (int)    is channel
      second (int) is value
      0xffffffff   twice  is  EOEvent
     */

    //    if (ANADEBUG>0){sprintf(ch,"ANA: entering while bufferempty%s","");table_log(2,ch);}


    while( !buffer->empty() ){// concurent queue "buffer" is an object HERE
      buffer->wait_and_pop(datum);cnt++;
      
      if (datum==0xffffffff){
	if (cnt2!=0){cnt3++;}//cnt3==real event#   ffffffff is twice!
	cnt2=0; // cnt2 - words in event block;
	// sprintf(ch,"ANA: ffffff%s","");table_log(2,ch);
      }else{cnt2++;}//datum 0xffffff

      if (cnt2==1){ //====================================================
	//------------------analyze here --------------


	//	if (ANADEBUG>0){sprintf(ch,"ANA: before actions%s","");table_log(2,ch);}


	
	#include "plug_analyze_actions.cpp"




	
	//	if (ANADEBUG>0){sprintf(ch,"ANA: after actions evt=%lld", events);table_log(2,ch);}

	//------------------analyze here --------------	 
	for (int i=0;i<MAXCHANA;i++){ chan[i]=0; } 	 events++;
	//------------------analyze here --------------
      }//  cnt2== fffffffff    ===== END OF THE EVENT  
 

     if (cnt2 % 2==1){// odd is always channel
	buffer->wait_and_pop(datum2);cnt++;cnt2++;
	if (datum<MAXCHANA){chan[datum]=datum2;}
	//  if (outfile!=NULL){fwrite ( &datum , 1 , 4  , outfile );fwrite ( &datum2 , 1 , 4  , outfile );}
      }
      
      if ((cnt%10000)==0){
	sprintf(ch,"ANA:%8lld kB %9lld events /%d",4*cnt/1000, events,cnt2);table_log(2,ch);
      } //printout every MB
      

    }//buffer not empty  
    //    sprintf(ch,"ANA:  buffer empty%s","");table_log(2,ch);
    usleep(1000*2000);
     
    respush=TokenGet( "push=" , mmap_file , pushis ); //takes a value
    if ( 0.0==TokenGet( "run=" , mmap_file , pushis ) ){
      respush=0;
    }; //takes a value to know when to STOP

    if ( 9.0==TokenGet( "run=" , mmap_file , pushis ) ){
       sprintf(ch,"reload was demanded  (words=%lld)",cnt );table_log(2,ch);
       TokenReplace( "run=",  "run=2.0",  mmap_file , repla );
       strcpy( mmap_file, repla );
       #include "plug_analyze_definitions.cpp"
    }; //
    
  }//respush>=0 ================================== WHILE LOOP END ============
  
  if (outfile!=NULL){fclose(outfile);}
  sprintf(ch,"EXITING analyze (words=%lld)",cnt );table_log(2,ch);
  sprintf(ch,"EXITING analyze (events+times=%d)",cnt3 );table_log(2,ch);

 
 }// ********************* end  of  function  **EMPTY2 ******
}




  //================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================
  //=================================================================================


