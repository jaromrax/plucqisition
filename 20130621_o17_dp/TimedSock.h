#ifndef TIMEDSOCK_H
#define TIMEDSOCK_H
/*     THIS PRAGMA IS VERY IMPORTANT- prevents double include
 *
 */
class Cat                   // begin declaration of the class
{
  public:                    // begin public section
    Cat(int initialAge);     // constructor
    ~Cat();                  // destructor
    int GetAge() ;            // accessor function
    void SetAge(int age);    // accessor function
    void Meow();
    void HandleTO();
  TTimer* qalarm;
  TSocket* GetSocket(const char* ip, int port);

 private:                   // begin private section
    int itsAge;              // member variable
    char * string;
};

#endif

