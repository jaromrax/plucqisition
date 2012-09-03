{
  // THIS IS A TYPICAL LINE TO COMPILE UNDER ROOT CINT
  // avoiding loading things via .rootlogon

  //gROOT->ProcessLine(".L mut_queue.C++g");


gSystem->Load("/usr/lib/libdl.so");

//gROOT->ProcessLine(".L nano_acquis_pureconvert.C++g");
gROOT->ProcessLine(".L nano_acquis_pureconvert.C++");

//gROOT->ProcessLine(".L mut_queue.C++g");
gROOT->ProcessLine(".L mut_queue.C++");

printf("\n\nroot -n -b -q  compile.C  %s\n\n\n","");

// I had some problems with versions....
// LOAD GUI  AS LAST - IT CAN INFLUENCE SOME LIBS and 
//gSystem->Load("kibbler_gui_C.so");

}
