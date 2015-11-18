{
#if __x86_64__
  gROOT->ProcessLine("cout<<\" ------------------ 64 BIT SYSTEM \"<<endl");
  gROOT->ProcessLine("cout<<\" ------------------ 64 BIT SYSTEM LOADED now\"<<endl");
   
#else
 gROOT->ProcessLine("cout<<\"-------------------- 32 BIT SYSTEM \"<<endl");
 gROOT->ProcessLine("cout<<\"-------------------- 32 BIT SYSTEM LOADED now\"<<endl");
#endif


 gROOT->ProcessLine("cout<<\"-------------------- acq libraries should be loaded now \"<<endl");

//------------gROOT->ProcessLine(".! echo ABRUPT QUIT");
//------------gROOT->ProcessLine(".qqqqqqq");
}
