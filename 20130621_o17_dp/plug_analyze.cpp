/*  ################################################################# 

PLUNG_ANALYZE 

plugin  for analysis of a tree 


circular buffer seems safe. but how to get right number?

 */
//#include <iostream>  //#include <math.h>
#include <stdio.h>
#include "TROOT.h"    // Main INCLUDE !!!!
#include "TSystem.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"  //i cannot draw in standalone???
#include "TRandom.h"
#include "TVector3.h"
#include "mut_queue.h"
//#include <pthread.h>
#include "xml_attr.h"    // bude xml
#include "nano_acquis_pureconvert.C"
#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......

#include "TSocket.h"   //net thread

#include <math.h>   //net thread

#include "TGraphErrors.h"   //net thread

//---------------------------- with mpad and tcounter comes::::::
#include "TText.h"

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
   //   if(XTERM!=NULL)fprintf(XTERM,"evt_analyze EMPTY...........%s\n","");
   sprintf(ch,"evt_analyze EMPTY...........%s","");table_log(2,ch);

    wait=MyCond.TimedWaitRelative( 300+700+2000  );
   if (wait==0){ 
     //      if(XTERM!=NULL)fprintf(XTERM,"POP got BROADCAST SIGNAL... %s\n", "" );
      sprintf(ch,"POP got BROADCAST SIGNAL... %s", "" );table_log(2,ch);
    }

 }// ********************* end  of  function  ***********







  //===================================================================================
  //===================================================================================









  //===================================================================================
  //===================================================================================
  //===================================================================================
  //===================================================================================






//==================================================================MPad
//==================================================================MPad
//==================================================================MPad
//==================================================================MPad
/***********************************************************************
 *  GetPadByName      returns  pointer to pad   OR  NULL -------general func
 *
 */
//===================================TPad* GetPadByName(const char* name){
TPad* MPadGetByName(const char* name){
   TPad *MyGPad=NULL;

  if (gROOT->GetListOfCanvases()->GetEntries()==0){
    /*
    new TCanvas();
    gPad->SetTitle( name );
    gPad->SetName( name );
    MyGPad=gPad;
    printf("(GetPadByName) %s was defined %d\n" , name, MyGPad  );
    */
    printf("(MPadGetByName) %s NOT found \n" , name  );
    return NULL;

  }else{
    for (int i=0;i<gROOT->GetListOfCanvases()->GetEntries();i++){
      TPad *cpad=(TPad*)(gROOT->GetListOfCanvases()->At(i));
      //      printf("canvas %d/%d\n" , i, gROOT->GetListOfCanvases()->GetEntries() );
      //1st Just tpad0.....

	  if (strcmp(cpad->GetName(),name)==0){
	    MyGPad=(TPad*)cpad;
	    //	    printf("%s was found here in (ROOT): %d\n" , name,  MyGPad  );
	  }//if name is good

      
      for (int j=0;j<cpad->GetListOfPrimitives()->GetEntries();j++){
	if ( strcmp(cpad->GetListOfPrimitives()->At(j)->ClassName(),"TPad")==0){
	  //	  printf("TPad %s looked j classname  %d/%d\n" ,  cpad->GetListOfPrimitives()->At(j)->GetName() , j, cpad->GetListOfPrimitives()->GetEntries());

	  if (strcmp(cpad->GetListOfPrimitives()->At(j)->GetName(),name)==0){
	    MyGPad=(TPad*)cpad->GetListOfPrimitives()->At(j);
	    //	    printf("%s was found here in j==%d: %d\n" , name, j, MyGPad  );
	  }//if name is good

	}// is TPad
      }// for j
    }//for i
  }//else-there were pads....
  if (MyGPad==NULL){

    printf("(MPadGetByName) %s NOT found\n" , name  );
    return NULL;
    /*     new TCanvas();
    gPad->SetTitle( name );
    gPad->SetName( name );
    MyGPad=(TPad*)gPad;   
    printf("%s was NOT found anywhere in pads, we define new %d\n" , name, (int)MyGPad  );
    */
  }//
  return MyGPad;
}// GetPadName ===============================================================






/****************************************************************
 *             Prints a text into PAD         -------general func
 *
 *
 */
//==========void PrintInPad(const char* label, const char* value){

void MPadPrintIn(const char* label, const char* value){
   TPad *orig=NULL;
   if (gROOT->GetListOfCanvases()->GetEntries()>0){orig=(TPad*)gPad;}
   TPad *cpad=MPadGetByName( label );

 if (cpad!=NULL){
   cpad->Clear();
   TText *t1=new TText(0.5 ,0.5,     value );   // position 0.5,0.5; size 0.5
   t1->SetTextFont(43);  // 42 was helvetica prec 2,   3 is precision 3!!!!pixels!!!
   int w=cpad->GetWw();
   //       w=cpad->GetWindowWidth();
   int h=cpad->GetWh();
   t1->SetTextAlign(22);//  CENTER === 2*10 +2
   //   t1->SetY(0.1)
   // EMPIRICAL FORMULA FOR HELVETICA FONT //   
   double size=0.4*w/h/strlen(value)*7;
   // OK    size=0.4*w/strlen(value)/15;//  this is quite reasonable....
   size=0.4*w/strlen(value)/15;//  this is quite reasonable....
   if (size>1.26){ size=1.26;} // from empirical test of height (350 pix)

   // HACK 43 PIXELS ======================
   //   t1->SetTextSize( size ); 
   if (  w/strlen(value)/1.1 < h/1.2 ){
     t1->SetTextSize( w/strlen(value)/1.1  ); 
   }else{
     t1->SetTextSize( h/1.2  ); 
   }
   //   printf("bounding box  %d:%d  :%f\n", w ,h,  size);
   t1->Draw(); 
   if (orig!=NULL){orig->cd();}
 }//cpad not NULL
}//DisplayInPad==================================================================


  
void MPadPrintInP(const char* label, int slot, const char* value){
  char ch[100];
  sprintf(ch, "%s_%d", label, slot);
  MPadPrintIn( ch, value );
}


void MPadCreate(const char* LABEL, int columns, int rows){

  /*
   *  HERE I (CONDITIONALY) CREATE SET OF PADS ..............................
   */
  TCanvas *cc;
  if (MPadGetByName( LABEL )==NULL){
    cc=new TCanvas( LABEL,LABEL);
    cc->Divide(columns , rows, 0.002,0.002 );
    cc->GetCanvas()->SetFillColor(9);cc->GetCanvas()->SetFillStyle(1);
    cc->Draw();
  }else{
    TPad *pp=(TPad*)MPadGetByName( LABEL );cc=pp->GetCanvas();
  }

for (int j=0;j<columns;j++){
for (int i=0;i<rows;i++){
  char aa[15];  sprintf(aa,"%s_%d",LABEL,i+1);
 char bb[15]; 
 sprintf(bb, "kuku %03d", i  );
 // sprintf(bb, "0.%d%d%d bar", i, (int)gRandom->Uniform(9),(int)gRandom->Uniform(9)  );
 MPadPrintIn( aa,bb );
 }
 }//for loop -----------------
  
MPadGetByName( LABEL )->Update();

}//MPadCreate(==================================================================

//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS
//=====================================================================COUNTERS OBJECTS


//-------------------------- class definition ----------------- 1
class TCounter{

 public:
// protected:
  // int is limited to  4e6 !
  int DEBUGo;
  double markVO,  markV; // Value (integer)
  double markTO, markT;  // time
  double currT, currV;   // currents , waiting to 
 
  TCounter();
  ~TCounter();
  void Register(double weight=1.0);
  double GetRate();
  double GetTime();
  void SetDebug(int deb);
};



TCounter::TCounter(  )
{ 
  printf("...creating counter%s\n","");
  DEBUGo=1;
  double now=GetTime();
  markVO=0.0;
  markTO=now - 1.0;
  markV=0.0;
  markT=now;  
  currT=now;
  currV=0.;
}

TCounter::~TCounter() { printf("...destroing counter%s\n","");}



// This makes it   in   seconds.............good for high rates
//              large error at low rates
double TCounter::GetTime(){

  //  TString sla;
  TDatime *dt=new TDatime();  // kTRUE previously...mess
  // sla.Append( dt->AsString() );//append date
  //  sla.Append( " : " );//append fname
  //  sla.Append( s_p.Data() );//append fname
  //  sla.Append( " : END" );//append openfile
  //  printf("%s\n",  sla.Data()  );
  return   1.0* dt->Get();
}// GetTime


double TCounter::GetRate(){
  return (markV-markVO)/(markT-markTO);
}

void TCounter::Register(double weight){
  double now=GetTime();
  currV=currV+weight;
  currT=now;
  if (currT - markT >= 1.0 ){
    markVO=markV;
    markTO=markT;
    markV=currV;
    markT=currT;
  }//if 1 sec
  if (DEBUG>0){ printf( "%09.3f ... %f/%f\n", GetRate() , currT, currV );}
  return;
}


void TCounter::SetDebug(int deb){
  DEBUGo=deb;
}


void test(){
  int i,imax=2;
  TCounter *c[2];
  for (i=0;i<imax;i++){
   c[i]=new TCounter;
  }


  for (i=0;i<imax;i++){
    delete c[i];
  }

}





//-------------------------- class definition ----------------- 2
class TCounterMulti{

 public:
// protected:
  // int is limited to  4e6 !
  double markTO, markT;  // time
  double currT, currV;   // currents , waiting to 

  static const int max=10;
  int N;
  TCounter *mcounter[max];
  int i;


  TCounterMulti( int counters );
  ~TCounterMulti();
  void Register(int counter, double weight=1.0);
  void GetRate();
  double GetTime(); // same
  void Display();
};



double TCounterMulti::GetTime(){

  TDatime *dt=new TDatime();  // kTRUE previously...mess
  return   1.0* dt->Get();
}// GetTime


TCounterMulti::TCounterMulti( int counters  ){ 
  markT=GetTime();
  markTO=markT-1;
  currT=markT;

  N=max;
  if (N>counters){ N=counters;}
  printf("...creating multicounter, # of counters is %d\n", N);
  
  for (i=0;i<N;i++){
    printf(" %d)", i );
    mcounter[i]=new TCounter();
    //    mcounter[i]->SetDebug(0);
  }
  MPadCreate("COUNTERS",1,N);
}//------------------constructor


TCounterMulti::~TCounterMulti() { 
  printf("...destroing multicounter%s\n","");
    for (i=0;i<N;i++){
      delete mcounter;
  }
}//-------------------------destructor


void TCounterMulti::GetRate(){
    for (i=0;i<N;i++){
      //   printf("%d/%d - \n", i , N );
      //     if ((i>=0)&&(i<N) ) {
      //HERE     printf("%9.2f ",mcounter[i]->GetRate( ) );
       //     }   
    }
    //  printf("%s","\n");
}//---------------------------

void TCounterMulti::Register(int counter, double weight){
  if ((counter>=0)&&(counter<N) ) {
    mcounter[counter]->Register( weight );
  }else{
    printf(" counter not in the predefined range (0-%d)\n", N );
  }
  currT=GetTime();
  if (currT>=markT+1){  markT=currT;   Display(); }// once per second autodisplay (max.)
  return;
}//----------------------------



void TCounterMulti::Display(){
  char c[40]; 
  //---taken from MySql  multipads2-----------
  TCanvas *cc;
  if (MPadGetByName( "COUNTERS" )==NULL){
    cc=new TCanvas( "COUNTERS","COUNTERS" );
    cc->Divide(1 ,N, 0.002,0.002 );
    cc->GetCanvas()->SetFillColor(9);cc->GetCanvas()->SetFillStyle(1);
    cc->Draw();
  }else{
    TPad *pp=(TPad*)MPadGetByName( "COUNTERS" );cc=pp->GetCanvas();
  }// I HAVE cc CANVAS now...
  //---------------------------------------------

  // ===   IT CAN SEEM STOPPED === NO WAY TO SEE THE notcomming COUNTS ?????==
  // GetRate();   
    for (i=0;i<N;i++){
      //   printf("%d/%d - \n", i , N );
      //     if ((i>=0)&&(i<N) ) {
      sprintf(c, "%09.1f",mcounter[i]->GetRate( ) );
      MPadPrintInP("COUNTERS", i+1,  c );
       //     }   
    }
    //    printf("%s","\n");
    cc->Modified();cc->Update();
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








  //=====================================================
  //==========================================
  //====================================
  //==        empty 
  /**************************************************
   *            TEST TO   TTREE 
   */
 int* evt_analyze_astro(int* par){
   char ch[400];
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   //   if(XTERM!=NULL)fprintf(XTERM,"evt_analyze_remote  ..........................\n" );
   sprintf(ch,"evt_analyze_remote  .........................." );   table_log(2,ch);
   usleep(1000*1000);

struct {
    double time;
    double btime;
     UInt_t n;// 4 bytr integer.... is enough for one run// decided in nano_acq
    unsigned short int rnum;
    unsigned short int bnum;
    unsigned short int chan[255];//  16bit /s
 } MyEvent;

   UShort_t cha[1255];// TRICK ~ I address channels by cha[1] later !!!!/s
   UInt_t cnt[1255];     // TRICK ~ I address counter  by cnt[1] later !!!!/i
   double time[12];
   

   double ran=gRandom->Uniform(4000.);

   //  counters   INIT

   TCounterMulti *mc=new TCounterMulti(8);

      //-----------------typical creation of TGraphErrors--------------
   //  gt6G  generators in t6 matrix  cut  cutm6_g
   //  gG    generators
   //  gQ    charge * 10 -10 C
   TGraphErrors *gt6G;
   gt6G=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6G");
   if (gt6G==NULL){
     printf("creating new gt6G%s\n","");
     gt6G=new TGraphErrors; 
     gt6G->SetName("gt6G");
     gROOT->GetListOfSpecials()->Add(gt6G);
   }
   TGraphErrors *gt6GG;
   gt6GG=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6GG");
   if (gt6GG==NULL){
     printf("creating new gt6GG%s\n","");
     gt6GG=new TGraphErrors; 
     gt6GG->SetName("gt6GG");
     gROOT->GetListOfSpecials()->Add(gt6GG);
   }
   TGraphErrors *gG;
   gG=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gG");
   if (gG==NULL){
     printf("creating new gG%s\n","");
     gG=new TGraphErrors; 
     gG->SetName("gG");
     gROOT->GetListOfSpecials()->Add(gG);
   }
   TGraphErrors *gQ;
   gQ=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gQ");
   if (gQ==NULL){
     printf("creating new gQ%s\n","");
     gQ=new TGraphErrors; 
     gQ->SetName("gQ");
     gROOT->GetListOfSpecials()->Add(gQ);
   }



   TGraphErrors *gt6t1; // ------  relation between t6 x t1 : should be independent
   gt6t1=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6t1");
   if (gt6t1==NULL){
     printf("creating new gt6t1%s\n","");
     gt6t1=new TGraphErrors; 
     gt6t1->SetName("gt6t1");
     gROOT->GetListOfSpecials()->Add(gt6t1);
   }



   TGraphErrors *gt1q;
   gt1q=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt1q");
   if (gt1q==NULL){
     printf("creating new gt1q%s\n","");
     gt1q=new TGraphErrors; 
     gt1q->SetName("gt1q");
     gROOT->GetListOfSpecials()->Add(gt1q);
   }
   TGraphErrors *gt6q;
   gt6q=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6q");
   if (gt6q==NULL){
     printf("creating new gt6q%s\n","");
     gt6q=new TGraphErrors; 
     gt6q->SetName("gt6q");
     gROOT->GetListOfSpecials()->Add(gt6q);
   }
   TGraphErrors *gt7q;
   gt7q=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt7q");
   if (gt7q==NULL){
     printf("creating new gt7q%s\n","");
     gt7q=new TGraphErrors; 
     gt7q->SetName("gt7q");
     gROOT->GetListOfSpecials()->Add(gt7q);
   }
 
      //-----------------typpical creation of 2D matrix-------------y,x-
   /*
      //--------------------------- special things - angular coincidences kinematics-----------
      TH2F *mtx1_6full;
      mtx1_6full=(TH2F*)gDirectory->Get("mtx1_6full");
      if (mtx1_6full==NULL){
	mtx1_6full=new TH2F("mtx1_6full","V022+V006:V017+V001",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx1_6de;
      mtx1_6de=(TH2F*)gDirectory->Get("mtx1_6de");
      if (mtx1_6de==NULL){
	mtx1_6de=new TH2F("mtx1_6de","V022:V017",1024,0,5000,1024,0,5000);
      }
   */
      TH2F *mtx1;
      mtx1=(TH2F*)gDirectory->Get("mtx1");
      if (mtx1==NULL){
	mtx1=new TH2F("mtx1","V017:V017+V001",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx2;
      mtx2=(TH2F*)gDirectory->Get("mtx2");
      if (mtx2==NULL){
	mtx2=new TH2F("mtx2","V018:V018+V002",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx3;
      mtx3=(TH2F*)gDirectory->Get("mtx3");
      if (mtx3==NULL){
	mtx3=new TH2F("mtx3","V019:V019+V003",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx4;
      mtx4=(TH2F*)gDirectory->Get("mtx4");
      if (mtx4==NULL){
	mtx4=new TH2F("mtx4","V020:V020+V004",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx5;
      mtx5=(TH2F*)gDirectory->Get("mtx5");
      if (mtx5==NULL){
	mtx5=new TH2F("mtx5","V021:V021+V005",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx6;
      mtx6=(TH2F*)gDirectory->Get("mtx6");
      if (mtx6==NULL){
	mtx6=new TH2F("mtx6","V022:V022+V006",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx7;
      mtx7=(TH2F*)gDirectory->Get("mtx7");
      if (mtx7==NULL){
	mtx7=new TH2F("mtx7","V023:V023+V007",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx8;
      mtx8=(TH2F*)gDirectory->Get("mtx8");
      if (mtx8==NULL){
	mtx8=new TH2F("mtx8","V024:V024+V008",1024,0,5000,1024,0,5000);
      }




      //-------------------------- typical load of cuts -----------------

      loadcuts();//  from cuts_manip.h             ONLY WHEN acq("start")
      //-------------------------- typical load of Gcuts -----------------
      // EXTRA CUTS - to compare Q /T1  or Q/T6
      TCutG *m1_monitor;
      m1_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_mon");
      TCutG *m6_monitor;
      m6_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_mon");
      TCutG *m7_monitor;
      m7_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_mon");

      
      //deuterons
      TCutG *m1_d;
      m1_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_d");
      TCutG *m2_d;
      m2_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm2_d");
      TCutG *m3_d;
      m3_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm3_d");
      TCutG *m4_d;
      m4_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm4_d");
      TCutG *m5_d;
      m5_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm5_d");
      TCutG *m6_d;
      m6_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_d");
      TCutG *m7_d;
      m7_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_d");
      TCutG *m8_d;
      m8_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm8_d");

      //protons
      TCutG *m1_p;
      m1_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_p");
      TCutG *m2_p;
      m2_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm2_p");
      TCutG *m3_p;
      m3_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm3_p");
      TCutG *m4_p;
      m4_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm4_p");
      TCutG *m5_p;
      m5_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm5_p");
      TCutG *m6_p;
      m6_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_p");
      TCutG *m7_p;
      m7_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_p");
      TCutG *m8_p;
      m8_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm8_p");

    //diagonal
      TCutG *m1_dia;
      m1_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_dia");
      TCutG *m2_dia;
      m2_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm2_dia");
      TCutG *m3_dia;
      m3_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm3_dia");
      TCutG *m4_dia;
      m4_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm4_dia");
      TCutG *m5_dia;
      m5_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm5_dia");
      TCutG *m6_dia;
      m6_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_dia");
      TCutG *m7_dia;
      m7_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_dia");
      TCutG *m8_dia;
      m8_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm8_dia");


      // generator
      TCutG *m1_g;
      m1_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_g");
      TCutG *m2_g;
      m2_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm2_g");
      TCutG *m3_g;
      m3_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm3_g");
      TCutG *m4_g;
      m4_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm4_g");
      TCutG *m5_g;
      m5_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm5_g");
      TCutG *m6_g;
      m6_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_g");
      TCutG *m7_g;
      m7_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_g");
      TCutG *m8_g;
      m8_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm8_g");




      //------------------------------- typical creation of 1d spectra --------------
      /*
      TH1F *mtx1_1d_ela;
      mtx1_1d_ela=(TH1F*)gDirectory->Get("mtx1_1d_ela");
      if (mtx1_1d_ela==NULL){
	mtx1_1d_ela=new TH1F("mtx1_1d_ela","mtx1_1d_ela",5000,0,5000);
      }

      TH1F *mtx1_1d_d;
      mtx1_1d_d=(TH1F*)gDirectory->Get("mtx1_1d_d");
      if (mtx1_1d_d==NULL){
	mtx1_1d_d=new TH1F("mtx1_1d_d","mtx1_1d_d",5000,0,5000);
      }
      */


      //------------------------------- typical creation of 1d spectra --------------
      TH1F *mtx1_d;      mtx1_d=(TH1F*)gDirectory->Get("mtx1_d");
      if (mtx1_d==NULL){ mtx1_d=new TH1F("mtx1_d","mtx1_d",5000,0,5000); }
      TH1F *mtx2_d;      mtx2_d=(TH1F*)gDirectory->Get("mtx2_d");
      if (mtx2_d==NULL){ mtx2_d=new TH1F("mtx2_d","mtx2_d",5000,0,5000); }
      TH1F *mtx3_d;      mtx3_d=(TH1F*)gDirectory->Get("mtx3_d");
      if (mtx3_d==NULL){ mtx3_d=new TH1F("mtx3_d","mtx3_d",5000,0,5000); }
      TH1F *mtx4_d;      mtx4_d=(TH1F*)gDirectory->Get("mtx4_d");
      if (mtx4_d==NULL){ mtx4_d=new TH1F("mtx4_d","mtx4_d",5000,0,5000); }
      TH1F *mtx5_d;      mtx5_d=(TH1F*)gDirectory->Get("mtx5_d");
      if (mtx5_d==NULL){ mtx5_d=new TH1F("mtx5_d","mtx5_d",5000,0,5000); }
      TH1F *mtx6_d;      mtx6_d=(TH1F*)gDirectory->Get("mtx6_d");
      if (mtx6_d==NULL){ mtx6_d=new TH1F("mtx6_d","mtx6_d",5000,0,5000); }
      TH1F *mtx7_d;      mtx7_d=(TH1F*)gDirectory->Get("mtx7_d");
      if (mtx7_d==NULL){ mtx7_d=new TH1F("mtx7_d","mtx7_d",5000,0,5000); }
      TH1F *mtx8_d;      mtx8_d=(TH1F*)gDirectory->Get("mtx8_d");
      if (mtx8_d==NULL){ mtx8_d=new TH1F("mtx8_d","mtx8_d",5000,0,5000); }



      TH1F *mtx1_p;      mtx1_p=(TH1F*)gDirectory->Get("mtx1_p");
      if (mtx1_p==NULL){ mtx1_p=new TH1F("mtx1_p","mtx1_p",5000,0,5000); }
      TH1F *mtx2_p;      mtx2_p=(TH1F*)gDirectory->Get("mtx2_p");
      if (mtx2_p==NULL){ mtx2_p=new TH1F("mtx2_p","mtx2_p",5000,0,5000); }
      TH1F *mtx3_p;      mtx3_p=(TH1F*)gDirectory->Get("mtx3_p");
      if (mtx3_p==NULL){ mtx3_p=new TH1F("mtx3_p","mtx3_p",5000,0,5000); }
      TH1F *mtx4_p;      mtx4_p=(TH1F*)gDirectory->Get("mtx4_p");
      if (mtx4_p==NULL){ mtx4_p=new TH1F("mtx4_p","mtx4_p",5000,0,5000); }
      TH1F *mtx5_p;      mtx5_p=(TH1F*)gDirectory->Get("mtx5_p");
      if (mtx5_p==NULL){ mtx5_p=new TH1F("mtx5_p","mtx5_p",5000,0,5000); }
      TH1F *mtx6_p;      mtx6_p=(TH1F*)gDirectory->Get("mtx6_p");
      if (mtx6_p==NULL){ mtx6_p=new TH1F("mtx6_p","mtx6_p",5000,0,5000); }
      TH1F *mtx7_p;      mtx7_p=(TH1F*)gDirectory->Get("mtx7_p");
      if (mtx7_p==NULL){ mtx7_p=new TH1F("mtx7_p","mtx7_p",5000,0,5000); }
      TH1F *mtx8_p;      mtx8_p=(TH1F*)gDirectory->Get("mtx8_p");
      if (mtx8_p==NULL){ mtx8_p=new TH1F("mtx8_p","mtx8_p",5000,0,5000); }

      TH1F *mtx1_g;      mtx1_g=(TH1F*)gDirectory->Get("mtx1_g");
      if (mtx1_g==NULL){ mtx1_g=new TH1F("mtx1_g","mtx1_g",5000,0,5000); }
      TH1F *mtx2_g;      mtx2_g=(TH1F*)gDirectory->Get("mtx2_g");
      if (mtx2_g==NULL){ mtx2_g=new TH1F("mtx2_g","mtx2_g",5000,0,5000); }
      TH1F *mtx3_g;      mtx3_g=(TH1F*)gDirectory->Get("mtx3_g");
      if (mtx3_g==NULL){ mtx3_g=new TH1F("mtx3_g","mtx3_g",5000,0,5000); }
      TH1F *mtx4_g;      mtx4_g=(TH1F*)gDirectory->Get("mtx4_g");
      if (mtx4_g==NULL){ mtx4_g=new TH1F("mtx4_g","mtx4_g",5000,0,5000); }
      TH1F *mtx5_g;      mtx5_g=(TH1F*)gDirectory->Get("mtx5_g");
      if (mtx5_g==NULL){ mtx5_g=new TH1F("mtx5_g","mtx5_g",5000,0,5000); }
      TH1F *mtx6_g;      mtx6_g=(TH1F*)gDirectory->Get("mtx6_g");
      if (mtx6_g==NULL){ mtx6_g=new TH1F("mtx6_g","mtx6_g",5000,0,5000); }
      TH1F *mtx7_g;      mtx7_g=(TH1F*)gDirectory->Get("mtx7_g");
      if (mtx7_g==NULL){ mtx7_g=new TH1F("mtx7_g","mtx7_g",5000,0,5000); }
      TH1F *mtx8_g;      mtx8_g=(TH1F*)gDirectory->Get("mtx8_g");
      if (mtx8_g==NULL){ mtx8_g=new TH1F("mtx8_g","mtx8_g",5000,0,5000); }


      TH1F *mtx1_dia;      mtx1_dia=(TH1F*)gDirectory->Get("mtx1_dia");
      if (mtx1_dia==NULL){ mtx1_dia=new TH1F("mtx1_dia","mtx1_dia",5000,0,5000); }
      TH1F *mtx2_dia;      mtx2_dia=(TH1F*)gDirectory->Get("mtx2_dia");
      if (mtx2_dia==NULL){ mtx2_dia=new TH1F("mtx2_dia","mtx2_dia",5000,0,5000); }
      TH1F *mtx3_dia;      mtx3_dia=(TH1F*)gDirectory->Get("mtx3_dia");
      if (mtx3_dia==NULL){ mtx3_dia=new TH1F("mtx3_dia","mtx3_dia",5000,0,5000); }
      TH1F *mtx4_dia;      mtx4_dia=(TH1F*)gDirectory->Get("mtx4_dia");
      if (mtx4_dia==NULL){ mtx4_dia=new TH1F("mtx4_dia","mtx4_dia",5000,0,5000); }
      TH1F *mtx5_dia;      mtx5_dia=(TH1F*)gDirectory->Get("mtx5_dia");
      if (mtx5_dia==NULL){ mtx5_dia=new TH1F("mtx5_dia","mtx5_dia",5000,0,5000); }
      TH1F *mtx6_dia;      mtx6_dia=(TH1F*)gDirectory->Get("mtx6_dia");
      if (mtx6_dia==NULL){ mtx6_dia=new TH1F("mtx6_dia","mtx6_dia",5000,0,5000); }
      TH1F *mtx7_dia;      mtx7_dia=(TH1F*)gDirectory->Get("mtx7_dia");
      if (mtx7_dia==NULL){ mtx7_dia=new TH1F("mtx7_dia","mtx7_dia",5000,0,5000); }
      TH1F *mtx8_dia;      mtx8_dia=(TH1F*)gDirectory->Get("mtx8_dia");
      if (mtx8_dia==NULL){ mtx8_dia=new TH1F("mtx8_dia","mtx8_dia",5000,0,5000); }




      TH1F *h_events; // THING to monitor analyzed event number 
      h_events=(TH1F*)gDirectory->Get("h_events");
      if (h_events==NULL){
	h_events=new TH1F("h_events","h_events",100000,0,100000);
      }


      int    remove_counter1=1;  // to remove 1st point in tgraph
      double t6G=0.0;
      double t1q=0.0;
      double t6q=0.0;
      double t7q=0.0;


      long long int last_event_n=0;
      long long int entr;
      long long int ii; 
        long long int last_ii;//printout after
     int down=5;
      double downtime; int downtimef, downtimei,  wait=1;
      while (wait!=0){//INFINITE INFINITE INFINITE INFINITE INFINITE INFINITE INFINITE 
	//	down--;
	
    TTree *tree_addr_old=(TTree*)gDirectory->FindObject("nanot");
    
    if ( tree_addr_old!=NULL ){ 
       TTree *tree_addr=(TTree*)tree_addr_old->Clone();
 	// ! zjistit, jestli existuje klon!!
      tree_addr->SetTitle("CLONE");
      
      tree_addr->SetMakeClass(1);//to use int,float.?http://root.cern.ch/drupal/content/accessing-ttree-tselector
      tree_addr->SetBranchStatus("*",0); //disable all branches
      tree_addr->SetBranchStatus("mainV",1);//active
      tree_addr->SetBranchStatus("counters",1);//active
      tree_addr->SetBranchStatus("time",1);//active

      tree_addr->SetBranchAddress("mainV", &cha[1] ); // MY TRICK TO HAVE V001 ~ cha[1] !!!!!!
      tree_addr->SetBranchAddress("counters", &cnt[1] );//active

      //      tree_addr->SetBranchAddress("time", &time[0] ); // worked ......
      tree_addr->SetBranchAddress("time", &MyEvent );

       // here is a little problem: circular buffer, entry number....end of the function ....
      entr=tree_addr->GetEntries();
       //  entr_first=tree_addr->GetEntryNumber(0);
       //  if(XTERM!=NULL)fprintf(XTERM,"  FTREE  found:  Entries==%lld ...  s.p.==%lld\n", entr, starting_point);
       //  printf("TTREE : %d entries\n", entr  );

      //last_event_n === MyEvent.n//

      long long  int circular_bias=0; // important to have both - pos/neg 
       if (entr>0){


       tree_addr->GetEntry(0);// this really starts at event #1
       //       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d (last_event==%lld  tot==%lld)\n", 
       //			      0, MyEvent.n, last_event_n, entr  );
       sprintf(ch,"A  FTREE  :B   entry:%6d .... evt.n==%6d (last:%lld tot:%lld)", 
			      0, MyEvent.n, last_event_n, entr  ); table_log(2,ch);

       circular_bias=last_event_n-MyEvent.n+1;
       //we can have last_event==0; but n==120123 ! ...negative event...so:
       if (circular_bias<0){ circular_bias=0;}

       tree_addr->GetEntry(1);
       //       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d\n", 1, MyEvent.n );
       sprintf(ch,"A  FTREE  :B   entry:%6d .... evt.n==%6d", 1, MyEvent.n );  table_log(2,ch);
       tree_addr->GetEntry(2);
       //       sprintf(ch,"A  FTREE  :    entry:%6d .... evt.n==%6d", 2, MyEvent.n); table_log(2,ch);
 


       //      if (circular_bias>0){//I need it for comparison
       tree_addr->GetEntry(circular_bias-1);
       //       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6lld .... evt.n==%6d \n", circular_bias-1, MyEvent.n );
       sprintf(ch,"A  FTREE  : E  entry:%6lld .... evt.n==%6d ", circular_bias-1, MyEvent.n ); table_log(2,ch);  
       tree_addr->GetEntry(circular_bias);
       //       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6lld .... evt.n==%6d \n", circular_bias, MyEvent.n  );
       sprintf(ch,"A  FTREE  : E  entry:%6lld .... evt.n==%6d ", circular_bias, MyEvent.n  );  table_log(2,ch);  
       //       }//cicr bias>0




       }else{//if (entr>0){
	 circular_bias=0;last_event_n=0;
       }//if (entr>0){
    

       //circular_bias LLD
       //entr          LLD
       //ii            LLD
       //MyEvent.n     UInt_t n;// 4 bytr integer ! decided in nano_acq
       //last_event    LLD
       //       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :   LOOP ii= %lld ;ii < %lld \n", circular_bias, entr );
       sprintf(ch,"A  FTREE  :   LOOP ii= %lld ;ii < %lld ", circular_bias, entr ); table_log(2,ch);
       if (last_event_n<MyEvent.n ){
	 //	 if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :   SKIP BY  %lld EVENTS\n",   MyEvent.n-last_event_n );
	 sprintf(ch,"A  FTREE  :   SKIP BY  %lld EVENTS",   MyEvent.n-last_event_n );  table_log(2,ch);
	 	 last_event_n=MyEvent.n-1;
       } // we fix the jumnp
       /*
	* LOOP 
	*/
       //       for(  ii=circular_bias; ii<circular_bias+entr; ii++)  {
       // pojedu od experimentalne nalezeneho biasu  az do entries.
       // pokud dalsi udalost neni v rade => breaknu to 
       long long int last_ii;//printout after
       for(  ii=circular_bias; ii<entr; ii++)  {
	 last_ii=ii;//printout after
	 //	 if ((ii%1000)==0){if(XTERM!=NULL)fprintf(XTERM,"T        %7lld e^3 evts\n",(int)(ii/1000) );} 
	 tree_addr->GetEntry(ii);


         if(  ii<circular_bias+2 ){
	      sprintf(ch,"A  FTREE:B   entry:%6lld .... evt.n==%6d last==%6lld", 
									    last_ii , MyEvent.n , last_event_n );
	     table_log(2,ch);
	   }
         if(ii>entr-3) {
	      sprintf(ch,"A  FTREE: E  entry:%6lld .... evt.n==%6d last==%6lld", 
									    last_ii , MyEvent.n , last_event_n );
	     table_log(2,ch);
	   }


 	 //last_event_n++;// THE most simple way to track the events processed
	 if (MyEvent.n!=last_event_n+1){ 
	   //	          if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  : circ.buff edge: myevent.n== %d at ii==%lld. Should be %lld. BREAK\n", 
	          sprintf(ch,"A  FTREE  : circ.buff edge: myevent.n== %d at ii==%lld. Should be %lld. BREAK", 
					 MyEvent.n , ii ,  last_event_n+1 );  table_log(2,ch);
		  if (last_event_n!=0){ break; }// I spoted this when last==0
		  break;

	 }
	 last_event_n=(long long int)MyEvent.n;
	 //	 printf(" %4d/ %11.2f chan 0 ====%d \n", ii, time[0], cha[1]   );
	 //	 	 printf(" %4d/               %11.2f chan 1 ====%d \n", ii, time[0], cha[1]   );

	 h_events->Fill( MyEvent.n );

	 //-----------when there is Q (every minute when there is a beam)
	 if (cnt[1]>0){ 
	   printf("*****************   Q==%5d :  t6/q=%14.4f  %6f  :  t7/q=%14.4f  %6f  : CNT %d*%d*%d  \n", 
		  cnt[1] , t6q/cnt[1],  sqrt(t6q)/cnt[1] ,  t7q/cnt[1],  sqrt(t7q)/cnt[1] , cnt[2], cnt[3],cnt[4] );
	   if (	 remove_counter1!=1 ){
	   int ima;
	   ima=gQ->GetN();
	   gQ->Expand(ima+1);
	   gQ->SetPoint(      ima, MyEvent.time, cnt[1]/60.0 );
	   gQ->SetPointError( ima, 0.0,  0.03*cnt[1]/60.0   );  // integrator error is typical 0.3% @ 50nA

	   ima=gt6G->GetN();//generator in the matrix t6 cut m6_g
	   gt6G->Expand(ima+1);
	   gt6G->SetPoint(      ima, MyEvent.time, t6G/60.0 );
	   gt6G->SetPointError( ima, 0.0, 1.0/60.0 );

	   ima=gt6GG->GetN();//generator in the matrix t6 cut m6_g / generator in counter
	   gt6GG->Expand(ima+1);
	   gt6GG->SetPoint(      ima, MyEvent.time, t6G/cnt[4] );
	   gt6GG->SetPointError( ima, 0.0, 1.0/74.1 );

	   ima=gG->GetN();//  74.1 counter generators
	   gG->Expand(ima+1);
	   gG->SetPoint(      ima, MyEvent.time, cnt[4]/60.0 );
	   gG->SetPointError( ima, 0.0, 1.0/60.0 );


	   ima=gt1q->GetN();
	   gt1q->SetPoint(      ima, MyEvent.time, t1q/cnt[1] );
	   gt1q->SetPointError( ima, 0.0, sqrt(t1q)/cnt[1] );

	   ima=gt6q->GetN();
	   gt6q->Expand(ima+1);
	   gt6q->SetPoint(      ima, MyEvent.time, t6q/cnt[1] );
	   gt6q->SetPointError( ima, 0.0, sqrt(t6q)/cnt[1] );

	   ima=gt7q->GetN();
	   gt7q->Expand(ima+1);
	   gt7q->SetPoint(      ima, MyEvent.time, t7q/cnt[1] );
	   gt7q->SetPointError( ima, 0.0, sqrt(t7q)/cnt[1] );

	   ima=gt6t1->GetN();
	   gt6t1->Expand(ima+1);
	   gt6t1->SetPoint(      ima, t6q,  t1q );
	   gt6t1->SetPointError( ima, sqrt(t6q), sqrt(t1q) );

	   }else{ remove_counter1=0; } //do not use 1st point
	   t6G=0.0;
	   t1q=0.0;
	   t6q=0.0;
	   t7q=0.0;
	 }// cnt[1] >0 == Q>0


	 // TCOUNTERMULTI
	
  //-----------------  typical matrix fill----------x,y
	 // not used  now  if ((cha[17]>0)&&(cha[22]>0)) mtx1_6full->Fill( cha[1]+cha[17],cha[6]+cha[22]  ); 
	 // not used now   if ((cha[17]>0)&&(cha[22]>0)) mtx1_6de->Fill( cha[17], cha[22]  ); 

	 if (cha[17]>0) {mtx1->Fill( cha[1]+cha[17], cha[17] ); mc->Register(0,1 ); }
	 if (cha[18]>0) {mtx2->Fill( cha[2]+cha[18], cha[18] ); mc->Register(1,1 ); }
	 if (cha[19]>0) {mtx3->Fill( cha[3]+cha[19], cha[19] ); mc->Register(2,1 );  } 
	 if (cha[20]>0) {mtx4->Fill( cha[4]+cha[20], cha[20] ); mc->Register(3,1 ); }
	 if (cha[21]>0) {mtx5->Fill( cha[5]+cha[21], cha[21] ); mc->Register(4,1 );  }
	 if (cha[22]>0) {mtx6->Fill( cha[6]+cha[22], cha[22] ); mc->Register(5,1 );  }
	 if (cha[23]>0) {mtx7->Fill( cha[7]+cha[23], cha[23] ); mc->Register(6,1 );  }
	 if (cha[24]>0) {mtx8->Fill( cha[8]+cha[24], cha[24] ); mc->Register(7,1 );  }

	 /*
	 if ( (mtx1_cut_ela!=NULL)&&(mtx1_cut_ela->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_1d_ela->Fill( cha[1]+cha[17]  );
	 }
	 if ( (mtx1_cut_d!=NULL)&&(mtx1_cut_d->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_1d_d->Fill( cha[1]+cha[17]  );
	 }
	 */
	 //	 v560na_1    33

	   // MONITOR T6 / q   should be the same
	 //------------------------ HERE we add the counters for superbig contour T1 T6 T7
	 if ( (m6_g !=NULL)&&(m6_g->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   t6G=t6G+1.0;
	 }
	 if ( (m1_monitor!=NULL)&&(m1_monitor->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   t1q=t1q+1.0;
	 }
	 if ( (m6_monitor!=NULL)&&(m6_monitor->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   t6q=t6q+1.0;
	 }
	 if ( (m7_monitor!=NULL)&&(m7_monitor->IsInside( cha[7]+cha[23], cha[23] ) ) ){
	   t7q=t7q+1.0;
	 }


	 //FILL 1d  spectra -  matrices 

	 if ( (m1_d!=NULL)&&(m1_d->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_d->Fill( cha[1]+cha[17]  );
	 }
	 if ( (m2_d!=NULL)&&(m2_d->IsInside( cha[2]+cha[18], cha[18] ) ) ){
	   mtx2_d->Fill( cha[2]+cha[18]  );
	 }
	 if ( (m3_d!=NULL)&&(m3_d->IsInside( cha[3]+cha[19], cha[19] ) ) ){
	   mtx3_d->Fill( cha[3]+cha[19]  );
	 }
	 if ( (m4_d!=NULL)&&(m4_d->IsInside( cha[4]+cha[20], cha[20] ) ) ){
	   mtx4_d->Fill( cha[4]+cha[20]  );
	 }
	 if ( (m5_d!=NULL)&&(m5_d->IsInside( cha[5]+cha[21], cha[21] ) ) ){
	   mtx5_d->Fill( cha[5]+cha[21]  );
	 }
	 if ( (m6_d!=NULL)&&(m6_d->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   mtx6_d->Fill( cha[6]+cha[22]  );
	 }
	 if ( (m7_d!=NULL)&&(m7_d->IsInside( cha[7]+cha[23], cha[23] ) ) ){
	   mtx7_d->Fill( cha[7]+cha[23]  );
	 }
	 if ( (m8_d!=NULL)&&(m8_d->IsInside( cha[8]+cha[24], cha[24] ) ) ){
	   mtx8_d->Fill( cha[8]+cha[24]  );
	 }



	 if ( (m1_p!=NULL)&&(m1_p->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_p->Fill( cha[1]+cha[17]  );
	 }
	 if ( (m2_p!=NULL)&&(m2_p->IsInside( cha[2]+cha[18], cha[18] ) ) ){
	   mtx2_p->Fill( cha[2]+cha[18]  );
	 }
	 if ( (m3_p!=NULL)&&(m3_p->IsInside( cha[3]+cha[19], cha[19] ) ) ){
	   mtx3_p->Fill( cha[3]+cha[19]  );
	 }
	 if ( (m4_p!=NULL)&&(m4_p->IsInside( cha[4]+cha[20], cha[20] ) ) ){
	   mtx4_p->Fill( cha[4]+cha[20]  );
	 }
	 if ( (m5_p!=NULL)&&(m5_p->IsInside( cha[5]+cha[21], cha[21] ) ) ){
	   mtx5_p->Fill( cha[5]+cha[21]  );
	 }
	 if ( (m6_p!=NULL)&&(m6_p->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   mtx6_p->Fill( cha[6]+cha[22]  );
	 }
	 if ( (m7_p!=NULL)&&(m7_p->IsInside( cha[7]+cha[23], cha[23] ) ) ){
	   mtx7_p->Fill( cha[7]+cha[23]  );
	 }
	 if ( (m8_p!=NULL)&&(m8_p->IsInside( cha[8]+cha[24], cha[24] ) ) ){
	   mtx8_p->Fill( cha[8]+cha[24]  );
	 }


	 if ( (m1_g!=NULL)&&(m1_g->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_g->Fill( cha[1]+cha[17]  );
	 }
	 if ( (m2_g!=NULL)&&(m2_g->IsInside( cha[2]+cha[18], cha[18] ) ) ){
	   mtx2_g->Fill( cha[2]+cha[18]  );
	 }
	 if ( (m3_g!=NULL)&&(m3_g->IsInside( cha[3]+cha[19], cha[19] ) ) ){
	   mtx3_g->Fill( cha[3]+cha[19]  );
	 }
	 if ( (m4_g!=NULL)&&(m4_g->IsInside( cha[4]+cha[20], cha[20] ) ) ){
	   mtx4_g->Fill( cha[4]+cha[20]  );
	 }
	 if ( (m5_g!=NULL)&&(m5_g->IsInside( cha[5]+cha[21], cha[21] ) ) ){
	   mtx5_g->Fill( cha[5]+cha[21]  );
	 }
	 if ( (m6_g!=NULL)&&(m6_g->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   mtx6_g->Fill( cha[6]+cha[22]  );
	 }
	 if ( (m7_g!=NULL)&&(m7_g->IsInside( cha[7]+cha[23], cha[23] ) ) ){
	   mtx7_g->Fill( cha[7]+cha[23]  );
	 }
	 if ( (m8_g!=NULL)&&(m8_g->IsInside( cha[8]+cha[24], cha[24] ) ) ){
	   mtx8_g->Fill( cha[8]+cha[24]  );
	 }



	 if ( (m1_dia!=NULL)&&(m1_dia->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_dia->Fill( cha[1]+cha[17]  );
	 }
	 if ( (m2_dia!=NULL)&&(m2_dia->IsInside( cha[2]+cha[18], cha[18] ) ) ){
	   mtx2_dia->Fill( cha[2]+cha[18]  );
	 }
	 if ( (m3_dia!=NULL)&&(m3_dia->IsInside( cha[3]+cha[19], cha[19] ) ) ){
	   mtx3_dia->Fill( cha[3]+cha[19]  );
	 }
	 if ( (m4_dia!=NULL)&&(m4_dia->IsInside( cha[4]+cha[20], cha[20] ) ) ){
	   mtx4_dia->Fill( cha[4]+cha[20]  );
	 }
	 if ( (m5_dia!=NULL)&&(m5_dia->IsInside( cha[5]+cha[21], cha[21] ) ) ){
	   mtx5_dia->Fill( cha[5]+cha[21]  );
	 }
	 if ( (m6_dia!=NULL)&&(m6_dia->IsInside( cha[6]+cha[22], cha[22] ) ) ){
	   mtx6_dia->Fill( cha[6]+cha[22]  );
	 }
	 if ( (m7_dia!=NULL)&&(m7_dia->IsInside( cha[7]+cha[23], cha[23] ) ) ){
	   mtx7_dia->Fill( cha[7]+cha[23]  );
	 }
	 if ( (m8_dia!=NULL)&&(m8_dia->IsInside( cha[8]+cha[24], cha[24] ) ) ){
	   mtx8_dia->Fill( cha[8]+cha[24]  );
	 }



		 //	 if (ii>200)break;
       }//FOR ALL AVAILABLE EVENTS    ii=circular_bias ->  entr....................
       /*       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d\n", 
			     last_ii , MyEvent.n );
       */
 
       //       last_event_n=entr;
       tree_addr->Delete();
    }// ttree NOT NULL
  


    //  ########################################
    // THIS IS TYPICAL SEQUENCE FOR WAIT "STOP"
    //
       TTimeStamp t_wait;
       downtime=t_wait.GetSec()+t_wait.GetNanoSec()*1e-9;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d , %f \n", wait, downtime );
       downtime=downtime+2.5;   // 0.5 sec was NOT ENOUGH !!!
       downtimei=(int)downtime;
       downtimef=(int)( 1e+9*(downtime-1.0*downtimei) );
       //       if(XTERM!=NULL)fprintf(XTERM,"M%s","" );
       sprintf(ch,"M%s","" );   table_log(2,ch);
       wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       //       if(XTERM!=NULL)fprintf(XTERM,"N%s","" );
       sprintf(ch,"N%s","" );  table_log(2,ch);
    //   //    if (wait==0)break;
       //  //  TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
      }//INFINITE LOOP ove ralways new clone


      //    if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  :  Exiting ANALYZE  - remote function ...\n\n%s","");
    sprintf(ch,"\n\n  FTREE  :  Exiting ANALYZE  - remote function ...\n\n%s",""); table_log(2,ch);
 }//***********************************************************



 









  //================================================= EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif

