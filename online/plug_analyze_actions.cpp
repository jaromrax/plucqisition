
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




 // if (chan[17]>0) {
 //   mtx1->Fill( mfact[1]*chan[1]+chan[17], chan[17] );  
 //   if (chan[1]<3){
 //     dia_m1->Fill(  chan[17] );
 //   }
 //   if ( (cm1_d!=NULL) && (cm1_d->IsInside(chan[1],chan[17]) )  ){
 //       m1_d->Fill( mfact[1]*chan[1]+chan[17] );
 //     }
 //   if ( (cm1_p!=NULL) && (cm1_p->IsInside(chan[1],chan[17]) )  ){
 //       m1_p->Fill( mfact[1]*chan[1]+chan[17] );
 //     }
 //  }

 if (chan[18]>0) {
   mtx2->Fill( mfact[2]*chan[2]+chan[18], chan[18] ); 
   if (chan[2]<3){
     dia_m2->Fill(  chan[18] );
   } 
   if ( (cm2_d!=NULL) && (cm2_d->IsInside(chan[2],chan[18]) )){
       m2_d->Fill( mfact[2]*chan[2]+chan[18] );
     }
   if ( (cm2_p!=NULL) && (cm2_p->IsInside(chan[2],chan[18]) )){
       m2_p->Fill( mfact[2]*chan[2]+chan[18] );
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
    if ( (cm3_p!=NULL) && (cm3_p->IsInside(chan[3],chan[19]) )){
      m3_p->Fill( mfact[3]*chan[3]+chan[19] );
    }
    if ( (cm3_t!=NULL) && (cm3_t->IsInside(chan[3],chan[19]) )){
      m3_t->Fill( mfact[3]*chan[3]+chan[19] );
    }

 } 


if (chan[20]>0) {
  mtx4->Fill( mfact[4]*chan[4]+chan[20], chan[20] );  
    if (chan[4]<3){
    dia_m4->Fill(  chan[20] );
  }
    if ( (cm4_d!=NULL) && (cm4_d->IsInside(chan[4],chan[20]) )){
      m4_d->Fill( mfact[4]*chan[4]+chan[20] );
    }
    if ( (cm4_p!=NULL) && (cm4_p->IsInside(chan[4],chan[20]) )){
      m4_p->Fill( mfact[4]*chan[4]+chan[20] );
    }
    if ( (cm4_t!=NULL) && (cm4_t->IsInside(chan[4],chan[20]) )){
      m4_t->Fill( mfact[4]*chan[4]+chan[20] );
    }

 } 


if (chan[21]>0) {
  mtx5->Fill( mfact[5]*chan[5]+chan[21], chan[21] );  
    if (chan[5]<3){
    dia_m5->Fill(  chan[21] );
  }
    if ( (cm5_d!=NULL) && (cm5_d->IsInside(chan[5],chan[21]) )){
      m5_d->Fill( mfact[5]*chan[5]+chan[21] );
    }
    if ( (cm5_p!=NULL) && (cm5_p->IsInside(chan[5],chan[21]) )){
      m5_p->Fill( mfact[5]*chan[5]+chan[21] );
    }
    if ( (cm5_t!=NULL) && (cm5_t->IsInside(chan[5],chan[21]) )){
      m5_t->Fill( mfact[5]*chan[5]+chan[21] );
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
  if ( (cm6_p!=NULL) && (cm6_p->IsInside(chan[6],chan[22] )) ){
      m6_p->Fill( mfact[6]*chan[6]+chan[22] );
    }
  if ( (cm6_t!=NULL) && (cm6_t->IsInside(chan[6],chan[22] )) ){
      m6_t->Fill( mfact[6]*chan[6]+chan[22] );
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
  if ( (cm7_p!=NULL) && (cm7_p->IsInside(chan[7],chan[23]) )){
      m7_p->Fill( mfact[7]*chan[7]+chan[23] );
    }
  if ( (cm7_t!=NULL) && (cm7_t->IsInside(chan[7],chan[23]) )){
      m7_t->Fill( mfact[7]*chan[7]+chan[23] );
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
   if ( (cm8_p!=NULL) && (cm8_p->IsInside(chan[8],chan[24]) )){
       m8_p->Fill( mfact[8]*chan[8]+chan[24] );
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



// MINUTES genestability
   // genestability
 if ( (chan[19]>1500)&&(chan[19]<2500)&&(chan[3]>1500)&&(chan[3]<2500)){
     genesum[2]= (chan[18]+chan[2])/2.0 ;
     genesum[3]= (chan[19]+chan[3])/2.0 ;
     genesum[4]= (chan[20]+chan[4])/2.0 ;
     genesum[5]= (chan[21]+chan[5])/2.0 ;
     genesum[6]= (chan[22]+chan[6])/2.0 ;
     genesum[7]= (chan[23]+chan[7])/2.0 ;
     genesum[8]= (chan[24]+chan[8])/2.0 ;
     genedif[2]= abs(chan[18]-chan[2]) ;
     genedif[3]= abs(chan[19]-chan[3]) ;
     genedif[4]= abs(chan[20]-chan[4]) ;
     genedif[5]= abs(chan[21]-chan[5]) ;
     genedif[6]= abs(chan[22]-chan[6]) ;
     genedif[7]= abs(chan[23]-chan[7]) ;
     genedif[8]= abs(chan[24]-chan[8]) ;
 }

//I TRY COUNTER .................................................... i define NMINUTE
if ( (chan[33]>0)  || (chan[35]>0)  )
  {
    char sysc[100];
    Long64_t sysc2, sysc22;
    sysc2= chan[33]+chan[34]*65536;
    sprintf(sysc , "echo %lld > /tmp/COUNTER.1&" ,sysc2/60 );
    system( sysc );
    sysc22= chan[35]+chan[36]*65536;
    sprintf(sysc , "echo %lld > /tmp/COUNTER.2&" , sysc22/60 );
    system( sysc );

    sprintf(ch,"%lld x10 nA  %lld cps",sysc2/60, sysc22/60 );
    table_log(2,ch);
    //  minute
    NMINUTE=NMINUTE+1;

    for (int i=2;i<=8;i++){
      genestab->Fill( NMINUTE,  genesum[i] );
      genestab2->Fill( NMINUTE,  genedif[i] );
      genesum[i]= 0. ;
      genedif[i]= 0. ;
    }
  }


