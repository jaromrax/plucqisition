#define DEBUG 0
/*
    ./compile_root xml_attr.C  ZH_data.C
*/
#include "ZH_data.h"
#include "xml_attr.h"



#include <stdio.h>        // also   4 strcmp

#include "TROOT.h"    // Main INCLUDE !!!!
//in h #include "TFile.h"
//in h #include "TH1F.h"
#include "TTree.h"
#include <stdlib.h>     /* atof */
#include <string.h>         // strcmp
#include <string>         // strcmp

int ZHbuffer[99000000];//="ahoj\0";  // I MUST USE int!!!!!!????
 int OEbuf[1000];//  ONE EVENT buffer; ;

 int OEBmax;// ONE EVENT LIMIT !!!!!!!!!!
 int DataRead; // HowMuch was read to buffer
 int64_t cnt_evt; // event number

 Long64_t cnt_evt_data; // event number, data not time// in TTree

//const int MAXCHAN=2048; // time is 1024+
   int       T_yn[MAXCHAN];//  4 levels (t1 t2 t3 t4)
   TH1F*     Thist;        //  one histogram for ALL
   int       C_yn[MAXCHAN]; //level lo or hi ==  1 or 2
   int64_t   COUN[MAXCHAN];// total (displays in title)
   TH1F*     COUNhist[MAXCHAN];//histo to fill 
   int64_t   COUNtmp[MAXCHAN]; //one shot value (2 passes)
   TH1F*     HIST[MAXCHAN];    //address of NULL
   UShort_t  TREE[MAXCHAN];    //variables to clear after fill
   UShort_t  ZERO[MAXCHAN];    //fast reset to TREE
// - s : a 16 bit unsigned integer (UShort_t)
   TTree *ZH_tree;


 double cTIME; // current data TIME (always>0)
 double cTIME_root; // current data TIME (always>0)
 double bTIME; // buffered time (mostly 0)
 double sTIME; // startup time
 double dTIME; // difference


 TFile *ftree;

/*----------------what can be done with channel-------------
1/ fill histo
 chan008
2/ decompose time ...1st approach: any time updates current
  a/ 1024
  b/ 1028
  c/ 141
  d/ ...
3/ run number 139
4/ block 140
5/ counters
X/   FILL TREE
nanot:
info                 souvisle pole      individ
timeV rnumV bnumV ...  V001 V002   ...  countrs

chan   histo   map_to_ttree   time
1       &TH1F    &var          
 */





//=======================  service===  read buffer from disk----
void fillbuffer(){
  FILE *f;
  int *pch=&ZHbuffer[0];
  //  printf( "I see   /%s/\n",   pch );
  f=fopen("/home/ojr/00_central/Root/03_programs/58_kibbler_narval/RUN018_V","rb");
  if (f==NULL){ printf("BAD FILE\%s\n", "" );return;}
  DataRead=fread( pch , 1, sizeof(ZHbuffer) , f  );
  printf("%d READ\n", DataRead  );
  fclose(f);
}


//==========================service=== prepare  <int> ====4Bytes
//0000  e001    0012    008b    0000    F000   from OD translates to 
//e0010000     008b0012         f0000000  


// int EOE= 0xf0000000;//end of event
// int BOR= 0xEFFFFFFF;//begin of run
// int BOE= 0xe0000000;//begin of event
// int BOEm=0xe000ffff;//begin of event




int fillOEB(int pos){ // 
  int c=0;
   while( 1==1  ){
     if (DEBUG)printf("/%08X/ ",  ZHbuffer[pos+c] );
    OEbuf[c]= ZHbuffer[pos+c];
    if (OEbuf[c]==EOE){ break;}
    c++; 
    if (pos+c> DataRead )return -1;
  }//while
   if (DEBUG)printf("\n%s", ""  );//4
  //  printf("\n    sizeof(int)==%ld\n", sizeof(word)  );//4

  return pos+c+1;
}



int channels_in_event(int word){// mask Exxx0000 and shif by two nibbles
  return (word&0x0fff0000)>>16;
}



void reset_chan_table(){//  PREPARE CLEAN TABLE
for (int i=0;i<MAXCHAN;i++){T_yn[i]=0;}//0 1 2 3 4
for (int i=0;i<MAXCHAN;i++){C_yn[i]=0;}
for (int i=0;i<MAXCHAN;i++){COUN[i]=0;}
for (int i=0;i<MAXCHAN;i++){COUNtmp[i]=0;}
for (int i=0;i<MAXCHAN;i++){HIST[i]=NULL;}   
for (int i=0;i<MAXCHAN;i++){TREE[i]=0;}   
 for (int i=0;i<MAXCHAN;i++){ZERO[i]=0;}//remains always 0
 cTIME=0.0;// current event time
 cTIME_root=0.0;// current event time
 cnt_evt=0;
 cnt_evt_data=0;
 sTIME=0.0;
 bTIME=0.0; // just for fun - (t)event buffer
 dTIME=0.0; // just for fun - difference
}


  //  sprintf( initline,   "chanlo=%d,chanhi=%d,kc014A=%d,kc014B=%d,kc014C=%d,ltime=%d,btime=%d,bnum=%d,c257=%d,c32=%d,v560na=%d,v560nb=%d,parnam=%s,tzero=%lf,circular=%d", 	   chanlo, chanhi,kc014A,kc014B,kc014C,ltime,btime,bnum,c257,c32,v560na,v560nb,parnam,tzero,	   circular); 











//=================================== TH1F ======= TTree ==========================
//  c001
//     ===  t1 t2 t3 t4  ;   s001...     1..999
//=================================================================================


void load_chan_table(const char *str2k ){ // LOAD channel properties into the table
                                      // EITHER histo OR time OR counter:   not two
  char conf[2000];
  //="c001=1,c002=2,c003=3,c004=4,c005=5,c006=6,c007=7,c008=8,c009=9,c010=10,c011=11,c1024=t1,c1025=t2,c1026=t3,c1027=t4,c033=s001,c035=s002,c037=s003";
  strcpy( conf, str2k );

  //-------------variables for tokens-------
  char tok[10];
  char tokres[100];
  int toki;


  char ch[1000];//  DEFINE TTree; I make it 1 leaf/branch
  char ttree_name[50];//
  char brname[100];   //
  int ttree_exists=0; // NO, but to be checked...

  sprintf( ttree_name, "%s",  "nanot"  ); 
  if ( gDirectory->Get( ttree_name ) != NULL){ ttree_exists=1;  }

  //if (ZH_tree==NULL){//==========================ZH TREE   BEGIN==
  if ( ttree_exists==0 ){//=========================ZH TREE   BEGIN==

    toki=TokenGet( "file=", conf , tokres);
    if (strlen(tokres)>0){
      ftree=new TFile(tokres, "NEW" );
      ftree->cd();
      ZH_tree = new TTree( ttree_name , "ttree_from_ZH_data");  
    }else{
      gROOT->cd(); // go to memory resident ttree
      ZH_tree = new TTree( ttree_name , "ttree_from_ZH_data");  
    }

    toki=TokenGet( "circtree=", conf , tokres);
    if (toki>0){//CIRCTREE = 100 000
      ZH_tree->SetCircular( toki ); //set it CIRCULAR....
      printf("circular TTree %d events\n",  toki );
    }
    ZH_tree->Branch( "time" , &cTIME_root, "time/D" );// /D == Double_t 64bit
    ZH_tree->Branch( "cnt_evt" , &cnt_evt_data, "n/L" );// /: == Long64_t 64bi
  }else{ // already exists..........???
    ZH_tree->SetBranchAddress( "time" ,&cTIME_root );
    ZH_tree->SetBranchAddress( "cnt_evt" ,&cnt_evt_data );// /i == Long64_t 64bit
  }//==============================================ZH TREE   END====




  //================================ histograms____vvvvv_________
  // HERE I DEFINE TH1F---------------------------TH1F---------
  for (int i=1; i< MAXCHAN; i++){//  c0001-c2000

    sprintf( tok, "c%03d=", i ); 
    toki=TokenGet( tok, conf, tokres); // get the integer after c001=

    if (toki!=0){// integer was found => it is HISTO------------
      sprintf( tok, "c%03d", toki ); // new channel=arbitrary integer<=999
      TH1F *h=new TH1F( tok, tok, 8000,0,8000);
      HIST[i]=h;

      // DEFINE TTREE...........for every histo channel....
      sprintf( brname,  "V%03d", toki );//  branch  c001 
      sprintf(ch ,"%s/s", brname );  //  UShort_t ===  /s  
      if (ttree_exists==0){// does not exist yet
	ZH_tree->Branch(brname , &TREE[i], ch );// 
      }else{// already exists
	ZH_tree->SetBranchAddress( brname ,&TREE[i] );
      }//ttree existence

    }else if(strlen(tokres)>0){//-----NOT HISTO-----------------

      if ( strstr(tokres,"t")!=NULL){//time--------
	tokres[0]='0';
	T_yn[i]=atoi( tokres );   //  Thist will be defined later
	printf("ZH:time %d  defined (part %d)\n", i, T_yn[i]  );
      }//TIME

      if ( strstr(tokres,"s")==tokres){//counter----------- =s001, =s002...
	//        sprintf( tok, "s%03d", i ); 
	TH1F *h=new TH1F( tokres, tokres, 60000,0,60000);
	COUNhist[i]=h;
	C_yn[i]=1;
	C_yn[i+1]=2; // his number 2 - cover
	printf("ZH:counter %d defined\n", i  );

	// DEFINE TTREE...........for every counter channel....
	sprintf( brname,  "%s", tokres );//  branch  s001 
	sprintf(ch ,"%s/s", brname );  //  UShort_t ===  /s  
	if (ttree_exists==0){// does not exist yet
	  ZH_tree->Branch(brname , &TREE[i], ch );// 
	}else{// already exists
	  ZH_tree->SetBranchAddress( brname ,&TREE[i] );
	}//ttree existence


     }//COUNTER
    }// else....if something in the string ....NOT SOLE NUMBER
    //-------------------------------------HISTO -----------------end----
 
  }//MAXCHAN
}//======================================================+END OF FUNCTION============
  /*
  T_yn[1024]=1;
  T_yn[1025]=2;
  T_yn[1026]=3;
  T_yn[1027]=4;
  */

  // search TokenGet for
  //  c001=1
  //  c023=23
  //  h02a=24
  //-----------this gets you max # of  channels
  //h0400=t1  :  time, everythig else NULL automaticaly
  //c1025=t2
  //c1026=t3
  //c1027=t4
  //-----------
  //c077=s001   :  counter 1    (it takes always TWO channels and SUMS)
  //c079=s002   :  counter 2
  //--------------this gets you max # of counters








void process_chan(int ch,  int val){// KEY:fill propper histos,counters,time

  char mmm[100]; // titles

  if (T_yn[ch]==0){ //not TIME
    //----------------------
    if (C_yn[ch]==1){// COUNTER Lo
      COUN[ch]+=val;
      COUNtmp[ch]=val;// Must be in lo-hi order
      
    }
    if (C_yn[ch]==2){// COUNTER Hi
      // VERY SPECIFIC HANDLE OF COUNTERS (PAIRS)
      COUN[ch-1]+=val<<16;
      COUNtmp[ch-1]+=val<<16;
      COUNhist[ch-1]->Fill( COUNtmp[ch-1] );// Fill Histogram
      sprintf(mmm,"TOTAL = %ld",COUN[ch-1]);
      COUNhist[ch-1]->SetTitle( mmm );
      TREE[ch-1]= COUNtmp[ch-1] ; // prepare the variable for tree
    }
    //----------------------
    if (HIST[ch]!=NULL){// HISTO
      HIST[ch]->Fill(val);
      TREE[ch]=val; // keep until TTree -> Fill
    }



  }else{// TIME...... forget everything else date -d @1299941976
    //=========VERY SPECIFIC TREATMENT OF TIME INFORMATION=======
    if (T_yn[ch]==1){bTIME=bTIME+0.000001*val;}
    if (T_yn[ch]==2){bTIME=bTIME+0.000001*(val<<16);}
    if (T_yn[ch]==3){bTIME=bTIME+val;}
    if (T_yn[ch]==4){bTIME=bTIME+(val<<16);
      if (Thist==NULL){
	Thist=new TH1F("Thist","Thist", 
		       86400, bTIME-ROOT_offset, bTIME+86400-ROOT_offset );
	Thist->GetXaxis()->SetTimeDisplay(1);
	Thist->GetXaxis()->SetTimeFormat( "#splitline{%d.%m}{%H:%M}%F1994-12-31 22:00:00s" );
      }
      Thist->Fill( bTIME-ROOT_offset );
      sprintf( mmm, "TOTAL %9.2f sec" , bTIME-sTIME );
      Thist->SetTitle( mmm );
      if (DEBUG)printf("TIME = %13.6f\n", bTIME );
    }//if ==4
  }//T_yn


}//ONE CHANNEL PROCESS









void process_BOE(){// begin of event - do cleaning
  //  cTIME=0.0; //cannot do, ay events dont have time mark
  bTIME=0.0;  // this checks if the event was data or time

 //?everytime 2048??? too slow.....
 // for (int i=0;i<MAXCHAN;i++){TREE[i]=0;}  // clear  the data for tree.
  memcpy ( TREE, ZERO,  sizeof( UShort_t)*MAXCHAN ) ;//FAST

  //  for (int i=0;i<MAXCHAN;i++){COUNtmp[i]=0;}//clear tmp cntr storage ??

}//BOE...finish






void process_EOE(){   // end of event - do filling
    cnt_evt++;
  if (bTIME==0){// bTIME==0  =>  real EVENT
    //enter cTIME to TTREE struct .....
    ZH_tree->Fill();//
    cnt_evt_data++;
  }//SAVE EVENT done
  else{// bTIME!=0  =>  just a new time.....
    cTIME=bTIME;
    cTIME_root=cTIME - ROOT_offset;
    if (sTIME==0.0)sTIME=cTIME; // one time only
  }//just a new time mark
}//EOE...finish






//=========================================
int process_ONE_EVENT(int *arr){// translate buffer with one event to data
  int pos=0; // normaly 1st place is e000, but can be bor...
  int chn;
  int val;
  process_BOE();// zero counters....?
  while( 
	(  (arr[pos] & BOEm) != BOE ) &&
           (arr[pos] != EOE)	// finish with EOE
	  &&(pos<OEBmax)
	 ){
    //    printf("==========> %08X\n",arr[pos] );
    pos++;
  }
  //  if ( arr[0]==BOR ){ printf("begin of run\n%s", "" ); }
  //  printf(":%08X\n",arr[pos] ); //{ printf("begin of run\n%s", "" ); }


  if (DEBUG)printf("::%6ld   (%2d)   d=%.3f \n", cnt_evt,  channels_in_event(arr[pos]),  cTIME-sTIME  ); 
  pos++; //move next from E000
  while(  (arr[pos] != EOE)&&(pos<OEBmax)  ){ // until EOE
    chn=(arr[pos] & 0xffff0000)>>16;
    val=(arr[pos] & 0x0000ffff);
    if (DEBUG)printf("%3d - %6d\n",  chn, val );
    process_chan( chn, val );
    pos++;
  }//parse events.........
  //------------------------END OF EVENT
  process_EOE();
  return 0;
}//print==========================









void ZH_data(int events){

  //======== STANDARD  STARTUP ==============================
  OEBmax=1000;// ONE EVENT LIMIT !!!!!!!!!!
  DataRead=0; // HowMuch was read to buffer
  cnt_evt=0; // event number

 cTIME=0.0; // current data TIME (always>0)
 cTIME_root=0.0; // current data TIME (always>0)
 bTIME=0.0; // buffered time (mostly 0)
 sTIME=0.0; // startup time
 dTIME=0.0; // difference


  reset_chan_table();
  /*
translation
c001  ... original channels from data (e.g.c1024==time)
translates to
if number      1..   -> histograms  c001.. (simple channel)
if t1..t4            -> time mark, 
if s001..            ->counter (1st+2nd channels x 65000); "TOTAL" in title
   */
  load_chan_table("c001=1,c002=2,c003=3,c004=4,c005=5,c006=6,c007=7,c008=8,c009=9,c010=10,c011=11,c1024=t1,c1025=t2,c1026=t3,c1027=t4,c033=s001,c035=s002,c037=s003" );

  fillbuffer();// one time READ DATA FROM FILE
  int pos=0;

  for (int i=0;i<events;i++){
    pos=fillOEB(pos);
    if (pos<0)break;
    process_ONE_EVENT(OEbuf);
  }
}//ZH_data__________________________________________
