#define ANADEBUG 0
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

//                                                                         ===
// ========= Here I have something for   mmap=============================MMAP
//                                                                         ===
int mmapfd;      //  =-1       file handle for mmap
char *mmap_file; // pointer to     mmap



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


  //=====================================================
  //==========================================
  //====================================
  //==        empty 
 int* evt_analyze_empty(int* par){
   int wait=1;
   char ch[400];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   sprintf(ch,"evt_analyze EMPTY...........%s","");table_log(2,ch);
    wait=MyCond.TimedWaitRelative( 300+700+2000  );
   if (wait==0){ 
      sprintf(ch,"POP got BROADCAST SIGNAL... %s", "" );table_log(2,ch);
    }
 }// ********************* end  of  function  ***********






//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS


//-------------------------- class definition ----------------- 1
class TACounter{

 public:
// protected:
  // int is limited to  4e6 !
  int DEBUGo;
  double delta;
  double markVO,  markV; // Value (integer)
  double markTO, markT;  // time 
  double currT, currV;   // currents , waiting to 
 
  TACounter();
  ~TACounter();
  void Register(double weight=1.0);
  double GetRate();
  double GetTime();
  void SetDebug(int deb);
};



TACounter::TACounter(  )
{ 
  printf("...creating counter%s\n","");
  DEBUGo=1;
  delta=10.0; //   10 second dT is fine for 2sec. netwrk fetch
  double now=GetTime();
  markVO=0.0;
  markTO=now - delta;
  markV=0.0;
  markT=now;  
  currT=now;
  currV=0.;
}

TACounter::~TACounter() { printf("...destroing counter%s\n","");}

// This makes it   in   seconds.............good for high rates
//                                     large error at low rates
double TACounter::GetTime(){
  //  TString sla;
  TDatime *dt=new TDatime();  // kTRUE previously...mess
  // sla.Append( dt->AsString() );//append date
  //  sla.Append( " : " );//append fname
  //  sla.Append( s_p.Data() );//append fname
  //  sla.Append( " : END" );//append openfile
  //  printf("%s\n",  sla.Data()  );
  return   1.0* dt->Get();
}// GetTime


double TACounter::GetRate(){
  return (markV-markVO)/(markT-markTO);
}

void TACounter::Register(double weight){
  double now=GetTime();
  currV=currV+weight;
  currT=now;
  if (currT - markT >= delta ){
    markVO=markV;
    markTO=markT;
    markV=currV;
    markT=currT;
  }//if 1 sec
  //  if (DEBUGo>0){ printf( "%09.3f ... %f/%f\n", GetRate() , currT, currV );}
  return;
}

void TACounter::SetDebug(int deb){
  DEBUGo=deb;
}

void test(){
  int i,imax=2;
  TACounter *c[2];
  for (i=0;i<imax;i++){
   c[i]=new TACounter;
  }

  for (i=0;i<imax;i++){
    delete c[i];
  }

}





//-------------------------- class definition ----------------- 2
class TACounterMulti{

 public:
// protected:
  // int is limited to  4e6 !
  double markTO, markT;  // time
  double currT, currV;   // currents , waiting to 

  static const int max=64;  // MAXIMUM   32  COUNTERS?
  int N;
  TACounter *mcounter[max];
  int i;
  int fd;  //  =-1       file handle for mmap
  char *cnt_file; // pointer to     mmap


  TACounterMulti( int counters );
  ~TACounterMulti();
  void Register(int counter, double weight=1.0);
  void GetRate();
  double GetTime(); // same
  void Display();
};



double TACounterMulti::GetTime(){

  TDatime *dt=new TDatime();  // kTRUE previously...mess
  return   1.0* dt->Get();
}// GetTime


TACounterMulti::TACounterMulti( int counters  ){ 
  markT=GetTime()-1;
  markTO=markT-1;
  currT=markT;

  N=max;
  if (N>counters){ N=counters;}
  printf("...creating multicounter, # of counters is %d\n", N);
  
  for (i=0;i<N;i++){
    printf(" %d)", i );
    mcounter[i]=new TACounter();
    mcounter[i]->SetDebug(0);
  }
  //  MAPadCreate("COUNTERS",1,N);
  // HERE I MUST CREATE     mmap  <<<  ========------------------------MMAP 
  //  int fd = -1;
  const char str1[] = "          \n";
  //  char  *cnt_file;  //pointer
  system("dd if=/dev/zero of=counters.dat  bs=4096  count=1");
  if ((fd = open("counters.dat", O_RDWR, 0)) == -1) err(1, "open");
  cnt_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
  if (cnt_file == MAP_FAILED) errx(1, "either mmap");
  strcpy(cnt_file, str1); 
  //  munmap(cnt_file, 4096);        
  //  close(fd);

  //==================-------------------------------------------------MMAP
}//------------------constructor


TACounterMulti::~TACounterMulti() { 
  printf("...destroing multicounter%s\n","");
    for (i=0;i<N;i++){
      delete mcounter;
  }
 munmap( cnt_file , 4096);
 close(  fd );
}//-------------------------destructor


void TACounterMulti::GetRate(){
    for (i=0;i<N;i++){
      //   printf("%d/%d - \n", i , N );
      //     if ((i>=0)&&(i<N) ) {
      //HERE     printf("%9.2f ",mcounter[i]->GetRate( ) );
       //     }   
    }
    //  printf("%s","\n");
}//---------------------------

void TACounterMulti::Register(int counter, double weight){
  if ((counter>=0)&&(counter<N) ) {
    mcounter[counter]->Register( weight );
  }else{
    printf(" counter not in the predefined range (0-%d)\n", N );
  }
  currT=GetTime();
  if (currT>=markT+1){  
    markT=currT;   
    Display();
  }// once per second autodisplay (max.)
  return;
}//----------------------------



void TACounterMulti::Display(){
  char c[40]; 
  TString s="";
  // //---taken from MySql  multipads2-----------
  // TCanvas *cc;
  // if (MAPadGetByName( "COUNTERS" )==NULL){
  //   cc=new TCanvas( "COUNTERS","COUNTERS" );
  //   cc->Divide(1 ,N, 0.002,0.002 );
  //   cc->GetCanvas()->SetFillColor(9);cc->GetCanvas()->SetFillStyle(1);
  //   cc->Draw();
  // }else{
  //   TPad *pp=(TPad*)MAPadGetByName( "COUNTERS" );cc=pp->GetCanvas();
  // }// I HAVE cc CANVAS now...

  //---------------------------------------------

  // ===   IT CAN SEEM STOPPED === NO WAY TO SEE THE notcomming COUNTS ?????==
  // GetRate();   
    for (i=0;i<N;i++){
      //   printf("%d/%d - \n", i , N );
      //     if ((i>=0)&&(i<N) ) {
      sprintf(c, "%3d.  %10.1f\n",i, mcounter[i]->GetRate( ) );
      s.Append( c );
      //      MAPadPrintInP("COUNTERS", i+1,  c );
       //     }   
    }
    //    printf("%s","\n");
    //    cc->Modified();cc->Update();
    sprintf( cnt_file, "%s", s.Data() );
    //    strcpy( cnt_file , s.Data()  );   // preklop do filu (mmap) obsah citacu...
}//---------------------------




//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS



  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================




  //================= TEST TO GET     ZH_tree
 int* evt_analyze_gettree(int* par){
   char ch[1000];
//-------- here I will control with    control.mmap    file------   
    if ((mmapfd = open("control.mmap", O_RDWR, 0)) == -1) err(1, "open");
    mmap_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, mmapfd, 0);
    if (mmap_file == MAP_FAILED) errx(1, "either mmap");
    char mmap_result[100];
//-------- here I will control with    control.mmap    file------   

  char  acqxml2[100];
  char definitions[2000];// channel definitions
  int delayms=100*1000; // delay can help with less printout....
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap
  TSmallish_xml xml(    acqxml2   );
  xml.DisplayTele( xml.mainnode, 0, "plugins","poper","definitions" );
  sprintf( definitions  ,"%s", xml.output  );

  xml.DisplayTele( xml.mainnode, 0, "plugins","analyze","delayms" );
  sprintf( acqxml2  ,"%s", xml.output  );
  delayms=atoi( acqxml2 ); if (delayms==0){ delayms=100*1000; }

   //NOT!!  concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   sprintf(ch,"evt_analyze_gettree." );   table_log(2,ch);
 //-----------------typical creation of TGraphErrors--------------
 //-----------------typpical creation of 2D matrix-------------y,x-
 //-------------------------- typical load of cuts -----------------
 //------------------------------- typical creation of 1d spectra ---



   // I move some control variables to here---------------------------
   Long64_t ii;    // main  for   LOOP control
   Long64_t prev;  // previous N - check for escaped events 
   int FLAG_process;  // 1==  work with the event; 0== not a good event 
   int reprint;       // just avoid duplicite printing of repeating events
   int reprint_nodata;// just avoid duplicite printing of repeating events

   //======================================SET of variables for TREE===
   int MAXCHAN=2048;
  //-------------variables for tokens-------
  char tok[10];
  char tokres[100];
  int toki;
  char brname[100];   //


  // SHADOWED SET OF VARIABLES CONNECTED TO TTREE============ a****
   Double_t acTIME_root; // time
   Double_t acTIME_root_start=0.0; // time
   Long64_t acnt_evt;   // event number
   UShort_t aTREE[MAXCHAN];  
   UShort_t aCOUNTER[MAXCHAN];  
   UShort_t aZERO[MAXCHAN]; 
   //   int refer[MAXCHAN]; // e.g. refer[0]==channel; aTREE[refer[0]]==1254
   //   int refermax=0;     // i go max up to 17 when checking
   // NO - typical task is to  plot aTREE[0]:aTREE[17]



   /* DUPLICIT to  evnum
   TH1F *h_events; // THING to monitor analyzed event number 
   h_events=(TH1F*)gDirectory->Get("h_events");
   if (h_events==NULL){
     h_events=new TH1F("h_events","h_events",500000,0,500000);
   }
   */

#include "plug_analyze_definitions.cpp"

   TTree *tree_addr_old;
   TTree *tree_addr;
   tree_addr_old=NULL;
   tree_addr=NULL;
   //THESE MUST SURVIVE=====================================
   //this is pointer
   Long64_t   entr;             // number of entries in the tree
   Long64_t  circular_bias=0;  // important to have both - pos/neg 
   Long64_t  total_processed=0;// good q.number
   Long64_t  lost_blocks=0;     // ++ any irregularity
   Long64_t  repeated_evts=0;  // ++ when evt_number < last_evt_n
   Long64_t  skipped_evts=0;   // ++ when evt_number > last_evt_n+1
   Long64_t  estim_span=0;   // estimated from #(@circ)..#(@entr)

   //this is value contained
   Long64_t  last_event_n=-1;   // my last processed event (#)

TH1F* evnumB; // throwing
TH1F* evnumC; // throwing

   int respop=1;// POP is running   "pop="  token.....

   while (respop==1){//INFINITE==========================

   while (tree_addr_old==NULL){
     usleep(1000*100);
     if (ANADEBUG){sprintf(ch,"..trying to get the tree-huh" );   table_log(2,ch);}
     tree_addr_old=(TTree*)gDirectory->FindObject("nanot");
   }
   if (ANADEBUG){sprintf(ch,"..got the tree. %llx", (Long64_t)tree_addr_old);table_log(2,ch);}
   tree_addr=(TTree*)tree_addr_old->Clone();
   tree_addr->SetTitle("CLONE");
   tree_addr->SetMakeClass(1);

   entr=0;
   while (entr<=0){
     entr=tree_addr->GetEntries(); 
     if (ANADEBUG){ sprintf(ch,"entry#%6d = %6lld",  0,  acnt_evt);table_log(2,ch); }
     if (entr<=0){ 
       usleep(100000);//wait a 100 milisecond for events to appear
       tree_addr=(TTree*)tree_addr_old->Clone(); 
       tree_addr->SetTitle("CLONE"); 
       tree_addr->SetMakeClass(1);
     }
   }

   if (ANADEBUG){sprintf(ch,"cloned the tree %llx", (Long64_t)tree_addr );table_log(2,ch);}

   tree_addr->SetBranchAddress( "time" ,&acTIME_root );
   tree_addr->SetBranchAddress( "cnt_evt" ,&acnt_evt );// /i == UInt_t 32bit

   for (int i=1; i< MAXCHAN; i++){//  c0001-c2000
     aZERO[i]=0;// ZERO ZERO forever
     sprintf( tok, "c%03d=", i ); // if  c002=2 ===>>> data channel
     toki=TokenGet( tok, definitions,tokres);//get the integer after c001=

     if (toki!=0){// integer was found => it is HISTO------------
      // DEFINE TTREE...........for every histo channel....
       sprintf( brname,  "V%03d", toki );//  branch  c001 
       tree_addr->SetBranchAddress( brname ,&aTREE[toki] );

     }else if(strlen(tokres)>0){//-----NOT HISTO-----------------

       if ( strstr(tokres,"s")==tokres){//counter--- =s001, =s002...
	 sprintf( brname,  "%s", tokres );//  branch  c001 
	 char num[15];	 strcpy( num,tokres); num[0]='0'; 
	 tree_addr->SetBranchAddress( brname ,&aCOUNTER[ atoi(num) ] );
	 
       }//------------------------------counter-------------end
     }//-------------------------------NOT HISTO--------------END
   }//for (int i=1; i< MAXCHAN; i++)-------------

   if (ANADEBUG){sprintf(ch,"clear everything%s...", "" );table_log(2,ch);}

   memcpy ( aTREE,    aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 
   memcpy ( aCOUNTER, aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 




   //-------===============from here I can repeat=======----------

   if (ANADEBUG){sprintf(ch,"entries== %6lld",entr );table_log(2,ch);}
   //EVERY LOOP I redefine   circular_bias;last_event_n

   tree_addr->GetEntry(0,1);// this really starts at event #1

   if (ANADEBUG){ sprintf(ch,"entry#%6d = %6lld",  0,  acnt_evt);table_log(2,ch); }

   circular_bias=last_event_n-acnt_evt+1;
   if (circular_bias<0){ circular_bias=0;}

   tree_addr->GetEntry(circular_bias,1);

 if (reprint_nodata==0){
   if (skipped_evts>0){
     sprintf(ch,"cir%6lld(+%6lld)  %6lld<- L# %6lld  - %6lld!", 
	     circular_bias, entr-1 -circular_bias, 
	     acnt_evt,last_event_n, skipped_evts);table_log(2,ch);
   }else{
     sprintf(ch,"cir%6lld(+%6lld)  %6lld<- L# %6lld", 
	     circular_bias, entr-1 -circular_bias,
	     acnt_evt,last_event_n);table_log(2,ch);
   }
 }//nodata == 0 =>  can reprint....

   if (ANADEBUG){
     tree_addr->GetEntry(1,1);// this really starts at event #1
     sprintf(ch,"entry#%6d = %6lld",  1,  acnt_evt);table_log(2,ch);

     tree_addr->GetEntry(circular_bias,1);// this really starts at event #1
     sprintf(ch,"entry#%6lld = %6lld",circular_bias,acnt_evt);table_log(2,ch);

     tree_addr->GetEntry(circular_bias+1);// this really starts at event #1
     sprintf(ch,"entry#%6lld = %6lld",circular_bias+1,acnt_evt);table_log(2,ch);
     tree_addr->GetEntry(circular_bias+2);// this really starts at event #1
     sprintf(ch,"entry#%6lld = %6lld",circular_bias+2,acnt_evt);table_log(2,ch);
   }
   if (ANADEBUG){
     tree_addr->GetEntry(entr-2);// this really starts at event #1
     sprintf(ch,"entry#%6lld = %6lld", entr-2,acnt_evt);table_log(2,ch);
   }
   tree_addr->GetEntry(entr-1,1 );// this really starts at event #1
   estim_span=acnt_evt-last_event_n;//ESTIMATED SPAN...........
   if (ANADEBUG){
     sprintf(ch,"entry#%6lld = %6lld", entr-1,acnt_evt);table_log(2,ch);
   }//------------------------------------ end of DEBUG pritnouts------


   //prepare fo LOOP
   //   int64_t ii;
   prev=last_event_n; //this can keep better track than last_e_n SET HERE

   //the only interruption up to now
   if (circular_bias>=entr){
     if (reprint_nodata==0){
       sprintf(ch,"       %s                    nodata..." ,"");table_log(2,ch);      
       sprintf(ch,"...       %9.2f           %9.2f", acTIME_root, acTIME_root-acTIME_root_start ); table_log(2,ch);
     }
     reprint_nodata++;
     usleep(1000*2); // was wait 200ms!!
     respop=TokenGet( "pop=" , mmap_file , acqxml2 ); //takes a value    
     //there is a big while loop- breaks with respop!=1
   }// no data


   if ( (entr-circular_bias)!=  estim_span ){
     sprintf(ch,"run4 %6lld evts (est.%6lld)   AdHoc",
	     entr-circular_bias,estim_span);table_log(2,ch);  
     sprintf(ch,"          %9.2f           %9.2f", acTIME_root, acTIME_root-acTIME_root_start );
     table_log(2,ch);
   }else{
     //     sprintf(ch,"run4 %6ld evts              ..",
     //	     entr-circular_bias);table_log(2,ch);  
   }

   FLAG_process=1; // normaly, do it
   reprint=0;
   if (ANADEBUG){ sprintf(ch,"going to enter CIRCULAR LOOP %lld - %lld", circular_bias,entr );table_log(2,ch); }

   for(  ii=circular_bias; ii<entr; ii++){//.......FOR LOOP BEGIN

     tree_addr->GetEntry(ii,1);   
     if (acTIME_root_start==0.0){acTIME_root_start=acTIME_root;}//set root start time


     if (prev+1 != acnt_evt){	// IF THE ORDER IS INTERRUPTED:
       if (reprint==0){
       sprintf(ch,"!@ %6lld .. prev=%6lld;curr=%6lld **********", 
	       ii,prev,acnt_evt); table_log(2,ch); 
       reprint++;// after the end, they jump by 2! useless printouts
       lost_blocks++; // I resign on the remaining part
       }//if reprint
       prev=acnt_evt-1; // RESET "prev"

     }//  IF   prev+1  !=  current______> lost block______ IF NOT SEQUENTIAL

     // I must put this here, it directs whether flag works or not
     if (acnt_evt<=last_event_n){
       repeated_evts++;
       FLAG_process=0;

       if ( (acTIME_root>0.0)&&(evnumB==NULL) ){//already elsewhere
	 evnumB=new TH1F("T_EventA_non","Throwing repeated events #",  300000, 
			 0, 300000 );
       }
       evnumB->Fill( acnt_evt  );

     }// repeating
     if (acnt_evt>last_event_n+1){// if skipped block (not seen yet)
       skipped_evts+=acnt_evt-last_event_n-1;
       FLAG_process=1;
       if ( (acTIME_root>0.0)&&(evnumC==NULL) ){//already elsewhere
	 evnumC=new TH1F("TimeEvNC","Skipped starting with event #",  300000, 
			 0, 300000 );
       }
       evnumC->Fill( acnt_evt  );
       
     }//skiiped


     if (FLAG_process==1){//NORMAL PROCESSING........BEGIN
       //=================================================
       prev++;
       reprint_nodata=0; //enable nodata mgs print
       total_processed++;
       last_event_n=acnt_evt;//last processed

       //DUPLICIT to evnum     h_events->Fill( acnt_evt );


      #include "plug_analyze_actions.cpp"


       memcpy ( aTREE,    aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 
       memcpy ( aCOUNTER, aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 

       //=================================================
     }//--------------------NORMAL PROCESSING........END
     //     else{
     //     }
   }//for(  ii=circular_bias; ii<entr; ii++).........FOR LOOP  END

   
   tree_addr->Delete(); // clean your CLONE
   if (last_event_n<0) break;// not seen yet
   //   tree_addr_old=NULL;  // why...
   }// while (respop==1) ================ INFINITE =================





  sprintf(ch,"EXITING AN (repeated  =%lld)",repeated_evts );table_log(2,ch);
  sprintf(ch,"EXITING AN (skipped   =%lld)",skipped_evts  );table_log(2,ch);
  sprintf(ch,"EXITING AN (lost blks.=%lld)",lost_blocks  );table_log(2,ch);
  sprintf(ch,"EXITING AN (total_proc=%lld)",total_processed);table_log(2,ch);
  sprintf(ch,"EXITING AN (last_evt_N=%lld)", last_event_n );table_log(2,ch);
 }//==========================================evt_analyze_gettree===END














  //================================================= EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif
//      ojr@edie:~$ od -v -h -A n  00_central/Root/03_programs/58_kibbler_narval/RUN018_V | perl -p -e 's/\n//g;s/0000 f000/0000 F000\n/g' | less

