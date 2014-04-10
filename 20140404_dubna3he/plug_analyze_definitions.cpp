
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

      TH2F *mtx1;
      mtx1=(TH2F*)gDirectory->Get("mtx1");
      if (mtx1==NULL){
	sprintf(mchx,"V017+%.3f*V001", mfact[1]);
	sprintf(mchy,"V017%s", "" );
	sprintf(mch,"%s:%s", mchy, mchx);
	mtx1=new TH2F("mtx1",mch,mbins,0,mrange,mbins,0,mrange);
	mtx1->GetXaxis()->SetTitle(mchx);
	mtx1->GetYaxis()->SetTitle(mchy);
      }

      TH2F *mtx2;
      mtx2=(TH2F*)gDirectory->Get("mtx2");
      if (mtx2==NULL){
	mtx2=new TH2F("mtx2","V018:V018+V002",4500,0,90000, mbins,0, mrange);
      }

      TH2F *mtx3;
      mtx3=(TH2F*)gDirectory->Get("mtx3");
      if (mtx3==NULL){
	mtx3=new TH2F("mtx3","V019:V019+V003",mbins,0,mrange,mbins,0,mrange);
      }
      TH2F *mtx4;
      mtx4=(TH2F*)gDirectory->Get("mtx4");
      if (mtx4==NULL){
	mtx4=new TH2F("mtx4","V020:V020+V004",mbins,0,mrange,mbins,0,mrange);
      }
/*
      TH2F *mtx5;
      mtx5=(TH2F*)gDirectory->Get("mtx5");
      if (mtx5==NULL){
	mtx5=new TH2F("mtx5","V021:V021+V005",mbins,0,mrange,mbins,0,mrange);
      }
*/
      TH2F *mtx6;
      mtx6=(TH2F*)gDirectory->Get("mtx6");
      if (mtx6==NULL){
	mtx6=new TH2F("mtx6","V022:V022+V006",mbins,0,mrange,mbins,0,mrange);
      }
      TH2F *mtx7;
      mtx7=(TH2F*)gDirectory->Get("mtx7");
      if (mtx7==NULL){
	mtx7=new TH2F("mtx7","V023:V023+V007",mbins,0,mrange,mbins,0,mrange);
      }
      TH2F *mtx8;
      mtx8=(TH2F*)gDirectory->Get("mtx8");
      if (mtx8==NULL){
	mtx8=new TH2F("mtx8","V024:V024+V008",mbins,0,mrange,mbins,0,mrange);
      }

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
