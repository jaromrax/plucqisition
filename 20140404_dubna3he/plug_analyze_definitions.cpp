
//   TACounterMulti *mctr;
// if (mctr==NULL){     mctr=new TACounterMulti(12);   }



      //-------------------------- typical load of cuts -----------------
      loadcuts();//  from cuts_manip.h          
      //-------------------------- typical load of Gcuts -----------------
      // EXTRA CUTS - to compare Q /T1  or Q/T6
      TCutG *m1_monitor;
      m1_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_mon");
      TCutG *m6_monitor;
      m6_monitor=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_mon");

      TCutG *m1_gene;
      m1_gene=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_gene");
      TCutG *m6_gene;
      m6_gene=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_gene");

      
//================================================DEFINE TH1F====
//TESTS...........

TH1F* timeH=new TH1F("hTimeDiff","time intervals between counters", 		     1800,0,180);

//TH1F* counterH1=new TH1F("s001_A","counter from analyze plug", 		     60000,0,60000);
//TH1F* counterH4=new TH1F("s004_A","counter from analyze plug", 		     60000,0,60000);


TH1F* timeCanA;    // event arrivals    TIME
TH1F* timeCNT;     // counters arrivals TIME
timeCanA=NULL;
timeCNT=NULL;

/*
TH1F* timeCanAdE1; //dE1 arrivals       TIME
timeCanAdE1=NULL;
TH1F* timeCanAdE2; //dE2 arrivals       TIME
timeCanAdE2=NULL;
TH1F* timeCanAdE3; //dE3 arrivals       TIME
timeCanAdE3=NULL;
TH1F* timeCanAdE5; //dE5 arrivals       TIME
timeCanAdE5=NULL;
TH1F* timeCanAdE6; //dE6 arrivals       TIME
timeCanAdE6=NULL;
TH1F* timeCanAdE7; //dE7 arrivals       TIME
timeCanAdE7=NULL;
*/

TH1F* evnum=new TH1F("T_EventA","Arriving data event # (analyze)",  300000, 
		 0, 300000 );

//TH2F* time_evnum;




//================================================DEFINE MARICES====
int mbins=2000;
int mrange=8000;
double mfact[32];
for (int i=0;i<32;i++){ mfact[i]=1.0;}  // factor to multiply

char mchx[100];
char mchy[100];
char mch[100];
char mnam[100];


//============== L 1x2
TH2F *mL12;
sprintf( mnam, "mL12" );
mL12=(TH2F*)gDirectory->Get( mnam );
if (mL12==NULL){
  sprintf(mchy,"V017%s", "" );
  sprintf(mchx,"%s+%.2f*V018", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mL12=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mL12->GetXaxis()->SetTitle(mchx);
  mL12->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------
//============== L 4x5
TH2F *mL45;
sprintf( mnam, "mL45" );
mL45=(TH2F*)gDirectory->Get( mnam );
if (mL45==NULL){
  sprintf(mchy,"V019%s", "" );
  sprintf(mchx,"%s+%.2f*V020", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mL45=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mL45->GetXaxis()->SetTitle(mchx);
  mL45->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//============== L 1+2 x 3
TH2F *mL123;
sprintf( mnam, "mL123" );
mL123=(TH2F*)gDirectory->Get( mnam );
if (mL123==NULL){
  sprintf(mchy,"V017%s", "" );              // Y
  sprintf(mchx,"%s+%.2f*V018+%.2f*V002", mchy, 1.0 , 1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mL123=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mL123->GetXaxis()->SetTitle(mchx);
  mL123->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------


//============== L 1+2 x 3b
TH2F *mL123b;
sprintf( mnam, "mL123b" );
mL123b=(TH2F*)gDirectory->Get( mnam );
if (mL123b==NULL){
  sprintf(mchy,"V017+%.2f*V018", 1.0 );              // Y
  sprintf(mchx,"%s+%.2f*V002", mchy,  1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mL123b=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mL123b->GetXaxis()->SetTitle(mchx);
  mL123b->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//============== L 4+5 x 6
TH2F *mL456;
sprintf( mnam, "mL456" );
mL456=(TH2F*)gDirectory->Get( mnam );
if (mL456==NULL){
  sprintf(mchy,"V019%s", "" );              // Y
  sprintf(mchx,"%s+%.2f*V020+%.2f*V004", mchy, 1.0 , 1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mL456=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mL456->GetXaxis()->SetTitle(mchx);
  mL456->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------


//============== L 4+5 x 6b
TH2F *mL456b;
sprintf( mnam, "mL456b" );
mL456b=(TH2F*)gDirectory->Get( mnam );
if (mL456b==NULL){
  sprintf(mchy,"V019+%.2f*V020", 1.0 );              // Y
  sprintf(mchx,"%s+%.2f*V004", mchy,  1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mL456b=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mL456b->GetXaxis()->SetTitle(mchx);
  mL456b->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------





//====================================SINGLE=========================


TH1F* dia_L1=new TH1F("dia_L1","diagonal V017",  4100,  0, 4100 );
TH1F* dia_L2=new TH1F("dia_L2","diagonal V018",  4100,  0, 4100 );
TH1F* dia_L4=new TH1F("dia_L4","diagonal V019",  4100,  0, 4100 );
TH1F* dia_L5=new TH1F("dia_L5","diagonal V020",  4100,  0, 4100 );

TH1F* dia_H1=new TH1F("dia_H1","diagonal V022",  4100,  0, 4100 );
TH1F* dia_H2=new TH1F("dia_H2","diagonal V023",  4100,  0, 4100 );
TH1F* dia_H4=new TH1F("dia_H4","diagonal V024",  4100,  0, 4100 );





//============== H 1x2
TH2F *mH12;
sprintf( mnam, "mH12" );
mH12=(TH2F*)gDirectory->Get( mnam );
if (mH12==NULL){
  sprintf(mchy,"V022%s", "" );              // Y
  sprintf(mchx,"%s+%.2f*V023", mchy, 1.0 ); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mH12=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mH12->GetXaxis()->SetTitle(mchx);
  mH12->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//============== H 1+2 x 3
TH2F *mH123;
sprintf( mnam, "mH123" );
mH123=(TH2F*)gDirectory->Get( mnam );
if (mH123==NULL){
  sprintf(mchy,"V022%s", "" );              // Y
  sprintf(mchx,"%s+%.2f*V023+%.2f*V007", mchy, 1.0 , 1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mH123=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mH123->GetXaxis()->SetTitle(mchx);
  mH123->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//============== H 1+2 x 3
TH2F *mH123b;
sprintf( mnam, "mH123b" );
mH123b=(TH2F*)gDirectory->Get( mnam );
if (mH123b==NULL){
  sprintf(mchy,"V022+%.2f*V023", 1.0 );              // Y
  sprintf(mchx,"%s+%.2f*V007", mchy,  1.0); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mH123b=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mH123b->GetXaxis()->SetTitle(mchx);
  mH123b->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------

//============== H  4x5
TH2F *mH45;
sprintf( mnam, "mH45" );
mH45=(TH2F*)gDirectory->Get( mnam );
if (mH45==NULL){
  sprintf(mchy,"V024%s", "" );              // Y
  sprintf(mchx,"%s+%.2f*V008", mchy, 1.0 ); // X
  sprintf(mch,"%s:%s", mchy, mchx);
  mH45=new TH2F( mnam ,mch,mbins,0,mrange,mbins,0,mrange);
  mH45->GetXaxis()->SetTitle(mchx);
  mH45->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------









//======================================================= GRAPHS =====
int t1gene=0;
int t6gene=0;
double t1q=0.0;
double t6q=0.0;
double cnt_TIME=0.0;
double cnt_TIMElast=0.0;

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
