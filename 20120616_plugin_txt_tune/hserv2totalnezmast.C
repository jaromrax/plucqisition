#include "TSocket.h"   //net thread
//---------------------------try server (for a client with txt...)
#include "TServerSocket.h"
#include "TMonitor.h"

void hserv2(){
   // This script shows how to make a simple iterative server that
   // can accept connections while handling currently open connections.
   // Compare this script to hserv.C that blocks on accept.
   // In this script a server socket is created and added to a monitor.
   // A monitor object is used to monitor connection requests on
   // the server socket. After accepting the connection
   // the new socket is added to the monitor and immediately ready
   // for use. Once two connections are accepted the server socket
   // is removed from the monitor and closed. The monitor continues
   // monitoring the sockets.

   TServerSocket *ss = new TServerSocket(9090, kTRUE);
   TMonitor *mon = new TMonitor;
   mon->Add(ss);

   TSocket *s0 = 0;
   char aaa[1000];
   int nekonec=1;  int radek=0;int i; int get;

   while ( nekonec ) {
     for (i=0;i<get;i++){ aaa[i]='\0';}
      printf("STOJIM NA POCATKU :%s\n", aaa );

      TSocket  *s;
      do{
	s = mon->Select(1000);   //  s=-1 if timeout // asi prestava fachat!!
      printf("stojim v  mon/select : ss==%d, s==%d s0==%d #%d  s:(%d)\n",
	     mon->IsActive(ss), mon->IsActive(s), mon->IsActive(s0),mon->GetActive(),   (int)s );
      }while(  (int)s==-1 );
      printf("JSEM ZA      mon/select \n", "");

      char newline='\n';

      if (s->IsA() == TServerSocket::Class()) {
         if (!s0) {
	   printf("acceptuji s do s0 a pridam s0 do monitoru mon\n", "");
            s0 = ((TServerSocket *)s)->Accept();
            mon->Add(s0);
	    get=s0->RecvRaw( aaa , 1000, kDontBlock);
	    if (  (strlen(aaa)>0)&& (aaa[ strlen(aaa)-1]==newline) ){
	      aaa[ strlen(aaa)-1]='\0';
	    }
	    printf("======== PRISLO RAW :<%s> RESKOD==%d\n", aaa , get );
	    radek++;
	     mon->Remove(s0);s0->Close(); s0=NULL;// s0:odpojuji socket

         }else{
	   usleep(1000*1000);
            printf(".......socket s0 already open\n");
	 }
      }//IsA
  usleep(1000);
  if (radek>3)nekonec=0;
   }// while
   mon->Remove(ss); ss->Close();
}
