{
  // THIS IS A TYPICAL LINE TO COMPILE UNDER ROOT CINT
  // avoiding loading things via .rootlogon

gROOT->ProcessLine(".L mut_queue.C++g");


printf("\n\nroot -n -b -q  compile.C  %s\n\n\n","");

}
