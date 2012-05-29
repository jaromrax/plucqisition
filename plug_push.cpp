//#include <iostream>  //#include <math.h>
#include <stdio.h>
#include "TROOT.h"    // Main INCLUDE !!!!
#include "TSystem.h" 
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"  //i cannot draw in standalone???
#include "TRandom.h"
#include "TVector3.h"
#include "mut_queue.h"
//#include <pthread.h>
#include "xml_attr.h"    // bude xml
#include "nano_acquis_pureconvert.C" 
#include "cuts_manip.h"  //loadcuts,savecut,rmcut,cpcut.......

#include "TSocket.h"   //net thread
//---------------------------try server (for a client with txt...)
#include "TServerSocket.h"
#include "TMonitor.h"

/****
IF C++ => care about namemangling....
// because of mixing  c and c++, namemangling.....
 */

#ifdef  __cplusplus
extern "C" {
#endif


  //=================== EXPORTED FUNCTIONS  START =========
  double* soarray;   //we allocate some memory in the first call
  //===========================================
 int* kolac(int* par){
    if (soarray==NULL){  //  is allocated once and always used to send pars
     soarray=new double [1024];
     printf("ALSO A HISTOGRAM IS DONE\n%s", "" );
     TCanvas *c=new TCanvas;
     c->Draw();
     TH1F *h=new TH1F("h","h",1000,0,1000);
     printf("canvas drawn......\n%s" ,"");
	 c->Draw(); c->Modified();c->Update(); 
         h->Draw();
	 //     printf("sleeping......\n%s" ,"");
	 //     gSystem->Sleep(2000);
   }
   soarray[0]=1.2;   soarray[1]=1.3;   soarray[2]=1.4;
   printf("inside kolac123 par[0] canbe NULL\n%s", "");
   return (int*)soarray;
}








  //=======================================================
  //==================================================
  //============================================
  //                  PUSHER PART ===============
   
  /**********************************************
   *            PUSH only integers 1.....200
   */
 int* evt_pusher_int(int* par){
      concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   //   concurrent_queue *buffer=(concurrent_queue*)par;
      if(XTERM!=NULL)fprintf(XTERM,
		  "PUSH (integers) START buffer=%d\n",(int)buffer );
      //   for (int i=1;i<1000*1000*1;i++){  //........4*300M = 1200MB
   for (int i=1;i<200;i++){  //........4*300M = 1200MB
     //    buffer.push ( i );
     usleep(1000); // wait 10 ms every push
     //     if ((i%1000)==0)usleep(1);
      if(XTERM!=NULL)fprintf(XTERM,"P %5d\n", i );
     buffer->push( i );
   }//for
      if(XTERM!=NULL)fprintf(XTERM,
		  "evt_pusher (integers)  EXIT buffer=%d\n",(int)buffer );
 }/*****************************end of function *********************/




  /**********************************************
   *            PUSH TEXT  
   */
 int* evt_pusher_txt(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,
		  "PUSH (text) START buffer=%d\n",(int)buffer );
   char ch[180];
   for (int i=1;i<15;i++){  //........4*300M = 1200MB
     sprintf(ch, "%d %d %d %d\n", i, i+1, i+2, i+3 );
     usleep(1000); // wait x  us every push
     if(XTERM!=NULL)fprintf(XTERM,"P <%s>\n", ch );
     for (int j=0;j<strlen(ch);j++){
       buffer->push( ch[j] );
     }
   }//for
   if(XTERM!=NULL)fprintf(XTERM,
		  "evt_pusher (text)  EXIT buffer=%d\n",(int)buffer );
 }/*****************************end of function *********************/








  /**********************************************
   *            PUSH TEXT  FILE
   */
 int* evt_pusher_txtfile(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,
		  "PUSH (textFILE) START buffer=%d\n",(int)buffer );
   char ch[180];
      FILE *infile;  char fname[400];

      int c;// character ... as in tutorial
      int wait=1; // wait==0 means broadcast....
      int first_timer=1;
      fpos_t position;// I want to save the position of the pointer

      TSmallish_xml xml(    acqxml   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","file" );
      sprintf( fname,"%s", xml.output  );

      do{ // MAIN WHILE

      infile=fopen( fname,"r"  );

      if(XTERM!=NULL)fprintf(XTERM,"opened %s in TEXT MODE\n", fname);
      if (infile!=NULL){
	if (first_timer==1){//set pos
	  first_timer=0;}else{
	  fsetpos (infile, &position);}// restore the last position
       while (!feof(infile)) {
	 
	c = fgetc ( infile );
	buffer->push( c ); 
	
	//	if(XTERM!=NULL)fprintf(XTERM,"P <%s>\n", ch );

       }// while  not feof 
      }// infile not NULL  
      fgetpos (infile, &position);
      fclose(infile); 

      if(XTERM!=NULL)fprintf(XTERM,
		  "evt_pusher (textFILE) closed file. %d\n",(int)buffer );

     wait=MyCond.TimedWaitRelative( 1000  ) ; // wait 500 

      }while(wait != 0);

   if(XTERM!=NULL)fprintf(XTERM,
		  "evt_pusher (textFILE)  EXIT buffer=%d\n",(int)buffer );
 }/*****************************end of function *********************/











  /**********************************************
   *            PUSH binary (nanot) DATA FROM A FILE ......
   */
 int* evt_pusher_file(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_remote (file)  par==%d; pointer==%d\n", par,(int)buffer );

      FILE *infile;  char fname[400];
      int buffer4;
      long long int cnt=0;
      size_t result;
      TSmallish_xml xml(    acqxml   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","file" );
      sprintf( fname,"%s", xml.output  );
      infile=fopen( fname,"rb"  );
      if(XTERM!=NULL)fprintf(XTERM,"opened %s\n", fname);
      if (infile!=NULL){
       while (!feof(infile)) {
	 if ((cnt%250000)==0){if(XTERM!=NULL)fprintf(XTERM,"R %7lld kB\n",4*cnt/1000);} cnt++;
	result=fread( &buffer4, 1, 4, infile );
	if (result == 4) {	buffer->push( buffer4 ); }
       }
       if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_file closed....%s\n", fname );
	fclose(infile);
      }else{
	if(XTERM!=NULL)fprintf(XTERM,"infile %s == NULL\n%s", fname );
      }
      if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_file call finished....%s: PUSHER FINISHED\n", fname );
 }/*****************************end of function ***********************/








  // TServerSocket *ss=new TServerSocket(i,kTRUE);
  //     tail -f text | nc localhost 9302
 int* evt_pusher_net_txtserv(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_remote (network,txt)  par==%d; pointer==%d\n", par,(int)buffer );
      long long int cnt=0;

   char ipaddress[100];
   int port;
      TSmallish_xml xml(    acqxml   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","ip" );
      sprintf( ipaddress,"%s", xml.output  ); // 127.0.0.1, else not possible
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","port" );
      port=atoi(xml.output  );


  while( 1==1 ){// GLOBAL WAIT
   TServerSocket *ss = new TServerSocket(port, kTRUE);
   int wait=1;

   TSocket *s0 = 0;
   char aaa[100000];
   int nekonec=1;  int i; int get=999;
   int eotran=1;
   while (wait||(eotran)) {
   //    for (i=0;i<get;i++){ aaa[i]='\0';}
   //      printf("STOJIM NA POCATKU :%s\n", aaa );
      if(XTERM!=NULL)fprintf(XTERM,
	  "(pusher:nettxtsrv)Starting Accept on %s:%d..\n", ipaddress, port);
      s0=ss->Accept(); // THIS ONE BLOCKS........
      //	printf("%s\n", "." );

      char newline='\n';
      if (s0){
	while (get>0){
	get=(int)s0->RecvRaw( aaa , 1, kDontBlock);
	//        if (  (strlen(aaa)>0)&& (aaa[ strlen(aaa)-1]==newline) ){//remove EOL
	//	  aaa[ strlen(aaa)-1]='\0';
	//        }
	//TEXTOUT if(XTERM!=NULL)fprintf(XTERM,"%s",aaa);
	for (int j=0;j<strlen(aaa);j++){buffer->push( aaa[j] );}		       
	//        printf("======== PRISLO RAW :<%s> RESKOD==%d\n", aaa , get );
	//	printf("%s (errno: %d)\n",aaa,gSystem->GetErrno()); 
       	}//while get>0
	for (i=0;i<get;i++){ aaa[i]='\0';}
	//	 printf("removing  s0:%s\n", aaa );
	 if(XTERM!=NULL)fprintf(XTERM,"%s","(pusher:nettxtsrv) killing s0 socket...\n");
	 s0->Close(); s0=NULL;// s0:odpojuji socket

      if ( get<=0){ 
	//	   printf("removing  from monitor 2 (possible ctrl-c; ctrl-d not wrk) :%s\n", aaa );
	if(XTERM!=NULL)fprintf(XTERM,"(pusher:nettxtsrv) wait<=0: because %d==get<=0...\n", get);
	eotran=0;//wait=0;// means konec while
      }//get<=
      }//s0
      //   printf( "INSIDE  WAITing, just after NOT IsA() :<%s>\n",  aaa );
   if (wait!=0){wait=MyCond.TimedWaitRelative( 100  ) ; }
   if(XTERM!=NULL)fprintf(XTERM,"%s","(pusher:nettxtsrv)waiting..\n");
   if (wait==0)break;
 }//while 1==1

   if(XTERM!=NULL)fprintf(XTERM,"%s","(pusher:nettxtsrv)END, closing ss serversocket..\n");
            ss->Close();
	    // WHEN I DO tail -f file | nc ...... then it goes through this

      }//GLOGABL WHILE


 }//=====================================================================END FUNCTION










  /**********************************************
   *              cat runx.dat |  nc -l -p 9302
   *                         this is a client that connects to a server...
   */
 int* evt_pusher_net(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_remote (network)  par==%d; pointer==%d\n", par,(int)buffer );
      long long int cnt=0;

   char ipaddress[100];
   int port;
      TSmallish_xml xml(    acqxml   );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","ip" );
      sprintf( ipaddress,"%s", xml.output  );
      xml.DisplayTele( xml.mainnode, 0, "plugins","pusher","port" );
      port=atoi(xml.output  );



  char strbuf[2000000];// 2MB
  int *buffer4p;
  buffer4p=(int*)&strbuf[0];
  int d,i,ii;
  int maxtrans=2000000;
  TSocket *socket;

  double downtime; int downtimef, downtimei,  wait=1;  // WAIT

  while (0==0){// ----- - -- READ ALL REPEATING CONNECTIONS --- -  -- -- - --   - - -

    //
    //  DRUHA STRANA LISTENS !!!!!!!!!!!!!!!!!!!!!!!!!!
    //
    //    printf("going to socket%s\n","");
  socket=new TSocket( ipaddress, port);
  //    printf("after the socket%s\n","");
 
  while ( (socket)&&(1==1) ){// ----- - -- READ ONE CONNECTION --- -  -- -- - --   - - -

    i=(int)socket->Select(TSocket::kRead, 1000);//timeout 1sec, repeat 5x 

    if (i>0) {//####CASE i>0 ####
      d=(int)socket->RecvRaw(strbuf, maxtrans, kDontBlock  ); // read small buffer
      if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_netw socket got %d bytes \n", d );
     ii=0;
      while(ii*4<d){
	buffer->push( buffer4p[ii]  );
	ii++;
	if ((cnt%25000)==0){if(XTERM!=NULL)fprintf(XTERM,"P %7lld kB\n",4*cnt/1000);} cnt++;
      }//while - push
      //      if(XTERM!=NULL)fprintf(XTERM,"PUSH evt_pusher_netw wait 100ms....%s; iii*4==%d, d=%d\n", ipaddress,ii*4,d );
    }//####CASE i>0 ####  socket select was DONE;   i >0

    if(XTERM!=NULL)fprintf(XTERM,"%s","^");fflush(XTERM);
    //    usleep(1000*300); //wait
    wait=MyCond.TimedWaitRelative( 300  ) ; //
    if (wait==0){ 
      if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_netw got BROADCAST SIGNAL... %s\n", "" );
      socket->Close(); 
      break; 
    }
    //cannot be here    TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.

    if (i<0){ //####CASE i<0 ####
       if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_netw SOCKET LOST....%s; iii*4==%d, d=%d\n", ipaddress,ii*4,d );
      socket->Close(); break; 
    }//####CASE i<0 #### 

    if (i==0){ //####CASE i==0  ####
       if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_netw I RELEASE SOCKET(ZERO)..%s; iii*4==%d, d=%d\n",
			                                                            ipaddress,ii*4,d);
      socket->Close(); break; 
    }//####CASE i==0  ####
    
  }// 1==1---- ---- --    --WHILE read all the time - ONE CONNECTION --------
  socket->Delete();

      //      double downtime; int downtimef, downtimei,  wait=1;
         TTimeStamp t_wait;
       downtime=t_wait.GetSec()+t_wait.GetNanoSec()*1e-9;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d , %f \n", wait, downtime );
       downtime=downtime+2.5 + 2.5;   // 0.5 sec was NOT ENOUGH !!!
       downtimei=(int)downtime;
       downtimef=(int)( 1e+9*(downtime-1.0*downtimei) );
       if(XTERM!=NULL)fprintf(XTERM,"R%s","" );
       wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       if(XTERM!=NULL)fprintf(XTERM,"S%s","" );

  }// 0==0----- - -- READ ALL REPEATING CONNECTIONS --- -  -- -- - --   - - -



  // socket must be already closed here....socket->Close();
      if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_file call finished....%s: PUSHER FINISHED\n", ipaddress );
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
