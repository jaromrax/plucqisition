
#include "ZH_data.h"
#include "xml_attr.h"


#include <stdio.h>        // also   4 strcmp

#include "TROOT.h"    // Main INCLUDE !!!!
//#include "TH1F.h"
#include <stdlib.h>     /* atof */
#include <string.h>         // strcmp
#include <string>         // strcmp

int buffer[99000000];//="ahoj\0";  // I MUST USE int!!!!!!????
 int OEbuf[1000];//="Ahoj\0";

 int OEBmax;// ONE EVENT LIMIT !!!!!!!!!!
 int DataRead; // HowMuch was read to buffer
 int64_t cnt_evt; // event number

//const int MAXCHAN=2048; // time is 1024+
   int       T_yn[MAXCHAN];//  4 levels
   int       C_yn[MAXCHAN]; //lo hi
   int64_t   COUN[MAXCHAN];// total
   TH1F*     COUNhist[MAXCHAN];//histo to fill
   int64_t   COUNtmp[MAXCHAN]; // values
   TH1F*     HIST[MAXCHAN];
   UShort_t* TREE[MAXCHAN];
// - s : a 16 bit unsigned integer (UShort_t)

 double cTIME; // current data TIME (always>0)
 double bTIME; // buffered time (mostly 0)
 double sTIME; // startup time
 double dTIME; // difference

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
  int *pch=&buffer[0];
  //  printf( "I see   /%s/\n",   pch );
  f=fopen("/home/ojr/00_central/Root/03_programs/58_kibbler_narval/RUN018_V","rb");
  if (f==NULL){ printf("BAD FILE\%s\n", "" );return;}
  DataRead=fread( pch , 1, sizeof(buffer) , f  );
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




int fillOEB(int pos){
  int c=0;
   int word;
   while( 1==1  ){
    printf("/%08X/ ",  buffer[pos+c] );
    OEbuf[c]= buffer[pos+c];
    if (OEbuf[c]==EOE){ break;}
    c++; 
    if (pos+c> DataRead )return -1;
  }//while
  printf("\n%s", ""  );//4
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
for (int i=0;i<MAXCHAN;i++){TREE[i]=NULL;}   
 cTIME=0.0;
 cnt_evt=0;
 sTIME=0.0;
}


  //  sprintf( initline,   "chanlo=%d,chanhi=%d,kc014A=%d,kc014B=%d,kc014C=%d,ltime=%d,btime=%d,bnum=%d,c257=%d,c32=%d,v560na=%d,v560nb=%d,parnam=%s,tzero=%lf,circular=%d", 	   chanlo, chanhi,kc014A,kc014B,kc014C,ltime,btime,bnum,c257,c32,v560na,v560nb,parnam,tzero,	   circular); 






void load_chan_table(const char *str2k ){ // LOAD channel properties into the table
  // EITHER histo OR time OR counter:   not two
  char conf[2000];

  //="c001=1,c002=2,c003=3,c004=4,c005=5,c006=6,c007=7,c008=8,c009=9,c010=10,c011=11,c1024=t1,c1025=t2,c1026=t3,c1027=t4,c033=s001,c035=s002,c037=s003";
  
    strcpy( conf, str2k );
  char tok[10];
  char tokres[100];
  int toki;
  for (int i=1; i< MAXCHAN; i++){//  c0001-c2000
    sprintf( tok, "c%03d=", i ); 
    toki=TokenGet( tok, conf, tokres);
    if (toki!=0){// HISTO
      sprintf( tok, "c%03d", toki ); // POZOR=whatever integer!
      TH1F *h=new TH1F( tok, tok, 8000,0,8000);
      HIST[i]=h;
    }else if(strlen(tokres)>0){
      if ( strstr(tokres,"t")!=NULL){//time
	tokres[0]='0';
	T_yn[i]=atoi( tokres );
	printf("time %d  defined (part %d)\n", i, T_yn[i]  );
      }//TIME
      if ( strstr(tokres,"s")!=NULL){//counter
	//        sprintf( tok, "s%03d", i ); 
	TH1F *h=new TH1F( tokres, tokres, 60000,0,60000);
	COUNhist[i]=h;
	C_yn[i]=1;
	C_yn[i+1]=2; // his number 2 - cover
	printf("counter %d defined\n", i  );
      }//COUNTER
    }
    //-------------------------------------HISTO -----------------end----
 
  }//MAXCHAN
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
  //c077=s001   :  counter 1
  //c078=s002   :  counter 2
  //--------------this gets you max # of counters
}


void process_chan(int ch,  int val){// MAIN PART: fill propper histos,counters,time

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
      char mmm[100];
      sprintf(mmm,"TOTAL = %ld",COUN[ch-1]);
      COUNhist[ch-1]->SetTitle( mmm );
    }
    //----------------------
    if (HIST[ch]!=NULL){// HISTO
      HIST[ch]->Fill(val);
    }
    //----------------------
    if (TREE[ch]!=NULL){// TTREE
      // memcpy(TREE[ch], val, 4 );
    }



  }else{// TIME...... forget everything else date -d @1299941976
    //=========VERY SPECIFIC TREATMENT OF TIME INFORMATION=======
    if (T_yn[ch]==1){bTIME=bTIME+0.000001*val;}
    if (T_yn[ch]==2){bTIME=bTIME+0.000001*(val<<16);}
    if (T_yn[ch]==3){bTIME=bTIME+val;}
    if (T_yn[ch]==4){bTIME=bTIME+(val<<16);
    if (T_yn[ch]==4){printf("TIME = %13.6f\n", bTIME );}
    }
  }//T_yn


}//ONE CHANNEL PROCESS


void process_BOE(){// begin of event - do cleaning
  //  cTIME=0.0; //cannot do, ay events dont have time mark
  bTIME=0.0;
  //  for (int i=0;i<MAXCHAN;i++){COUNtmp[i]=0;}// clear temporary counter storage ??

}//BOE...finish

void process_EOE(){   // end of event - do filling
    cnt_evt++;
  if (bTIME==0){// SAVE EVENT ONLY
    //enter cTIME to TTREE struct .....
    //TTree->Fill();.........
  }//SAVE EVENT done
  else{// just new time.....
    cTIME=bTIME;
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


  printf("::%6ld   (%2d)   d=%.3f \n", cnt_evt,  channels_in_event(arr[pos]),  cTIME-sTIME  ); 
  pos++; //move next from E000
  while(  (arr[pos] != EOE)&&(pos<OEBmax)  ){ // until EOE
    chn=(arr[pos] & 0xffff0000)>>16;
    val=(arr[pos] & 0x0000ffff);
    printf("%3d - %6d\n",  chn, val );
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
 bTIME=0.0; // buffered time (mostly 0)
 sTIME=0.0; // startup time
 dTIME=0.0; // difference


  reset_chan_table();

  load_chan_table("c001=1,c002=2,c003=3,c004=4,c005=5,c006=6,c007=7,c008=8,c009=9,c010=10,c011=11,c1024=t1,c1025=t2,c1026=t3,c1027=t4,c033=s001,c035=s002,c037=s003" );

  fillbuffer();// one time
  int pos=0;

  for (int i=0;i<events;i++){
    pos=fillOEB(pos);
    if (pos<0)break;
    process_ONE_EVENT(OEbuf);
  }
}
