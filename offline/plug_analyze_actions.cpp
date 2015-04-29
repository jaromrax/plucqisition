
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
// if (ANADEBUG){ sprintf(ch,"entered ACTIONS%s",  "");table_log(2,ch); }

if (chan[17]>0) {
  mtx1->Fill( mfact[1]*chan[1]+chan[17], chan[17] );  
  if (chan[1]<3){
    dia_m1->Fill(  chan[17] );
  }
  if ( (cm1_d!=NULL) && (cm1_d->IsInside(chan[1],chan[17]) )  ){
      m1_d->Fill( mfact[1]*chan[1]+chan[17] );
    }
 }
if (chan[18]>0) {
  mtx2->Fill( mfact[2]*chan[2]+chan[18], chan[18] ); 
  if (chan[2]<3){
    dia_m2->Fill(  chan[18] );
  } 
  if ( (cm2_d!=NULL) && (cm2_d->IsInside(chan[2],chan[18]) )){
      m2_d->Fill( mfact[2]*chan[2]+chan[18] );
    }
 }
if (chan[19]>0) {
  mtx3->Fill( mfact[3]*chan[3]+chan[19], chan[19] );  
    if (chan[3]<3){
    dia_m3->Fill(  chan[19] );
  }
    if ( (cm3_d!=NULL) && (cm3_d->IsInside(chan[3],chan[19]) )){
      m3_d->Fill( mfact[3]*chan[3]+chan[19] );
    }

 } 
//if (chan[20]>0) {mtx4->Fill( mfact[4]*chan[4]+chan[20], chan[20] );  }
//if (chan[21]>0) {mtx5->Fill( mfact[5]*chan[5]+chan[21], chan[21] );  }

if (chan[22]>0) {
  mtx6->Fill( mfact[6]*chan[6]+chan[22], chan[22] );  
  if (chan[6]<3){
    dia_m6->Fill(  chan[22] );
  }
  if ( (cm6_d!=NULL) && (cm6_d->IsInside(chan[6],chan[22] )) ){
      m6_d->Fill( mfact[6]*chan[6]+chan[22] );
    }

 }

if (chan[23]>0) {
  mtx7->Fill( mfact[7]*chan[7]+chan[23], chan[23] );  
  if (chan[7]<3){
    dia_m7->Fill(  chan[23] );
  }
  if ( (cm7_d!=NULL) && (cm7_d->IsInside(chan[7],chan[23]) )){
      m7_d->Fill( mfact[7]*chan[7]+chan[23] );
    }
 }

if (chan[24]>0) {
  mtx8->Fill( mfact[8]*chan[8]+chan[24], chan[24] );  
  if (chan[8]<3){
    dia_m8->Fill(  chan[24] );
  }
  if ( (cm8_d!=NULL) && (cm8_d->IsInside(chan[8],chan[24]) )){
      m8_d->Fill( mfact[8]*chan[8]+chan[24] );
    }
 }





  for (int i=0;i<MAXCHANA;i++){ 
    if (chan[i]!=0){ chansH->Fill(i);}
  }



//---------IF  dE1
//if (chan[17]>0) {

//----------------event number----------------
// POZOR cnt je ZVENKU!!!!!!!!!!!!!!!!! 
 evnum->Fill( cnt  );  // event number recorded



//I TRY COUNTER

if (chan[33]>0) 
  {
    char sysc[100];
    Long64_t sysc2;
    sysc2= chan[33]+chan[34]*65536;
    sprintf(sysc , "echo %lld > COUNTER.1&" ,sysc2 );
    system( sysc );
    sysc2= chan[35]+chan[36]*65536;
    sprintf(sysc , "echo %lld > COUNTER.2&" , sysc2 );
    system( sysc );
  }




//}//IF  dE1-----------

//if (ANADEBUG){ sprintf(ch,"monitor cuts ...%s",  "");table_log(2,ch); }
/*
//===============================================================
// this is CUT that is made with 1:1
if ((m1_monitor!=NULL)&&(m1_monitor->IsInside(chan[1],chan[17] ))){
	   t1q=t1q+1.0;
 }
if ((m6_monitor!=NULL)&&(m6_monitor->IsInside(chan[6],chan[22] ))){
	   t6q=t6q+1.0;
 }

 if ((m1_gene!=NULL)&&(m1_gene->IsInside(chan[1],chan[17] ))){
 	   t1gene=t1gene++;
  }
 if ((m6_gene!=NULL)&&(m6_gene->IsInside(chan[6],chan[22] ))){
 	   t6gene=t6gene++;
  }
*/
