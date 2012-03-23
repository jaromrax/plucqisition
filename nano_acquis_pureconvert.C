/*
 *  conversion   _u   to  ttree
 *
 **********2011 03 24******************
 *  one_buffer   ... jakykoliv buffer muze byt zpracovavan
 *  conv_u_init ... inicializace je venku
 *    kruhovy  buffer .... pokud v memory - zustava nanot...
 *
 *
 *
 * nactu plny buffer. kdyz je file > buffer, 
 * posunu fajl a pointer a nactu na pozici.SLZOITY
 * nove - proste prerusim a resetuju pointry
 *
 *
 *
 *  20101104 - pridal jsem btime (=141), cellrenderer hardkoduje block time
 *
 *  20100311 - nove - chci KC014 a C257 a kanaly a LINTIME
 *                    definovat nekde na CMDLINE
 *
 *
 *conv_u("RUN004","chan=1-256,kc014A=17,ltime=49")
 *conv_u("RUN004","chan=1-15,c257=17,kc014A=17,ltime=49")
 *
 * conv_u("RUN004", "chanlo=1,chanhi=5,kc014A=17,kc014B=25,ltime=49,c257=33")
 *  41858084 2010-03-12 10:42 RUN004.root
 ********************************************************
 *   2010 10 13 - nyni JSEM TO nasel ve stavu kdy to umi vytvorit
 *   root file 
 *      TTree se jmenuje nanotL
 *      s parametry:  timeL   L001 L002  atd...
 *      muzu napr odstranit kc014B tim, ze mu dam =-1, tim budu mit
 *      unikatni set parametru pro kazdy crate
 *
 *--------------------------------------------------
 *     btime (kanal 141 je sem davan pomoci 
 *
 *
 *
 */


#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <stdio.h>
#include "TF1.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
// TFile - ttree one
#include "TFile.h"
#include "TTree.h"

#include <time.h>
// gROOT->cd()
#include "TROOT.h"

//static const int BUFSIZE  =100E+6;        // allocate  
//  BUFSIZE must be   4* something not to split datum
//  dd if=run17.all of=run17 count=32 bs=1 
static const int BUFSIZE  =4*10e+6;   // buffer is 4byte size=(4*BUFSIZE)



int verbose=0;

//static const int LINUX_TIME=41;   // 1st of the 4 channels for time(us,us,s,s)
// 14o - alex    41
//static const int LINUX_TIME=49;   // 1st of the 4 channels for time (us,us, 
int CIRCULAR=0;  // ttree setcircular (for online)
double TZERO;
int BLOCK_TIME;
int LINUX_TIME;
int BNUM;  // block number
int RNUM;  // RUN   number
//  14c - at 10/23
//static const int CODING_CHANNELS_LOW=1;
int CODING_CHANNELS_LOW;
//static const int CODING_CHANNELS_HIGH=25;
//14o alex -  25 is high
//static const int CODING_CHANNELS_HIGH=256
 int CODING_CHANNELS_HIGH;
// try  chan 56 for 
//static const int KC014A_COUNTER=24;// 2 words and BDE coding!
//  14o alex
//static const int KC014A_COUNTER=17;// 2 words and BDE coding!
int KC014A_COUNTER;
int KC014B_COUNTER;
int KC014C_COUNTER;
int C257_COUNTER;
int V560NA_COUNTER; //VME 16 channel 32bit
int V560NB_COUNTER;
int C32_COUNTER;    // deprec. v560n alias
char PARNAM[10];

const int minutes30=30;




/*
 *  keep linux_time in several variables
 */
double       time_my,time_mylast,time_myfirst;// always contains the last time mark
double       btime_my,btime_mylast,btime_myfirst;// always contains the last time mark
unsigned int time_sec;// always contains the last time mark
unsigned int time_usec;// always contains the last time mark
/*
 *  Histograms 
 */
TH1F  *counting=0;       // channel x seen -> record one count into bin x
TH1F  *chan[255];  // 1 TH1F  histogram per channel, 255 histograms should be ok
/*
 *  TTree
 */
TTree *ttree; TFile *ftree;  
int missing_time_events=0; // if time not defined, remove the event
long long int events_total=0;
/*
 *  structure for   TTree
 */   
/*           - C : a character string terminated by the 0 character
            - B : an 8 bit signed integer (Char_t)
            - b : an 8 bit unsigned integer (UChar_t)
            - S : a 16 bit signed integer (Short_t)
            - s : a 16 bit unsigned integer (UShort_t)
            - I : a 32 bit signed integer (Int_t)
            - i : a 32 bit unsigned integer (UInt_t)
            - F : a 32 bit floating point (Float_t)
            - D : a 64 bit floating point (Double_t)
            - L : a 64 bit signed integer (Long64_t)
            - l : a 64 bit unsigned integer (ULong64_t)
            - O : a boolean (Bool_t)}  */

  typedef struct {
    double time;
    double btime;
    UInt_t n;// 4 bytr integer.... is enough for one run
    unsigned short int rnum;
    unsigned short int bnum;
    unsigned short int chan[255];//  16bit /s
  } Tevent;
Tevent event;



// counter KCo14  (/i)
unsigned int kc014A[4];
  long int kc014A_total[4]={0,0,0,0};	  
unsigned int kc014B[4];
  long int kc014B_total[4]={0,0,0,0};	  
unsigned int kc014C[4];
  long int kc014C_total[4]={0,0,0,0};	  

unsigned int c257[16];
long int c257_total[16]={0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0};

unsigned int v560na[32];
long int v560na_total[32]={0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0};

unsigned int v560nb[32];
long int v560nb_total[32]={0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0};

unsigned int c32[32];
long int c32_total[32]={0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0};

	  
// GLOBALHITS
//const int MAXCHAN=299;
// thanks to time in VME 1024     + 
const int MAXCHAN=1900;
  long int ghits_counter[MAXCHAN];
//   int ghits_counter[MAXCHAN];

/*
 * pointers to file, to buffer, total filesize
 */

long long int file_ptr;
int  buf_ptr;
long long int file_max;
FILE *fajl;
unsigned int readbuf[BUFSIZE];

/*
 * just for better display   in kB
 */
//int print_bytes_div=1024;
int print_bytes_div=1024;
const char *units="kB";



char chtxt[6000];  // IT IS LOG FILE.......... GLOBAL DEF


/*
 *
 *          LOAD_BUFFER
 *
 */




//  buf_ptr is reset in main!
//  file_ptr  is obsolete, we dont care
int load_buffer(const char* filename ){
  long long int   ToRead, sizeread;

  if ( (file_ptr==0)&&(file_max==0) ){//1st open

    //  if (fajl==NULL){   }
    fajl=fopen64( filename ,"rb" );   
    if (fajl == NULL){ printf("NO FILE %s\n\n\n", filename);return 0; } // PROBLEM
    fseek( fajl,  0, SEEK_END );  // set to end
    file_max=ftello64 ( fajl );	  // get the size
    fseeko64( fajl,  0, SEEK_SET );  // set to start
    printf("File size is %lld\n", file_max );
    if (file_max % 4 !=0){ printf("File size is not 4*n - Crash\n%s",""); return 0;}
  }//                                 1st open

  /*
   *  we assume that each return here we keep the position in file
   */

    ToRead = file_max - file_ptr;
    //    ToRead = file_max;
    if (ToRead>BUFSIZE){ ToRead=BUFSIZE; }
    sizeread=fread(  readbuf ,  1 , ToRead  ,  fajl );
    file_ptr+=sizeread;
    /*
     *   
     *   39062 read, 39062 of 75524 is being proc..
     *   36462 read  75524 of 75524 is ...
     */
    printf("\nread: %9lld  %s read, %9lld  of %9lld %s is being processed\n", 
	   sizeread/print_bytes_div , units,    //read
	   file_ptr/print_bytes_div ,           //
	   file_max/print_bytes_div  , units ); //filemax
 
    return sizeread;     // THI IS IN BYTES !!!!!!!!!!!!!!!!!!!
}// load_buffer





/*
 *
 *          GET_TOKEN
 *
 */




/**************************************************
 *  token things like   run=15,evt=4
 *    this func is identical to the one in osele.C
 */
TString* GetToken(const char *what, const char *token){
   //---------------  parameters sent --------------------------
   TString act, tok;
   int found=0;
  TString args(what);//   args.ReplaceAll("("," "); args.ReplaceAll(")"," ");
  if (args.Length()==0){ return NULL;}
   args.ReplaceAll(","," ");
   args.ReplaceAll("#+"," ");// slave has    #+
   args.Append(" ");
   TString MyTok=token;   MyTok.Append("=");

   TObjArray *tar=args.Tokenize(" ");
   //   ((TObjString*)(tar->At(0)))->GetString()->Data();
   for (int i=0;i<tar->GetEntries();i++){
     tok= ((TObjString*)(tar->At(i)))->GetString();
     //          printf("token %d =:%s (searching %s)\n", i,  tok.Data() , token );
      if (   tok.BeginsWith( MyTok.Data()  )  ){
	     tok.ReplaceAll( MyTok.Data()   ,"");
	     found++; 
	     break;
      }//if
   }
     if ( found==0 ){   return NULL;   }
     TString *ret=new TString(tok);
      tar->Delete();
      return ret;
}// GetToken
int GetTokenInt(const char *what, const char *token, int err){
  TString *a;  a=GetToken( what, token );
  if (a==NULL){ return err;}  return atoi(a->Data() );
}
double GetTokenDouble(const char *what, const char *token, double err){
  TString *a;  a=GetToken( what, token );
  if (a==NULL){ printf("not found - returns %lf\n", err );return err;}  return atoi(a->Data() );
}








/*
 *   GLOBAL - EVENTNUM ==0
 *
 */

  int res , channels=0,channel_n=0, headfoot=0;

UInt_t eventnum=0;
UInt_t eventnum_fill=0;

  int NCASE; // main split
  int pair_chan=0,pair_value=0; // PAIR channel - value 
  int startstop=0;// runstart runstop
time_t timet;
//time_t timet2;









/***********************************************
 *  INIT
 *
 */

int  conv_u_init(const char* filename, const char* options){
  char ch[6000],  ch2[6000];
  //  char chtxt[6000];  // IT IS LOG FILE.......... GLOBAL DEF
  char ttree_name[50];




/*
*    READOUT  THE COMMANDLINE
*/

 CODING_CHANNELS_LOW=  GetTokenInt(options,  "chanlo" , -1 );
 CODING_CHANNELS_HIGH=  GetTokenInt(options,  "chanhi"  , -1);
 KC014A_COUNTER=        GetTokenInt(options,  "kc014A"  , -1);
 KC014B_COUNTER=        GetTokenInt(options,  "kc014B"  , -1);
 KC014C_COUNTER=        GetTokenInt(options,  "kc014C"  , -1);
 C257_COUNTER=          GetTokenInt(options,  "c257"  , -1);
 V560NA_COUNTER=        GetTokenInt(options,  "v560na"  , -1);
 V560NB_COUNTER=        GetTokenInt(options,  "v560nb"  , -1);
 C32_COUNTER=           GetTokenInt(options,  "c32"  , -1);
 LINUX_TIME=            GetTokenInt(options,  "ltime" , -1 );
 BLOCK_TIME=            GetTokenInt(options,  "btime" , -1 );
 RNUM=                  GetTokenInt(options,  "rnum" , -1 );
 BNUM=                  GetTokenInt(options,  "bnum" , -1 );
 TZERO     =            GetTokenDouble(options,  "tzero" , 0.0 );

 CIRCULAR     =0;
 CIRCULAR     =         GetTokenInt(options,  "circular" , 0 );//circularbuffer


 TString *sss;
 sss=GetToken(options,  "parnam"  ); 
 if (sss==NULL){ sss=new TString("x");}
 sprintf(PARNAM,"%s",   sss->Data()    );


 printf( " CODING_CHANNELS_LOW =%d  \n",  CODING_CHANNELS_LOW     );
 printf( " CODING_CHANNELS_HIGH =%d  \n",    CODING_CHANNELS_HIGH   );
 printf( " KC014A_COUNTER  =%d \n",    KC014A_COUNTER   );
 printf( " KC014B_COUNTER  =%d \n",    KC014B_COUNTER   );
 printf( " KC014C_COUNTER  =%d \n",    KC014C_COUNTER   );
 printf( " C257_COUNTER  =%d \n",    C257_COUNTER   );
 printf( " V560NA_COUNTER  =%d \n",    V560NA_COUNTER   );
 printf( " V560NB_COUNTER  =%d \n",    V560NB_COUNTER   );
 printf( " C32_COUNTER  =%d \n",    C32_COUNTER   );
 printf( " LINUX_TIME  =%d \n",    LINUX_TIME   );
 printf( " BLOCK_TIME  =%d \n",    BLOCK_TIME   );
 printf( " RNUM        =%d \n",    RNUM   );
 printf( " BNUM        =%d \n",    BNUM   );
 printf( " TZERO       =%lf \n",    TZERO   );
  printf( " PARNAM  =%s \n",    PARNAM   );

 /*
   *  initialize  pointers and histograms  HISTOGRAMS
   */
  printf("       zeroing   %s\n" , "" );

  file_ptr=0;   file_max=0; buf_ptr=0; //  Zacinam na 0, file closed....
  printf("       zeroing, TH1F   %s\n" , "" );

  counting=(TH1F*)gDirectory->Get("counting");
  if (counting==NULL){
    counting=new TH1F("counting","counting statistics",255,0,255);// 255 possible channels
    printf("NEW TH COUNTING %x\n", (int)counting );
  }
  startstop=0;   time_mylast=0;   time_myfirst=0;    time_my=0;
  startstop=0;   btime_mylast=0;  btime_myfirst=0;   btime_my=0;
  printf("       zeroing counters   %s\n" , "" );
  for (int i=0;i<4;i++){     kc014A_total[i]=0; kc014A[i]=0; }
  for (int i=0;i<4;i++){     kc014B_total[i]=0; kc014B[i]=0; }
  for (int i=0;i<4;i++){     kc014C_total[i]=0; kc014C[i]=0; }
  for (int i=0;i<16;i++){    c257_total[i]=0;  c257[i]=0; }
  for (int i=0;i<32;i++){    v560na_total[i]=0;  v560na[i]=0; }
  for (int i=0;i<32;i++){    v560nb_total[i]=0;  v560nb[i]=0; }
  for (int i=0;i<32;i++){    c32_total[i]=0;  c32[i]=0; }

  time_my=0;time_mylast=0;time_myfirst=0;time_sec=0;time_usec=0;missing_time_events=0;



  for (int i=0;i<MAXCHAN;i++){      ghits_counter[i]=0;  } // clear global hits counter


  printf("      parameters zeroed  ok  %s\n" , "" );
  

  /*******************************   
   *  initialize  more  histograms   HISTOGRAMS
   */

for (int i=CODING_CHANNELS_LOW;i<=CODING_CHANNELS_HIGH;i++){
  int XBINS=16384;
	sprintf(ch ,"chan%03d", i);
	sprintf(ch2,"chan%03d", i);
	//	if ( i==190 ){ xbins=10000;sprintf(ch2,"%s KC014 - Gene", ch2  );	}
	if ( gDirectory->Get(ch)==NULL ){	
	  chan[i]=new TH1F(ch, ch2,  XBINS,  0,  XBINS );
	  printf("NEW channel #%3d init   \n" , i );
	}else{
	  chan[i]=(TH1F*)gDirectory->Get(ch);
	}
 }

  printf("      chan histograms ok    %s\n" , "" );


/*
 *  .........................  initialize  tree .............................. ||||
 *
 */
//   test if   _O  _L is already there
    char brname[100];

  sprintf( ttree_name, "%s",  "nanot"  ); 
  //NEW IDEA _ IF IT ALREADY EXISTS _ JUST CONTINUE ____________________
  if ( gDirectory->Get("nanot") != NULL){
     ttree= (TTree*) gDirectory->Get("nanot");
     printf("taking previously existing ttree %x !!!!!!!!! skipping the rest\n", (int)ttree);

     ttree->SetBranchAddress( "time" , &event.time );
     sprintf( brname, "%s%s",  "main", PARNAM  );
     ttree->SetBranchAddress( brname , &event.chan[0] );
  }else{//--------_>>>> now, whole bunch of definitions-------------


  sprintf(ch,"_%s",PARNAM);
  int fileopen=0;// FILE OPEN - EXISTS AND IS OPEN *


  if (strlen(filename)>0){ //if there is a filename.......
   printf( "filename = %s\n\n",  filename );
 
  if ( strstr(filename, ch )==NULL){
     sprintf( ch    ,"%s_%s.root",   filename, PARNAM );
     sprintf( chtxt ,"%s_%s.txt",  filename, PARNAM );
  }else{
     sprintf( ch    ,"%s.root",   filename );
     sprintf( chtxt ,"%s.txt",  filename);
  }

  ftree=new TFile( ch  , "NEW");  // crashes if file exists
  printf("NEW filename of root-tree file  (NEW)  ==== <%s> =======\n",ch);
  if (ftree->IsOpen()) {fileopen=1;}else{return 1;}
  }//if there is a filename...............

  else{  printf("going memory resident ftree==%x\n",(int)ftree); gROOT->cd();   }// make it memory resident
  

  // ok, all is names _O _L
  /* -------------------------------------------------------------------
   *  ---------------------->>> start    here -------------------------
   */    


  if ( (fileopen==1) || (strlen(filename)==0)  ) {//file exists OR nofile :""

    ttree = new TTree( ttree_name , "ttree_from_zdenek_hons_unidata");
    printf("NEW TTREE %x\n", (int)ttree );
    if (CIRCULAR!=0){ ttree->SetCircular(CIRCULAR);}
    //    printf("ttree - captured in fileopen==1\n", ttree ); //ttree->Print();
    /*
     * /D 64bit floating,  /s 16bit unsigned int
     */
    // first parameter into ttree is time_L (for lecroy, say)
    sprintf(ch ,"time%s/D", PARNAM );   // timeO/D:btimeO/D:bnumO/s
    sprintf(ch ,"%s:btime%s/D", ch,  PARNAM );  
    sprintf(ch ,"%s:n%s/i", ch,  PARNAM );   // UInt_t n;   event number
    sprintf(ch ,"%s:rnum%s/s", ch,  PARNAM );  
    sprintf(ch ,"%s:bnum%s/s", ch,  PARNAM );  
    ttree->Branch( "time" , &event.time, ch );//  TIME BRANCH ######################
    // then bunch of channels comes    
    sprintf(ch ,"%s%03d/s", PARNAM,  CODING_CHANNELS_LOW );  
    for (int i=CODING_CHANNELS_LOW+1;i<=CODING_CHANNELS_HIGH;i++){
      sprintf(ch ,"%s:%s%03d/s", ch,  PARNAM,  i );  
      //      sprintf(ch ,"%s:par 003/s", ch, , PARNAM,  i );  
    }// all channels branch

    printf("This row defines the  Branch  main :\n%s\n", ch );  

    sprintf( brname, "%s%s",  "main", PARNAM  );
    //    ttree->Branch("main", &event.time, ch );// 
    ttree->Branch(brname , &event.chan[0], ch );// &event.chan[0] creates extra __noname00 #######
     if (KC014A_COUNTER>0){
      ttree->Branch("counters", &kc014A, "kc014A_1/i:kc014A_2/i:kc014A_3/i:kc014A_4/i" );// #####
     }
     if (KC014B_COUNTER>0){
      ttree->Branch("counters", &kc014B, "kc014B_1/i:kc014B_2/i:kc014B_3/i:kc014B_4/i" );// #####
     }
    if (KC014C_COUNTER>0){
      ttree->Branch("counters", &kc014C, "kc014C_1/i:kc014C_2/i:kc014C_3/i:kc014C_4/i" );// #####
     }
    if (C257_COUNTER>0){
    ttree->Branch("counters", &c257, "c257_1/i:c257_2/i:c257_3/i:c257_4/i:c257_5/i:c257_6/i:c257_7/i:c257_8/i:c257_9/i:c257_10/i:c257_11/i:c257_12/i:c257_13/i:c257_14/i:c257_15/i:c257_16/i" );// #####
    }// c257 exists

    if (V560NA_COUNTER>0){
    ttree->Branch("counters", &v560na, "v560na_1/i:v560na_2/i:v560na_3/i:v560na_4/i:v560na_5/i:v560na_6/i:v560na_7/i:v560na_8/i:v560na_9/i:v560na_10/i:v560na_11/i:v560na_12/i:v560na_13/i:v560na_14/i:v560na_15/i:v560na_16/i:v560na_17/i:v560na_18/i:v560na_19/i:v560na_20/i:v560na_21/i:v560na_22/i:v560na_23/i:v560na_24/i:v560na_25/i:v560na_26/i:v560na_27/i:v560na_28/i:v560na_29/i:v560na_30/i:v560na_31/i:v560na_32/i" );// ######
    }// c32 exists

    if (V560NB_COUNTER>0){
    ttree->Branch("counters", &v560nb, "v560nb_1/i:v560nb_2/i:v560nb_3/i:v560nb_4/i:v560nb_5/i:v560nb_6/i:v560nb_7/i:v560nb_8/i:v560nb_9/i:v560nb_10/i:v560nb_11/i:v560nb_12/i:v560nb_13/i:v560nb_14/i:v560nb_15/i:v560nb_16/i:v560nb_17/i:v560nb_18/i:v560nb_19/i:v560nb_20/i:v560nb_21/i:v560nb_22/i:v560nb_23/i:v560nb_24/i:v560nb_25/i:v560nb_26/i:v560nb_27/i:v560nb_28/i:v560nb_29/i:v560nb_30/i:v560nb_31/i:v560nb_32/i" );// ######
    }// c32 exists


    if (C32_COUNTER>0){
    ttree->Branch("counters", &c32, "c32_1/i:c32_2/i:c32_3/i:c32_4/i:c32_5/i:c32_6/i:c32_7/i:c32_8/i:c32_9/i:c32_10/i:c32_11/i:c32_12/i:c32_13/i:c32_14/i:c32_15/i:c32_16/i:c32_17/i:c32_18/i:c32_19/i:c32_20/i:c32_21/i:c32_22/i:c32_23/i:c32_24/i:c32_25/i:c32_26/i:c32_27/i:c32_28/i:c32_29/i:c32_30/i:c32_31/i:c32_32/i" );// ######
    }// c32 exists

    //  }// tree not defined previously..........--_>>>> now, whole bunch of defini..



  }else{
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf(" cannot create file <%s> to save the tree. Crashing (    .!rm %s   )\n", ch,ch );
    printf("                                                         .!rm %s    \n", ch);
    sleep(1);
    printf("RETURNING FROM INIT WITH AN ERROR............%s\n","");
   return 1;
  }//ftree not open

  //  set circular 1000 entries
  //  if (    (CIRCULAR==0)&&(strlen(filename)==0)){  // ONLY IF NO FILENAME (i.e. in cd()  )
  //    ttree->SetCircular(100000);   //100k events in memory
  //  }

  }//--------_>>>> now, whole bunch of definitions-------------


  printf("      ttree initialized ok    %x\n" , (int)ttree );
  printf("      ttree initialized ok see ls Print()   %x\n" , (int)ttree );
  ttree->ls();
  printf("      ttree initialized ok see ls Print()   %x  n== %lld\n" , (int)ttree ,ttree->GetEntries());
  
  //  ttree->Print();
  printf("#######################################################%s\n","");
return 0;
}// conv_u_init........................................................









/**************************************************************************
 *   one buffer ONLY
 * nice trick - send anything, any pointe  ....  addapt to char*
 *
 *
 * 201105 - res is in BYTES/4 !!!  I didnt touch anything but the call after res=load_buffer
 *
 */
void one_buffer_process(void *readbuf_ptr, int res,  int startup){
  /*  how is this done?????
   *
   *   I always    set   buf_ptr  to  startup value or 0
   *     and navigate through the buffer  ++
   *
   *   buf_ptr <  res(sizeread) + startup 
   *
   */
  //res and startup are in int*4 !!!!
 unsigned int *readbufQ= (unsigned int*) readbuf_ptr;
 int events_this_loop=0;
 int loop_passes=0;
 int LIN_TIME_ROW=0; // do not store on LINUX_TIME row data
 int LIN_TIME_ROW_X=0; // I donbt know -I try to set this NONZERO if channel DATA are present
 //  I increment only if counters or data have come.....  Additionaly I repaired a bug when counter=-1
 //  and it was treated as data...........20111113
 // 46k x 1.2MB for counters only...
 //


 buf_ptr=0;// my thing...
 buf_ptr = (int)startup; // big buffer, small minibuffers .....
 // printf("new buffer processing started. %d -- %d\n",  (int)readbufQ,  res );
 if (verbose>0){
 printf("    hf==%d  chan_in_evt==%d/%d\n", headfoot,channel_n,channels  );
 }
      //LOOP parsing the buffer
 // headfoot=0; // initial value.....
 if (verbose>0){
 printf("buf_ptr <  res + startup:   %d <  %d + %d\n ",  buf_ptr, res, startup);
 }
      while ( (buf_ptr<res+startup)&&(res>0) ){
	loop_passes++;
	//MAIN_DEBUG_PRINT
	//	printf(" %08X \n", readbufQ[ buf_ptr ]  ); 

        NCASE=0;

	if ( readbufQ[ buf_ptr ] == 0xefffffff){ NCASE=1;}//RunStart
	if ( readbufQ[ buf_ptr ] == 0xffffffff){ NCASE=2;}//RunStop
	if ( readbufQ[ buf_ptr ] == 0xf0000000){ NCASE=3;}// footer==ttree fill,eventnum++
	if ( (readbufQ[ buf_ptr ] & 0xff00ffff)==0xe0000000 ) { NCASE=4;}
	// event header

	if ((NCASE==0)&&(headfoot==1)&&(channel_n<=channels)){
	  NCASE=5; // going thru the channels == inside event
	}
	/*   headfoot      2               1            0 
	 *               event start     evt end     not def
	 *                 1               2
	 *              head found        foot found
	 */
	//	printf("NCASE %d, ptr=%d,  hf=%d\n",NCASE,buf_ptr,headfoot);
	switch (NCASE){
	case 0: 
	  /*
	  printf("This should not happen, NCASE=0, maybe a wrong event (%d)\n", buf_ptr); 
	  if (buf_ptr>8) printf("%08x\n",  readbufQ[ buf_ptr-8 ]  );
	  if (buf_ptr>7) printf("%08x\n",  readbufQ[ buf_ptr-7 ] );
	  if (buf_ptr>6) printf("%08x\n",  readbufQ[ buf_ptr-6 ] );
	  if (buf_ptr>5) printf("%08x\n",  readbufQ[ buf_ptr-5 ] );
	  if (buf_ptr>4) printf("%08x\n",  readbufQ[ buf_ptr-4 ] );
	  if (buf_ptr>3) printf("%08x\n",  readbufQ[ buf_ptr-3 ] );
	  if (buf_ptr>2) printf("%08x\n",  readbufQ[ buf_ptr-2 ] );
	  if (buf_ptr>1) printf("%08x\n",  readbufQ[ buf_ptr-1 ] );
                    	 printf("%08x ... hf==%d ch==%d/%d\n",  readbufQ[ buf_ptr ] , 
				headfoot,channel_n, channels );
	  */
	  break;
	case 1: printf(">---------------------RunStart marker @%d\n",eventnum);
	  //	  printf("    --- buf_ptr==%d   at event ==%d\n",  buf_ptr, eventnum  );
	  headfoot=0;
	  if (startstop==0){startstop=1;}else{startstop=3;}
	  break;
	case 2: printf(">---------------------RunStop marker @ %d\n",eventnum);
	  //	  printf("    --- buf_ptr==%d   at event ==%d   headfoot=%d\n",  buf_ptr, eventnum , headfoot );
	  if (startstop==1){startstop=2;}else{startstop=4;}
	  break;




	  /**************************************
	   *
	   *
	   *          case  3    event footer
	   *           FILL TREE  etc....
	   *
	   *
	   */
	case 3: 	  


	  if ( 
	      	       (eventnum<2) || 
	       //	       (eventnum>3860416) ||  
	       //       ((channels>4) )   
	       //	       ((eventnum>=32456)&&(eventnum<=32486)) ||
	       //	       	       ((eventnum>=33000)&&(eventnum<=34779))
	       	       (eventnum==0)
	       ){
	    printf("DBG: %03d: evt.time=%14.4lf/%14.4lf r/b=%04d/%04d #ch==%d :::  hf=%d\n",
				  eventnum+1, event.time,event.btime,
		   event.rnum,event.bnum,  channels,
		   headfoot );
	  }//DBG  LIST ------------------------------------------------


	  if ( (headfoot==2)||(headfoot==0)){
	    printf("broken event (footer without header) at event #%d\n",eventnum  );                    	 
	    printf("%08x ... hf==%d ch==%d/%d  (broken footer)\n",  
		   readbufQ[ buf_ptr ], headfoot, channel_n, channels ); 
	  }
	  //printf(" footer \n%s","");
	  headfoot=2;channels=0;
	  if ( (event.time>0.0) || (LINUX_TIME<=0)  ){	 
	    

	    // WAS THIS AND WORKED - ALL TIMES WERE THERE     if (LIN_TIME_ROW==0){  	      ttree->Fill();
	    if ((LIN_TIME_ROW==0)&&(LIN_TIME_ROW_X>0)){
	      // ------------      event.chan[ 0 ]=LIN_TIME_ROW_X; // DEBUG TRICK!!
	      /*
	      if (eventnum<10){
		printf("ttree == %X to prn\n", (int)ttree );
		ttree->Print();
		printf("ttree == %X to fill\n", (int)ttree );
	      }
	      */
	      //	      event.n=eventnum;
	      eventnum_fill++;
	      event.n=eventnum_fill; // THIS IS without ltime 
	      if (eventnum_fill<=10){ printf("########### ENUMFILL==%d\n", eventnum_fill);}
 	      ttree->Fill();
	      if (eventnum<10)printf("ttree == %d filled\n", (int)ttree );
	    } // STORE ONLY WHEN OTHER DATA THEN LINUX_TIME


	    //	    printf("entries = %d\n", ttree->GetEntries() );
	  }// save one event
	  else{ //do not  if time defined and event without it
	    missing_time_events++; 
	    // printf("DBG: no time,no fill for event %5d    ...  time=%d\n", eventnum, event.time  );
	  }


	  //clear ttree event structure
	  for (int i=CODING_CHANNELS_LOW;i<=CODING_CHANNELS_HIGH;i++){
	    event.chan[ i-CODING_CHANNELS_LOW  ] =0;
	  }
	  //RESETT TEMPORARY COUNTERS
	  for (int i=0;i<4;i++){  kc014A_total[i]+=kc014A[i];  kc014A[i]=0; }// add totals, clear locals
	  for (int i=0;i<4;i++){  kc014B_total[i]+=kc014B[i];  kc014B[i]=0; }// add totals, clear locals
	  for (int i=0;i<4;i++){  kc014C_total[i]+=kc014C[i];  kc014C[i]=0; }// add totals, clear locals
	  for (int i=0;i<16;i++){  c257_total[i]+=c257[i];  c257[i]=0; }// add totals, clear locals
	  for (int i=0;i<32;i++){  v560na_total[i]+=v560na[i];  v560na[i]=0; }// add totals, clear locals
	  for (int i=0;i<32;i++){  v560nb_total[i]+=v560nb[i];  v560nb[i]=0; }// add totals, clear locals
	  for (int i=0;i<32;i++){  c32_total[i]+=c32[i];  c32[i]=0; }// add totals, clear locals
	  pair_chan=0;pair_value=0;
	  eventnum++; headfoot=2;
	  //channel_n=0;// prepare for next
	  break;




	  /*
	   *          case  4    event header ...............................................
	   *
	   */
	case 4:	
	  LIN_TIME_ROW=0; // We do not store when linux time is coded - there is no detector/counter data there
	  LIN_TIME_ROW_X=0; // We do not store when linux time is coded - there is no detector/counter data there
	  events_total++;
	  events_this_loop++;
	  //clear ttree event structure
     	  for (int i=CODING_CHANNELS_LOW;i<=CODING_CHANNELS_HIGH;i++){
	    event.chan[ i-CODING_CHANNELS_LOW  ] =0;
	  }

	  channels=(readbufQ[ buf_ptr ] & 0x00ff0000)>>16; //maximum 256 channels in event
	  if ( (headfoot==1) &&(eventnum<200) ){
	                  printf("broken event (another header) at event #%d ( %d-chan) \n", eventnum  ,channels );
	    }
	  pair_chan=0;pair_value=0;
	  //	  printf(" header %9d.  %d...  ", eventnum, channels  ); 
	  //	  printf(" Event# %9d.  %d channels,  hf==%d=>1...\n", eventnum, channels, headfoot  ); 
	  //	  }//if pattern e0xx0000 
	  headfoot=1;  // header found..............
	  channel_n=1;// prepare the channel (inevent) counter
	  // 	  printf(" Event# %9d.  %d channels,  hf==%d=>1...\n", eventnum, channels, headfoot  ); 
	  break;





	  /*
	   *  channel - value  pair analysis here............. CASE 5 ....................
	   */
	case 5:
	  pair_value=  (readbufQ[ buf_ptr ] & 0x0000ffff);
	  pair_chan = (readbufQ[ buf_ptr ] & 0xffff0000)>>16;
	  if (pair_chan>MAXCHAN){ printf("ERROR:  channel %d > maximum (%d)\n ",pair_chan, MAXCHAN );}
	  channel_n++; // next item in the event
	  ghits_counter[ pair_chan ]++;
	  //	  printf("filling counting TH\n%s","");
	  counting->Fill( pair_chan );// channel seen - bin incremented
	  //	  printf("counting TH filled \n%s","");

	  //	  if ( ( (pair_chan>=CODING_CHANNELS_LOW)&&(pair_chan<=CODING_CHANNELS_HIGH))||
	  //	       ( (pair_chan>=LINUX_TIME)&&(pair_chan<=LINUX_TIME+3) ) ) {




	  //TIME-----------------------------------------------------
	  if ( (LINUX_TIME>0)&& (pair_chan>=LINUX_TIME)&&(pair_chan<=LINUX_TIME+3)   ) {

	  if (pair_chan == LINUX_TIME  ){time_usec=pair_value; }
	  if (pair_chan == LINUX_TIME+1){time_usec=time_usec+65536*pair_value; }
	  if (pair_chan == LINUX_TIME+2){time_sec=pair_value; }
	  if (pair_chan == LINUX_TIME+3){
	    LIN_TIME_ROW=1; // DO NOT STORE IF LINUX_TIME row only    
	    // OK-I understand-this is ALWAYS the whole event!!! I dont want to code this
	    //
	    //  there is ==0   before ttree->Fill()
	    //
	    //suppose all 3 previous are read.
	    time_sec = time_sec + 65536*pair_value; 
	    time_my= (1.0e-6 * time_usec) + (1.0* time_sec);
	    event.time=time_my - TZERO;
	    if (time_myfirst==0.0){ time_myfirst=time_my;}
	    // for every  x  sec -  printout 

	    if (time_my<time_mylast){
	      time_mylast=time_my;
	      time (&timet);
	      int ttt = time_my;
	      timet= (time_t)ttt;
	      printf("TIME GOES PAST !!!!!!!!!! t=%f    %20s", time_my , ctime( &timet ) );//nefunguje..
	      
	    }
	    // every 30 minutes....
	    if (time_my-time_mylast > minutes30 *60.0){
	      time_mylast=time_my;
	      time (&timet);
	      int ttt = time_my;
	      timet= (time_t)ttt;
	      printf("DBG: t=%f    %20s", time_my , ctime( &timet ) ); //tohle ukazuje 
	    }
	    // printf("  t=%f  /  %f\n",event.time, time_mylast );
	  }//last linuxtime word - make time
	  }//only selected time channels, dont do statistical info .....LINUX_TIME>0

	  //	  printf("before RNUM \n%s","");

	  // RNUM -1 means NO coding of this.  pair_chan==the_channel
	  if ( ( RNUM>=0) && (pair_chan==RNUM) ) {
	    event.rnum=pair_value;
	  }

	  //	  printf("before BNUM \n%s","");
	  // BNUM -1 means NO coding of this.  pair_chan==the_channel
	  if ( ( BNUM>=0) && (pair_chan==BNUM) ) {
	    event.bnum=pair_value;
	  }


	  //TIME------------------------BLOCK-----------------------------
	  if ( (BLOCK_TIME>0)&& (pair_chan>=BLOCK_TIME)&&(pair_chan<=BLOCK_TIME+3)   ) {

	  if (pair_chan == BLOCK_TIME  ){time_usec=pair_value; }
	  if (pair_chan == BLOCK_TIME+1){time_usec=time_usec+65536*pair_value; }
	  if (pair_chan == BLOCK_TIME+2){time_sec=pair_value; }
	  if (pair_chan == BLOCK_TIME+3){
	    //suppose all 3 previous are read.
	    time_sec = time_sec + 65536*pair_value; 
	    btime_my= (1.0e-6 * time_usec) + (1.0* time_sec);
	    event.btime=btime_my  - TZERO;
	    if (btime_myfirst==0){ btime_myfirst=btime_my;}
	    // for every  x  sec -  printout 
	    if (btime_my-btime_mylast > minutes30*60.0){
	      btime_mylast=btime_my;
	      time (&timet);
	      int ttt = btime_my;
	      timet= (time_t)ttt;
	      printf("DB:  t=%f    %20s", btime_my , ctime( &timet ) );
	    }
	    // printf("  t=%f  /  %f\n",event.time, time_mylast );
	  }//last linuxtime word - make time
	  }//only selected time channels, dont do statistical info .....BLOCK_TIME>0







	  //CHANNELS-----------------------------------------------
	  if  ( (pair_chan>=CODING_CHANNELS_LOW)&&(pair_chan<=CODING_CHANNELS_HIGH) ){
	  LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

	  //	  printf("before histofill \n%s","");

	    //printf("   %9d - %9d \n", pair_chan, pair_value );
	  chan[pair_chan]->Fill( pair_value );
	  event.chan[ pair_chan-CODING_CHANNELS_LOW  ]=pair_value;
	  }//only selected channels, dont do statistical info
	  break;
	}//switch(NCASE)========================================================++CASE





	  //COUNTER KC014A----------------------------------------------
	if (   (KC014A_COUNTER>=0)&&(pair_chan>=KC014A_COUNTER)&&(pair_chan<=KC014A_COUNTER+7)  ) {
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

	    if (pair_chan == KC014A_COUNTER  ){
	     kc014A[0]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014A_COUNTER+1 ){
	     kc014A[0]=kc014A[0]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014A_COUNTER+2 ){
	     kc014A[1]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014A_COUNTER+3 ){
	     kc014A[1]=kc014A[1]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014A_COUNTER+4  ){
	     kc014A[2]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014A_COUNTER+5 ){
	     kc014A[2]=kc014A[2]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014A_COUNTER+6 ){
	     kc014A[3]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014A_COUNTER+7 ){
	     kc014A[3]=kc014A[3]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel


	  }//any kc014A channel



	  //COUNTER KC014B----------------------------------------------
	  if (  (KC014B_COUNTER>=0)&&(pair_chan>=KC014B_COUNTER)&&(pair_chan<=KC014B_COUNTER+7)  ) {
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

	    if (pair_chan == KC014B_COUNTER  ){
	     kc014B[0]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014B_COUNTER+1 ){
	     kc014B[0]=kc014B[0]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014B_COUNTER+2 ){
	     kc014B[1]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014B_COUNTER+3 ){
	     kc014B[1]=kc014B[1]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014B_COUNTER+4  ){
	     kc014B[2]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014B_COUNTER+5 ){
	     kc014B[2]=kc014B[2]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014B_COUNTER+6 ){
	     kc014B[3]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014B_COUNTER+7 ){
	     kc014B[3]=kc014B[3]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel


	  }//any kc014B channel





	  //COUNTER KC014C----------------------------------------------
	  if (  (KC014C_COUNTER>=0)&&(pair_chan>=KC014C_COUNTER)&&(pair_chan<=KC014C_COUNTER+7)  ) {
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

	    if (pair_chan == KC014C_COUNTER  ){
	     kc014C[0]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014C_COUNTER+1 ){
	     kc014C[0]=kc014C[0]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014C_COUNTER+2 ){
	     kc014C[1]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014C_COUNTER+3 ){
	     kc014C[1]=kc014C[1]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014C_COUNTER+4  ){
	     kc014C[2]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014C_COUNTER+5 ){
	     kc014C[2]=kc014C[2]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel

	    if (pair_chan == KC014C_COUNTER+6 ){
	     kc014C[3]=(pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf);
	    }//half a channel
	    if (pair_chan == KC014C_COUNTER+7 ){
	     kc014C[3]=kc014C[3]+
	       10000*( (pair_value&0xf) + 10*((pair_value>>4)&0xf)  +100*((pair_value>>8)&0xf) + 1000*((pair_value>>12)&0xf) );
	    }//half a channel


	  }//any kc014C channel





	  //COUNTER c257----------------------------------------------
	  if ( (C257_COUNTER>=0)&&(C257_COUNTER>=0)&&(pair_chan>=C257_COUNTER)&&(pair_chan<=C257_COUNTER+15)  ) {
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

	    for (int i=0;i<16;i++){
	       if (pair_chan == C257_COUNTER+ i  ){  c257[i]=pair_value;   }
	    }

	  }//any c257 channel






          //COUNTER c32----------------------------------------------         
	  // this counter is 32bit in zdenek data....
          if ((V560NA_COUNTER>=0)&&(V560NA_COUNTER>=0)&&(pair_chan>=V560NA_COUNTER)&&(pair_chan<=V560NA_COUNTER+31)){
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

            for (int i=0;i<32;i++){
	      if (pair_chan == V560NA_COUNTER+ 2*i    ){  v560na[i]=pair_value;   }
	      if (pair_chan == V560NA_COUNTER+ 2*i+1  ){  v560na[i]=v560na[i]+65535*pair_value;   }
            }
	    //	    if (pair_chan == C32_COUNTER ){ printf("debug c32: %d\n", c32[0] ); }
          }//any c32 channel           


          //COUNTER c32----------------------------------------------         
	  // this counter is 32bit in zdenek data....
          if ((V560NB_COUNTER>=0)&&(V560NB_COUNTER>=0)&&(pair_chan>=V560NB_COUNTER)&&(pair_chan<=V560NB_COUNTER+31)){
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

            for (int i=0;i<32;i++){
	      if (pair_chan == V560NB_COUNTER+ 2*i    ){  v560nb[i]=pair_value;   }
	      if (pair_chan == V560NB_COUNTER+ 2*i+1  ){  v560nb[i]=v560nb[i]+65535*pair_value;   }
            }
	    //	    if (pair_chan == C32_COUNTER ){ printf("debug c32: %d\n", c32[0] ); }
          }//any c32 channel           



          //COUNTER c32----------------------------------------------         
	  // this counter is 32bit in zdenek data....
          if (  (C32_COUNTER>=0)&&(C32_COUNTER>=0)&&(pair_chan>=C32_COUNTER)&&(pair_chan<=C32_COUNTER+31)  ) {
	    LIN_TIME_ROW_X++;  // 20111113 - test to remove empty polls from data.....

            for (int i=0;i<32;i++){
	      if (pair_chan == C32_COUNTER+ 2*i    ){  c32[i]=pair_value;   }
	      if (pair_chan == C32_COUNTER+ 2*i+1  ){  c32[i]=c32[i]+65535*pair_value;   }
            }
	    //	    if (pair_chan == C32_COUNTER ){ printf("debug c32: %d\n", c32[0] ); }
          }//any c32 channel           







	buf_ptr++;
      }//LOOP parsing the buffer    buf_ptr<res + startup
      //------------------------------------

      if (verbose>0){
      printf("Breaks Out from the LOOP:   buf_ptr==%d\n",  buf_ptr );

      printf("EVENTS: total(headers)     = %lld\n", events_total );
      printf("        this loop(headers) = %d\n", events_this_loop );
      printf("        start/res/loop_passes=(%d,%d,%d)\n", startup,res,loop_passes );
      }
      /*
 printf("new buffer processing ENDED. 4x %d / %d\n", buf_ptr ,  res );
	  if (buf_ptr>10)printf("%08x\n",  readbufQ[ buf_ptr-10 ] );
	  if (buf_ptr>9) printf("%08x\n",  readbufQ[ buf_ptr-9 ] );
	  if (buf_ptr>8) printf("%08x\n",  readbufQ[ buf_ptr-8 ] );
	  if (buf_ptr>7) printf("%08x\n",  readbufQ[ buf_ptr-7 ] );
	  if (buf_ptr>6) printf("%08x\n",  readbufQ[ buf_ptr-6 ] );
	  if (buf_ptr>5) printf("%08x\n",  readbufQ[ buf_ptr-5 ] );
	  if (buf_ptr>4) printf("%08x\n",  readbufQ[ buf_ptr-4 ] );
	  if (buf_ptr>3) printf("%08x\n",  readbufQ[ buf_ptr-3 ] );
	  if (buf_ptr>2) printf("%08x\n",  readbufQ[ buf_ptr-2 ] );
	  if (buf_ptr>1) printf("%08x ....hf==%d   ch=%d/%d\n",  readbufQ[ buf_ptr-1 ], headfoot,channel_n, channels );
      */

}//one_buffer_process..............................................................................................





/*****************************************************************************
 *                         MAIN 
 *
 */





void conv_u(const char* filename, const char* options){
  // channels - one event has n channels, parsing _n
  // headfoot 0,1,2,  init, headfound, footfound
  //  unsigned int w;
  // GLOBAL DEFINITION - LOG FILE   char  chtxt[6000];


  printf("Starting       ****  conv_u  **** %s\n" , "" ); 

 //============================init start
int resinit= conv_u_init(filename, options);
 if (resinit!=0) return;
 //============================init stop
  printf("initialization phase ever...**** %s\n" , "" ); 


  //printf("-------------------------------------------------------%s\n","");
  res=load_buffer( filename );   // THIS IS IN BYTES !!!!!!!!!!!!!!!!
  printf("buffer loaded saizeread==%d bytes\n", res );
  if (res!=0){  

    printf (" ...buff size = %9d %s\n",        BUFSIZE/print_bytes_div, units  );
    printf (" ...file size = %9lld %s\n",        file_max/print_bytes_div, units   );
    printf (" ...read size = %9d %s  (res)\n", res/print_bytes_div, units   );
    printf (" ...parsing only channels from %d to %d\n", CODING_CHANNELS_LOW, CODING_CHANNELS_HIGH );
  printf("-------------------------------------------------------%s\n","");






  buf_ptr=0;  // reset anyway in every  ????
    // LOOP if data from file are in buffer
    while (res>0){
      printf("one buffer process starts (res==sizeread==%d)\n", res );
      one_buffer_process( (void*)&readbuf,  int(res/4.0) ,  0 );   // NEEDS NOT BYTES BUT BYTES/4 !!!!!!!!!
      printf("ttree entries after obp = %lld \n", ttree->GetEntries() );

      // next read
      //      if (headfoot==1){printf("broken event (no footer)\n%s","");}
      res=load_buffer( filename ); 
      buf_ptr=0;  // HERE - ALL THE BUFFER - FROM 0
      //  buf_ptr reset inside one_buffer_process  anyway ??


    }    // LOOP if data from file are in buffer





    //  THIS IS  DATA FILE    load_buffer...............
    fclose( fajl); // .root    ----- was after printout, but maybe better here




    printf("%s\n","*******************");
    printf("%s\n","* processing info *");
    printf("%s\n","*******************");

    // ghits_counter[1]++;
    for (int i=0;i<MAXCHAN;i++){  
       if (ghits_counter[i]>0){ 
	printf("channel %3d - %9ld\n",  i, ghits_counter[i] );
       }  
    } // clear global hits counter

     printf("# of events in total (all channels): %d\n",eventnum );
     printf("from %f s to %f s  (dt = %f s)\n", time_myfirst, time_my, time_my-time_myfirst );
     int ttt;
	      time (&timet);
	       ttt = time_myfirst;
	      timet= (time_t)ttt;
	      printf("from %20s\n", ctime( &timet)  );

	      time (&timet);
	       ttt = time_my;
	      timet= (time_t)ttt;
	      printf("  to %20s\n", ctime( &timet)   );

    if (headfoot!=2){ printf("Stream ended before event footer marker came\n%s","");}
    if (startstop==0){ printf("Neither runstart nor runstop marker seen\n%s","");}
    if (startstop==1){ printf("Stream ended before runstop marker came\n%s","");}
    if (startstop==3){ printf("Stream ended before runstop marker came. More runstart/runstops seen\n%s","");}
    if (startstop==4){ printf("More runstart/runstops markers seen\n%s","");}
    if (missing_time_events>0){ 
      printf("# of removed events because of missing time: %d\n", missing_time_events);
          if (missing_time_events>100){ 
      printf("# TRY SETTING LINUX_TIME=0 --- TOO MANY MISSING EVENTS: %d\n", missing_time_events);
	  }
    }
    //    if (headfoot!=2){ printf("\n%s","");}
     if (KC014A_COUNTER>0){
   printf("Counter KC014A totals:  %8ld\n                       %8ld\n                       %8ld\n                       %8ld\n",
	   kc014A_total[0], kc014A_total[1], kc014A_total[2], kc014A_total[3] );
     }//A
     if (KC014B_COUNTER>0){
    printf("Counter KC014B totals:  %8ld\n                       %8ld\n                       %8ld\n                       %8ld\n",
	   kc014B_total[0], kc014B_total[1], kc014B_total[2], kc014B_total[3] );
     }//B
     if (KC014C_COUNTER>0){
    printf("Counter KC014C totals:  %8ld\n                       %8ld\n                       %8ld\n                       %8ld\n",
	   kc014C_total[0], kc014C_total[1], kc014C_total[2], kc014C_total[3] );
     }//C
    printf("\n\n%s","");

    if (C257_COUNTER>0){
    printf("Counter C257   totals:%s\n","");
    for (int i=0;i<16;i++){     printf("%8ld\n",  c257_total[i] );     }
    }//C257

    if (V560NA_COUNTER>0){
    printf("Counter V560NA    totals:%s\n","");
    for (int i=0;i<32;i++){     printf("%8ld\n",  v560na_total[i] );     }
    }//C32


    if (V560NB_COUNTER>0){
    printf("Counter V560NB    totals:%s\n","");
    for (int i=0;i<32;i++){     printf("%8ld\n",  v560nb_total[i] );     }
    }//C32


    if (C32_COUNTER>0){
    printf("Counter C32    totals:%s\n","");
    for (int i=0;i<32;i++){     printf("%8ld\n",  c32_total[i] );     }
    }//C32






    printf("Trying to open file<%s> and make a printout \n",     chtxt );
    FILE *fout;
    fout=fopen(chtxt,"w");

    fprintf(fout, "%s\n","*******************");
    fprintf(fout, "%s\n","* processing info *");
    fprintf(fout, "%s\n","*******************");
     fprintf(fout, "# of events in total (all channels): %d\n",eventnum );
     fprintf(fout, "from %f s to %f s  (dt = %f s)\n", time_myfirst, time_my, time_my-time_myfirst );
	      time (&timet);
	       ttt = time_myfirst;
	      timet= (time_t)ttt;
	      fprintf(fout,"from %20s\n", ctime( &timet)  );

	      time (&timet);
	       ttt = time_my;
	      timet= (time_t)ttt;
	      fprintf(fout,"  to %20s\n", ctime( &timet)  );

    if (headfoot!=2){  fprintf(fout, "Stream ended before event footer marker came\n%s","");}
    if (startstop==0){ fprintf(fout, "Neither runstart nor runstop marker seen\n%s","");}
    if (startstop==1){ fprintf(fout, "Stream ended before runstop marker came\n%s","");}
    if (startstop==3){ fprintf(fout, "Stream ended before runstop marker came. More runstart/runstops seen\n%s","");}
    if (startstop==4){ fprintf(fout, "More runstart/runstops markers seen\n%s","");}
    if (missing_time_events>0){ fprintf(fout, "# of removed events because of missing time: %d\n", missing_time_events);}
    //    if (headfoot!=2){ printf(fout, "\n%s","");}
    if (KC014A_COUNTER>0){
    fprintf(fout, "Counter KC014A totals:  %8ld\n                        %8ld\n                        %8ld\n                        %8ld\n",
	   kc014A_total[0], kc014A_total[1], kc014A_total[2], kc014A_total[3] );
    }//KC014A
     if (KC014B_COUNTER>0){
   fprintf(fout, "Counter KC014B totals:  %8ld\n                        %8ld\n                        %8ld\n                        %8ld\n",
	   kc014B_total[0], kc014B_total[1], kc014B_total[2], kc014B_total[3] );
    }//KC014B
     if (KC014C_COUNTER>0){
   fprintf(fout, "Counter KC014C totals:  %8ld\n                        %8ld\n                        %8ld\n                        %8ld\n",
	   kc014C_total[0], kc014C_total[1], kc014C_total[2], kc014C_total[3] );
    }//KC014C
    fprintf(fout, "\n\n%s","");   
    
    if (C257_COUNTER>0){
    fprintf(fout,"Counter C257   totals:%s\n","");
    for (int i=0;i<16;i++){     fprintf(fout,"%8ld\n",  c257_total[i] );     }
    }//C257

    if (V560NA_COUNTER>0){
    fprintf(fout,"Counter V560NA    totals:%s\n","");
    for (int i=0;i<32;i++){     fprintf(fout,"%8ld\n",  v560na_total[i] );     }
    }//C32

    if (V560NB_COUNTER>0){
    fprintf(fout,"Counter V560NB    totals:%s\n","");
    for (int i=0;i<32;i++){     fprintf(fout,"%8ld\n",  v560nb_total[i] );     }
    }//C32

    if (C32_COUNTER>0){
    fprintf(fout,"Counter C32    totals:%s\n","");
    for (int i=0;i<32;i++){     fprintf(fout,"%8ld\n",  c32_total[i] );     }
    }//C32

    for (int i=0;i<MAXCHAN;i++){  
      if (ghits_counter[i]>0){ 
	fprintf( fout, "channel %3d - %9ld\n",  i,ghits_counter[i]);
      }  
    } // clear global hits counter

    fclose(fout);// close  txt file
 

    printf(" printout (%s>  DONE...   creating histograms\n",     chtxt );





     if (KC014A_COUNTER>=0){

    TH1F *h_kc014A_1=new TH1F("kc014A_1tot","total kc014A_1",10, kc014A_total[0]-5, kc014A_total[0]+5  ); 
    TH1F *h_kc014A_2=new TH1F("kc014A_2tot","total kc014A_2",10, kc014A_total[1]-5, kc014A_total[1]+5  ); 
    TH1F *h_kc014A_3=new TH1F("kc014A_3tot","total kc014A_3",10, kc014A_total[2]-5, kc014A_total[2]+5  ); 
    TH1F *h_kc014A_4=new TH1F("kc014A_4tot","total kc014A_4",10, kc014A_total[3]-5, kc014A_total[3]+5  ); 
    h_kc014A_1->Fill( kc014A_total[0]);  h_kc014A_1->Write();
    h_kc014A_2->Fill( kc014A_total[1]);  h_kc014A_2->Write();
    h_kc014A_3->Fill( kc014A_total[2]);  h_kc014A_3->Write();
    h_kc014A_4->Fill( kc014A_total[3]);  h_kc014A_4->Write();
    delete h_kc014A_1;
    delete h_kc014A_2;
    delete h_kc014A_3;
    delete h_kc014A_4;  
     }


     if (KC014B_COUNTER>=0){
    TH1F *h_kc014B_1=new TH1F("kc014B_1tot","total kc014B_1",10, kc014B_total[0]-5, kc014B_total[0]+5  ); 
    TH1F *h_kc014B_2=new TH1F("kc014B_2tot","total kc014B_2",10, kc014B_total[1]-5, kc014B_total[1]+5  ); 
    TH1F *h_kc014B_3=new TH1F("kc014B_3tot","total kc014B_3",10, kc014B_total[2]-5, kc014B_total[2]+5  ); 
    TH1F *h_kc014B_4=new TH1F("kc014B_4tot","total kc014B_4",10, kc014B_total[3]-5, kc014B_total[3]+5  ); 
    h_kc014B_1->Fill( kc014B_total[0]);  h_kc014B_1->Write();
    h_kc014B_2->Fill( kc014B_total[1]);  h_kc014B_2->Write();
    h_kc014B_3->Fill( kc014B_total[2]);  h_kc014B_3->Write();
    h_kc014B_4->Fill( kc014B_total[3]);  h_kc014B_4->Write();
    delete h_kc014B_1;
    delete h_kc014B_2;
    delete h_kc014B_3;
    delete h_kc014B_4;  
     }


     if (KC014C_COUNTER>=0){
    TH1F *h_kc014C_1=new TH1F("kc014C_1tot","total kc014C_1",10, kc014C_total[0]-5, kc014C_total[0]+5  ); 
    TH1F *h_kc014C_2=new TH1F("kc014C_2tot","total kc014C_2",10, kc014C_total[1]-5, kc014C_total[1]+5  ); 
    TH1F *h_kc014C_3=new TH1F("kc014C_3tot","total kc014C_3",10, kc014C_total[2]-5, kc014C_total[2]+5  ); 
    TH1F *h_kc014C_4=new TH1F("kc014C_4tot","total kc014C_4",10, kc014C_total[3]-5, kc014C_total[3]+5  ); 
    h_kc014C_1->Fill( kc014C_total[0]);  h_kc014C_1->Write();
    h_kc014C_2->Fill( kc014C_total[1]);  h_kc014C_2->Write();
    h_kc014C_3->Fill( kc014C_total[2]);  h_kc014C_3->Write();
    h_kc014C_4->Fill( kc014C_total[3]);  h_kc014C_4->Write();
    delete h_kc014C_1;
    delete h_kc014C_2;
    delete h_kc014C_3;
    delete h_kc014C_4;  
     }


     if (C257_COUNTER>=0){
    TH1F *h_c257_1= new TH1F("xc257_01tot","total c257_01",10, c257_total[0]-5, c257_total[0]+5  ); 
    TH1F *h_c257_2= new TH1F("xc257_02tot","total c257_02",10, c257_total[1]-5, c257_total[1]+5  ); 
    TH1F *h_c257_3= new TH1F("xc257_03tot","total c257_03",10, c257_total[2]-5, c257_total[2]+5  ); 
    TH1F *h_c257_4= new TH1F("xc257_04tot","total c257_04",10, c257_total[3]-5, c257_total[3]+5  ); 
    TH1F *h_c257_5= new TH1F("xc257_05tot","total c257_05",10, c257_total[4]-5, c257_total[4]+5  ); 
    TH1F *h_c257_6= new TH1F("xc257_06tot","total c257_06",10, c257_total[5]-5, c257_total[5]+5  ); 
    TH1F *h_c257_7= new TH1F("xc257_07tot","total c257_07",10, c257_total[6]-5, c257_total[6]+5  ); 
    TH1F *h_c257_8= new TH1F("xc257_08tot","total c257_08",10, c257_total[7]-5, c257_total[7]+5  ); 
    TH1F *h_c257_9= new TH1F("xc257_09tot","total c257_09",10, c257_total[8]-5, c257_total[8]+5  ); 
    TH1F *h_c257_10=new TH1F("xc257_10tot","total c257_10",10, c257_total[9]-5, c257_total[9]+5  ); 
    TH1F *h_c257_11=new TH1F("xc257_11tot","total c257_11",10, c257_total[10]-5, c257_total[10]+5  ); 
    TH1F *h_c257_12=new TH1F("xc257_12tot","total c257_12",10, c257_total[11]-5, c257_total[11]+5  ); 
    TH1F *h_c257_13=new TH1F("xc257_13tot","total c257_13",10, c257_total[12]-5, c257_total[12]+5  ); 
    TH1F *h_c257_14=new TH1F("xc257_14tot","total c257_14",10, c257_total[13]-5, c257_total[13]+5  ); 
    TH1F *h_c257_15=new TH1F("xc257_15tot","total c257_15",10, c257_total[14]-5, c257_total[14]+5  ); 
    TH1F *h_c257_16=new TH1F("xc257_16tot","total c257_16",10, c257_total[15]-5, c257_total[15]+5  ); 
    h_c257_1->Fill( c257_total[0]);  h_c257_1->Write();
    h_c257_2->Fill( c257_total[1]);  h_c257_2->Write();
    h_c257_3->Fill( c257_total[2]);  h_c257_3->Write();
    h_c257_4->Fill( c257_total[3]);  h_c257_4->Write();
    h_c257_5->Fill( c257_total[4]);  h_c257_5->Write();
    h_c257_6->Fill( c257_total[5]);  h_c257_6->Write();
    h_c257_7->Fill( c257_total[6]);  h_c257_7->Write();
    h_c257_8->Fill( c257_total[7]);  h_c257_8->Write();
    h_c257_9->Fill( c257_total[8]);  h_c257_9->Write();
    h_c257_10->Fill( c257_total[9]);  h_c257_10->Write();
    h_c257_11->Fill( c257_total[10]);  h_c257_11->Write();
    h_c257_12->Fill( c257_total[11]);  h_c257_12->Write();
    h_c257_13->Fill( c257_total[12]);  h_c257_13->Write();
    h_c257_14->Fill( c257_total[13]);  h_c257_14->Write();
    h_c257_15->Fill( c257_total[14]);  h_c257_15->Write();
    h_c257_16->Fill( c257_total[15]);  h_c257_16->Write();
    delete h_c257_1;
    delete h_c257_2;
    delete h_c257_3;
    delete h_c257_4;  
    delete h_c257_5;
    delete h_c257_6;
    delete h_c257_7;
    delete h_c257_8;  
    delete h_c257_9;
    delete h_c257_10;
    delete h_c257_11;
    delete h_c257_12;  
    delete h_c257_13;
    delete h_c257_14;
    delete h_c257_15;
    delete h_c257_16;  
     }

     // V560NA-----------------------------
     if (V560NA_COUNTER>=0){
    TH1F *h_v560na_1= new TH1F("xv560na_01tot","total v560na_01",10, v560na_total[0]-5, v560na_total[0]+5  ); 
    TH1F *h_v560na_2= new TH1F("xv560na_02tot","total v560na_02",10, v560na_total[1]-5, v560na_total[1]+5  ); 
    TH1F *h_v560na_3= new TH1F("xv560na_03tot","total v560na_03",10, v560na_total[2]-5, v560na_total[2]+5  ); 
    TH1F *h_v560na_4= new TH1F("xv560na_04tot","total v560na_04",10, v560na_total[3]-5, v560na_total[3]+5  ); 
    TH1F *h_v560na_5= new TH1F("xv560na_05tot","total v560na_05",10, v560na_total[4]-5, v560na_total[4]+5  ); 
    TH1F *h_v560na_6= new TH1F("xv560na_06tot","total v560na_06",10, v560na_total[5]-5, v560na_total[5]+5  ); 
    TH1F *h_v560na_7= new TH1F("xv560na_07tot","total v560na_07",10, v560na_total[6]-5, v560na_total[6]+5  ); 
    TH1F *h_v560na_8= new TH1F("xv560na_08tot","total v560na_08",10, v560na_total[7]-5, v560na_total[7]+5  ); 
    TH1F *h_v560na_9= new TH1F("xv560na_09tot","total v560na_09",10, v560na_total[8]-5, v560na_total[8]+5  ); 
    TH1F *h_v560na_10=new TH1F("xv560na_10tot","total v560na_10",10, v560na_total[9]-5, v560na_total[9]+5  ); 
    TH1F *h_v560na_11=new TH1F("xv560na_11tot","total v560na_11",10, v560na_total[10]-5, v560na_total[10]+5  ); 
    TH1F *h_v560na_12=new TH1F("xv560na_12tot","total v560na_12",10, v560na_total[11]-5, v560na_total[11]+5  ); 
    TH1F *h_v560na_13=new TH1F("xv560na_13tot","total v560na_13",10, v560na_total[12]-5, v560na_total[12]+5  ); 
    TH1F *h_v560na_14=new TH1F("xv560na_14tot","total v560na_14",10, v560na_total[13]-5, v560na_total[13]+5  ); 
    TH1F *h_v560na_15=new TH1F("xv560na_15tot","total v560na_15",10, v560na_total[14]-5, v560na_total[14]+5  ); 
    TH1F *h_v560na_16=new TH1F("xv560na_16tot","total v560na_16",10, v560na_total[15]-5, v560na_total[15]+5  ); 
    TH1F *h_v560na_17=new TH1F("xv560na_17tot","total v560na_17",10, v560na_total[16]-5, v560na_total[16]+5  ); 
    TH1F *h_v560na_18=new TH1F("xv560na_18tot","total v560na_18",10, v560na_total[17]-5, v560na_total[17]+5  ); 
    TH1F *h_v560na_19=new TH1F("xv560na_19tot","total v560na_19",10, v560na_total[18]-5, v560na_total[18]+5  ); 
    TH1F *h_v560na_20=new TH1F("xv560na_20tot","total v560na_20",10, v560na_total[19]-5, v560na_total[19]+5  ); 
    TH1F *h_v560na_21=new TH1F("xv560na_21tot","total v560na_21",10, v560na_total[20]-5, v560na_total[20]+5  ); 
    TH1F *h_v560na_22=new TH1F("xv560na_22tot","total v560na_22",10, v560na_total[21]-5, v560na_total[21]+5  ); 
    TH1F *h_v560na_23=new TH1F("xv560na_23tot","total v560na_23",10, v560na_total[22]-5, v560na_total[22]+5  ); 
    TH1F *h_v560na_24=new TH1F("xv560na_24tot","total v560na_24",10, v560na_total[23]-5, v560na_total[23]+5  ); 
    TH1F *h_v560na_25=new TH1F("xv560na_25tot","total v560na_25",10, v560na_total[24]-5, v560na_total[24]+5  ); 
    TH1F *h_v560na_26=new TH1F("xv560na_26tot","total v560na_26",10, v560na_total[25]-5, v560na_total[25]+5  ); 
    TH1F *h_v560na_27=new TH1F("xv560na_27tot","total v560na_27",10, v560na_total[26]-5, v560na_total[26]+5  ); 
    TH1F *h_v560na_28=new TH1F("xv560na_28tot","total v560na_28",10, v560na_total[27]-5, v560na_total[27]+5  ); 
    TH1F *h_v560na_29=new TH1F("xv560na_29tot","total v560na_29",10, v560na_total[28]-5, v560na_total[28]+5  ); 
    TH1F *h_v560na_30=new TH1F("xv560na_30tot","total v560na_30",10, v560na_total[29]-5, v560na_total[29]+5  ); 
    TH1F *h_v560na_31=new TH1F("xv560na_31tot","total v560na_31",10, v560na_total[30]-5, v560na_total[30]+5  ); 
    TH1F *h_v560na_32=new TH1F("xv560na_32tot","total v560na_32",10, v560na_total[31]-5, v560na_total[31]+5  ); 
    h_v560na_1->Fill( v560na_total[0]);  h_v560na_1->Write();
    h_v560na_2->Fill( v560na_total[1]);  h_v560na_2->Write();
    h_v560na_3->Fill( v560na_total[2]);  h_v560na_3->Write();
    h_v560na_4->Fill( v560na_total[3]);  h_v560na_4->Write();
    h_v560na_5->Fill( v560na_total[4]);  h_v560na_5->Write();
    h_v560na_6->Fill( v560na_total[5]);  h_v560na_6->Write();
    h_v560na_7->Fill( v560na_total[6]);  h_v560na_7->Write();
    h_v560na_8->Fill( v560na_total[7]);  h_v560na_8->Write();
    h_v560na_9->Fill( v560na_total[8]);  h_v560na_9->Write();
    h_v560na_10->Fill( v560na_total[9]);  h_v560na_10->Write();
    h_v560na_11->Fill( v560na_total[10]);  h_v560na_11->Write();
    h_v560na_12->Fill( v560na_total[11]);  h_v560na_12->Write();
    h_v560na_13->Fill( v560na_total[12]);  h_v560na_13->Write();
    h_v560na_14->Fill( v560na_total[13]);  h_v560na_14->Write();
    h_v560na_15->Fill( v560na_total[14]);  h_v560na_15->Write();
    h_v560na_16->Fill( v560na_total[15]);  h_v560na_16->Write();
    h_v560na_17->Fill( v560na_total[16]);  h_v560na_17->Write();
    h_v560na_18->Fill( v560na_total[17]);  h_v560na_18->Write();
    h_v560na_19->Fill( v560na_total[18]);  h_v560na_19->Write();
    h_v560na_20->Fill( v560na_total[19]);  h_v560na_20->Write();
    h_v560na_21->Fill( v560na_total[20]);  h_v560na_21->Write();
    h_v560na_22->Fill( v560na_total[21]);  h_v560na_22->Write();
    h_v560na_23->Fill( v560na_total[22]);  h_v560na_23->Write();
    h_v560na_24->Fill( v560na_total[23]);  h_v560na_24->Write();
    h_v560na_25->Fill( v560na_total[24]);  h_v560na_25->Write();
    h_v560na_26->Fill( v560na_total[25]);  h_v560na_26->Write();
    h_v560na_27->Fill( v560na_total[26]);  h_v560na_27->Write();
    h_v560na_28->Fill( v560na_total[27]);  h_v560na_28->Write();
    h_v560na_29->Fill( v560na_total[28]);  h_v560na_29->Write();
    h_v560na_30->Fill( v560na_total[29]);  h_v560na_30->Write();
    h_v560na_31->Fill( v560na_total[30]);  h_v560na_31->Write();
    h_v560na_32->Fill( v560na_total[31]);  h_v560na_32->Write();
    delete h_v560na_1;
    delete h_v560na_2;
    delete h_v560na_3;
    delete h_v560na_4;  
    delete h_v560na_5;
    delete h_v560na_6;
    delete h_v560na_7;
    delete h_v560na_8;  
    delete h_v560na_9;
    delete h_v560na_10;
    delete h_v560na_11;
    delete h_v560na_12;  
    delete h_v560na_13;
    delete h_v560na_14;
    delete h_v560na_15;
    delete h_v560na_16;  
    delete h_v560na_17;
    delete h_v560na_18;
    delete h_v560na_19;
    delete h_v560na_20;  
    delete h_v560na_21;
    delete h_v560na_22;
    delete h_v560na_23;
    delete h_v560na_24;  
    delete h_v560na_25;
    delete h_v560na_26;
    delete h_v560na_27;
    delete h_v560na_28;  
    delete h_v560na_29;
    delete h_v560na_30;
    delete h_v560na_31;
    delete h_v560na_32;  
     }

     // V560NB-----------------------------
     if (V560NB_COUNTER>=0){
    TH1F *h_v560nb_1= new TH1F("xv560nb_01tot","total v560nb_01",10, v560nb_total[0]-5, v560nb_total[0]+5  ); 
    TH1F *h_v560nb_2= new TH1F("xv560nb_02tot","total v560nb_02",10, v560nb_total[1]-5, v560nb_total[1]+5  ); 
    TH1F *h_v560nb_3= new TH1F("xv560nb_03tot","total v560nb_03",10, v560nb_total[2]-5, v560nb_total[2]+5  ); 
    TH1F *h_v560nb_4= new TH1F("xv560nb_04tot","total v560nb_04",10, v560nb_total[3]-5, v560nb_total[3]+5  ); 
    TH1F *h_v560nb_5= new TH1F("xv560nb_05tot","total v560nb_05",10, v560nb_total[4]-5, v560nb_total[4]+5  ); 
    TH1F *h_v560nb_6= new TH1F("xv560nb_06tot","total v560nb_06",10, v560nb_total[5]-5, v560nb_total[5]+5  ); 
    TH1F *h_v560nb_7= new TH1F("xv560nb_07tot","total v560nb_07",10, v560nb_total[6]-5, v560nb_total[6]+5  ); 
    TH1F *h_v560nb_8= new TH1F("xv560nb_08tot","total v560nb_08",10, v560nb_total[7]-5, v560nb_total[7]+5  ); 
    TH1F *h_v560nb_9= new TH1F("xv560nb_09tot","total v560nb_09",10, v560nb_total[8]-5, v560nb_total[8]+5  ); 
    TH1F *h_v560nb_10=new TH1F("xv560nb_10tot","total v560nb_10",10, v560nb_total[9]-5, v560nb_total[9]+5  ); 
    TH1F *h_v560nb_11=new TH1F("xv560nb_11tot","total v560nb_11",10, v560nb_total[10]-5, v560nb_total[10]+5  ); 
    TH1F *h_v560nb_12=new TH1F("xv560nb_12tot","total v560nb_12",10, v560nb_total[11]-5, v560nb_total[11]+5  ); 
    TH1F *h_v560nb_13=new TH1F("xv560nb_13tot","total v560nb_13",10, v560nb_total[12]-5, v560nb_total[12]+5  ); 
    TH1F *h_v560nb_14=new TH1F("xv560nb_14tot","total v560nb_14",10, v560nb_total[13]-5, v560nb_total[13]+5  ); 
    TH1F *h_v560nb_15=new TH1F("xv560nb_15tot","total v560nb_15",10, v560nb_total[14]-5, v560nb_total[14]+5  ); 
    TH1F *h_v560nb_16=new TH1F("xv560nb_16tot","total v560nb_16",10, v560nb_total[15]-5, v560nb_total[15]+5  ); 
    TH1F *h_v560nb_17=new TH1F("xv560nb_17tot","total v560nb_17",10, v560nb_total[16]-5, v560nb_total[16]+5  ); 
    TH1F *h_v560nb_18=new TH1F("xv560nb_18tot","total v560nb_18",10, v560nb_total[17]-5, v560nb_total[17]+5  ); 
    TH1F *h_v560nb_19=new TH1F("xv560nb_19tot","total v560nb_19",10, v560nb_total[18]-5, v560nb_total[18]+5  ); 
    TH1F *h_v560nb_20=new TH1F("xv560nb_20tot","total v560nb_20",10, v560nb_total[19]-5, v560nb_total[19]+5  ); 
    TH1F *h_v560nb_21=new TH1F("xv560nb_21tot","total v560nb_21",10, v560nb_total[20]-5, v560nb_total[20]+5  ); 
    TH1F *h_v560nb_22=new TH1F("xv560nb_22tot","total v560nb_22",10, v560nb_total[21]-5, v560nb_total[21]+5  ); 
    TH1F *h_v560nb_23=new TH1F("xv560nb_23tot","total v560nb_23",10, v560nb_total[22]-5, v560nb_total[22]+5  ); 
    TH1F *h_v560nb_24=new TH1F("xv560nb_24tot","total v560nb_24",10, v560nb_total[23]-5, v560nb_total[23]+5  ); 
    TH1F *h_v560nb_25=new TH1F("xv560nb_25tot","total v560nb_25",10, v560nb_total[24]-5, v560nb_total[24]+5  ); 
    TH1F *h_v560nb_26=new TH1F("xv560nb_26tot","total v560nb_26",10, v560nb_total[25]-5, v560nb_total[25]+5  ); 
    TH1F *h_v560nb_27=new TH1F("xv560nb_27tot","total v560nb_27",10, v560nb_total[26]-5, v560nb_total[26]+5  ); 
    TH1F *h_v560nb_28=new TH1F("xv560nb_28tot","total v560nb_28",10, v560nb_total[27]-5, v560nb_total[27]+5  ); 
    TH1F *h_v560nb_29=new TH1F("xv560nb_29tot","total v560nb_29",10, v560nb_total[28]-5, v560nb_total[28]+5  ); 
    TH1F *h_v560nb_30=new TH1F("xv560nb_30tot","total v560nb_30",10, v560nb_total[29]-5, v560nb_total[29]+5  ); 
    TH1F *h_v560nb_31=new TH1F("xv560nb_31tot","total v560nb_31",10, v560nb_total[30]-5, v560nb_total[30]+5  ); 
    TH1F *h_v560nb_32=new TH1F("xv560nb_32tot","total v560nb_32",10, v560nb_total[31]-5, v560nb_total[31]+5  ); 
    h_v560nb_1->Fill( v560nb_total[0]);  h_v560nb_1->Write();
    h_v560nb_2->Fill( v560nb_total[1]);  h_v560nb_2->Write();
    h_v560nb_3->Fill( v560nb_total[2]);  h_v560nb_3->Write();
    h_v560nb_4->Fill( v560nb_total[3]);  h_v560nb_4->Write();
    h_v560nb_5->Fill( v560nb_total[4]);  h_v560nb_5->Write();
    h_v560nb_6->Fill( v560nb_total[5]);  h_v560nb_6->Write();
    h_v560nb_7->Fill( v560nb_total[6]);  h_v560nb_7->Write();
    h_v560nb_8->Fill( v560nb_total[7]);  h_v560nb_8->Write();
    h_v560nb_9->Fill( v560nb_total[8]);  h_v560nb_9->Write();
    h_v560nb_10->Fill( v560nb_total[9]);  h_v560nb_10->Write();
    h_v560nb_11->Fill( v560nb_total[10]);  h_v560nb_11->Write();
    h_v560nb_12->Fill( v560nb_total[11]);  h_v560nb_12->Write();
    h_v560nb_13->Fill( v560nb_total[12]);  h_v560nb_13->Write();
    h_v560nb_14->Fill( v560nb_total[13]);  h_v560nb_14->Write();
    h_v560nb_15->Fill( v560nb_total[14]);  h_v560nb_15->Write();
    h_v560nb_16->Fill( v560nb_total[15]);  h_v560nb_16->Write();
    h_v560nb_17->Fill( v560nb_total[16]);  h_v560nb_17->Write();
    h_v560nb_18->Fill( v560nb_total[17]);  h_v560nb_18->Write();
    h_v560nb_19->Fill( v560nb_total[18]);  h_v560nb_19->Write();
    h_v560nb_20->Fill( v560nb_total[19]);  h_v560nb_20->Write();
    h_v560nb_21->Fill( v560nb_total[20]);  h_v560nb_21->Write();
    h_v560nb_22->Fill( v560nb_total[21]);  h_v560nb_22->Write();
    h_v560nb_23->Fill( v560nb_total[22]);  h_v560nb_23->Write();
    h_v560nb_24->Fill( v560nb_total[23]);  h_v560nb_24->Write();
    h_v560nb_25->Fill( v560nb_total[24]);  h_v560nb_25->Write();
    h_v560nb_26->Fill( v560nb_total[25]);  h_v560nb_26->Write();
    h_v560nb_27->Fill( v560nb_total[26]);  h_v560nb_27->Write();
    h_v560nb_28->Fill( v560nb_total[27]);  h_v560nb_28->Write();
    h_v560nb_29->Fill( v560nb_total[28]);  h_v560nb_29->Write();
    h_v560nb_30->Fill( v560nb_total[29]);  h_v560nb_30->Write();
    h_v560nb_31->Fill( v560nb_total[30]);  h_v560nb_31->Write();
    h_v560nb_32->Fill( v560nb_total[31]);  h_v560nb_32->Write();
    delete h_v560nb_1;
    delete h_v560nb_2;
    delete h_v560nb_3;
    delete h_v560nb_4;  
    delete h_v560nb_5;
    delete h_v560nb_6;
    delete h_v560nb_7;
    delete h_v560nb_8;  
    delete h_v560nb_9;
    delete h_v560nb_10;
    delete h_v560nb_11;
    delete h_v560nb_12;  
    delete h_v560nb_13;
    delete h_v560nb_14;
    delete h_v560nb_15;
    delete h_v560nb_16;  
    delete h_v560nb_17;
    delete h_v560nb_18;
    delete h_v560nb_19;
    delete h_v560nb_20;  
    delete h_v560nb_21;
    delete h_v560nb_22;
    delete h_v560nb_23;
    delete h_v560nb_24;  
    delete h_v560nb_25;
    delete h_v560nb_26;
    delete h_v560nb_27;
    delete h_v560nb_28;  
    delete h_v560nb_29;
    delete h_v560nb_30;
    delete h_v560nb_31;
    delete h_v560nb_32;  
     }


     // C32-----------------------------
     if (C32_COUNTER>=0){
    TH1F *h_c32_1= new TH1F("xc32_01tot","total c32_01",10, c32_total[0]-5, c32_total[0]+5  ); 
    TH1F *h_c32_2= new TH1F("xc32_02tot","total c32_02",10, c32_total[1]-5, c32_total[1]+5  ); 
    TH1F *h_c32_3= new TH1F("xc32_03tot","total c32_03",10, c32_total[2]-5, c32_total[2]+5  ); 
    TH1F *h_c32_4= new TH1F("xc32_04tot","total c32_04",10, c32_total[3]-5, c32_total[3]+5  ); 
    TH1F *h_c32_5= new TH1F("xc32_05tot","total c32_05",10, c32_total[4]-5, c32_total[4]+5  ); 
    TH1F *h_c32_6= new TH1F("xc32_06tot","total c32_06",10, c32_total[5]-5, c32_total[5]+5  ); 
    TH1F *h_c32_7= new TH1F("xc32_07tot","total c32_07",10, c32_total[6]-5, c32_total[6]+5  ); 
    TH1F *h_c32_8= new TH1F("xc32_08tot","total c32_08",10, c32_total[7]-5, c32_total[7]+5  ); 
    TH1F *h_c32_9= new TH1F("xc32_09tot","total c32_09",10, c32_total[8]-5, c32_total[8]+5  ); 
    TH1F *h_c32_10=new TH1F("xc32_10tot","total c32_10",10, c32_total[9]-5, c32_total[9]+5  ); 
    TH1F *h_c32_11=new TH1F("xc32_11tot","total c32_11",10, c32_total[10]-5, c32_total[10]+5  ); 
    TH1F *h_c32_12=new TH1F("xc32_12tot","total c32_12",10, c32_total[11]-5, c32_total[11]+5  ); 
    TH1F *h_c32_13=new TH1F("xc32_13tot","total c32_13",10, c32_total[12]-5, c32_total[12]+5  ); 
    TH1F *h_c32_14=new TH1F("xc32_14tot","total c32_14",10, c32_total[13]-5, c32_total[13]+5  ); 
    TH1F *h_c32_15=new TH1F("xc32_15tot","total c32_15",10, c32_total[14]-5, c32_total[14]+5  ); 
    TH1F *h_c32_16=new TH1F("xc32_16tot","total c32_16",10, c32_total[15]-5, c32_total[15]+5  ); 
    TH1F *h_c32_17=new TH1F("xc32_17tot","total c32_17",10, c32_total[16]-5, c32_total[16]+5  ); 
    TH1F *h_c32_18=new TH1F("xc32_18tot","total c32_18",10, c32_total[17]-5, c32_total[17]+5  ); 
    TH1F *h_c32_19=new TH1F("xc32_19tot","total c32_19",10, c32_total[18]-5, c32_total[18]+5  ); 
    TH1F *h_c32_20=new TH1F("xc32_20tot","total c32_20",10, c32_total[19]-5, c32_total[19]+5  ); 
    TH1F *h_c32_21=new TH1F("xc32_21tot","total c32_21",10, c32_total[20]-5, c32_total[20]+5  ); 
    TH1F *h_c32_22=new TH1F("xc32_22tot","total c32_22",10, c32_total[21]-5, c32_total[21]+5  ); 
    TH1F *h_c32_23=new TH1F("xc32_23tot","total c32_23",10, c32_total[22]-5, c32_total[22]+5  ); 
    TH1F *h_c32_24=new TH1F("xc32_24tot","total c32_24",10, c32_total[23]-5, c32_total[23]+5  ); 
    TH1F *h_c32_25=new TH1F("xc32_25tot","total c32_25",10, c32_total[24]-5, c32_total[24]+5  ); 
    TH1F *h_c32_26=new TH1F("xc32_26tot","total c32_26",10, c32_total[25]-5, c32_total[25]+5  ); 
    TH1F *h_c32_27=new TH1F("xc32_27tot","total c32_27",10, c32_total[26]-5, c32_total[26]+5  ); 
    TH1F *h_c32_28=new TH1F("xc32_28tot","total c32_28",10, c32_total[27]-5, c32_total[27]+5  ); 
    TH1F *h_c32_29=new TH1F("xc32_29tot","total c32_29",10, c32_total[28]-5, c32_total[28]+5  ); 
    TH1F *h_c32_30=new TH1F("xc32_30tot","total c32_30",10, c32_total[29]-5, c32_total[29]+5  ); 
    TH1F *h_c32_31=new TH1F("xc32_31tot","total c32_31",10, c32_total[30]-5, c32_total[30]+5  ); 
    TH1F *h_c32_32=new TH1F("xc32_32tot","total c32_32",10, c32_total[31]-5, c32_total[31]+5  ); 
    h_c32_1->Fill( c32_total[0]);  h_c32_1->Write();
    h_c32_2->Fill( c32_total[1]);  h_c32_2->Write();
    h_c32_3->Fill( c32_total[2]);  h_c32_3->Write();
    h_c32_4->Fill( c32_total[3]);  h_c32_4->Write();
    h_c32_5->Fill( c32_total[4]);  h_c32_5->Write();
    h_c32_6->Fill( c32_total[5]);  h_c32_6->Write();
    h_c32_7->Fill( c32_total[6]);  h_c32_7->Write();
    h_c32_8->Fill( c32_total[7]);  h_c32_8->Write();
    h_c32_9->Fill( c32_total[8]);  h_c32_9->Write();
    h_c32_10->Fill( c32_total[9]);  h_c32_10->Write();
    h_c32_11->Fill( c32_total[10]);  h_c32_11->Write();
    h_c32_12->Fill( c32_total[11]);  h_c32_12->Write();
    h_c32_13->Fill( c32_total[12]);  h_c32_13->Write();
    h_c32_14->Fill( c32_total[13]);  h_c32_14->Write();
    h_c32_15->Fill( c32_total[14]);  h_c32_15->Write();
    h_c32_16->Fill( c32_total[15]);  h_c32_16->Write();
    h_c32_17->Fill( c32_total[16]);  h_c32_17->Write();
    h_c32_18->Fill( c32_total[17]);  h_c32_18->Write();
    h_c32_19->Fill( c32_total[18]);  h_c32_19->Write();
    h_c32_20->Fill( c32_total[19]);  h_c32_20->Write();
    h_c32_21->Fill( c32_total[20]);  h_c32_21->Write();
    h_c32_22->Fill( c32_total[21]);  h_c32_22->Write();
    h_c32_23->Fill( c32_total[22]);  h_c32_23->Write();
    h_c32_24->Fill( c32_total[23]);  h_c32_24->Write();
    h_c32_25->Fill( c32_total[24]);  h_c32_25->Write();
    h_c32_26->Fill( c32_total[25]);  h_c32_26->Write();
    h_c32_27->Fill( c32_total[26]);  h_c32_27->Write();
    h_c32_28->Fill( c32_total[27]);  h_c32_28->Write();
    h_c32_29->Fill( c32_total[28]);  h_c32_29->Write();
    h_c32_30->Fill( c32_total[29]);  h_c32_30->Write();
    h_c32_31->Fill( c32_total[30]);  h_c32_31->Write();
    h_c32_32->Fill( c32_total[31]);  h_c32_32->Write();
    delete h_c32_1;
    delete h_c32_2;
    delete h_c32_3;
    delete h_c32_4;  
    delete h_c32_5;
    delete h_c32_6;
    delete h_c32_7;
    delete h_c32_8;  
    delete h_c32_9;
    delete h_c32_10;
    delete h_c32_11;
    delete h_c32_12;  
    delete h_c32_13;
    delete h_c32_14;
    delete h_c32_15;
    delete h_c32_16;  
    delete h_c32_17;
    delete h_c32_18;
    delete h_c32_19;
    delete h_c32_20;  
    delete h_c32_21;
    delete h_c32_22;
    delete h_c32_23;
    delete h_c32_24;  
    delete h_c32_25;
    delete h_c32_26;
    delete h_c32_27;
    delete h_c32_28;  
    delete h_c32_29;
    delete h_c32_30;
    delete h_c32_31;
    delete h_c32_32;  
     }




    // Insert also total time info
    TH1F *h_tottime=new TH1F("time_tot","last-first_time;[s]",10000, 
			     int(time_my-time_myfirst) -5, 
			     int(time_my-time_myfirst)+5  ); 
    h_tottime->Fill( time_my-time_myfirst );h_tottime->Write();
    delete h_tottime;




    /*
     * write ttree   nanot,   write  histos
     */
    if ( strlen(filename)>0){   ftree->Write(); }
    for (int i=CODING_CHANNELS_LOW;i<=CODING_CHANNELS_HIGH;i++){
      chan[i]->Write();
    }
    counting->Write();
    if ( strlen(filename)>0){ ftree->Close(); }


  }// res!=0 ... fajl was open successfuly



}// conv_u


