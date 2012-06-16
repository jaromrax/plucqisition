{
 
   TServerSocket *ss = new TServerSocket(9090, kTRUE);
   TMonitor *mon = new TMonitor;
   mon->Add(ss);
   TSocket *s0 = 0, *s1 = 0, *s2 = 0;
   while (1) {
      TSocket  *s;
     do{ s = mon->Select( 1000 ); printf("","" );}while(  (int)s==-1 );

      if (s->IsA() == TServerSocket::Class()) {
         if (!s0) {
           s0 = ((TServerSocket *)s)->Accept();
            mon->Add(s0);
         } else if (!s1) {
	   printf("STOJIM NA s1 :%s\n","" );
            s1 = ((TServerSocket *)s)->Accept();
            mon->Add(s1);
         }  
        continue;
      }

      //   printf("STOJIM  na RECVRAW  :active ==%d\n", mon->GetActive() );
      char aaa[1000]; int get;
       char newline='\n';
     get=s->RecvRaw( aaa , 1000, kDontBlock);
     aaa[get]='\0';
     //      if (  (strlen(aaa)>0)&& (aaa[ strlen(aaa)-1]==newline) ){ aaa[ strlen(aaa)-1]='\0'; }

      printf("Client %d: get==%d:           <%s>\n", s==s0 ? 0 : 1, get, aaa );
      if (get==0){
         mon->Remove(s);
	 s0=NULL; // one client is fine for me (i can do two), here I recycle the s0 socket
      }
      if (strstr(aaa,"kill")!=0){
         printf("STOJIM  v kill : active==%d\n", mon->GetActive() );
	 mon->Remove(s);// this is removed. others not
         mon->Remove(ss); ss->Close();
         printf("STOJIM  v kill : active==%d\n", mon->GetActive() );
      }
      if (mon->GetActive() == 0) {
            printf("No more active clients... stopping\n");
            break;
      }

   }//while (1)

}
