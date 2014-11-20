
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


TCutG *cm1_d;
TCutG *cm2_d;
TCutG *cm3_d;
TCutG *cm6_d;
TCutG *cm7_d;
TCutG *cm8_d;
cm1_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm1_d");
cm2_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm2_d");
cm3_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm3_d");
cm6_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm6_d");
cm7_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm7_d");
cm8_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("cutm8_d");


//================================================DEFINE TH1F====
//TESTS...........
TH1F* chansH=new TH1F("TchansH","channels arrived from pop (analyze)",  1000,0,1000);

//TH1F* timeH=new TH1F("hTimeDiff","time intervals between counters", 		     1800,0,180);

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
int mbins=1000;
int mrange=4000;

double mfact[32];
for (int i=0;i<32;i++){ mfact[i]=1.0;}  // factor to multiply

char mchx[100];
char mchy[100];
char mch[100];
char mnam[100];


//=========================
TH2F *mtx1;
sprintf( mnam, "mtx1" );
mtx1=(TH2F*)gDirectory->Get( mnam );
if (mtx1==NULL){
  sprintf(mchy,"V017%s", "" );
  sprintf(mchx,"%s+%.2f*V001", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx1=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx1->GetXaxis()->SetTitle(mchx);
  mtx1->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------



//=========================
TH2F *mtx2;
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
TH2F *mtx3;
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
TH2F *mtx6;
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
TH2F *mtx7;
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
TH2F *mtx8;
sprintf( mnam, "mtx8" );
mtx8=(TH2F*)gDirectory->Get( mnam );
if (mtx8==NULL){
  sprintf(mchy,"V023%s", "" );
  sprintf(mchx,"%s+%.2f*V008", mchy, 1.0 );
  sprintf(mch,"%s:%s", mchy, mchx);
  mtx8=new TH2F( mnam  ,mch,mbins,0,mrange,mbins,0,mrange);
  mtx8->GetXaxis()->SetTitle(mchx);
  mtx8->GetYaxis()->SetTitle(mchy);
 }//------------------------------------------------------






//====================================SINGLE=========================


TH1F* dia_m1=new TH1F("dia_m1","diagonal V017",  4000,  0, 4000 );
TH1F* dia_m2=new TH1F("dia_m2","diagonal V018",  4000,  0, 4000 );
TH1F* dia_m3=new TH1F("dia_m3","diagonal V019",  4000,  0, 4000 );

TH1F* dia_m6=new TH1F("dia_m6","diagonal V022",  4000,  0, 4000 );
TH1F* dia_m7=new TH1F("dia_m7","diagonal V023",  4000,  0, 4000 );
TH1F* dia_m8=new TH1F("dia_m8","diagonal V024",  4000,  0, 4000 );


TH1F* m1_d=new TH1F("m1_d","deuterons m1",  4000,  0, 4000 );
TH1F* m2_d=new TH1F("m2_d","deuterons m2",  4000,  0, 4000 );
TH1F* m3_d=new TH1F("m3_d","deuterons m3",  4000,  0, 4000 );

TH1F* m6_d=new TH1F("m6_d","deuterons m6",  4000,  0, 4000 );
TH1F* m7_d=new TH1F("m7_d","deuterons m7",  4000,  0, 4000 );
TH1F* m8_d=new TH1F("m8_d","deuterons m8",  4000,  0, 4000 );








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
