#include <stdlib.h>
#include <stdio.h>  // printf
#include <unistd.h> // usleep
#include<sys/stat.h>
using namespace std;
FILE* XTERM;

int fexists (char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;
       
}

int logterm(){
  char devpts[80];
  for (int i=0;i<35;i++){
    sprintf(devpts,"/dev/pts/%d", i);
    //    printf( "file = %d\n", file_exists(devpts) );
    if ( fexists(devpts)!=0 ){
      printf("The %s exists\n", devpts);
    }else{
      printf("The %s doesnot exist\n", devpts);
      system("xterm  cat&");
      usleep(1000*1000);
      XTERM=fopen(devpts,"a");
      fprintf(XTERM,"AHOJAHOJ\n\n%s","");
      //      usleep(1000*1000*3);
      //      fclose(XTERM);
      break;
    }
  }
}
