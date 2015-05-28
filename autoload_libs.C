{
#if __x86_64__
  gROOT->ProcessLine("cout<<\" ------------------ 64 BIT SYSTEM \"<<endl");
//  gSystem->Load("/lib/libdl.so");
//   gSystem->Load("/lib/x86_64-linux-gnu/libdl.so.2");
// debian8
  gSystem->Load("/lib/x86_64-linux-gnu/libdl-2.19.so");
#else
 gROOT->ProcessLine("cout<<\"-------------------- 32 BIT SYSTEM \"<<endl");
 gSystem->Load("/usr/lib/libdl.so");
#endif

gROOT->ProcessLine(".L xml_attr.C+");
gROOT->ProcessLine(".L log_term.C+");
gROOT->ProcessLine(".L ZH_data.C+");



 gROOT->ProcessLine("cout<<\"-------------------- ZHDATA libraries should be loaded now \"<<endl");

gROOT->ProcessLine(".L mkmat.C");
}
