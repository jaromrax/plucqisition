#include <stdio.h>
#include <zmq.hpp>  // aptitude install libzmq5-dev
/***************************************
 *
 *   start the client, 
 *   start the publisher, run it,
 *   client will register later and read the data
 *
 *-------------------------TESTS DONE------------
 * 1. files identical 1k 1M 10M
 * 2. speed test - server needs to sleep 50us to match client 1kB
 * 3. client 1st; server 2nd. works
 * 4. server runs, client 2nd. works, looses 1st data
 * 5. client 1st; server 2nd-runsaway: server 3rd: clients reconnected!
 *      few zero buffers
 ***********************************************/

//client.c
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

//--------- when mmap:
#include <err.h>
#include <sys/mman.h>
#include <fcntl.h>   //  open,  O_RW
#include <TMapFile.h>
#include <TH1F.h>
//-------- get_time
//#include <time.h>
//#include <ctime>
//#include <iomanip>
//#include <sys/timeb.h>
#include <sys/time.h>
long CurrentTime, PrevRateTime, ElapsedTime; //StartTime=global

unsigned char zmqtext[10240*1024]; //3boardsx8 chans x3 vals ... 100*4 chars  NO
int PORT=25700;
// g++  -Wall --permissive -g  zmqtest2sub.c -lzmq   -o zmqtest2sub  

// FOR FILE READ/ WRITE
unsigned char buffer[10240*1024];

TMapFile *mmapfd3=NULL;
TH1F *harray[7*8];

bool  writefile = true; // arguments: client nowrite 
char address[100];


/***************************************************
 *
 *
 *
 *
 ***************************************************/
long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}





TMapFile* init_histograms_mmap(  int records  ){

  int reco=records;
  printf("D...  record lenght for  HARRAY=%d\n", reco);  char fname3[500];
  char DataPath[1000]=".";
  sprintf( fname3, "%s/mmap.histo",  DataPath );  
  mmapfd3 = TMapFile::Create( fname3  ,"RECREATE" , 16*1024000 ,  "TH1F_16");   // why 4194304 ?
  char thname[10];
  char tnamed[100];
  ///    CHANNELS OF the 1st board only..................
  
  for (int i=0;i<8;i++){ // channels
    sprintf(thname,"b0c%02d", i);
    harray[i]=new TH1F( thname ,"vme_histogram",8192*2,0,8192*2);
    printf("D... NEW   spe  harray %d,   addr=%lu\n", i, (long unsigned)harray[i] );

    sprintf(tnamed,"b0c%02d", i+1);
    TNamed *n=new TNamed("display_with",tnamed);
    harray[i]->GetListOfFunctions()->Add(n);
    
    mmapfd3->Add( harray[i] );
    harray[i]->Fill( i*400+400 );
  
  }
  printf("D... histo added  %s\n",fname3 );
  mmapfd3->Update(  );

  return mmapfd3;
}



/***************************************************
 *
 *
 *
 *
 ***************************************************/


//dd if=/dev/urandom of=test.bin count=1024 bs=2

unsigned long read_binary_file(){
 FILE *ptr;
 char filename[]="test.bin";
 //struct stat info;
 unsigned long fileLen;
 
 printf("i... opening the binary file (r) %s\n", filename );
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
 
 printf("i... opening the binary file (w) %s\n", filename );
 ptr = fopen( filename ,"wb");  // r for read, b for binary
 printf("w... FILE %s SIZE: %lu\n", filename , fileLen );
 if (fileLen>sizeof(buffer)) {
   fileLen=sizeof(buffer);
   printf("w... FILE %s SIZE: %lu\n", filename , fileLen );
 }
 fwrite(buffer, fileLen ,1,ptr); //
 fclose(ptr);
}










/***************************************************
 *
 *
 *       CLIENT
 *
 ***************************************************/
int client ( bool writefile)
{
    CurrentTime = get_time();
    PrevRateTime= get_time();
    int rc;
    void *context= zmq_ctx_new();
    void* subscriber = zmq_socket(context, ZMQ_SUB);
    printf("i... CLIENT: to connect : %s\n",address );
    rc = zmq_connect (subscriber,  address );
    assert (rc == 0);
    zmq_setsockopt( subscriber, ZMQ_SUBSCRIBE, "", 0 );
    printf("i... CLIENT: connected  : %s\n",address );
    printf("i... CLIENT: entering while-loop and init(): %s\n",address );
    //zmq_msg_init( ZMQ_POLLIN );


    if (writefile){
      init_histograms_mmap(  500  ); //  TMAPFILE HISTOGRAMS
    }
    
    int count=0;
    while(1){
      // Receive message from server
      zmq_msg_t message;
      //printf ("c... CLIENT: init ...I am SUB\n");
      zmq_msg_init (&message);
      printf ("c... CLIENT: after init() ...waiting to receive\n");
      //int a=zmq_msg_recv(subscriber, &message, 0);
      int a=zmq_msg_recv( &message, subscriber, 0);
      printf("c... CLIENT: just received: a==%d \n",a );
      if( a==0){
	// CLIENT - no need to sleep, no cpuload without.
	usleep(1); //0.1 ms    10ms sleep// NO SLEEP ALSO OK
	continue;   // go back to start
      }
      //printf("...CLIENT: %d. after recv, a!=0\n" , count);
      unsigned int size = zmq_msg_size (&message);
      if (size>sizeof(buffer)){size=sizeof(buffer);} //dont overflow
      
      //char string[10000]; // = malloc (size + 1);
      printf("c... CLIENT: %d. copy to mem/buffer %d\n",count,size);
      //memcpy (string, zmq_msg_data (&message), size);
      memcpy (buffer, zmq_msg_data (&message), size);
      zmq_msg_close (&message);
      //string [size] = 0;
      //printf("W..CLIENT:Message is: %s\n",string);
      if (writefile){
	//==========  WRITE TEST HERE: UNCOMMENT
	write_binary_file( size ); //== write from buffer
	//================ Fill MMAP Histo Test
	harray[0]->Fill(count % 10000);
	harray[1]->Fill(count % 12000);
      
	CurrentTime = get_time();
	ElapsedTime = CurrentTime - PrevRateTime; /* milliseconds */
	if (ElapsedTime>500){
	  mmapfd3->Update(  ); // update NOT TOO FREQUENTLY
	  PrevRateTime=CurrentTime; /* milliseconds */
	}
      }

      count++;
    }

    zmq_close (subscriber);
    zmq_term (context);
    return 0;
}







/***************************************************
 *
 *
 *               SERVER
 *
 ***************************************************/
void server( int loops ){
  unsigned long rd;
  rd=read_binary_file(); //======== read bin file to buffer 
  //write_binary_file( rd ); //========write from buffer 
  char address[100];
  char tmp[2000];
   //  Prepare our context and socket
   zmq::context_t context (1);
   zmq::socket_t  zmqsocket (context, ZMQ_PUB); //ZMQ_REP
   sprintf( address, "%s:%d", "tcp://*",  PORT );
   printf("s... SERVER: binding  %s\n",address );
   zmqsocket.bind ( address   );
   usleep(1000*300);  // wait 300ms for register. Cleint 1st, this is SAFE
   
   //   for (int i=0;i<100000;i++){
   //==========  SINGLE/MULTI TEST HERE: 
   for (int i=0;i<loops;i++){
     // it has delay 200 loops 2048kB/1ms when sleep is 10us
     sprintf( tmp, "%05d%d", i,12345 );
     memcpy( &zmqtext, tmp, strlen(tmp) );
     printf("s... SERVER: %4d/%4d   %ld ... is not real sizenow, %lu is\n",
	    i, loops, strlen(tmp),  rd  );
     if (1==1){
       // // //========== ZMQ SEND PUB : define "replay"
       //zmq::message_t reply ( strlen(tmp) );
       zmq::message_t reply ( rd  );
       //memcpy ((void *) reply.data(), zmqtext, strlen(tmp) );
       memcpy ((void *) reply.data(), buffer, rd  );
       // // //printf("hwserver  PUB  sending /%s/\n", zmqtext);
       zmqsocket.send ( reply ); 
       usleep(50*1);  // SEND EVERY  1ms
       usleep(1000*1000);  // DELAY FOR BIG FILE
       // IF every event is MMAP->Update(): 0.25ms is ok (1sec delay client)
       // TEST with 500ms UPDATE in CLIENT:
       //          1us sleep is enough to catchup==100us in fact
       
     }
   }//======= for i 1000
}





/***********************************************
 *
 *  MAIN -------------------
 *
 **********************************************/

int main( int argc, char *argv []){
  printf("I am an zmq publisher/subscriber test on port=%d\n", PORT );

  printf("parameters...%d\n", argc);
  int loops=1;
  if (argc>1){
    printf("i... parameter: %s\n", argv[1]);
    if ( strstr(argv[1],"server")!=NULL){
      printf(".... %s\n","server");
      if (argc>2){
	if ( atoi(argv[2])>0){
	  loops=atoi(argv[2]);}
	printf("D... LOOPS %s : %d\n", argv[2], loops );
      }
      server( loops );
    }
    if ( strstr(argv[1],"client")!=NULL){
      printf(".... %s\n","client");
      writefile=true;
      if (argc>2){
	if ( strstr(argv[2],"nowrite")!=NULL){
	  writefile=false;}
	printf("D... NEW write status : %d\n", writefile );

      }
      sprintf( address, "%s:%d", "tcp://127.0.0.1",  PORT );
      if (argc>3){
	sprintf( address, "tcp://%s:%d" , argv[3] ,PORT );
	printf("D... NEW address  %s\n", address);
      }
      client( writefile );
    }
  }else{
    printf("\nNO ARGUMENTS. [server|client]%s\n","");
  }
  printf("\n    client [nowrite|aaa]  [127.0.0.1] %s\n","");
  printf("\n    server [1|10|1000]  %s\n","");
  
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
