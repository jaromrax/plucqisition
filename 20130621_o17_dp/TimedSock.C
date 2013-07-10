#include <iostream>      // for cout and cin
#include "TSocket.h"   //net thread
#include "TimedSock.h" // MY HEADER

 // constructor of Cat,
Cat::Cat(int initialAge)
{
  itsAge = initialAge;
  string = new char[10];

   qalarm=new TTimer(0, kFALSE);
   qalarm->SetInterruptSyscalls();
   qalarm->Connect("Timeout()", "Cat", this , "HandleTO()");
   qalarm->Start(7000, kTRUE);
}

Cat::~Cat()                 // destructor, just an example
{
  // delete[] ;
}

// GetAge, Public accessor function
// returns value of itsAge member
int Cat::GetAge()
{
   return itsAge;
}

// Definition of SetAge, public
// accessor function

 void Cat::SetAge(int age)
{
   // set member variable its age to
   // value passed in by parameter age
   itsAge = age;
}

// definition of Meow method
// returns: void
// parameters: None
// action: Prints "meow" to screen
void Cat::Meow()
{
   cout << "Meow.\n";
}

void Cat::HandleTO()
{
   cout << "Kurna Timeout\n";
}
 TSocket* Cat::GetSocket(const char* ip, int port){
     printf("inside GetSocket   %s:%d\n" ,  ip, port );
    TSocket *socks=new TSocket( ip, port);
    return socks;
 }





// create a cat, set its age, have it


// class TObj{
//  public:
//   int fTimeOut;
//   void HandleTimeOut();
//   TTimer* qalarm;
//   TObj();
//   ~TObj();
//   TSocket* GetSocket(const char* ip, int port);
// };

// void TObj::HandleTimeOut(){
//    Info("HandleTimeOut", "timeout expired");
//    // printf(" HADLE Timeout=%s\n" ,  "" );
//    fTimeOut=1;
//    return;
// }
// TObj::TObj(){
//   fTimeOut=0;
//   qalarm=new TTimer(0, kFALSE);
//   qalarm->SetInterruptSyscalls();
//   qalarm->Connect("Timeout()", 0, this , "HandleTimeOut()");
//   qalarm->Start(5000, kTRUE);
// }
// TObj::~TObj(){
//   delete qalarm;
// }

// TSocket* TObj::GetSocket(const char* ip, int port){
//     printf("inside GetSocket   %s:%d\n" ,  ip, port );
//    TSocket *socks=new TSocket( ip, port);
//    return socks;
// }


// void TimedSock(){

//  TObj *SpecTimerObj=new TObj();

// TSocket*    socket=SpecTimerObj->GetSocket( "127.0.0.1", 9302 ) ;

//  if (SpecTimerObj->fTimeOut) {
//     sprintf(ch,"P %s\n", "After TSocket - fTimeOut==1"); table_log(0,ch);
//  }

// }

