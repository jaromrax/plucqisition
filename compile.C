{
  // THIS IS A TYPICAL LINE TO COMPILE UNDER ROOT CINT
  // avoiding loading things via .rootlogon



 gSystem->Load("/usr/lib/libdl.so");                                                                                                              
gROOT->ProcessLine(".L nano_acquis_pureconvert.C++g");
gROOT->ProcessLine(".L mut_queue.C++g");


printf("\n\nroot -n -b -q  compile.C  %s\n\n\n","");

}

/*
 
a little problem on camac1 with 5.32
    I must do

gInterpreter->AddIncludePath("/usr/include"); (probably not important)
and
gSystem->Load("/usr/lib/libdl.so");

to get it to compile!!!!
AND I cannot get proper links to TSocket ....crashes....

 */
