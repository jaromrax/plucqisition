void replay(int run){

  gROOT->ProcessLine("cout<<\" .... XTERM ......\"<<endl<<endl; ");
  
  char cmd[300];
  sprintf(cmd,".! xterm -uc -geometry 100x4 -e 'cd;pv -L 1711k RUN0_%d | nc6 -l -p 9302'&", run);

  printf( "%s\n", cmd  );
  gROOT->ProcessLine( cmd );
  
  acq("acq_local.xml", run);
  


  printf( "\n\n%s\n", "acq(\"stop\")"  );

 sprintf(cmd,".! sleep 4;echo mv `ls -tr output*root | tail -1` run%d.root &", run);
  printf( "%s\n", cmd  );
  gROOT->ProcessLine( cmd );

 
  TCanvas *c=new TCanvas("cc","",300,100);
  TText *t=new TText(0.1,0.1,"make acq(\"stop\")");
  t->SetTextSize(0.4);
  t->SetTextColor(2);
  t->Draw();

}
