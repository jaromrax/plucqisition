
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

if (aTREE[17]>0) {mtx1->Fill( mfact*aTREE[1]+aTREE[17], aTREE[17] ); mctr->Register(0,1 ); }
if (aTREE[18]>0) {mtx2->Fill( mfact*aTREE[2]+aTREE[18], aTREE[18] ); mctr->Register(1,1 ); }
if (aTREE[19]>0) {mtx3->Fill( mfact*aTREE[3]+aTREE[19], aTREE[19] ); mctr->Register(2,1 );  } 
if (aTREE[20]>0) {mtx4->Fill( mfact*aTREE[4]+aTREE[20], aTREE[20] ); mctr->Register(3,1 ); }
if (aTREE[21]>0) {mtx5->Fill( mfact*aTREE[5]+aTREE[21], aTREE[21] ); mctr->Register(4,1 );  }
if (aTREE[22]>0) {mtx6->Fill( mfact*aTREE[6]+aTREE[22], aTREE[22] ); mctr->Register(5,1 );  }
if (aTREE[23]>0) {mtx7->Fill( mfact*aTREE[7]+aTREE[23], aTREE[23] ); mctr->Register(6,1 );  }
if (aTREE[24]>0) {mtx8->Fill( mfact*aTREE[8]+aTREE[24], aTREE[24] ); mctr->Register(7,1 );  }


//===============================================================
// this is CUT that is made with 1:1
if ((m1_monitor!=NULL)&&(m1_monitor->IsInside(aTREE[1]+aTREE[17],aTREE[17] ))){
	   t1q=t1q+1.0;
 }
if ((m6_monitor!=NULL)&&(m6_monitor->IsInside(aTREE[6]+aTREE[22],aTREE[22] ))){
	   t6q=t6q+1.0;
 }


//=====================WHEN COUNTER COMES=====================
if (aCOUNTER[1]>0){ 
int ima;

ima=gt1q->GetN();
gt1q->Expand(ima+1);
gt1q->SetPoint(      ima, acTIME_root, t1q/aCOUNTER[1] );
gt1q->SetPointError( ima, 0.0,   sqrt(t1q)/aCOUNTER[1] );
t1q=0.0;

ima=gt6q->GetN();
gt6q->Expand(ima+1);
gt6q->SetPoint(      ima, acTIME_root, t6q/aCOUNTER[1] );
gt6q->SetPointError( ima, 0.0,   sqrt(t6q)/aCOUNTER[1] );
t6q-0.0;

}//===============================WHEN aCOUNTER[1]>0=====END
