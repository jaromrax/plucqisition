
//fill matrices=========================================================
// ALWAYS make cuts in space   dE:E - easier to follow afterwards
//        1/ Draw( dE:E )
//        2/ make a cut
//        3/ savecut( ,"cm1_d") convention
//--------------------
//            Draw("dE:2.1E+dE",  "cm1_d" )    will work
//            cm1_d->IsInside( dE,E)   will work more simple
//            even shspe may in future recalculate ?! to transform (GetTitle,GetXaxis-gettitle
//

 if (ANADEBUG){ sprintf(ch,"entered ACTIONS%s",  "");table_log(2,ch); }

if (aTREE[17]>0) {mtx1->Fill( mfact*aTREE[1]+aTREE[17], aTREE[17] );  }
if (aTREE[18]>0) {mtx2->Fill( mfact*aTREE[2]+aTREE[18], aTREE[18] );  }
if (aTREE[19]>0) {mtx3->Fill( mfact*aTREE[3]+aTREE[19], aTREE[19] );  } 
if (aTREE[20]>0) {mtx4->Fill( mfact*aTREE[4]+aTREE[20], aTREE[20] );  }
if (aTREE[21]>0) {mtx5->Fill( mfact*aTREE[5]+aTREE[21], aTREE[21] );  }
if (aTREE[22]>0) {mtx6->Fill( mfact*aTREE[6]+aTREE[22], aTREE[22] );  }
if (aTREE[23]>0) {mtx7->Fill( mfact*aTREE[7]+aTREE[23], aTREE[23] );  }
if (aTREE[24]>0) {mtx8->Fill( mfact*aTREE[8]+aTREE[24], aTREE[24] );  }

//---------IF  dE1
//if (aTREE[17]>0) {




 
if ( (acTIME_root>0.0)&&(timeCanA==NULL) ){
  timeCanA=new TH1F("TimeCanA","Data at time t (Analyze plug)", 86400 , 
		    acTIME_root, acTIME_root+86400 );
 }


 if (timeCanA!=NULL)timeCanA->Fill(acTIME_root);



if ( (acTIME_root>0.0)&&(timeCanAdE1==NULL) ){
  timeCanAdE1=new TH1F("TimeCanAdE1","Data in dE1 at time t (Analyze plug)",86400 , 
		    acTIME_root, acTIME_root+86400 );
 }



if (aTREE[17]>0){
 if (timeCanAdE1!=NULL)timeCanAdE1->Fill(acTIME_root);
}


//----------------event number----------------

evnum->Fill( acnt_evt  );  // event number recorded



//}//IF  dE1-----------

 if (ANADEBUG){ sprintf(ch,"monitor cuts ...%s",  "");table_log(2,ch); }


//===============================================================
// this is CUT that is made with 1:1
if ((m1_monitor!=NULL)&&(m1_monitor->IsInside(aTREE[1],aTREE[17] ))){
	   t1q=t1q+1.0;
 }
if ((m6_monitor!=NULL)&&(m6_monitor->IsInside(aTREE[6],aTREE[22] ))){
	   t6q=t6q+1.0;
 }


 if ((m1_gene!=NULL)&&(m1_gene->IsInside(aTREE[1],aTREE[17] ))){
 	   t1gene=t1gene++;
  }
 if ((m6_gene!=NULL)&&(m6_gene->IsInside(aTREE[6],aTREE[22] ))){
 	   t6gene=t6gene++;
  }


//=====================WHEN COUNTER COMES=====================
if (aCOUNTER[1]>0){ 
  int ima;


  //  counterH1->Fill( aCOUNTER[1] );  // let us see if it equals to s001
  //  counterH4->Fill( aCOUNTER[4] );  // let us see if it equals to s001


  if ( cnt_TIMElast>0.0){// not the very first counter
    timeH->Fill(acTIME_root- cnt_TIMElast );
  }
  cnt_TIMElast = acTIME_root;


if ( (acTIME_root>0.0)&&(timeCNT==NULL) ){
  timeCNT=new TH1F("ThistCNTA","Arriving counters to Analyze",86400 , 
		    acTIME_root, acTIME_root+86400 );
 }
 timeCNT->Fill( acTIME_root  );


 if (gt1q!=NULL){
  ima=gt1q->GetN();
  gt1q->Expand(ima+1);
  gt1q->SetPoint(      ima, acTIME_root, t1q/aCOUNTER[1] );
 gt1q->SetPointError( ima, 0.0,   sqrt(t1q)/aCOUNTER[1] );
 t1q=0.0;
 }

if (gt6q!=NULL){
 ima=gt6q->GetN();
 gt6q->Expand(ima+1);
 gt6q->SetPoint(      ima, acTIME_root, t6q/aCOUNTER[1] );
 gt6q->SetPointError( ima, 0.0,   sqrt(t6q)/aCOUNTER[1] );
 t6q=0.0;
 }


if (gt1gene!=NULL){
 ima=gt1gene->GetN();
 gt1gene->Expand(ima+1);
 gt1gene->SetPoint(      ima, acTIME_root, t1gene );
 gt1gene->SetPointError( ima, 0.0,          1 );
 t1gene=0.0;
 }

if (gt6gene!=NULL){
 ima=gt6gene->GetN();
 gt6gene->Expand(ima+1);
 gt6gene->SetPoint(      ima, acTIME_root, t6gene );
 gt6gene->SetPointError( ima, 0.0,          1 );
 t6gene=0.0;
 }

}//===============================WHEN aCOUNTER[1]>0=====END
