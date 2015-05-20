{
#if __x86_64__
  gROOT->ProcessLine("cout<<\" ------------------ 64 BIT SYSTEM \"<<endl");
  gSystem->Load("/lib/libdl.so");
  //  gSystem->Load("/lib/x86_64-linux-gnu/libdl.so.2");
#else
 gROOT->ProcessLine("cout<<\"-------------------- 32 BIT SYSTEM \"<<endl");
 gSystem->Load("/usr/lib/libdl.so");
#endif

 // gSystem->Load("nrfit_C.so");

gROOT->ProcessLine(".L ../xml_attr.C+");
gROOT->ProcessLine(".L ../log_term.C+");
gROOT->ProcessLine(".L ../mut_queue.C+");
gROOT->ProcessLine(".L ../acq_core.C+");

 gROOT->ProcessLine("cout<<\"-------------------- acq libraries should be loaded now \"<<endl");

gROOT->ProcessLine(".L mkmat.C");

}
