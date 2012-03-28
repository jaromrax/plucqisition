/**********************************************
 *  THIS IS USEFUL WHEN USING like  savecut( cutname, "tele1_deuter" )
 *    I do not compile, but may be possible
 *
 */
#include <unistd.h>  //sleep 
#include "TXMLEngine.h"
#include "TString.h" // good for xml

#include "TCutG.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TFile.h"
#include "TROOT.h"
// string
#include <stdio.h>
// FileExists:
#include <sys/stat.h> 
//
#include "TTree.h"
#include "TH1.h"
#include "TPad.h"


/**********************************************************************************
 *
 *   cuts
 *
 *
 **********************************************************************************/





/*
 *      save the defined cut into    cuts.root
 */
void savecut(TCutG *cut, const char* name){
  TDirectory *dir=gDirectory;
  TCutG *newcut=(TCutG*)cut->Clone( name ); // better to clone before (other dir...)
  TFile *nf=new TFile("cuts.root", "UPDATE");
  newcut->Write();                         
  nf->ls();
  nf->Close();
  dir->cd();
  //  dir->ls();
}//savecut--------------

void loadcuts(){
  gROOT->GetListOfSpecials()->ls();// ORIGINAL
  TDirectory *dir=gDirectory;
  TFile *nf=new TFile("cuts.root", "READ"); // WAS UPDATE, touched cuts.root..
  // UNUSED int n=gDirectory->GetNkeys();
  if (gDirectory->GetListOfKeys()){
      TObject *o;
      int max=gDirectory->GetList()->GetEntries();
      max=gDirectory->GetListOfKeys()->GetEntries();
      for (int iii=0 ; iii<max ; iii++ ){
	TString sa1=gDirectory->GetListOfKeys()->At(iii)->GetName();
	  gDirectory->GetObject( sa1.Data() , o );
	  TString sa2=o->ClassName();
	  // important check - else it makes double entries...
	  if ((sa2.Index("TCutG")==0)&&(gROOT->GetListOfSpecials()->FindObject(o)==NULL)) {
	    //	  if ((sa2.Index("TCutG")==0)&&(gDirectory->FindObject(o)==NULL)) {
	    //	    gDirectory->Add( (TH1F*)o );
	    gROOT->GetListOfSpecials()->Add( (TCutG*)o );
	  }// TCutG
      }
  }//gDirectory->GetListOfKeys()

  nf->ls();
  nf->Close();
  dir->cd();
  gROOT->GetListOfSpecials()->ls();
}//savecut--------------
/*  remove   rename.............
 * gDirectory->rmdir("cutt7d") !!!
 *  cutt7p->Clone("cutt7pV");cutt7pV->Write()
 */



void rmcut(const char* name, int version=0){
  if (version==0){
    printf("make a backup and use the version number  ;1 ;2%s\n","");
    return;
  }
  TDirectory *dir=gDirectory;
    TFile *nf=new TFile("cuts.root", "UPDATE");
    char name2[100];
    sprintf( name2 , "%s;%d", name, version );
    printf("deleting  %s\n", name2  );
    gDirectory->rmdir( name2 );
    nf->ls();
    nf->Close();
  dir->cd();
}


void cpcut(const char* name, const char* newname){
  TDirectory *dir=gDirectory;

  TFile *nf=new TFile("cuts.root", "UPDATE");
  TObject *o;
  gDirectory->GetObject( name , o );
  TCutG* cut=(TCutG*)o;
  TCutG* newcut=(TCutG*)cut->Clone( newname );
  newcut->Write();
    //  newcut->Print();  newcut->Draw("pawl");
  nf->ls();
  nf->Close();
  dir->cd();

}


void lscuts(){
  TDirectory *dir=gDirectory;

  TFile *nf=new TFile("cuts.root", "");
  nf->ls();
  nf->Close();
  dir->cd();

}





