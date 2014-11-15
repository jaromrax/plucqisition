void replay(int run){

  gROOT->ProcessLine("cout<<\" .... XTERM ......\"<<endl<<endl; ");
  
  char cmd[300];
  sprintf(cmd,".! xterm -e 'cd;pv -L 1711k RUN0_%d | nc6 -l -p 9302'&", run);

  printf( "%s\n", cmd  );
  gROOT->ProcessLine( cmd );
  
  acq("acq_local.xml");


}
