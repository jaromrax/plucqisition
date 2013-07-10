#include "xml_attr.h"    // bude xml
#include "log_term.h"    // bude xml
#include "mut_queue.h"
#include "acq_core.h"
#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......


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




  //=================== EXPORTED FUNCTIONS  START =========
  double* soarray;   //we allocate some memory in the first call
  //===========================================
 int* kolac(int* par){
    if (soarray==NULL){  //  is allocated once and always used to send pars
     soarray=new double [1024];
     printf("ALSO A HISTOGRAM IS DONE\n%s", "" );
     TCanvas *c=new TCanvas;
     c->Draw();
     TH1F *h=new TH1F("h","h",1000,0,1000);
     printf("canvas drawn......\n%s" ,"");
	 c->Draw(); c->Modified();c->Update(); 
         h->Draw();
	 //     printf("sleeping......\n%s" ,"");
	 //     gSystem->Sleep(2000);
   }
   soarray[0]=1.2;   soarray[1]=1.3;   soarray[2]=1.4;
   printf("inside kolac123 par[0] canbe NULL\n%s", "");
   return (int*)soarray;
}





  // ==========================================================
  // ===================================================
  // ============================================
  //                                 POPER PART .

  /**************************************************
   *            SIMPLE PRINTOUT  int
   */
  int* pop_empty(int* par){// SIMPLE PRINTOUT ....
    char chL[500];
    concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
    int i=0;
    int wait=1;
    while (i<30){
      sprintf(chL,"POP: empty i==%d" ,  i );table_log(1,chL);
      //   if(XTERM!=NULL)fprintf(XTERM,"  POP empty: pointer==%d\n", (int)buffer );
   wait=1;
   wait=MyCond.TimedWaitRelative( 1000  ) ; // wait 500
   if (wait==0){break;}
   //   usleep(1* 1000*1000);
   i++;
    }
   sprintf(chL,"POP: empty EXITed i==%d" ,  i );table_log(1,chL);

   //   if(XTERM!=NULL)fprintf(XTERM,"  POP empty: EXIT buf==%d\n", (int)buffer );

  }//-----------------------------------------------












  /**************************************************
   *            SIMPLE PRINTOUT  int
   */
  int* pop_logint(int* par){// SIMPLE PRINTOUT ....
    char chL[500];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"  POP log: pointer==%ld\n", (int64_t)buffer );
   int datum=0;// (int datum  ***)
   int runempty=0; int wait=1;
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
     while( !buffer->empty() ){
       buffer->wait_and_pop(datum); //( datum ***)   
       sprintf(chL,"POP:W #%3d. <%ld> <%ld>" , cnt,(int64_t)datum,(int64_t)&datum  );table_log(1,chL);

       //          if(XTERM!=NULL)fprintf(XTERM,"W #%3d. <%d> <%d>\n",
       //				 cnt,datum,&datum); //(&datum ***)
          cnt++; 
  	TThread::CancelPoint(); // When CancelOn()...
    }
     //useless here...............but 
    wait=MyCond.TimedWaitRelative( 300  );
   if (wait==0){ 
     sprintf(chL,"POP:got BROADCAST SIGNAL...%s " ,  "" );table_log(1,chL);
      //if(XTERM!=NULL)fprintf(XTERM,"POP got BROADCAST SIGNAL... %s\n","");
    }
     sprintf(chL,"POP:  EXIT... buf==%ld " ,  (int64_t)buffer );table_log(1,chL);
     // if(XTERM!=NULL)fprintf(XTERM,"  POP log: EXIT buf==%d\n", (int)buffer );
 }// ****************************end of function **********














  /**************************************************
   *            SIMPLE PRINTOUT  txt
   */
  int* pop_logtxt(int* par){// I DONT KNOW .... just printout??? NOt useful, ends very fasst.
    char chL[500];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   //   if(XTERM!=NULL)fprintf(XTERM,"  POP logtxt v.2 : %d\n", (int)buffer );
   sprintf(chL,"POP: logtxt : %ld" , (int64_t)buffer  );table_log(1,chL);

   int datum=0;// (int datum  ***)
   int runempty=0; int wait=1;
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
   while(1==1){
   while( !buffer->empty() ){
     buffer->wait_and_pop(datum); //( datum ***)
     //unsigned short int si=(int)datum;//short int si=(int)datum; !!!
     cc=char( datum ); 
     //     if(XTERM!=NULL)fprintf(XTERM,"W #%3d. <%d> <%d> si=%s\n",
     //			    cnt,datum,&datum,si); //(&datum ***)
     if ( (int)cc!=10){//  10== \n -------------- EOL ----
       sprintf( ch, "%s%c", ch, cc  );
       //       if(XTERM!=NULL)fprintf(XTERM,"RUNNI:%s\n",ch);
     }else{//  \n found
       //       if(XTERM!=NULL)fprintf(XTERM,"FINAL:%s\n",ch);
        sprintf(chL,"POP: FINAL:%s" , ch  );table_log(1,chL);
      sprintf( ch , "%s", ""); // reset
     }//--------------------------- EOL/not EOL------------
      cnt++; 
      // TThread::CancelPoint(); // When CancelOn()...
   }// buffer empty 
   wait=MyCond.TimedWaitRelative( 1000  );
   if (wait==0){ break;}
  }
   //useless here...............but 
   if (wait!=0){wait=MyCond.TimedWaitRelative( 300  );}
   if (wait==0){ 
        sprintf(chL,"POP:  got BROADCAST SIGNAL...:%s" , ""  );table_log(1,chL);
	//     if(XTERM!=NULL)fprintf(XTERM,"POP got BROADCAST SIGNAL... %s\n","");
   }
        sprintf(chL,"POP: EXIT  %ld" ,  (int64_t)buffer );table_log(1,chL);
	//   if(XTERM!=NULL)fprintf(XTERM,"POP logtxt v.2.: EXIT %d\n",(int)buffer);
 }// ****************************end of function **********
















  //===================== SHOULD BE IN SEPARATE FILE ==========
  TTree *txt_ttree;  // similar to global def in nano_acquis_pureconvert

  // prepare TTree===========================================
  // tell number of values on the line
  int pop_get_npar( const char* ch ){
    char chL[500];
    TString oneline=ch, token; int j=0; double bu; 
      TObjArray *tar; 
       sprintf(chL,"POP: Analyzing string:%s" , ch  );table_log(1,chL);
       //       if(XTERM!=NULL)fprintf(XTERM,"Analyzing string:%s\n",ch);
        tar= oneline.Tokenize(" ");


        while (j<tar->GetEntries()){
         token= ((TObjString*)(tar->At(j)))->GetString();
         bu= token.Atof(); 
         sprintf(chL,"POP:   token %d/  <%f>" , j,bu  );table_log(1,chL);
	 //	 if(XTERM!=NULL)fprintf(XTERM,"  token %d/  <%f>\n", j, bu);
	 j++;
	}
	return (j); // j-1? not... here 0->  names are 1->
  }


  struct{    
    UInt_t n;// 4 bytr integer.... is enough for one run (same as nano_acquis)
    double t[1024]; // structre to address by ttree.
  } t_event;      //text event------------------------------



  /***************************************************
   *
   *  PLACE FOR TTREE      and   TH1 also
   *
   */
   int ttree_inited_npars=0;

  int pop_txt_record( const char* chse ){
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




  int conv_t_init( int npar ){; // similar to conv_u_init; t[0]...t[n]
    char chL[500];
   char brname[100];  // main branch in texto......
   int  CIRCULAR=100000;
   char ttree_name[50];
   char ch[1000];
   

    sprintf( ttree_name, "%s",  "texto"  ); 
    sprintf( brname, "%s",  "main" );  // main,  not mainV
    if ( gDirectory->Get( ttree_name ) != NULL){

      //      if(XTERM!=NULL)fprintf(XTERM,  "taking previously existing ttree %x !!!!\n", (int)txt_ttree);
     sprintf(chL,"POP: taking previously existing ttree %lx !!!!" , (int64_t)txt_ttree  );table_log(1,chL);
     //     txt_ttree->SetBranchAddress( brname , &t_event.t[0] );
     txt_ttree->SetBranchAddress( brname , &t_event.n );

    }else{// texto already exists  

      gROOT->cd(); // go memory resident ttree like in "" and nano_conv.
      txt_ttree = new TTree( ttree_name , "ttree_from_textline");
      //       if(XTERM!=NULL)fprintf(XTERM,"NEW TTREE %x\n", (int)txt_ttree );
      sprintf(chL,"POP: NEW TTREE    %lx" , (int64_t)txt_ttree  );table_log(1,chL);
     if (CIRCULAR!=0){ txt_ttree->SetCircular(CIRCULAR);}
      //  /s  is short.  
      sprintf(ch ,"%s/i", "n" );              // n    UInt_t : ordered
      sprintf(ch ,"%s:%s%03d/D", ch, "T",  1 );  //T001  double
    for (int i=2;i<=npar;i++){
      sprintf(ch ,"%s:%s%03d/D", ch,  "T",  i );  

    }// all channels branch
    //     if(XTERM!=NULL)fprintf(XTERM,"This row defines the  Branch  main :\n%s\n", ch );  
     sprintf(chL,"POP: Branch(main):%s" , ch  );table_log(1,chL);
     //    txt_ttree->Branch(brname , &t_event.t[0], ch );// 
    txt_ttree->Branch(brname , &t_event.n, ch );// 
    //     if(XTERM!=NULL)fprintf(XTERM,    " ttree initialized ok    %x\n" , (int)txt_ttree );
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

















  //  ted bych chtel takovy plugin, ktery by tvoril TTree z txt.
  /**************************************************
   *            TXT TO TTREE
   */
  int* pop_txttree(int* par){// THIS WORKS - IT takes 1st line and defines the tree - and accumulates

    char chL[500];
    int lines_received=0;
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   sprintf(chL,"POP: txttree : %ld" , (int64_t)buffer  );table_log(1,chL);
   //   if(XTERM!=NULL)fprintf(XTERM,"  POP logtxt v.2 : %d\n", (int)buffer );
   int datum=0;// (int datum  ***)
   int runempty=0; int wait=1;
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
       //   if(XTERM!=NULL)fprintf(XTERM,"TEXT=%s\n",ch);

       if (ttree_inited==0){
	 //       if(XTERM!=NULL)fprintf(XTERM,"%s\n","creating the tree.....");
       sprintf(chL,"POP: creating the tree... %s" , ""  );table_log(1,chL);
       nparams=pop_get_npar( ch );
       ttree_inited_npars=nparams;
       sprintf(chL,"POP:  nparams==%d" , nparams  );table_log(1,chL);
       //       if(XTERM!=NULL)fprintf(XTERM,"       nparams==%d\n",nparams);
       conv_t_init( nparams ); // similar to conv_u_init; t[0]...t[n]
       ttree_inited=1;// no more try to init
       }// ---ttree was not initied.........

       pop_txt_record( ch ); line++;
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
 }// ****************************end of function **********









  /**************************************************
   *            TO FILE OR JUST  POP     
   */
  int* pop_file(int* par){// POP TO FILE ...
    char chL[500];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   //   if(XTERM!=NULL)fprintf(XTERM,"  POP pop-remote  par==%d; pointer==%d\n", par,(int)buffer );
   sprintf(chL,"POP: file  par==%ld; pointer==%ld " , (int64_t)par,(int64_t)buffer  );table_log(1,chL);
   int datum=0;
   int runempty=0;

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


  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap

      TSmallish_xml xml(    acqxml2   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","poper","file" );
      sprintf( fname ,"%s", xml.output  );

      if ( strlen(fname)>0){
	//----------------OPEN FILE append and write
      outfile=fopen( fname,"ab"  );
      //      if(XTERM!=NULL)	fprintf(XTERM,"  POP opened %s for WRITE - appending\n", fname);
        sprintf(chL,"POP: opened %s for WRITE - appending" , fname  );table_log(1,chL);

      if (outfile!=NULL){
	while( !buffer->empty() ){
	  buffer->wait_and_pop(datum);
          if ((cnt%250000)==0){
	    //	    if(XTERM!=NULL) fprintf(XTERM,"W     %7lld kB\n",4*cnt/1000);
	    sprintf(chL,"POP:W     %7lld kB " , 4*cnt/1000  );table_log(1,chL);
	  } 
	  cnt++; 
	  fwrite ( &datum , 1 , 4  , outfile );
	}
	fclose(outfile);
  	TThread::CancelPoint(); // When CancelOn(), 
      }else{// outfile not NULL
	//	if(XTERM!=NULL)fprintf(XTERM,"  POP outfile %s == NULL\n%s", fname );
	sprintf(chL,"POP:   outfile %s == NULL " , fname  );table_log(1,chL);

	runempty=1;
      }//      outfile not NULL
      }//strlen fname > 0   NOT ""
      else{
	runempty=1;
      }
      if (runempty==1){
	//-----------run empty--------
	while( !buffer->empty() ){
	  buffer->wait_and_pop(datum);
          if ((cnt%250000)==0){
	    //  if(XTERM!=NULL)fprintf(XTERM,"-   %7lld kB\n",4*cnt/1000);
	    sprintf(chL,"POP: -   %7lld kB  " ,  4*cnt/1000 );table_log(1,chL);
	  } 
	  cnt++; 
  	TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
	}//while,  no writeout	
      }//runempty
}/**********************end of function ************/














  /*****************************************************************
   *            TEST TO SORT THE EVENTS ...........  CREATE TTREE
   *
   */
  int* pop_sort(int* par){// POP ... nanot ZD data 4*int system; CONVERSION
    char chL[500];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
//   if(XTERM!=NULL)fprintf(XTERM,"  POP pop SORT *******%s\n", "" );
//   if(XTERM!=NULL)fprintf(XTERM,"  POP pop SORT *******%s\n", "" );
//   if(XTERM!=NULL)fprintf(XTERM,"  POP pop SORT *******%s\n", "" );
   sprintf(chL,"POP: SORT int .... starting  %ld" ,  (int64_t)buffer );table_log(1,chL);

      double downtime; int downtimef, downtimei,  wait=1;

   int datum=0;
   int runempty=1;
   int oneeventbuf[1000]; //usualy 1header,one footer,4 channels=>12 integers
 /* 
  * data from:
  * . 
  */ 
  int chanlo= 1;  
  int chanhi=32;  
  int kc014A=-1; 
  int kc014B=-1; 
  int kc014C=-1; 
  int c257  =-1 ; 
  int v560na=33; // 32 words in data but only 16 channels on the module     0x21
                 // 2 uni-channels per one real-data-channel
  int v560nb=65; // 32 words in data but only 16 channels on the module     0x41
                 // 2 uni-channels per one real-data-channel
  int c32  = -1; // 32 words in data but only 16 channels on the module
  //  int c32  = 33; // 32 words in data but only 16 channels on the module
                 // 2 uni-channels per one real-data-channel

  int ltime= 1024 ; 
  int btime= 141 ; // hardcoded in cellrenderer.py
  int bnum= 140;   // hardcoded in cellrenderer.py
  // rnum  --  139 --- 08b  run
  //           140     08c  blok
  //           141     08d  time-block (x.0 us)
 double tzero=7.889112e+08;//7.889112e+08
 char parnam[5]="V" ; 
  char opt[1000];
  //=================================================

   //STANDARD  XML  READ-------------
      char initline[400];
      int buffer4;
      long long int cnt=0;  long long int cnt_evt=0;
      size_t result;

   //-------- here I will control with    control.mmap    file------   
    if ((mmapfd = open("control.mmap", O_RDWR, 0)) == -1) err(1, "open");
    mmap_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, mmapfd, 0);
    if (mmap_file == MAP_FAILED) errx(1, "either mmap");
    char mmap_result[100];
   //-------- here I will control with    control.mmap    file------   
  char  acqxml2[100];
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap



      TSmallish_xml xml(    acqxml2   );
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","data","chanlo" );
      chanlo=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","data","chanhi" );
      chanhi=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014A" );
      kc014A=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014B" );
      kc014B=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014C" );
      kc014C=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","c257" );
      c257=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","c32" );
      c32=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","v560na" );
      v560na=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","v560nb" );
      v560nb=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","ltime" );
      ltime=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","btime" );
      btime=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","number","bnum" );
      bnum=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","tzero" );
      tzero=atof(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","crate","parnam" );
      sprintf(parnam,"%s",xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","buffer","circular" );
      int circular=atoi(xml.output);



  sprintf( initline, 
  "chanlo=%d,chanhi=%d,kc014A=%d,kc014B=%d,kc014C=%d,ltime=%d,btime=%d,bnum=%d,c257=%d,c32=%d,v560na=%d,v560nb=%d,parnam=%s,tzero=%lf,circular=%d", 
	   chanlo, chanhi,kc014A,kc014B,kc014C,ltime,btime,bnum,c257,c32,v560na,v560nb,parnam,tzero,
	   circular);
 

  //  if(XTERM!=NULL)fprintf(XTERM,"  POP PQ%s\n\n",initline);
        sprintf(chL,"POP: %s" , initline  );table_log(1,chL);
	//  conv_u_init("", initline ); // file-name makes a problem for analyze- // FROM NANOCONVERT


  //  if(XTERM!=NULL)fprintf(XTERM,"  POP PQPrepared to runempty......\n","");
        sprintf(chL,"POP: to enter (runempty==1)%s" , ""  );table_log(1,chL);

      if (runempty==1){
	//-----------run empty--------
	int pointer; //verbose=0;// from nano_acquis_pureconvert // FROM NANOCONVERT

	//	if(XTERM!=NULL)fprintf(XTERM,
	//			       "POP PQ....bufsize==%10d empty==%d \n", 
	//			       buffer->size(), buffer->empty() );	
        sprintf(chL,"POP: bufsize==%10d empty==%d" ,buffer->size(), buffer->empty()  );table_log(1,chL);

	  int status=0;
	  while(1==1){// INFINITE--------------------------------

	while( !buffer->empty() ){// NEXT EVENT
	  pointer=0; status=0;
	  while( !buffer->empty() ){ //ONE-EVENT -CONSTRUCT
	  buffer->wait_and_pop(datum);
	  oneeventbuf[pointer] = datum; pointer++; // integer array
	  //          if ((cnt%25000)==0){if(XTERM!=NULL)fprintf(XTERM,"S        %7lld kB\n",4*cnt/1000);} cnt++;//1MB
	  if (datum==0xf0000000){ break;} // event footer.....  BREAKOUT
	  //	  if (cnt<18){
	  //	    printf( "%10d:  %04X  %04X    -  %10d\n", datum, datum&0xffff,  (datum>>16)&0xffff, 0xf0000000 );
	  //	  }
	  }//while buffer not empty------------------collect 
	  //do something with event HERE
	  cnt_evt++;
	  //one_buffer_process( (void*)buffer,  int(nminibuffer/4) , int(startup/4) ); //

	  //	  one_buffer_process( (void*)&oneeventbuf, pointer , 0 ); // FROM NANOCONVERT

	  if (cnt_evt%50000 ==0){
	    //  if(XTERM!=NULL)fprintf(XTERM,"PQ OBP %lld\n",cnt_evt );fflush(XTERM);
           sprintf(chL,"POP: evt#= %lld" , cnt_evt  );table_log(1,chL);
	  }
	}//WHILE --- next event
	if (status==0){
           sprintf(chL,"POP: cnt==%lld, evts= %lld --leaving" ,cnt, cnt_evt  );table_log(1,chL);
	   //      if(XTERM!=NULL)fprintf(XTERM,
	   //		 "POP PQ (int)cnt=== %lld,  events=%lld --leaving poper\n",
	   //			     cnt,  cnt_evt);
	}//status==0
	//	if(XTERM!=NULL)fprintf(XTERM,"PQ %lld\n",cnt_evt );fflush(XTERM);
        sprintf(chL,"POP:  evt#= %lld (@status++)" , cnt_evt );table_log(1,chL);
	//	usleep(1000*300);//buffer empty:100ms wait before next try
	status++;


	//###########################################
	//  STANDARD  WAIT  POINT FOR thread  EXIT
	//
       TTimeStamp t_wait;
       downtime=t_wait.GetSec()+t_wait.GetNanoSec()*1e-9;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d , %f \n", wait, downtime );
       downtime=downtime+2.5;   // 0.5 sec was NOT ENOUGH !!!
       downtimei=(int)downtime;
       downtimef=(int)( 1e+9*(downtime-1.0*downtimei) );
       //       if(XTERM!=NULL)fprintf(XTERM,"P%s","" );
         sprintf(chL,"POP:  wait-> %s" , "" );table_log(1,chL);
      wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       //       if(XTERM!=NULL)fprintf(XTERM,"Q%s","" );
         sprintf(chL,"POP:   -> wait %s" , "" );table_log(1,chL);

       //	TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
	}//---------------------------------------------INFINITE

      }//runempty
      //this neve happenes
      //if(XTERM!=NULL)fprintf(XTERM,"  POPER SORT END  cnt=== %lld,  events=%lld -------leaving poper\n",cnt,  cnt_evt);       
      sprintf(chL,"POP:  SORT END  cnt=== %lld,  events=%lld----leaving " , cnt,  cnt_evt );table_log(1,chL);


}/**********************end of function ************/








  //============================== EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif
