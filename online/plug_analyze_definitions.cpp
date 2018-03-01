#ifndef PLUG_ANA_DEF
#define PLUG_ANA_DEF
// HERE I NEED TO SEPARATE 1st RUN and subsequent runs...... #endif



int NMINUTE; // I want to record minute of the measurements 20171126 
NMINUTE=0;    // 10 000 minutes in a week
TH2F *genestab;  //matrix for gene stability 
TH2F *genestab2;  //matrix for gene stability 
double genesum[8];  // this will kee last generator value
double genedif[8];  // this will kee last generator value



TCutG *m1_monitor;
TCutG *m6_monitor;
TCutG *m1_gene;
TCutG *m6_gene;

TCutG *cm1_d;
TCutG *cm2_d;
TCutG *cm3_d;
TCutG *cm4_d;
TCutG *cm5_d;

TCutG *cm6_d;
TCutG *cm7_d;
TCutG *cm8_d;

// protons
TCutG *cm1_p;
TCutG *cm2_p;
TCutG *cm3_p;
TCutG *cm4_p;
TCutG *cm5_p;

TCutG *cm6_p;
TCutG *cm7_p;
TCutG *cm8_p;

// tritons
TCutG *cm1_t;
TCutG *cm2_t;
TCutG *cm3_t;
TCutG *cm4_t;
TCutG *cm5_t;

TCutG *cm6_t;
TCutG *cm7_t;
TCutG *cm8_t;


TH1F* chansH;

TH1F* timeCanA;    // event arrivals    TIME
TH1F* timeCNT;     // counters arrivals TIME

timeCanA=NULL;
timeCNT=NULL;
TH1F* evnum;
TH1F *hh;
//================================================DEFINE MARICES====
int mbins=1000;
int mrange=4000;

double mfact[32];
char mchx[100];
char mchy[100];
char mch[100];
char mnam[100];
TH2F *mtx1;
TH2F *mtx2;
TH2F *mtx3;
TH2F *mtx4;
TH2F *mtx5;
TH2F *mtx6;
TH2F *mtx7;
TH2F *mtx8;
//==========================================  diagonal
TH1F* dia_m1;
TH1F* dia_m2;
TH1F* dia_m3;

TH1F* dia_m4;
TH1F* dia_m5;

TH1F* dia_m6;
TH1F* dia_m7;
TH1F* dia_m8;

//==========================================  deuterons
TH1F* m1_d;
TH1F* m2_d;
TH1F* m3_d;

TH1F* m4_d;
TH1F* m5_d;

TH1F* m6_d;
TH1F* m7_d;
TH1F* m8_d;
//==========================================  protons
TH1F* m1_p;
TH1F* m2_p;
TH1F* m3_p;

TH1F* m4_p;
TH1F* m5_p;

TH1F* m6_p;
TH1F* m7_p;
TH1F* m8_p;
//==========================================  tritons
TH1F* m1_t;
TH1F* m2_t;
TH1F* m3_t;

TH1F* m4_t;
TH1F* m5_t;

TH1F* m6_t;
TH1F* m7_t;
TH1F* m8_t;
//======================================  GENE
int t1gene=0;
int t6gene=0;
double t1q=0.0;
double t6q=0.0;
double cnt_TIME=0.0;
double cnt_TIMElast=0.0;

//=========================================What remain after reload: HERE:


chansH=new TH1F("zTAchansH","channels arrived from pop (analyze)",  1000,0,1000);
evnum=new TH1F("zTA_EventA","event number arrived from pop (analyze)",  300000, 
		 0, 300000 );

//================================================DEFINE MARICES====

for (int i=0;i<32;i++){ mfact[i]=1.0;}  // factor to multiply


// //=========================
// sprintf( mnam, "mtx1" );
// mtx1=(TH2F*)gDirectory->Get( mnam );
// if (mtx1==NULL){
//   sprintf(mchy,"V017%s", "" );
//   sprintf(mchx,"%s+%.2f*V001", mchy, 1.0 );
//   sprintf(mch,"%s:%s", mchy, mchx);
//   mtx1=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
//   mtx1->GetXaxis()->SetTitle(mchx);
//   mtx1->GetYaxis()->SetTitle(mchy);
//  }//------------------------------------------------------


//=========================
sprintf( mnam, "genestab" );
genestab=(TH2F*)gDirectory->Get( mnam );
if (genestab ==NULL){
  genestab=new TH2F( mnam  ,"generator stabibility",   10000,0,10000, 500, 1500,2500 );
  //  i make sum dE + E /2
  genestab->GetYaxis()->SetTitle("gene (dE+E)/2");
  genestab->GetXaxis()->SetTitle("minutes from the start");
 }//------------------------------------------------------
sprintf( mnam, "genestab2" );
genestab2=(TH2F*)gDirectory->Get( mnam );
if (genestab2 ==NULL){
  genestab2=new TH2F( mnam  ,"generator stabibility2",   10000,0,10000, 500, 0,1000 );
  //  i make sum dE - E 
  genestab2->GetYaxis()->SetTitle("gene abs(dE-E)");
  genestab2->GetXaxis()->SetTitle("minutes from the start");
 }//------------------------------------------------------






//=========================
sprintf( mnam, "mtx2" );
mtx2=(TH2F*)gDirectory->Get( mnam );
if (mtx2==NULL){
  sprintf(mchy,"V018%s", "" );
  sprintf(mchx,"%s+%.2f*V002", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx2=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx2->GetXaxis()->SetTitle(mchx);
  mtx2->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//=========================
sprintf( mnam, "mtx3" );
mtx3=(TH2F*)gDirectory->Get( mnam );
if (mtx3==NULL){
  sprintf(mchy,"V019%s", "" );
  sprintf(mchx,"%s+%.2f*V003", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx3=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx3->GetXaxis()->SetTitle(mchx);
  mtx3->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//=========================
sprintf( mnam, "mtx4" );
mtx4=(TH2F*)gDirectory->Get( mnam );
if (mtx4==NULL){
  sprintf(mchy,"V020%s", "" );
  sprintf(mchx,"%s+%.2f*V004", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx4=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx4->GetXaxis()->SetTitle(mchx);
  mtx4->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------


//=========================
sprintf( mnam, "mtx5" );
mtx5=(TH2F*)gDirectory->Get( mnam );
if (mtx5==NULL){
  sprintf(mchy,"V021%s", "" );
  sprintf(mchx,"%s+%.2f*V005", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx5=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx5->GetXaxis()->SetTitle(mchx);
  mtx5->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------



//=========================
sprintf( mnam, "mtx6" );
mtx6=(TH2F*)gDirectory->Get( mnam );
if (mtx6==NULL){
  sprintf(mchy,"V022%s", "" );
  sprintf(mchx,"%s+%.2f*V006", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx6=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx6->GetXaxis()->SetTitle(mchx);
  mtx6->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------


//=========================
sprintf( mnam, "mtx7" );
mtx7=(TH2F*)gDirectory->Get( mnam );
if (mtx7==NULL){
  sprintf(mchy,"V023%s", "" );
  sprintf(mchx,"%s+%.2f*V007", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx7=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx7->GetXaxis()->SetTitle(mchx);
  mtx7->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//=========================
sprintf( mnam, "mtx8" );
mtx8=(TH2F*)gDirectory->Get( mnam );
if (mtx8==NULL){
  sprintf(mchy,"V024%s", "" );
  sprintf(mchx,"%s+%.2f*V008", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx8=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx8->GetXaxis()->SetTitle(mchx);
  mtx8->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------






//====================================SINGLE=========================
//dia_m1=new TH1F("dia_m1","diagonal V017",  4000,  0, 4000 );
 dia_m2=new TH1F("dia_m2","diagonal V018",  4000,  0, 4000 );
 dia_m3=new TH1F("dia_m3","diagonal V019",  4000,  0, 4000 );

 dia_m4=new TH1F("dia_m4","diagonal V020",  4000,  0, 4000 );
 dia_m5=new TH1F("dia_m5","diagonal V021",  4000,  0, 4000 );

 dia_m6=new TH1F("dia_m6","diagonal V022",  4000,  0, 4000 );
 dia_m7=new TH1F("dia_m7","diagonal V023",  4000,  0, 4000 );
 dia_m8=new TH1F("dia_m8","diagonal V024",  4000,  0, 4000 );




// m1_d=new TH1F("m1_d","deuterons m1",  4000,  0, 4000 );
 m2_d=new TH1F("m2_d","deuterons m2",  4000,  0, 4000 );
 m3_d=new TH1F("m3_d","deuterons m3",  4000,  0, 4000 );

 m4_d=new TH1F("m4_d","deuterons m4",  4000,  0, 4000 );
 m5_d=new TH1F("m5_d","deuterons m5",  4000,  0, 4000 );

 m6_d=new TH1F("m6_d","deuterons m6",  4000,  0, 4000 );
 m7_d=new TH1F("m7_d","deuterons m7",  4000,  0, 4000 );
 m8_d=new TH1F("m8_d","deuterons m8",  4000,  0, 4000 );




// m1_p=new TH1F("m1_p","protons m1",  4000,  0, 4000 );
 m2_p=new TH1F("m2_p","protons m2",  4000,  0, 4000 );
 m3_p=new TH1F("m3_p","protons m3",  4000,  0, 4000 );

 m4_p=new TH1F("m4_p","protons m4",  4000,  0, 4000 );
 m5_p=new TH1F("m5_p","protons m5",  4000,  0, 4000 );

 m6_p=new TH1F("m6_p","protons m6",  4000,  0, 4000 );
 m7_p=new TH1F("m7_p","protons m7",  4000,  0, 4000 );
 m8_p=new TH1F("m8_p","protons m8",  4000,  0, 4000 );



// // m1_t=new TH1F("m1_t","tritons m1",  4000,  0, 4000 );
//  m2_t=new TH1F("m2_t","tritons m2",  4000,  0, 4000 );
//  m3_t=new TH1F("m3_t","tritons m3",  4000,  0, 4000 );

//  m4_t=new TH1F("m4_t","tritons m4",  4000,  0, 4000 );
//  m5_t=new TH1F("m5_t","tritons m5",  4000,  0, 4000 );

//  m6_t=new TH1F("m6_t","tritons m6",  4000,  0, 4000 );
//  m7_t=new TH1F("m7_t","tritons m7",  4000,  0, 4000 );
//  m8_t=new TH1F("m8_t","tritons m8",  4000,  0, 4000 );



#endif
//=================================================================NEXT IS
//===============================================WHAT CAN BE RELOADED !!

      //-------------------------- typical load of cuts -----------------
loadcuts();//  from cuts_manip.h          
      //-------------------------- typical load of Gcuts -----------------
      // EXTRA CUTS - to compare Q /T1  or Q/T6
//m1_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_mon");
m6_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_mon");

//m1_gene=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_gene");
m6_gene=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_gene");


cm1_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm1_d");
cm2_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm2_d");
cm3_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm3_d");

cm4_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm4_d");
cm5_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm5_d");

cm6_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm6_d");
cm7_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm7_d");
cm8_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm8_d");



cm1_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm1_p");
cm2_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm2_p");
cm3_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm3_p");

cm4_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm4_p");
cm5_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm5_p");

cm6_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm6_p");
cm7_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm7_p");
cm8_p=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm8_p");





// cm1_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm1_t");
// cm2_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm2_t");
// cm3_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm3_t");

// cm4_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm4_t");
// cm5_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm5_t");

// cm6_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm6_t");
// cm7_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm7_t");
// cm8_t=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cm8_t");


//hh=(TH1F*)gDirectory->Get( "c017");if (hh !=NULL){hh->SetTitle("dE1");}
hh=(TH1F*)gDirectory->Get( "c018");if (hh !=NULL){hh->SetTitle("dE2");}
hh=(TH1F*)gDirectory->Get( "c019");if (hh !=NULL){hh->SetTitle("dE3");}
hh=(TH1F*)gDirectory->Get( "c020");if (hh !=NULL){hh->SetTitle("dE4");}
hh=(TH1F*)gDirectory->Get( "c021");if (hh !=NULL){hh->SetTitle("dE5");}

hh=(TH1F*)gDirectory->Get( "c022");if (hh !=NULL){hh->SetTitle("dE6");}
hh=(TH1F*)gDirectory->Get( "c023");if (hh !=NULL){hh->SetTitle("dE7");}
hh=(TH1F*)gDirectory->Get( "c024");if (hh !=NULL){hh->SetTitle("dE8");}



//hh=(TH1F*)gDirectory->Get( "c001");if (hh !=NULL){hh->SetTitle("thick E1");}
hh=(TH1F*)gDirectory->Get( "c002");if (hh !=NULL){hh->SetTitle("thick E2");}
hh=(TH1F*)gDirectory->Get( "c003");if (hh !=NULL){hh->SetTitle("thick E3");}
hh=(TH1F*)gDirectory->Get( "c004");if (hh !=NULL){hh->SetTitle("thick E4");}
hh=(TH1F*)gDirectory->Get( "c005");if (hh !=NULL){hh->SetTitle("thick E5");}

hh=(TH1F*)gDirectory->Get( "c006");if (hh !=NULL){hh->SetTitle("thick E6");}
hh=(TH1F*)gDirectory->Get( "c007");if (hh !=NULL){hh->SetTitle("thick E7");}
hh=(TH1F*)gDirectory->Get( "c008");if (hh !=NULL){hh->SetTitle("thick E8");}









//======================================================= GRAPHS =====
 t1gene=0;
 t6gene=0;
 t1q=0.0;
 t6q=0.0;
 cnt_TIME=0.0;
 cnt_TIMElast=0.0;

/*
//-------------------------------------GENE-------------------
TGraphErrors *gt1gene;
gt1gene=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt1gene");
if (gt1gene==NULL){
  printf("creating new gt1gene%s\n","");
  gt1gene=new TGraphErrors;   gt1gene->SetName("gt1gene");
  gROOT->GetListOfSpecials()->Add(gt1gene);
 }

TGraphErrors *gt6gene;
gt6gene=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6gene");
if (gt6gene==NULL){
  printf("creating new gt6gene%s\n","");
  gt6gene=new TGraphErrors;   gt6gene->SetName("gt6gene");
  gROOT->GetListOfSpecials()->Add(gt6gene);
 }


//------------------------------------------ Q---------------
TGraphErrors *gt1q;
gt1q=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt1q");
if (gt1q==NULL){
  printf("creating new gt1q%s\n","");
  gt1q=new TGraphErrors;   gt1q->SetName("gt1q");
  gROOT->GetListOfSpecials()->Add(gt1q);
 }

TGraphErrors *gt6q;
gt6q=(TGraphErrors*)gROOT->GetListOfSpecials()->FindObject("gt6q");
if (gt6q==NULL){
  printf("creating new gt6q%s\n","");
  gt6q=new TGraphErrors;   gt6q->SetName("gt6q");
  gROOT->GetListOfSpecials()->Add(gt6q);
 }
*/

  printf("end of ------definitions ---------%s\n","");


