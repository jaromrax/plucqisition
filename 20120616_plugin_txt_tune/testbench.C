#include <stdio.h>
#include "TSocket.h"   //net thread
#include "TServerSocket.h"
#include "TMonitor.h"
#include "TThread.h"   // CINT? //  -lThread

#include <iostream>
#include <fstream>    
using namespace std;

const int MAXTHREADS=10;

void net_server( void *arg )  
{
    int port=9999;
//===================================================================
  while( 1==1 ){
    printf("%s\n" ,"entered main while loop in net_server");
   TServerSocket *ss = new TServerSocket(port, kTRUE);
   TMonitor *mon = new TMonitor;
   mon->Add(ss);
   TSocket *s0 = 0, *s1 = 0, *s2 = 0;
   int wait=1; //EXTRA
   while (1) {
      TSocket  *s=NULL;
      while (s==NULL){
	do{ mon->ResetInterrupt();
	  s=mon->Select( );
	  printf("%d:%s",(int)s,"\n" );fflush(stdout);}while(s==(TSocket *)-1 );
      printf("out of select  .... %d\n" , (int)s );fflush(stdout);
      }
      printf("out of select2 .... %d\n" , (int)s );fflush(stdout);

      if (s->IsA() == TServerSocket::Class()) {
         if (!s0) {
      printf("%s","I am in s0\n" );fflush(stdout);
          s0 = ((TServerSocket *)s)->Accept();
            mon->Add(s0);
         } else if (!s1) {
	   printf("I am in s1 :%s\n","" );fflush(stdout);
            s1 = ((TServerSocket *)s)->Accept();
            mon->Add(s1);
         }  
        continue;
      }//if  IsA
      printf("%s","after continue...\n" );fflush(stdout);

      char aaa[1000]; int get;
       char newline='\n';
     get=s->RecvRaw( aaa , 1000, kDontBlock);
      printf("Client %d: get==%d:           <%s>\n", s==s0 ? 0 : 1, get, aaa );
     aaa[get]='\0';
      printf("Client %d: get==%d:           <%s>\n", s==s0 ? 0 : 1, get, aaa );

      if (get==0){
      printf("%s","mon remove....\n" );
         mon->Remove(s);
	 s0=NULL; // one client is fine for me (i can do two), here I recycle the s0 socket
      }// get==0
      if (strstr(aaa,"kill")!=0){
         printf("in a kill : active==%d\n", mon->GetActive() );
	 mon->Remove(s);// this is removed. others not
         mon->Remove(ss); ss->Close();
         printf("in a  kill : active==%d\n", mon->GetActive() );
      }
      if (mon->GetActive() == 0) {
            printf("No more active clients... stopping\n");
            break;
      }
   }//while (1)
  } // BIG  WHILE 1==1
//===================================================================

 printf("%s","net_server stop \n");
 return ;
}//--------void *evt_pusher( void *arg )  // loads the queue 




void testbench(){

TThread *shspe_threads[MAXTHREADS];

    shspe_threads[1] = new TThread( "my_server" , net_server, NULL );
    if (shspe_threads[1]==NULL){ printf("exiting, thread 1 not running\n%s","");return ;}
    shspe_threads[1]->Run();

}// .....................................................................testbench


