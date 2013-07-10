#include "mut_queue.h"

/*
 *   buffer defined, should be ok with one thread pushing
 *   and the other poping... (priority is push)
 */


void queue_test(){

  int iii=1234, ooo;
  concurrent_queue<int> buffer;   // empty queue

  buffer.push( iii );
  printf( "size %d\n", buffer.size() );
  buffer.wait_and_pop( ooo );
  printf( "size %d; output %d\n", buffer.size(), ooo );
  
  //NOT .   delete buffer;
}
