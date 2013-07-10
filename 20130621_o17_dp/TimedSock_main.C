/*
 *   IN ROOT"
 *  .L TimedSock.C
 *  .L TimedSocs_main.C
 *   main()
 */
#include "TimedSock.h" // MY HEADER

// create a cat, set its age, have it
// meow, tell us its age, then meow again.
int main()
{
  int Age;
  cout<<"How old is Frisky? ";
  cin>>Age;
  Cat Frisky(Age);
  Frisky.Meow();
  cout << "Frisky is a cat who is " ;
  cout << Frisky.GetAge() << " years old.\n";
  Frisky.Meow();
  Age++;
  Frisky.SetAge(Age);
  cout << "Now Frisky is " ;
  cout << Frisky.GetAge() << " years old.\n";
  usleep(1000*1000*1);
  Frisky.GetSocket("127.0.0.1",  9302 );
  usleep(1000*1000*1);
  cout << "Now Frisky is "  << Frisky.GetAge() << "\n";
  usleep(1000*1000*1);
  cout << "Now Frisky is " << Frisky.GetAge() << "\n" ;
  usleep(1000*1000*1);
  cout << "Now Frisky is " << Frisky.GetAge() << "\n" ;
  usleep(1000*1000*1);
  cout << "Now Frisky is " << Frisky.GetAge() << "\n" ;
  usleep(1000*1000*1);
  cout << "Now Frisky is " << Frisky.GetAge() << "\n" ;
  usleep(1000*1000*1);
  cout << "Now Frisky is " << Frisky.GetAge() << "\n" ;
  return 0;
}
