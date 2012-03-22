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

/**********last compile command:
g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o 

gcc -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o 

gcc  -ggdb3  -fPIC -c plugins.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plugins.so  plugins.o 



IF C++ => care about namemangling....


*****************
 results transfer :
     int*  (int*)  seems very general, anything can go
     -
     alloc memory in each cycle=too much time and nobody cares about dealoc
     better to alloc once and later just recheck

 */


// because of mixing  c and c++, namemangling.....
#ifdef  __cplusplus
extern "C" {
#endif

  //================================================= EXPORTED FUNCTIONS  START =========
  double* soarray;   //we allocate some memory in the first call to save time later
  //===========================================

  /*int my_c_function();
void hello() {
    std::cout << "hello Ahoj Vole" << '\n';
}
  */
 int* kolac(int* par){
   /*
    * kdybych chtel :   pozice 3x double   a  vektor 3xdouble
    *
    */

   if (soarray==NULL){  // YES, this is allocated only once and always used to send params
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
   //   double* array;
   //   array=new double [6];
   soarray[0]=1.2;
   soarray[1]=1.3;
   soarray[2]=1.4;
   
   printf("jsem uvnitr jakesi funkce kolac123 par[0] muze byt NULL\n%s", "");
   return (int*)soarray;
}



  /*********************************************************************
   *              FUNCTION
   */
 int* pekac(int* par){
   /*
    * kdybych chtel :   pozice 3x double   a  vektor 3xdouble
    *
    */
   double* array;

   if (soarray==NULL){  // YES, this is allocated only once and always used to send params
     soarray=new double [1024];
     printf("ALSO A HISTOGRAM IS DONE\n%s", "" );
     TH1F *h=new TH1F("h","h",1000,0,1000);
     h->Draw();
     //     printf("sleeping......\n%s" ,"");
     //     gSystem->Sleep(2000);
   }
   //   array=new double [1024];// MUST !!!!!  ALLOCATE 1k.   Deallocates in the code....
    soarray[0]=-17.0+((double*)par)[0]*((double*)par)[1];  // recast to double*; take an index from it(in fact derreference)
    soarray[1]=-17.0;
    soarray[2]=-17.0;
   
   printf("jsem uvnitr jakesi funkce PEKAC par[0]==%f\n",  ((double*)par)[0]  );
   
   return (int*)soarray;
 }/*****************************end of function *******************************************/













  /*********************************************************************
   *********************************************************************
   *              FUNCTION   prvni realna  funkce
   *********************************************************************
   */
 int* adubna(int* par){
   /*
    *   pozice 3x double   a  vektor 3xdouble
    *
    */
   double* array;

   if (soarray==NULL){  // YES, this is allocated only once and always used to send params
     soarray=new double [1024];
   }
   soarray[0]=gRandom->Uniform(2.0)-1.0;     //  x is left-right
   soarray[1]=gRandom->Uniform(8.0)-4.0;    //  y is top bottom
   soarray[2]=gRandom->Uniform(0.01)+0.0;    //  z is beamwise (xml will bias)

    //E
    soarray[5]=gRandom->Gaus(5.804,0.1);


    //MARSAGLIA: 1972
    double x1,x2;
    double pi=3.141592653589;
    double x,y,z;
    TVector3 v;
     //==========================================================
    /*
     *  I can select flux into a conus:  theta
     *  Elipsiodic conus would be better
     */
    do{
    gRandom->Sphere(x,y,z,1.0);
     v.SetXYZ(x,y,z);
    }while (   (v.Theta()>pi/6)   ||(y>0.2)||(y<-0.2)   );
    soarray[3]=v.Theta();
    soarray[4]=v.Phi();


    soarray[3]=soarray[3]*180./pi;
    soarray[4]=soarray[4]*180./pi;

   return (int*)soarray;
 }/*****************************end of function *******************************************/










 /*********************************************************************
   *********************************************************************
   *              FUNCTION   druha realna  funkce - test pro evt_pusher
      var 1/     staci cokoliv (int) vratit a uvnitr loopu se zavola push
      var 2/  mnohem lepsi - sam pouziva class concurrent_queue
   *********************************************************************
   */
 int* evt_pusher_forloop(int* par){
      concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   //   concurrent_queue *buffer=(concurrent_queue*)par;
      if(XTERM!=NULL)fprintf(XTERM,
	     "evt_pusher_remote (forloop)  par==%d; par0==%d;  pointer==%d\n", par,par[0],(int)buffer );
   for (int i=1;i<1000*1000*1;i++){  //........4*300M = 1200MB
     //    buffer.push ( i );
     if ((i%1000)==0)usleep(1);
     //     printf( " empty==%d\n",buffer->empty() );
     //     printf( " front==%d\n",(int)buffer->front() );
     buffer->push( i );
   }//for
 }/*****************************end of function *******************************************/








  /**********************************************
   *
   */
 int* evt_pusher_file(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_remote (file)  par==%d; pointer==%d\n", par,(int)buffer );
   int  wait=1;
      FILE *infile;  char fname[400];
      int buffer4;
      long long int cnt=0;
      size_t result;
      int kilo=250000;
      TSmallish_xml xml("mut_queue.xml");
      xml.DisplayTele( xml.mainnode, 0, "files","pusher","file" );
      sprintf( fname,"%s", xml.output  );
      infile=fopen( fname,"rb"  );
      if(XTERM!=NULL)fprintf(XTERM,"opened %s\n", fname);
      if (infile!=NULL){
       while (!feof(infile)) {
	 kilo=25;
	 if ((cnt%kilo)==0){if(XTERM!=NULL)fprintf(XTERM,"R %7lld kB\n",4*cnt/1000);} cnt++;
	result=fread( &buffer4, 1, 4, infile );
	if (result == 4) {	buffer->push( buffer4 );}
	wait=MyCond.TimedWaitRelative( 3  ) ; //
	if (wait==0){ 
	  if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_netw got BROADCAST SIGNAL... %s\n", "" );
	  fclose(infile);
	  break; 
	}//wait==0
       }//WHILE
       if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_file closed....%s\n", fname );
	fclose(infile);
      }else{
	if(XTERM!=NULL)fprintf(XTERM,"infile %s == NULL\n%s", fname );
      }
      if(XTERM!=NULL)fprintf(XTERM,"evt_pusher_file call finished....%s: PUSHER FINISHED\n", fname );
 }/*****************************end of function ***********************/











  /**********************************************
   *              cat runx.dat |  nc -l -p 9302
   */
 int* evt_pusher_net(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"PUSH RS evt_pusher_remote (network)  par==%d; pointer==%d\n", par,(int)buffer );
      long long int cnt=0;

   char ipaddress[100];
   int port;
      TSmallish_xml xml("mut_queue.xml");
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








 int* evt_poper_empty(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"  POP evt_poper_remote  par==%d; pointer==%d\n", par,(int)buffer );
   int datum=0;
   int runempty=0;
   usleep(1000*1000*5);
 }


  /**************************************************
   *            TO FILE OR JUST  POP     
   */
 int* evt_poper_file(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"  POP evt_poper_remote  par==%d; pointer==%d\n", par,(int)buffer );
   int datum=0;
   int runempty=0;

   //STANDARD  XML  READ-------------
      FILE *outfile;  char fname[400];
      int buffer4;
      long long int cnt=0;
      size_t result;
      TSmallish_xml xml("mut_queue.xml");
      xml.DisplayTele( xml.mainnode, 0, "files","poper","file" );
      sprintf( fname ,"%s", xml.output  );

      if ( strlen(fname)>0){
	//----------------OPEN FILE append and write
      outfile=fopen( fname,"ab"  );
      if(XTERM!=NULL)fprintf(XTERM,"  POP opened %s for WRITE - appending\n", fname);
      if (outfile!=NULL){
	while( !buffer->empty() ){
	  buffer->wait_and_pop(datum);
          if ((cnt%250000)==0){if(XTERM!=NULL)fprintf(XTERM,"W     %7lld kB\n",4*cnt/1000);} cnt++; 
	  fwrite ( &datum , 1 , 4  , outfile );
	}
	fclose(outfile);
  	TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
      }else{// outfile not NULL
	if(XTERM!=NULL)fprintf(XTERM,"  POP outfile %s == NULL\n%s", fname );
	runempty=1;
      }//      outfile not NULL
      }//strlen fname > 0   NOT ""
      else{
	runempty=1;
      }
      if (runempty==1){
	//-----------run empty--------
	while( !buffer->empty() ){
	  buffer->wait_and_pop(datum);
          if ((cnt%250000)==0){if(XTERM!=NULL)fprintf(XTERM,"-   %7lld kB\n",4*cnt/1000);} cnt++; 
  	TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
	}//while,  no writeout	
      }//runempty
}/**********************end of function ************/







 



  /*****************************************************************
   *            TEST TO SORT THE EVENTS ...........  CREATE TTREE
   */
 int* evt_poper_sort(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"  POP evt_poper_remote  ***********************%s\n", "" );
   if(XTERM!=NULL)fprintf(XTERM,"  POP evt_poper_remote  ***********************%s\n", "" );
   if(XTERM!=NULL)fprintf(XTERM,"  POP evt_poper_remote  ***********************%s\n", "" );

      double downtime; int downtimef, downtimei,  wait=1;

   int datum=0;
   int runempty=1;
   int oneeventbuf[1000]; //usualy 1header,one footer,4 channels=>12 integers
 /* 
  * data from:
  * . 
  */ 
  int chanlo= 1;  
  int chanhi=32;  
  int kc014A=-1; 
  int kc014B=-1; 
  int kc014C=-1; 
  int c257  =-1 ; 
  int v560na=33; // 32 words in data but only 16 channels on the module     0x21
                 // 2 uni-channels per one real-data-channel
  int v560nb=65; // 32 words in data but only 16 channels on the module     0x41
                 // 2 uni-channels per one real-data-channel
  int c32  = -1; // 32 words in data but only 16 channels on the module
  //  int c32  = 33; // 32 words in data but only 16 channels on the module
                 // 2 uni-channels per one real-data-channel

  int ltime= 1024 ; 
  int btime= 141 ; // hardcoded in cellrenderer.py
  int bnum= 140;   // hardcoded in cellrenderer.py
  // rnum  --  139 --- 08b  run
  //           140     08c  blok
  //           141     08d  time-block (x.0 us)
 double tzero=7.889112e+08;//7.889112e+08
 char parnam[5]="V" ; 
  char opt[1000];
  //=================================================

   //STANDARD  XML  READ-------------
      char initline[400];
      int buffer4;
      long long int cnt=0;  long long int cnt_evt=0;
      size_t result;
      TSmallish_xml xml("mut_queue.xml");
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","data","chanlo" );
      chanlo=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","data","chanhi" );
      chanhi=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014A" );
      kc014A=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014B" );
      kc014B=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","kc014C" );
      kc014C=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","c257" );
      c257=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","c32" );
      c32=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","v560na" );
      v560na=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","counter","v560nb" );
      v560nb=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","ltime" );
      ltime=atoi(xml.output);
      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","btime" );
      btime=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","number","bnum" );
      bnum=atoi(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","time","tzero" );
      tzero=atof(xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","crate","parnam" );
      sprintf(parnam,"%s",xml.output);

      xml.DisplayTele( xml.mainnode, 0, "acq_channels","buffer","circular" );
      int circular=atoi(xml.output);



  sprintf( initline, 
  "chanlo=%d,chanhi=%d,kc014A=%d,kc014B=%d,kc014C=%d,ltime=%d,btime=%d,bnum=%d,c257=%d,c32=%d,v560na=%d,v560nb=%d,parnam=%s,tzero=%lf,circular=%d", 
	   chanlo, chanhi,kc014A,kc014B,kc014C,ltime,btime,bnum,c257,c32,v560na,v560nb,parnam,tzero,
	   circular);
 

  if(XTERM!=NULL)fprintf(XTERM,"  POP PQ%s\n\n",initline);
  conv_u_init("", initline ); // file-name makes a problem for analyze-


    if(XTERM!=NULL)fprintf(XTERM,"  POP PQPrepared to runempty.............\n","");
      if (runempty==1){
	//-----------run empty--------
	int pointer; verbose=0;// from nano_acquis_pureconvert

	  if(XTERM!=NULL)fprintf(XTERM,"  POP PQpopping ........bufsize==%10d   empty==%d \n", buffer->size(), buffer->empty() );	
	  int status=0;
	  while(1==1){// INFINITE
	   

	while( !buffer->empty() ){// NEXT EVENT
	  pointer=0; status=0;
	  while( !buffer->empty() ){ //ONE-EVENT -CONSTRUCT
	  buffer->wait_and_pop(datum);
	  oneeventbuf[pointer] = datum; pointer++; // integer array
	  //          if ((cnt%25000)==0){if(XTERM!=NULL)fprintf(XTERM,"S        %7lld kB\n",4*cnt/1000);} cnt++;//1MB
	  if (datum==0xf0000000){ break;} // event footer.....  BREAKOUT
	  //	  if (cnt<18){
	  //	    printf( "%10d:  %04X  %04X    -  %10d\n", datum, datum&0xffff,  (datum>>16)&0xffff, 0xf0000000 );
	  //	  }
	  }//while
	  //do something with event HERE
	  cnt_evt++;
	  //one_buffer_process( (void*)buffer,  int(nminibuffer/4) , int(startup/4) ); //
	  one_buffer_process( (void*)&oneeventbuf, pointer , 0 ); //
	}//WHILE --- next event
	if (status==0){
      if(XTERM!=NULL)fprintf(XTERM,"  POP PQ Celkem  (int)cnt=== %lld,  events=%lld -------leaving poper\n",
			     cnt,  cnt_evt);
	}
	if(XTERM!=NULL)fprintf(XTERM,"%s",".");fflush(XTERM);
	//	usleep(1000*300);//buffer empty:100ms wait before next try
	status++;


	//###########################################
	//  STANDARD  WAIT  POINT FOR thread  EXIT
	//
       TTimeStamp t_wait;
       downtime=t_wait.GetSec()+t_wait.GetNanoSec()*1e-9;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d , %f \n", wait, downtime );
       downtime=downtime+2.5;   // 0.5 sec was NOT ENOUGH !!!
       downtimei=(int)downtime;
       downtimef=(int)( 1e+9*(downtime-1.0*downtimei) );
       if(XTERM!=NULL)fprintf(XTERM,"P%s","" );
       wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       if(XTERM!=NULL)fprintf(XTERM,"Q%s","" );

       //	TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
	}//INFINITE

      }//runempty
      //this neve happenes
      if(XTERM!=NULL)fprintf(XTERM,"  POPER SORT END  cnt=== %lld,  events=%lld -------leaving poper\n",cnt,  cnt_evt);


      /*
0xefffffff        -268435457       start of RUN - run header
0xe0040000        -536608768       4 channels will arrive
0xf0000000        -268435456       event footer
0xffffffff                         run footer
      */
}/**********************end of function ************/













  /**************************************************
   *            TEST TO   TTREE 
   */

  /*
 int* evt_analyze_test(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   if(XTERM!=NULL)fprintf(XTERM,"evt_analyze_remote  ..........................\n" );
   usleep(1000*1000);

   UShort_t cha[1255];// TRICK ~ I address channels by ch[1] later !!!!
   double time[12];

   double ran=gRandom->Uniform(4000.);


      //-----------------typpical creation of 2D matrix--------------
      TH2F *mtx1;
      mtx1=(TH2F*)gDirectory->Get("mtx1");
      if (mtx1==NULL){
	mtx1=new TH2F("mtx1","mtx1",1024,0,5000,1024,0,5000);
      }
      TH2F *mtx2;
      mtx2=(TH2F*)gDirectory->Get("mtx2");
      if (mtx2==NULL){
	mtx2=new TH2F("mtx2","mtx2",1024,0,5000,1024,0,5000);
      }

      TH1F *mtx1_1d_ela;
      mtx1_1d_ela=(TH1F*)gDirectory->Get("mtx1_1d_ela");
      if (mtx1_1d_ela==NULL){
	mtx1_1d_ela=new TH1F("mtx1_1d_ela","mtx1_1d_ela",5000,0,5000);
      }

      TH1F *mtx1_1d_d;
      mtx1_1d_d=(TH1F*)gDirectory->Get("mtx1_1d_d");
      if (mtx1_1d_d==NULL){
	mtx1_1d_d=new TH1F("mtx1_1d_d","mtx1_1d_d",5000,0,5000);
      }
      loadcuts();//  from cuts_manip.h  //  THIS BLOCKS COMLETELY dlclose
      TCutG *mtx1_cut_ela;
      mtx1_cut_ela=(TCutG*)gROOT->GetListOfSpecials()->FindObject("Cut00_mtx1");
      TCutG *mtx1_cut_d;
      mtx1_cut_d=(TCutG*)gROOT->GetListOfSpecials()->FindObject("Cut01_mtx1");
      long long int starting_point=0;
      long long int entr;
      long long int ii; 
      int down=5;
      double downtime; int downtimef, downtimei,  wait=1;
      while (wait!=0){//INFINITE INFINITE INFINITE INFINITE INFINITE INFINITE INFINITE 
	//	down--;
	
    TTree *tree_addr_old=(TTree*)gDirectory->FindObject("nanot");
    
    if ( tree_addr_old!=NULL ){ 
       TTree *tree_addr=(TTree*)tree_addr_old->Clone();
 	// ! zjistit, jestli existuje klon!!
      tree_addr->SetTitle("CLONE");
      
      tree_addr->SetMakeClass(1);//to use int,float.?http://root.cern.ch/drupal/content/accessing-ttree-tselector
      tree_addr->SetBranchStatus("*",0); //disable all branches
      tree_addr->SetBranchStatus("mainV",1);//active
      tree_addr->SetBranchStatus("time",1);//active

      tree_addr->SetBranchAddress("mainV", &cha[1] ); // MY TRICK TO HAVE V001 ~ cha[1] !!!!!!
      tree_addr->SetBranchAddress("time", &time[0] );
       // here is a little problem: circular buffer, entry number....end of the function ....
      entr=tree_addr->GetEntries();
       //  entr_first=tree_addr->GetEntryNumber(0);
       //  if(XTERM!=NULL)fprintf(XTERM,"  FTREE  found:  Entries==%lld ...  s.p.==%lld\n", entr, starting_point);
       //  printf("TTREE : %d entries\n", entr  );

       if ( (starting_point>0)&&(starting_point!=entr)){
       if(XTERM!=NULL)fprintf(XTERM,"  FTREE  :  Starting ANALYZE  - %lld begining at %lld ...\n",
			      entr, starting_point);
       }
       
       for(  ii=starting_point; ii<entr; ii++)  {
	 if ((ii%1000)==0){if(XTERM!=NULL)fprintf(XTERM,"T        %7lld e^3 evts\n",(int)(ii/1000) );} 
	 tree_addr->GetEntry(ii);
	 //	 printf(" %4d/ %11.2f chan 0 ====%d \n", ii, time[0], cha[1]   );
	 //	 	 printf(" %4d/               %11.2f chan 1 ====%d \n", ii, time[0], cha[1]   );
  //-----------------  typical matrix fill----------
	 if (cha[17]>0) mtx1->Fill( cha[1]+cha[17], cha[17] ); 
	 if (cha[18]>0) mtx2->Fill( cha[2]+cha[18], cha[18] ); 

	 if ( (mtx1_cut_ela!=NULL)&&(mtx1_cut_ela->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_1d_ela->Fill( cha[1]+cha[17]  );
	 }

	 if ( (mtx1_cut_d!=NULL)&&(mtx1_cut_d->IsInside( cha[1]+cha[17], cha[17] ) ) ){
	   mtx1_1d_d->Fill( cha[1]+cha[17]  );
	 }
		 //	 if (ii>200)break;
       }//FOR ALL AVAILABLE EVENTS
       
       starting_point=entr;
       //       tree_addr->Delete();
       tree_addr->Delete();
       //if(XTERM!=NULL)fprintf(XTERM,"  FTREE  deleted      %lld %lld\n", entr, starting_point);
    }// ttree NOT NULL

       TTimeStamp t_wait;
       downtime=t_wait.GetSec()+t_wait.GetNanoSec()*1e-9;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d , %f \n", wait, downtime );
       downtime=downtime+2.5;   // 0.5 sec was NOT ENOUGH !!!
       downtimei=(int)downtime;
       downtimef=(int)( 1e+9*(downtime-1.0*downtimei) );
       if(XTERM!=NULL)fprintf(XTERM,"M%s","" );
       wait=MyCond.TimedWait(  downtimei , downtimef  ) ;
       //       if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  wait %d ! %d.%d\n", wait, downtimei,downtimef);
       if (wait==0)break;
       //       usleep(1000*300);
       if(XTERM!=NULL)fprintf(XTERM,"N%s","" );
    //   //    if (wait==0)break;
       //  //  TThread::CancelPoint(); // When CancelOn(), here the thread can be interrupted.
      }//INFINITE LOOP ove ralways new clone


    if(XTERM!=NULL)fprintf(XTERM,"\n\n  FTREE  :  Exiting ANALYZE  - remote function ...\n\n%s","");
 }//***********************************************************



 












  // **************************************************
//   *                NOT ACTUAL ANYMORE 
//  
 int* evt_butcher_printtest(int* par){
   concurrent_queue<int> *buffer=(concurrent_queue<int>*)par;
   int datum=0;
   buffer->wait_and_pop(datum); // removes one element from NON EMPTY
   if ((datum%1000000)==0) printf("###############butcher %d  size=%6d kevt\n",datum, buffer->size()/1024);
   return (int*)datum;
   //   int i=(int)par[0];
   //   if ((i%100000)==0) printf("#################################butcher remote %d\n",i);

}//   end of function #################################################################


*/








  //================================================= EXPORTED FUNCTIONS  STOP =========
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
