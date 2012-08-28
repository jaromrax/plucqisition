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
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"evt_analyze EMPTY...........%s\n","");
    wait=MyCond.TimedWaitRelative( 300+700+2000  );
   if (wait==0){ 
      if(XTERM!=NULL)fprintf(XTERM,"POP got BROADCAST SIGNAL... %s\n", "" );
    }

 }// ********************* end  of  function  ***********






















  //=====================================================
  //==========================================
  //====================================
  //==        empty 
  /**************************************************
   *            TEST TO   TTREE 
   */
 int* evt_analyze_test(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"evt_analyze_remote  ..........................\n" );
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


      //-----------------typpical creation of TGraphErrors--------------
   //  gt6G  generators in t6 matrix  cut  m6_g
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
      /*
      TCutG *mtx1_cut_ela;
      mtx1_cut_ela=(TCutG*)gROOT->GetListOfSpecials()->FindObject("Cut00_mtx1");
      TCutG *mtx1_cut_d;
      mtx1_cut_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("Cut01_mtx1");
      */

      //-------------------------- typical load of Gcuts -----------------
      // EXTRA CUTS
      TCutG *m1_monitor;
      m1_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m1_monitor");
      TCutG *m6_monitor;
      m6_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m6_monitor");
      TCutG *m7_monitor;
      m7_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m7_monitor");

      
      //deuterons
      TCutG *m1_d;
      m1_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m1_d");
      TCutG *m2_d;
      m2_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m2_d");
      TCutG *m3_d;
      m3_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m3_d");
      TCutG *m4_d;
      m4_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m4_d");
      TCutG *m5_d;
      m5_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m5_d");
      TCutG *m6_d;
      m6_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m6_d");
      TCutG *m7_d;
      m7_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m7_d");
      TCutG *m8_d;
      m8_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m8_d");

      //protons
      TCutG *m1_p;
      m1_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m1_p");
      TCutG *m2_p;
      m2_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m2_p");
      TCutG *m3_p;
      m3_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m3_p");
      TCutG *m4_p;
      m4_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m4_p");
      TCutG *m5_p;
      m5_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m5_p");
      TCutG *m6_p;
      m6_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m6_p");
      TCutG *m7_p;
      m7_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m7_p");
      TCutG *m8_p;
      m8_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m8_p");

    //diagonal
      TCutG *m1_dia;
      m1_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m1_dia");
      TCutG *m2_dia;
      m2_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m2_dia");
      TCutG *m3_dia;
      m3_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m3_dia");
      TCutG *m4_dia;
      m4_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m4_dia");
      TCutG *m5_dia;
      m5_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m5_dia");
      TCutG *m6_dia;
      m6_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m6_dia");
      TCutG *m7_dia;
      m7_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m7_dia");
      TCutG *m8_dia;
      m8_dia=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m8_dia");


      // generator
      TCutG *m1_g;
      m1_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m1_g");
      TCutG *m2_g;
      m2_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m2_g");
      TCutG *m3_g;
      m3_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m3_g");
      TCutG *m4_g;
      m4_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m4_g");
      TCutG *m5_g;
      m5_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m5_g");
      TCutG *m6_g;
      m6_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m6_g");
      TCutG *m7_g;
      m7_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m7_g");
      TCutG *m8_g;
      m8_g=(TCutG*)gROOT->GetListOfSpecials()->FindObject("m8_g");




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
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d (last_event==%lld  tot==%lld)\n", 
			      0, MyEvent.n, last_event_n, entr  );

       circular_bias=last_event_n-MyEvent.n+1;
       //we can have last_event==0; but n==120123 ! ...negative event...so:
       if (circular_bias<0){ circular_bias=0;}

       tree_addr->GetEntry(1);
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d\n", 1, MyEvent.n );
       tree_addr->GetEntry(2);
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6d .... evt.n==%6d\n", 2, MyEvent.n);
 
       //      if (circular_bias>0){//I need it for comparison
       tree_addr->GetEntry(circular_bias-1);
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6lld .... evt.n==%6d \n", circular_bias-1, MyEvent.n );
       tree_addr->GetEntry(circular_bias);
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :    entry:%6lld .... evt.n==%6d \n", circular_bias, MyEvent.n  );
       //       }//cicr bias>0

       }else{//if (entr>0){
	 circular_bias=0;last_event_n=0;
       }//if (entr>0){
    

       //circular_bias LLD
       //entr          LLD
       //ii            LLD
       //MyEvent.n     UInt_t n;// 4 bytr integer ! decided in nano_acq
       //last_event    LLD
       if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :   LOOP ii= %lld ;ii < %lld \n", 
			      circular_bias, entr );
       if (last_event_n<MyEvent.n ){
	 if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  :   SKIP BY  %lld EVENTS\n", 
			 MyEvent.n-last_event_n );
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
         if(  ((ii<circular_bias+5)||(ii>entr-5))  &&(XTERM!=NULL) )fprintf(XTERM,"A  FTREE  :    entry:%6lld .... evt.n==%6d last==%6lld\n", 
									    last_ii , MyEvent.n , last_event_n );
 	 //last_event_n++;// THE most simple way to track the events processed
	 if (MyEvent.n!=last_event_n+1){ 
	          if(XTERM!=NULL)fprintf(XTERM,"A  FTREE  : circ.buff edge: myevent.n== %d at ii==%lld. Should be %lld. BREAK\n", 
					 MyEvent.n , ii ,  last_event_n+1 );
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
	   gQ->SetPoint(      ima, MyEvent.time, cnt[1]/60.0 );
	   gQ->SetPointError( ima, 0.0,  0.03*cnt[1]/60.0   );  // integrator error is typical 0.3% @ 50nA

	   ima=gt6G->GetN();//generator in the matrix t6 cut m6_g
	   gt6G->SetPoint(      ima, MyEvent.time, t6G/60.0 );
	   gt6G->SetPointError( ima, 0.0, 1.0/60.0 );

	   ima=gt6GG->GetN();//generator in the matrix t6 cut m6_g / generator in counter
	   gt6GG->SetPoint(      ima, MyEvent.time, t6G/cnt[4] );
	   gt6GG->SetPointError( ima, 0.0, 1.0/74.1 );

	   ima=gG->GetN();//  74.1 counter generators
	   gG->SetPoint(      ima, MyEvent.time, cnt[4]/60.0 );
	   gG->SetPointError( ima, 0.0, 1.0/60.0 );


	   ima=gt1q->GetN();
	   gt1q->SetPoint(      ima, MyEvent.time, t1q/cnt[1] );
	   gt1q->SetPointError( ima, 0.0, sqrt(t1q)/cnt[1] );

	   ima=gt6q->GetN();
	   gt6q->SetPoint(      ima, MyEvent.time, t6q/cnt[1] );
	   gt6q->SetPointError( ima, 0.0, sqrt(t6q)/cnt[1] );

	   ima=gt7q->GetN();
	   gt7q->SetPoint(      ima, MyEvent.time, t7q/cnt[1] );
	   gt7q->SetPointError( ima, 0.0, sqrt(t7q)/cnt[1] );

	   ima=gt6t1->GetN();
	   gt6t1->SetPoint(      ima, t6q,  t1q );
	   gt6t1->SetPointError( ima, sqrt(t6q), sqrt(t1q) );

	   }else{ remove_counter1=0; } //do not use 1st point
	   t6G=0.0;
	   t1q=0.0;
	   t6q=0.0;
	   t7q=0.0;
	 }// cnt[1] >0 == Q>0


  //-----------------  typical matrix fill----------x,y
	 if ((cha[17]>0)&&(cha[22]>0)) mtx1_6full->Fill( cha[1]+cha[17],cha[6]+cha[22]  ); 
	 if ((cha[17]>0)&&(cha[22]>0)) mtx1_6de->Fill( cha[17], cha[22]  ); 

	 if (cha[17]>0) mtx1->Fill( cha[1]+cha[17], cha[17] ); 
	 if (cha[18]>0) mtx2->Fill( cha[2]+cha[18], cha[18] ); 
	 if (cha[19]>0) mtx3->Fill( cha[3]+cha[19], cha[19] ); 
	 if (cha[20]>0) mtx4->Fill( cha[4]+cha[20], cha[20] ); 
	 if (cha[21]>0) mtx5->Fill( cha[5]+cha[21], cha[21] ); 
	 if (cha[22]>0) mtx6->Fill( cha[6]+cha[22], cha[22] ); 
	 if (cha[23]>0) mtx7->Fill( cha[7]+cha[23], cha[23] ); 
	 if (cha[24]>0) mtx8->Fill( cha[8]+cha[24], cha[24] ); 

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
       if(XTERM!=NULL)fprintf(XTERM,"M%s","" );
       wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       if(XTERM!=NULL)fprintf(XTERM,"N%s","" );
    //   //    if (wait==0)break;
       //  //  TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
      }//INFINITE LOOP ove ralways new clone


    if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  :  Exiting ANALYZE  - remote function ...\n\n%s","");
 }//***********************************************************



 









  //================================================= EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif

