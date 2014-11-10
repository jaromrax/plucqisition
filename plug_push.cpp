#define PUSHDEBUG 0
#include "xml_attr.h"    // bude xml
#include "log_term.h"    // bude xml
#include "mut_queue.h"
//#include "acq_core.h"  // WHY HERE????
//#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......


  // I should define the variables that are declared (extern) in header

// TCondition MyCond(0);

// FILE* XTERM;When it is declared in logtermh, defined in log_term.C, no prob. here



//#include <iostream>  //#include <math.h>
#include <stdio.h>
#include "TROOT.h"    // Main INCLUDE !!!!
#include "TSystem.h" 
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"  //i cannot draw in standalone???
#include "TRandom.h"
#include "TVector3.h"
//#include <pthread.h>
//#include "nano_acquis_pureconvert.C" 

#include "TSocket.h"   //net thread
//---------------------------try server (for a client with txt...)
#include "TServerSocket.h"
#include "TMonitor.h"

//############################################ sockets/ raw c++
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* port we're listening on */
//////#define PORT 2020
//############################################ sockets/ raw c++

//----------------------------with   mmap ------------------------
#include <err.h>
#include <sys/mman.h>



/****
IF C++ => care about namemangling....
// because of mixing  c and c++, namemangling.....
 */

#ifdef  __cplusplus
extern "C" {
#endif

//                                                                         ===
// ========= Here I have something for   mmap=============================MMAP
//                                                                         ===

    int mmapfd;      //  =-1       file handle for mmap
    char *mmap_file; // pointer to     mmap





  //    nc6   --exec 'cat RUN25 | pv -L 711k' -l -p 9302



class Cat{
 public:
  int fTimeOut;
  void HandleTO();
  void Print();
  int WasTimeOut();
  TTimer* qalarm;
  Cat();
  ~Cat();
  TSocket* GetSocket(const char* ip, int port);
};

 int Cat::WasTimeOut(){
   return fTimeOut;
 }
 void Cat::HandleTO(){
   Info("HandleTimeOut", "timeout expired");
   // printf(" HADLE Timeout=%s\n" ,  "" );
   fTimeOut=1;
   return;
}
 void Cat::Print(){
   //just    printf(" print Cat%s\n" ,  "" );
   return;
}
 Cat::Cat(){
  fTimeOut=0;
  qalarm=new TTimer(0, kFALSE);
  qalarm->SetInterruptSyscalls();
  //  qalarm->Connect("Timeout()", "Cat", this , "HandleTO()"); //doesnot work...
  //  qalarm->Connect("Timeout()", "Cat", this , "");
  // // qalarm->Start(3000, kTRUE);
}
 Cat::~Cat(){
  delete qalarm;
}

TSocket* Cat::GetSocket(const char* ip, int port){
  //    printf("inside GetSocket alarm start  %s:%d\n" ,  ip, port );
   qalarm->Start(3000, kTRUE);
   //    printf("inside GetSocket   %s:%d\n" ,  ip, port );
   TSocket *socks=new TSocket( ip, port);
   // unfortunately it makes  STDERR print
   //    printf("outside GetSocket   %s:%d\n" ,  ip, port );
   return socks;
}
//---------------------------end of CATS-













  /**********************************************
   * client to                  ZDENEK HONS SERVER
   *              cat runx.dat |  nc -l -p 9302
   *                         this is a client that connects to a server...
   */
int* PLUG(int* par, int* par2){
 


  concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;

   char ch[200];
   //   if(XTERM!=NULL)fprintf(XTERM,"PUSH RS push-remote (network)  par==%d; pointer==%d\n", par,(int)buffer );
   sprintf(ch,"%s","PUSHNET:  entered..." );table_log(0,ch);

      Long64_t cnt=0;

   char ipaddress[100];
   int port;


   //-------- here I will control with    control.mmap    file------   
    if ((mmapfd = open("control.mmap", O_RDWR, 0)) == -1) err(1, "open");
    mmap_file=(char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, mmapfd, 0);
    if (mmap_file == MAP_FAILED) errx(1, "either mmap");
    char mmap_result[100];
   //-------- here I will control with    control.mmap    file------   
  char  acqxml2[100];
  TokenGet( "file=" , mmap_file , acqxml2 ); // takes a value from mmap



      TSmallish_xml xml(    acqxml2   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","ip" );
      sprintf( ipaddress,"%s", xml.output  );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","port" );
      port=atoi(xml.output  );



      //original  char strbuf[2000000];// 20MB
  char strbuf[4000000];// 2MB 711kB; 4MB 1700kB
  int *buffer4p;
  buffer4p=(int*)&strbuf[0];
  int d,i,ii;
  int maxtrans=2000000;
  TSocket *socket;

  double   resrun=1.0; 
  //  int downtimef;//, downtime;

  int trials=10; //10 seconds of timeouts

  
  while (resrun>0.0){// ----- - -- READ ALL REPEATING CONNECTIONS --- -  -- -- - --   - - -

    //  DRUHA STRANA LISTENS !!!!!!!!!!!!!!!!!!!!!!!!!!

    resrun=TokenGet(  "run=", mmap_file , mmap_result ); // if run==0 => KILL HERE
    if (resrun<1.0){ break;}


    Cat Blbka;
    Blbka.Print();

   socket=Blbka.GetSocket( ipaddress, port ) ;
 
   if ( Blbka.WasTimeOut()!=0 ) {
     sprintf(ch,"P %s\n", "After TSocket - fTimeOut==1"); table_log(0,ch);
     break;
   }
   int ii_init;
   trials=10; //GOOD TO BE DEFINED IN XML  as also select timeout
   ii_init=0;// offset if data%4 != 0


    resrun=TokenGet(  "run=", mmap_file , mmap_result ); // if run==0 => KILL HERE
    if (resrun<1.0){ break;}


   while ( (socket)&&(resrun>0.0) ){// ----- - -- READ ONE CONNECTION -------


     //DANGER THAT I MISS 3/10 of EVENTS..... MAYBE THIS IS TO TUNE:
     //3000:50 ==1.6%
     // i==0 => TIMEOUT...... ??
     //  FINALY  2sec timeout, 10x repeat, 50ms wait (TO BE TESTED)
     
     if (PUSHDEBUG!=0){sprintf(ch,"PUSH-net    waiting result=%d ", i ); table_log(0,ch);}
     i=(int)socket->Select(TSocket::kRead, 2000);//timeout 1sec, repeat 5x 
     if (PUSHDEBUG!=0){sprintf(ch,"PUSH-net    Select result i=%d ", i ); table_log(0,ch);}
     
     
     //d=0;//nevim jestli to tu nedela binec
     if (i>0) {//####CASE i>0 ####
       //--  sprintf(ch,"P %s\n", "before recvraw"); table_log(0,ch);
       
       d=(int)socket->RecvRaw( &strbuf[ii_init], maxtrans, kDontBlock  ); // read small buffer
       
       if (PUSHDEBUG!=0){ sprintf(ch,"PUSH-netw socket got %d bytes; init=%d ", d, ii_init ); table_log(0,ch);}
       //     ii=0;
       
       int sizenow;
       //---------------------------------------
       for (ii=0;4*ii<d-(d%4)+ii_init;ii++){
	 //     while( (ii*4<d)&&(d>=4) ){
	 buffer->push( buffer4p[ii] ); 
	 //this helps	 if ( i %10 == 0 ){usleep(1); }
	 /*
	   sizenow= buffer->size();
	 while (sizenow>5000){
	   usleep(100000); 
	   sizenow=buffer->size();
	 }
	 */
	 //  usleep(100000);
	 if (PUSHDEBUG!=0){ sprintf(ch,"%4lld ii= %4d  %08x   %4d",cnt,ii,  buffer4p[ii],d ); table_log(0,ch);}
	 //	ii++;
	 if ((cnt%25000)==0){
	    sizenow= buffer->size();
	   sprintf(ch,"P %7.1f MB : %d",4.*cnt/1000000, sizenow ); table_log(0,ch);    
	   resrun=TokenGet(  "run=", mmap_file , mmap_result ); // if run==0 => KILL HERE
	 
	   if (resrun<1.0){ break;}
	   // if (wait==0) {break;}
	   usleep(100000);
	 }
	 cnt++;
       }//for loop  xwhile - push
       
       
       if (PUSHDEBUG!=0){ sprintf(ch,"PUSH-net  modulo %4d  buf4pii=%08x ",((d+ii_init)%4) , buffer4p[ii] ); table_log(0,ch);}

       
       // I assume thAT D IS even
       if (( (d+ii_init)%4)==0){  ii_init=0;}else{
	 memcpy(&strbuf[0],&strbuf[4*(ii)], 2);
	 memcpy(&strbuf[2],&strbuf[4*(ii)], 2);
	 //       memcpy(&strbuf[0],&strbuf[4*(ii)], d%4);
	 ii_init=2;
       }
       
       if (PUSHDEBUG!=0){ sprintf(ch,"PUSH-net  offset %4d  buf4p0 =%08x ", ii_init , buffer4p[0] ); table_log(0,ch);}
     }//#### if i>0
     
     
 
     resrun=TokenGet(  "run=", mmap_file , mmap_result ); // if run==0 => KILL HERE

     if (resrun<1.0){ 
       sprintf(ch,"PUSH got BROADCAST SIGNAL... %s\n", "" );table_log(0,ch);
       socket->Close(); 
       sprintf(ch,"PUSH socket closed... %s\n", "" );table_log(0,ch);
     }//if wait ==0
     



     if (resrun>0.0){
       
       if (i<0){ //####CASE i<0 ####
	 sprintf(ch,"PUSH SOCKET LOST...%s; iii*4=%d, d=%d\n", ipaddress,ii*4,d );table_log(0,ch);
	 usleep(2000*1000);    
	 //============this helped to have  nice STOP THREAD ============
	 	 if (PUSHDEBUG!=0){sprintf(ch,"PUSH-net    closing,breakin=%d ", i ); table_log(0,ch);}
	 	 socket->Close(); // i dont know // delete socket;
	 	 if (PUSHDEBUG!=0){sprintf(ch,"PUSH-net    closed ,breaked=%d ", i ); table_log(0,ch);}
        break; // i removed this to have easy thread stop // 
       }//####CASE i<0 #### 
       


       if (i==0){ //####CASE i==0  ####
	 trials--;
	 sprintf(ch,"PUSH (ZERO)..%s;d=%d (%d)\n",ipaddress,d, trials);table_log(0,ch);
	 if (trials<=0){
	   sprintf(ch,"PUSH I RELEASE SOCKET(ZERO)..%s; iii*4==%d, d=%d",ipaddress,ii*4,d);table_log(0,ch);
	   socket->Close(); break; 
	 }//trials
       }//####     if  i==0
       
       
       
      //     if (resrun<1.0){break;}
     }//  if resrun >0.0
     if (resrun<1.0){socket->Close(); break;} // this must be
     if (PUSHDEBUG!=0){sprintf(ch,"PUSH-net    end of whil socket %d", i ); table_log(0,ch);}
     
   }// if sock and resrun>0.0
    //    if (wait==0){break;}
  }// while   resrun>0.0   ::::   wait!=0   1==1       WHILE read all the time - ONE CONNECTION


  sprintf(ch,"PUSH deleting socket..%s; iii*4==%d, d=%d",ipaddress,ii*4,d);table_log(0,ch);
  //  socket->Delete();
  delete socket;
  sprintf(ch,"PUSH socket deleted..%s; iii*4==%d, d=%d",ipaddress,ii*4,d);table_log(0,ch);
  

  //  if (wait!=0){ wait=MyCond.TimedWaitRelative( 5000  ) ; }else{ break;}
  //  if (wait==0)break;
  //       usleep(1000*300);
  //if(XTERM!=NULL)fprintf(XTERM,"S%s","" );
  
  //}// 0==0----- - -- READ ALL REPEATING CONNECTIONS --- -  -- -- - --   - - -
  
  
  
  // socket must be already closed here....socket->Close();
  //      if(XTERM!=NULL)fprintf(XTERM,"PUSH RS push-file call finished....%s: PUSHER FINISHED\n", ipaddress );
  sprintf(ch,"PUSH call finished....%s:%d\n", ipaddress,port );table_log(0,ch);
}/*****************************end of function ***********************/
  
  
  







  //============================== EXPORTED FUNCTIONS  STOP =========
#ifdef  __cplusplus
}
#endif

/*
Makefile 
all:
	g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o
	g++ -fPIC -c plug_analyze.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_analyze.so plug_analyze.o
*/



//1:   vraci pointer na x,y,z (MKARLIK)
//
//2:   convertuje matice ??? jak?? do pameti vytvori,pak zapisuje?
//     lze parallel? Fork, nekolik matic, kazda svoji funkci...
//
//2a:    natlaci udaje z ruznych zdroju dat - vystup buffer(channel-value)
//
