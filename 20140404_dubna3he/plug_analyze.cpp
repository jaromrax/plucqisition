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
  int chan[32];
  int datum2=0;
  for (int i=0;i<32;i++){ chan[i]=0; }
 //================================================DEFINE MATRICES====
#include "plug_analyze_definitions.cpp"
  usleep(1000*1000);

  sprintf(ch,"ANA: after definitions%s","");table_log(2,ch);

  printf("ANA %s\n"," start");
  while (respush>=1.0){//run while push is running........
    /*
      HERE I USE CONCURENT QUEUE BUFFER but this time special and easy:
      one (int)    is channel
      second (int) is value
      0xffffffff   twice  is  EOEvent
     */
    while( !buffer->empty() ){// concurent queue "buffer" is an object HERE
      buffer->wait_and_pop(datum);cnt++;
      
      if (datum==0xffffffff){
	if (cnt2!=0){cnt3++;}//cnt3==real event#   ffffffff is twice!
	cnt2=0; // cnt2 - words in event block;
	// sprintf(ch,"ANA: ffffff%s","");table_log(2,ch);
      }else{cnt2++;}//datum 0xffffff
      if (cnt2==1){ //====================================================
	//------------------analyze here --------------
	//	 	 if (chan[17]>0) {mtx1->Fill( chan[1]+chan[17], chan[17]);}
	//	 sprintf(ch,"ANA: analyze action%s","");table_log(2,ch);
	
#include "plug_analyze_actionsB.cpp"
	
	
	//------------------analyze here --------------	 
	for (int i=0;i<32;i++){ chan[i]=0; } 	 events++;
	//------------------analyze here --------------
      }//  cnt2== fffffffff    ===== END OF THE EVENT  
      if (cnt2 % 2==1){// odd is always channel
	buffer->wait_and_pop(datum2);cnt++;cnt2++;
	chan[datum]=datum2;
	//  if (outfile!=NULL){fwrite ( &datum , 1 , 4  , outfile );fwrite ( &datum2 , 1 , 4  , outfile );}
      }
      
      if ((cnt%10000)==0){
	sprintf(ch,"ANA:%8lld kB %9lld events /%d",4*cnt/1000, events,cnt2);table_log(2,ch);
      } //printout every MB
      

    }//buffer not empty  
    sprintf(ch,"ANA:  buffer empty%s","");table_log(2,ch);
    usleep(1000*500);
     
    respush=TokenGet( "push=" , mmap_file , pushis ); //takes a value
    if ( 0==TokenGet( "run=" , mmap_file , pushis ) ){
      respush=0;
    }; //takes a value to know when to STOP
  }//respush>=0
  
  if (outfile!=NULL){fclose(outfile);}
  sprintf(ch,"EXITING analyze (words=%lld)",cnt );table_log(2,ch);
  sprintf(ch,"EXITING analyze (events+times=%d)",cnt3 );table_log(2,ch);

 
 }// ********************* end  of  function  **EMPTY2 ******





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

     tree_addr_old=NULL;
     while (tree_addr_old==NULL){
       usleep(1000*1000);
       if (ANADEBUG){sprintf(ch,"..trying to get the tree-huh" );   table_log(2,ch);}
       tree_addr_old=(TTree*)gDirectory->FindObject("nanot");
     }

   
    entr=0;
    while (tree_addr_old->GetEntries()<100){
     usleep(1000*1000);
     if (ANADEBUG){sprintf(ch,"..trying to get the tree-huh2" );   table_log(2,ch);}
   }
   


   if (ANADEBUG){sprintf(ch,"..got the tree. %llx", (Long64_t)tree_addr_old);table_log(2,ch);}
   if (tree_addr_old!=NULL){

     if (tree_addr!=NULL){ tree_addr->Delete();}

     
     tree_addr=(TTree*)tree_addr_old->Clone();
     tree_addr->SetName("CLONE");
     tree_addr->SetTitle("CLONE");
     tree_addr->SetMakeClass(1);
     
     /*
     tree_addr=tree_addr_old->CloneTree(0 );
     tree_addr_old->CopyEntries( tree_addr, kTRUE ); //to separate from the tree
     tree_addr_old->CopyAddresses( tree_addr, kTRUE ); //to separate from the tree
     */

     if (ANADEBUG){sprintf(ch,"cloned the tree %llx", (Long64_t)tree_addr );table_log(2,ch);}

     tree_addr->SetBranchAddress( "time" ,&acTIME_root );
     tree_addr->SetBranchAddress( "cnt_evt" ,&acnt_evt );// /i == UInt_t 32bit
     //     tree_addr->SetBranchStatus("

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
   }//tree_addr_old != NULL << exists......................

   if (ANADEBUG){sprintf(ch,"clear everything%s...", "" );table_log(2,ch);}

   memcpy ( aTREE,    aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 
   memcpy ( aCOUNTER, aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 



    entr=tree_addr->GetEntries();

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
     if (respop>1.0){
       respop=TokenGet( "push=" , mmap_file , acqxml2 ); 
     }
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




       //       #include "plug_analyze_actions.cpp"





       memcpy ( aTREE,    aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 
       memcpy ( aCOUNTER, aZERO,  sizeof( UShort_t)*MAXCHAN ) ;  //FAST CLEAR 

       //=================================================
     }//--------------------NORMAL PROCESSING........END
     //     else{
     //     }
   }//for(  ii=circular_bias; ii<entr; ii++).........FOR LOOP  END



   
   if (tree_addr != NULL ){
     tree_addr->Delete(); // clean your CLONE
     tree_addr=NULL;
   }   

   if (ANADEBUG){ sprintf(ch,"AFTER CIRCULAR LOOP %lld - %lld", circular_bias,entr );table_log(2,ch); }

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



/*



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

*/


///////////////////////////////////////////////////////////
/*
//   DUPLICIT to  evnum
   TH1F *h_events; // THING to monitor analyzed event number 
   h_events=(TH1F*)gDirectory->Get("H_events");
   if (h_events==NULL){
     h_events=new TH1F("H_events","unique event numbers in original tree",500000,0,500000);
   }
  ////// DUPLICIT to  evnum
   TH1F *h_eventsA; // THING to monitor analyzed event number 
   h_eventsA=(TH1F*)gDirectory->Get("H_eventsA");
   if (h_eventsA==NULL){
     h_eventsA=new TH1F("H_eventsA","unique event numbers in analyze",500000,0,500000);
   }
   
  // nanot NT((TTree*) gDirectory->FindObject(   treename  ) );//------

   // Declaration of leaf types
   Double_t        time;
   Long64_t        cnt_evt;
   UShort_t        V001;
   UShort_t        V002;
   UShort_t        V003;
   UShort_t        V004;
   UShort_t        V005;
   UShort_t        V006;
   UShort_t        V007;
   UShort_t        V008;
   UShort_t        V017;
   UShort_t        V018;
   UShort_t        V019;
   UShort_t        V020;
   UShort_t        V021;
   UShort_t        V022;
   UShort_t        V023;
   UShort_t        V024;
   UShort_t        V032;
   UShort_t        s001;
   UShort_t        s002;
   UShort_t        s003;
   UShort_t        s004;
   UShort_t        s101;
   UShort_t        s102;
   UShort_t        s103;
   UShort_t        s104;
 // List of branches
   TBranch        *b_time;   //!
   TBranch        *b_n;   //!
   TBranch        *b_V001;   //!
   TBranch        *b_V002;   //!
   TBranch        *b_V003;   //!
   TBranch        *b_V004;   //!
   TBranch        *b_V005;   //!
   TBranch        *b_V006;   //!
   TBranch        *b_V007;   //!
   TBranch        *b_V008;   //!
   TBranch        *b_V017;   //!
   TBranch        *b_V018;   //!
   TBranch        *b_V019;   //!
   TBranch        *b_V020;   //!
   TBranch        *b_V021;   //!
   TBranch        *b_V022;   //!
   TBranch        *b_V023;   //!
   TBranch        *b_V024;   //!
   TBranch        *b_V032;   //!
   TBranch        *b_s001;   //!
   TBranch        *b_s002;   //!
   TBranch        *b_s003;   //!
   TBranch        *b_s004;   //!
   TBranch        *b_s101;   //!
   TBranch        *b_s102;   //!
   TBranch        *b_s103;   //!
   TBranch        *b_s104;   //!


   /////////////////////////////////////////////////////////////////////////////////////////////
   Long64_t newok=0;
   Long64_t cnt_evt_last=0;
  while (respush>=1.0){//run while push is running........
    usleep(10*1000);

    while (tree_ori==NULL){
      usleep(1000*500);
      if (ANADEBUG){sprintf(ch,"..trying to get the tree-huh" );   table_log(2,ch);}
      tree_ori=(TTree*)gDirectory->FindObject(   treename  ); //nanot
    }


    if (firsttime==0){
      firsttime++;
      //      tree_ori->MakeClass(0);
      //asi nelze      gROOT->ProcessLine(".L nanot.C+");
      //   tree_ori->SetMakeClass(1);

   //   TFile *nowFile=gFile;
   //   TFile newfile("/tmp/123.root","RECREATE") ;   
   
   //   nowFile->cd();
   //   tree_cpy=(TTree*)tree_ori->Clone();
   //   tree_cpy->SetMakeClass(1);
   //   tree_cpy->SetCircular(300000);
    }// firsttime


    entr_ori=tree_ori->GetEntries();
    h_events->Fill(  entr_ori );



    if (entr_ori_last!=entr_ori){

      //      tree_cpy->CopyEntries( tree_ori  ); 
      //KOPY TRIII     

//         TFile *f=new TFile("file.root","RECREATE");
  //    if (f->IsZombie()) {
//	printf("%s","Error opening file\n");
//	exit(-1);
  //    }
    
      if (tree_cpy!=NULL){ delete tree_cpy; }
      tree_cpy = new TTree("nanotBIS","Klon");  
      //      tree_cpy = new TTree("nanotBIS","Klon");
      tree_cpy=tree_ori->CopyTree( "", "", entr_ori-entr_ori_last,  entr_ori_last ); 
      //      tree_cpy->SetCircular(10000);
      tree_cpy->SetBranchAddress("time", &time, &b_time);
      tree_cpy->SetBranchAddress("cnt_evt", &cnt_evt, &b_n);
      tree_cpy->SetBranchAddress("V001", &V001, &b_V001);
      tree_cpy->SetBranchAddress("V002", &V002, &b_V002);
      tree_cpy->SetBranchAddress("V003", &V003, &b_V003);
      tree_cpy->SetBranchAddress("V004", &V004, &b_V004);
      tree_cpy->SetBranchAddress("V005", &V005, &b_V005);
      tree_cpy->SetBranchAddress("V006", &V006, &b_V006);
      tree_cpy->SetBranchAddress("V007", &V007, &b_V007);
      tree_cpy->SetBranchAddress("V008", &V008, &b_V008);
      tree_cpy->SetBranchAddress("V017", &V017, &b_V017);
      tree_cpy->SetBranchAddress("V018", &V018, &b_V018);
      tree_cpy->SetBranchAddress("V019", &V019, &b_V019);
      tree_cpy->SetBranchAddress("V020", &V020, &b_V020);
      tree_cpy->SetBranchAddress("V021", &V021, &b_V021);
      tree_cpy->SetBranchAddress("V022", &V022, &b_V022);
      tree_cpy->SetBranchAddress("V023", &V023, &b_V023);
      tree_cpy->SetBranchAddress("V024", &V024, &b_V024);
      tree_cpy->SetBranchAddress("V032", &V032, &b_V032);
      tree_cpy->SetBranchAddress("s001", &s001, &b_s001);
      tree_cpy->SetBranchAddress("s002", &s002, &b_s002);
      tree_cpy->SetBranchAddress("s003", &s003, &b_s003);
      tree_cpy->SetBranchAddress("s004", &s004, &b_s004);
      tree_cpy->SetBranchAddress("s101", &s101, &b_s101);
      tree_cpy->SetBranchAddress("s102", &s102, &b_s102);
      tree_cpy->SetBranchAddress("s103", &s103, &b_s103);
      tree_cpy->SetBranchAddress("s104", &s104, &b_s104);
      tree_cpy->SetName("nanotBIS");
      tree_cpy->SetTitle("clone of the original nanot");

      if (ANADEBUG){sprintf(ch,"ori  - last      %lld - %lld", entr_ori,entr_ori_last );table_log(2,ch);}

      entr_cpy=tree_cpy->GetEntries(); // entries in CPY


     Long64_t iii;
      for (iii=0; iii<= entr_cpy; iii++){
	//for (iii=entr_ori_last; iii<entr_ori; iii++){
	tree_cpy->GetEntry( iii , 1 );

	if (cnt_evt> cnt_evt_last){//Only unique events
	  cnt_evt_last=cnt_evt;
	  h_eventsA->Fill(  cnt_evt );
	  
	}//Only unique events

      }//FOR LOOP
      //      tree_cpy->Delete();

      //      if (cnt_evt==newok){ }
      //newok++;
      if (ANADEBUG){sprintf(ch,"ori/cpy/evt %lld/%lld/%lld   %d x %d", entr_ori,entr_cpy,cnt_evt, V001,V017 );table_log(2,ch);}
      
      entr_ori_last=entr_ori;
    }//different entr_ori






     usleep(100); // wait 100ms and retry again..
    respush=TokenGet( "push=" , mmap_file , pushis ); //takes a value
  }// while (respush>=1.0){//run while push is running.

  sprintf(ch,"EXITING ANALYZE (bytes=%lld)", 4*cnt );table_log(3,ch);
*/
