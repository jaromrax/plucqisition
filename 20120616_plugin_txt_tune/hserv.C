void hserv() {
   // Server program which waits for two clients to connect. It then monitors
   // the sockets and displays the objects it receives. To see how to
   // make a non-blocking server see the script hserv2.C.
   TServerSocket *ss = new TServerSocket(9090, kTRUE);
   // Accept a connection and return a full-duplex communication socket.
   int get=1;
   while (get>0){
   TSocket *s0 = ss->Accept();
   // Check some options of socket 0.
   int val;
   s0->GetOption(kSendBuffer, val);
   printf("sendbuffer size: %d\n", val);
   s0->GetOption(kRecvBuffer, val);
   printf("recvbuffer size: %d\n", val);
   char ccc[1000]=" ";
   get= s0->RecvRaw( ccc , 1000, kDontBlock );

   printf("%s\nClient 0: bytes recv = %d, bytes sent = %d\n", 
	  ccc,
	  s0->GetBytesRecv(),
          s0->GetBytesSent());
   // Close the socket.
   s0->Close();
   }//while

   // Close the server socket (unless we will use it later to wait for
   // another connection).
   ss->Close();
}
