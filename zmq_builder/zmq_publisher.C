#include <stdio.h>
#include <zmq.hpp>  // aptitude install libzmq5-dev
/***************************************
 *
 *   start the client, 
 *   start the publisher, run it,
 *   client will register later and read the data
 *
 ***********************************************/

//client.c
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>



unsigned char zmqtext[1024*1024]; //3boardsx8 chans x3 vals ... 100*4 chars  NO
int PORT=25700;
// g++  -Wall --permissive -g  zmqtest2sub.c -lzmq   -o zmqtest2sub  

// FOR FILE READ/ WRITE
unsigned char buffer[1024*1024];
// FILE *ptr;










//dd if=/dev/urandom of=test.bin count=1024 bs=2

unsigned long read_binary_file(){
 FILE *ptr;
 char filename[]="test.bin";
 //struct stat info;
 unsigned long fileLen;
 
 printf("i... opening the binary file %s\n", filename );
 ptr = fopen( filename ,"rb");  // r for read, b for binary
 //info.st_size * sizeof(char);
 fseek(ptr, 0, SEEK_END);
 fileLen=ftell(ptr);
 fseek(ptr, 0, SEEK_SET);
 
 // stat(filename, &info);
 printf("r...FILE %s SIZE: %lu\n", filename , fileLen );
 if (fileLen>sizeof(buffer)) { fileLen=sizeof(buffer);}
 fread(buffer, fileLen ,1,ptr); //
 fclose(ptr);
 return fileLen;
}






void write_binary_file( unsigned long afileLen ){
 FILE *ptr;
 char filename[]="tcpy.bin";
 unsigned long fileLen=afileLen;
 //struct stat info;
 // unsigned long fileLen;
 
 printf("i... opening the binary file %s\n", filename );
 ptr = fopen( filename ,"wb");  // r for read, b for binary
 printf("w...FILE %s SIZE: %lu\n", filename , fileLen );
 if (fileLen>sizeof(buffer)) {
   fileLen=sizeof(buffer);
   printf("w...FILE %s SIZE: %lu\n", filename , fileLen );
 }
 fwrite(buffer, fileLen ,1,ptr); //
 fclose(ptr);
}










//======================= CLIENT ===========================
int client ()
{
    int rc;
    //zmq::context_t *context= zmq_ctx_new();
    void *context= zmq_ctx_new();
    //zmq::socket_t  zmqsocket (context, ZMQ_SUB);
    //zmq::socket_t subscriber = zmq_socket(context, ZMQ_SUB);
    void* subscriber = zmq_socket(context, ZMQ_SUB);

    //void *context = zmq_ctx_new();
    //  Socket to talk to server
    //printf ("Collecting updates from server...I am SUB\n");
    //void *subscriber = zmq_socket(context, ZMQ_SUB);
    char address[100];
    sprintf( address, "%s:%d", "tcp://127.0.0.1",  PORT );
    printf("i..CLIENT: to connect : %s\n",address );
    rc = zmq_connect (subscriber,  address );
    assert (rc == 0);
    zmq_setsockopt( subscriber, ZMQ_SUBSCRIBE, "", 0 );
    printf("i..CLIENT: connected  : %s\n",address );
    printf("i..CLIENT: registering: %s\n",address );
    //zmq_msg_init( ZMQ_POLLIN );

    int count=0;
    while(1){
      // Receive message from server
      zmq_msg_t message;
      //printf ("...CLIENT: init ...I am SUB\n");
      zmq_msg_init (&message);
      printf ("...CLIENT: after init ...waiting to receive\n");
      //int a=zmq_msg_recv(subscriber, &message, 0);
      int a=zmq_msg_recv( &message, subscriber, 0);
      printf("...CLIENT: just received a==%d / \n",a );
      if( a==0){
	//usleep(1*1); //0.1 ms    10ms sleep// NO SLEEP ALSO OK
	continue;   // go back to start
      }
      printf("...CLIENT: %d. after if recv\n" , count);
      int size = zmq_msg_size (&message);
      if (size>sizeof(buffer)){size=sizeof(buffer);}
      
      //char string[10000]; // = malloc (size + 1);
      printf("...CLIENT: %d. copy to mem %d\n",count++,size);
      //memcpy (string, zmq_msg_data (&message), size);
      memcpy (buffer, zmq_msg_data (&message), size);
      zmq_msg_close (&message);
      //string [size] = 0;
      //printf("W..CLIENT:Message is: %s\n",string);
      write_binary_file( size ); //== write from buffer
    }

    zmq_close (subscriber);
    zmq_term (context);
    return 0;
}







//============================== SERVER =========

int server(){
  unsigned long rd;
  rd=read_binary_file(); //======== read bin file to buffer 
  //write_binary_file( rd ); //========write from buffer 
  char address[100];
  char tmp[2000];
   //  Prepare our context and socket
   zmq::context_t context (1);
   zmq::socket_t  zmqsocket (context, ZMQ_PUB); //ZMQ_REP
   sprintf( address, "%s:%d", "tcp://*",  PORT );
   printf("SERVER: binding  %s\n",address );
   zmqsocket.bind ( address   );
   usleep(1000*300);  // wait 300ms for register
   
   for (int i=0;i<1;i++){
     // it has delay 200 loops 2048kB/1ms when sleep is 10us
     sprintf( tmp, "%05d%d", i,12345 );
     memcpy( &zmqtext, tmp, strlen(tmp) );
     printf("SERVER: %d.   %ld ... is not real sizenow, %lu is\n",
	    i, strlen(tmp),  rd  );
     if (1==1){
       // // //========== ZMQ SEND PUB : define "replay"
       //zmq::message_t reply ( strlen(tmp) );
       zmq::message_t reply ( rd  );
       //memcpy ((void *) reply.data(), zmqtext, strlen(tmp) );
       memcpy ((void *) reply.data(), buffer, rd  );
       // // //printf("hwserver  PUB  sending /%s/\n", zmqtext);
       zmqsocket.send ( reply ); 
       usleep(1000*1);  // SEND EVERY  1ms
     }
   }//======= for i 1000
}





/***********************************************
 *
 *  MAIN -------------------
 *
 **********************************************/

int main( int argc, char *argv []){
  printf("I am zmq publisher/subscriber on port=%d\n", PORT );

  printf("parameters...%d\n", argc);
  if (argc>1){
    printf("i... parameter %s\n", argv[1]);
    if ( strstr(argv[1],"server")!=NULL){
      printf("%s\n","server");
      server();
    }
    if ( strstr(argv[1],"client")!=NULL){
      printf("%s\n","client");
      client();
    }
  }else{
    printf("\nNO ARGUMENTS. [server|client]%s\n","");
  }
  
  // zmq::context_t context(1);
  // //  Subscriber tells us when it's ready here
  // zmq::socket_t sync(context, ZMQ_PULL);
  // sync.bind("tcp://*:5564");
  // //  We send updates via this socket
  // zmq::socket_t publisher (context, ZMQ_PUB);
  // publisher.bind("tcp://*:5565");
  // //  Wait for synchronization request
  // s_recv(sync);
  // //  Now broadcast exactly 10 updates with pause
  // int update_nbr;
  // for (update_nbr = 0; update_nbr < 10; update_nbr++) {
    
  //   std::ostringstream oss;
  //   oss << "Update "<< update_nbr ;
  //   s_send (publisher, oss.str());
  //   sleep (1);
  // }
  // s_send (publisher, "END");
  
  // sleep (1);              //  Give 0MQ/2.0.x time to flush output
  return 0;

    
}//================================== MAIN END
