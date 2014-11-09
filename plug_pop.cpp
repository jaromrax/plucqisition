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











  /******************************************************************
###################################################################
###################################################################
###################################################################
   *      THIS PART CONCERNS THE 
   *    UNIFIED DATA  of 
   *                           ZDENEK HONS
   *
   */


  // prepare TTree===========================================
  //===================== SHOULD BE IN SEPARATE FILE ==========


  TTree *txt_ttree;  // similar to global def in nano_acquis_pureconvert

  struct{    
    UInt_t n;// 4 bytr integer.... is enough for one run (same as nano_acquis)
    double t[1024]; // structre to address by ttree.
  } t_event;      //text event------------------------ simple array of doubles



  // returns    number   of values on the line (space separated)
  //                        serves for  TXT TREE pop txt record
  //                                                              SERVICE...4...pop txttree
  int poptxt_get_npar( const char* ch ){
    char chL[500];
    TString oneline=ch, token; int j=0; double bu; 
      TObjArray *tar; 
       sprintf(chL,"POP: Analyzing string:%s" , ch  );table_log(1,chL);
        tar= oneline.Tokenize(" ");

        while (j<tar->GetEntries()){
         token= ((TObjString*)(tar->At(j)))->GetString();
         bu= token.Atof(); 
         sprintf(chL,"POP:   token %d/  <%f>" , j,bu  );table_log(1,chL);
	 j++;
	}
	return (j); // j-1? not... here 0->  names are 1->
  }



  /***************************************************TTREE    TXT
   *
   *  PLACE FOR TTREE      and   TH1 also
   */

   int ttree_inited_npars=0;
  //                                                              SERVICE...4...pop _txttree
  int poptxt_txt_record( const char* chse ){//.......SERVICE FUNCTION 
    TString oneline=chse, token; int j=0; double bu; 
      TObjArray *tar; 

   char brname[100];  // main branch in texto......
   int  CIRCULAR=100000;
   char ttree_name[50];
   char ch[1000];

    sprintf( ttree_name, "%s",  "texto"  ); 
    sprintf( brname, "%s",  "main" );  // main,  not mainV
    if ( gDirectory->Get( ttree_name ) != NULL){
      //printf("taking prev. existing ttree %x !!!!!!!!!\n", (int)ttree);
      //     txt_ttree->SetBranchAddress( brname , &t_event.t[0] );
     txt_ttree->SetBranchAddress( brname , &t_event.n );
    }
    //        if(XTERM!=NULL)fprintf(XTERM,"Analyzing string:%s\n",ch);
        tar= oneline.Tokenize(" ");
	j=0; 
        while (j<tar->GetEntries()){
         token= ((TObjString*)(tar->At(j)))->GetString();
         t_event.t[j]= token.Atof(); 
	 // printf("  token %d/  <%f>\n", j, t_event.t[j] );
	 j++;
	}
	t_event.n++;
	txt_ttree->Fill();    

	//I assume at least 2 numbers
	for (int i=0;i<ttree_inited_npars;i++){
	  char txname[100];
	  sprintf( txname, "textoH%02d", i);
	  TH1F *texto_h1;     texto_h1=(TH1F*)gDirectory->Get( txname );
	  if (texto_h1==NULL){texto_h1=new TH1F(txname, txname,2000,-1000,1000); }
	  texto_h1->Fill(t_event.t[i]);
	}
	/*
	TH1F *texto_h1;     texto_h1=(TH1F*)gDirectory->Get("texto_h1");
	if (texto_h1==NULL){texto_h1=new TH1F("texto_h1","texto_h1",5000,0,5000); }
	TH1F *texto_h2;     texto_h2=(TH1F*)gDirectory->Get("texto_h2");
	if (texto_h2==NULL){texto_h2=new TH1F("texto_h2","texto_h2",5000,0,5000); }
	texto_h1->Fill(t_event.t[0]);
	texto_h2->Fill(t_event.t[1]);
	*/

    return j;
  }
  //--------------------------------------end of txt_record (TTree) --- SERVICE FUNCTION







  //                                                              SERVICE...4...pop _txttree
  int conv_t_init( int npar ){; // similar to conv_u_init; t[0]...t[n]
    char chL[500];
   char brname[100];  // main branch in texto......
   int  CIRCULAR=100000;
   char ttree_name[50];
   char ch[1000];
   

    sprintf( ttree_name, "%s",  "texto"  ); 
    sprintf( brname, "%s",  "main" );  // main,  not mainV
    if ( gDirectory->Get( ttree_name ) != NULL){

     sprintf(chL,"POP: taking previously existing ttree %lx !!!!" , (int64_t)txt_ttree  );table_log(1,chL);
     //     txt_ttree->SetBranchAddress( brname , &t_event.t[0] );
     txt_ttree->SetBranchAddress( brname , &t_event.n );

    }else{// texto already exists  

      gROOT->cd(); // go memory resident ttree like in "" and nano_conv.
      txt_ttree = new TTree( ttree_name , "ttree_from_textline");

      sprintf(chL,"POP: NEW TTREE  %lx",(int64_t)txt_ttree);table_log(1,chL);
     if (CIRCULAR!=0){ txt_ttree->SetCircular(CIRCULAR);}
      //  /s  is short.  
      sprintf(ch ,"%s/i", "n" );              // n    UInt_t : ordered
      sprintf(ch ,"%s:%s%03d/D", ch, "T",  1 );  //T001  double
    for (int i=2;i<=npar;i++){
      sprintf(ch ,"%s:%s%03d/D", ch,  "T",  i );  

    }// all channels branch

     sprintf(chL,"POP: Branch(main):%s" , ch  );table_log(1,chL);

    txt_ttree->Branch(brname , &t_event.n, ch );// 

     sprintf(chL,"POP: tree initialized OK  %lx" , (int64_t)txt_ttree  );table_log(1,chL);

    txt_ttree->ls();
      if(XTERM!=NULL)fprintf(XTERM, "      ttree initialized ok see ls Print()   %lx  n== %lld\n" , (int64_t)txt_ttree ,txt_ttree->GetEntries());
      sprintf(chL,"POP: tree initialized ok,see ls Print() %lx n==%lld" , (int64_t)txt_ttree ,txt_ttree->GetEntries() );
      table_log(1,chL);

      //      if(XTERM!=NULL)fprintf(XTERM, "      ttree initialized ok see ls Print()   %x  n== %lld\n" , (int)txt_ttree ,txt_ttree->GetEntries());

    }//-------------------------------------TTree 
    //    TTree 
    //->SetBranch("MAIN", &t_event.t[0] );
  }













  /**************************************************..................pop_txttree
   *            TXT TO TTREE
   */
  int* pop_txttree(int* par){// THIS WORKS - IT takes 1st line and defines the tree - and accumulates

    char chL[500];
    int lines_received=0;
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   sprintf(chL,"POP: txttree : %ld" , (int64_t)buffer  );table_log(1,chL);

   int datum=0;// (int datum  ***)
   //int runempty=0; 
   int wait=1;
   int ttree_inited=0;// initialized?-NOT yet

   int nparams=0;

   usleep(1000*1000);
   //STANDARD  XML  READ-------------
   FILE *outfile;  char fname[400];
   int buffer4;
   long long int cnt=0;
   size_t result;
   //      TSmallish_xml xml(    acqxml   );
   //      xml.DisplayTele( xml.mainnode, 0, "files","poper","file" );
   //      sprintf( fname ,"%s", xml.output  );
   //----------------- LOOP ----------------
   char ch[1024]=""; // 1kB text line
   // I AM COMPLETELY LOST WITH THERE CONVERSIONS &...
   char cc;
   t_event.n=0; // reset the number to 0 .... 
   do{//-------------------
     int line=0;
   sprintf( ch , "%s", ""); //reset
   while( !buffer->empty() ){
     buffer->wait_and_pop(datum); //( datum ***)
     cc=char( datum ); 
     if ( (int)cc!=10){//  10== \n -------------- EOL ----
       sprintf( ch, "%s%c", ch, cc  );//accumulate line
     }else{//  \n found

       if (ttree_inited==0){// ---ttree was not initied.........
       sprintf(chL,"POP: creating the tree... %s" , ""  );table_log(1,chL);
       nparams=poptxt_get_npar( ch );
       ttree_inited_npars=nparams;
       sprintf(chL,"POP:  nparams==%d" , nparams  );table_log(1,chL);
       //       if(XTERM!=NULL)fprintf(XTERM,"       nparams==%d\n",nparams);
       conv_t_init( nparams ); // similar to conv_u_init; t[0]...t[n]
       ttree_inited=1;// no more try to init
       }// ---ttree was not initied.........

       poptxt_txt_record( ch ); line++;
       if (line>10000){
         sprintf(chL,"POP:  lines==%d" , lines_received  );table_log(1,chL);
	 line=0;
       }
       lines_received++;
       sprintf( ch , "%s", ""); //reset
     }//--------------------------- EOL/not EOL------------
      cnt++; 
      TThread::CancelPoint(); // When CancelOn()...
   }// while ! buffer empty

   //useless here...............but 
   wait=MyCond.TimedWaitRelative( 1000  );
   if (wait==0){ 
       sprintf(chL,"POP: got BROADCAST SIGNAL...%s" , ""  );table_log(1,chL);
       //     if(XTERM!=NULL)fprintf(XTERM,"POP got BROADCAST SIGNAL... %s\n","");
   }
  sprintf(chL,"POP: onWait lines==%d" , lines_received  );table_log(1,chL);
 }while(wait!=0);// ON BROADCAST
   //   if(XTERM!=NULL)fprintf(XTERM,"POP logtxt v.2.: EXIT %d\n",(int)buffer);
        sprintf(chL,"POP: EXIT  %ld" ,  (int64_t)buffer );table_log(1,chL);
 }// ****************************end of function **********.................TXT 2  TTREE








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
      
      
    while( !buffer->empty() ){// concurent queue "buffer" is an object HERE
      //    while( buffer->size()>2 ){// concurent queue "buffer" is an object HERE

      buffer->wait_and_pop(datum);
      if (POPDEBUG!=0){sprintf(chL,"POP:  datum; cnt==%lld   /c=%d", cnt,c);table_log(1,chL);}

      if ((cnt%10000)==0){
	sprintf(chL,"POP:%8lld kB %9lld  evts",4*cnt/1000, cnt_evt_data);table_log(1,chL);
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
