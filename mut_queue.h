#ifndef mut_queue_H
#define mut_queue_H

#include <queue>   // STD LIB
//NOT IN HEADERusing namespace std;


//              g++ queue.c -o queue 
//              g++ -lpthread mut_queue.c -o mut_queue
/*  
   .L mut_queue.C+ 
...maybe I do gSystem->Load("libThread"); in .rootlogon, but necessary?
   g++  -ggdb3  -lpthread  -fPIC  `root-config --libs --cflags --glibs`   -Wall -rdynamic -ldl mut_queue.c -o  mut_queue2
   g++  -fPIC  `root-config --libs --cflags --glibs`   -Wall -rdynamic -ldl standalone.cpp -o standalone
*/
//******* * test plugins.so :
//  nm -g plugins.so 
//----------------------------------------------------------
/*  LOGIKA
 *    - chanel     n-v,   def. -1
 *    - scaler     n-v-accv-t0
 *    - flag       n-v  (time, run, block, evt-number)
 */
//http://root.cern.ch/download/doc/ROOTUsersGuideHTML/ch12s07.html

/* TIMING - TThread: 848MB push/pop in 70 sec., main part was call pop
 *        65 mil  = 240MB buffer used
 * withou writeout -  848MB in  51 sec  ... 16MBps
 *        5  mil = 20MB max buffer used
 *
 * 75%CPU when similar
 * 34%CPU when 6.2 MBps
 *  7%    when 0.8MBps
 */


// I DEPRECIATE ....TO *.SO   const char acqxml[100]="acq_setup.xml";


#include "TThread.h"   //  CINT?   //  -lThread

// I declare only, definition comes elsewhere
//extern TCondition MyCond(0);
extern TCondition MyCond;
//this has an internal mutex-LINK TO cpp plugins....


template<typename Data>
class concurrent_queue
{
private:
    std::queue<Data> the_queue;
public:
     ~concurrent_queue(){ printf("queue destructor called\n%s","");  }
    void push(Data const& data)
    {    
             TThread::Lock();
      //      printf("push - entered and locked with data %d\n", (int)data );
      the_queue.push( data );
      //      printf("push - pushed   empty=%d\n", (int)the_queue.empty() );
           TThread::UnLock();
    }

    
    Data front()
    {    
             TThread::Lock();
      //      printf("front -    empty=%d the queue=%d\n", (int)the_queue.empty(), (int)&the_queue  );
      Data aaa=the_queue.front();
           TThread::UnLock();
      return aaa;
    }

    /*
    Data back()
    {    
      pthread_mutex_lock( &mutex_queue );
      Data aaa=the_queue.back();
      pthread_mutex_unlock( &mutex_queue );
      return aaa;
    }
    void pop()
    {    
      pthread_mutex_lock( &mutex_queue );
      the_queue.pop();
      pthread_mutex_unlock( &mutex_queue );
    }
    */


    bool empty() const
    {
      //       pthread_mutex_lock( &mutex_queue );
        bool em=the_queue.empty();
	//       pthread_mutex_unlock( &mutex_queue );
       return em;
    }

    int size() const
    {
      //       pthread_mutex_lock( &mutex_queue );
       int ssize=the_queue.size();
       //       pthread_mutex_unlock( &mutex_queue );
        return ssize;
    }

    /*    bool try_pop(Data& popped_value)
    {

        if(the_queue.empty())
        {
            return false;
        }
        
       pthread_mutex_lock( &mutex_queue );
        popped_value=the_queue.front();
        the_queue.pop();
       pthread_mutex_unlock( &mutex_queue );
        return true;
    }
    */
    void wait_and_pop(Data& popped_value)
    {
        while(the_queue.empty())
        {
	  usleep(1000);//  1 ms wait
        }
            TThread::Lock();
        popped_value=the_queue.front();
        the_queue.pop();
	//	printf("poped %d, empty ==  %d\n",popped_value, the_queue.empty()  );
           TThread::UnLock();
    }
};
//=====================CLASS DEFINED COMPLETELY IN HEADER===================

#endif

