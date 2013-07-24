#include "log_term.h"


#include <unistd.h>   //  stat (fexists) file size
#include <stdio.h>    // printf
#include<sys/stat.h>  // struct stat - gets loaded into root  .C+
#include <iostream>
#include <stdlib.h>    // getenv  system  strlen 


#ifdef __CINT__
//#include "vmblock.cxx+"
#else
//#include "vmblock.cxx"
#include <iostream>
#include <dlfcn.h>
#include "TString.h"        //  -lCore

#endif
//----------------------------with   mmap ------------------------
//#include <err.h>
//#include <sys/mman.h>

using namespace std;
//not here but better in acq_core...main file...FILE* XTERM;

// When I put it here,  root loads acq_core.C+  OK
// but I have a problem when I use plugin 
FILE * XTERM;

// The idea is to run this to have some nice table/like Log
//   maybe, saving the input for later (accum.)output....?
//   what is needed :   sprintf(ch, "", i )
void table_log(int plugin, const char* text){ // 0,1,2=Push,Pop,Ana
  if (plugin<0){
      if(XTERM!=NULL){
	fprintf(XTERM,  "%s\n", text );
	fflush(XTERM);
      }//XTERM
      FILE *fp;
      fp = fopen("acq.log","a");
      fseek(fp,0, SEEK_END);                
      fprintf(fp, "%s\n", text ); 
      fclose(fp);     
 }// MASTER.... not plugin......

  //IF PLUGIN::::::::::::::::::::::::
  if(XTERM!=NULL){
    //  5x  #   and then  29x " "
    fprintf(XTERM,  "%*s%*s%s\n", 3,"#",   plugin*29," " , text ); // number of spaces
    fflush(XTERM);
  }//XTERM

 FILE *fp;
 // char str[] = "test";     

 fp = fopen("acq.log","a");
 fseek(fp,0, SEEK_END);                
 fprintf(fp, "%*s%*s%s\n", 3,"#",   plugin*29," " , text ); // number of spaces
 fclose(fp);     

}//table_log





int fexists (const char * fileName){ // 0==exists;  >0  ==SIZE
   struct stat filestatus;
   int i = stat ( fileName, &filestatus );
   //0 == exists
   //   printf("file name <%s>\n", fileName);
   //    printf("file size %d\n", (int)filestatus.st_size);
   //    cout << filestatus.st_size << " bytes\n";
   /* File found */
     if ( i == 0 )
     {
       //   0    printf("i==0, ret 1, found\n");
       return filestatus.st_size;
     }
     //       printf("i!=0, ret 0, NOTfound\n");
     return 0;
       
}



//ps -ef |grep xterm | grep mut_terminal_log | awk '{print "/dev/"$6}'
//NEWER TRICK           xterm -e 'echo `tty`>/tmp/MYTTY;sleep 5;cat'&

// RUNS XTERM TO LOG
int logterm(){
  char devpts[80]="";
  int already_ex=0; // 0 is TRUE
  //unused  char devpts2[80]="";
  //unused  int res=4;
  /*
   *  part I. tricky.  sort all /dev/pts/ *   by reverse number, first believe is xterm with cat shell
   *
   *  I search ps for "cat" (my xterm shell)....
   */

	std::string name;
	name = getenv("USER");
	char touchfile[200];
	char commandrm[200];
	sprintf(touchfile, "/tmp/mut_terminal_%s.log" , name.c_str() );
	sprintf(commandrm, "rm %s" , touchfile);
  system(commandrm);
      usleep(1000*100);
      //  system("ps -ef |grep xterm | grep mut_terminal_log >/tmp/mut_terminal_log");
      //  system("ps -ef |grep xterm | grep mut_terminal_log | awk '{print \"/dev/\"$6}' >/tmp/mut_terminal_log");
	char commandps[300];
	sprintf( commandps, "ps -ef |grep \"00 cat$\"|grep -v \"00 sh\"|grep -v grep|grep pts|sort -rn >%s2", touchfile );
	system( commandps );
	sprintf( commandps, "ps -ef |grep \"00 cat$\"|grep -v \"00 sh\"|grep -v grep|grep pts|sort -rn| awk '{print \"/dev/\"$6}' > %s", touchfile );
	system( commandps );
      usleep(1000*100);
  FILE *f=fopen( touchfile ,"r");
  if (f!=NULL){
    fscanf(f,"%s", devpts);
    printf("1Content of devpts == %s, \n",devpts);
    /*    if (!feof(f)){
    fscanf(f,"%s", devpts2);
    printf("2Content of devpts == %s, \n",devpts2);
    }
    */
    fclose(f);
    //  int res=fread(devpts,1,sizeof(devpts),f);
    if ( strlen(devpts)>0){
    printf("Content of devpts == %s, trying to open\n",devpts);
    XTERM=fopen(devpts,"a");
    printf("Content of devpts == %s\n",devpts);
    fprintf(XTERM,"AHOJAHOJ\n\n%s","");
    }//res>0 jsou dve, 1. je xterm....
  }
  /*
   *   If nothing works,  open directly now xterm
   *   and believe that new /dev/pts/ii  belongs to it
   *
   */
  if (XTERM==NULL){
    printf("XTERM file NOT readable\n%s","");
  for (int i=0;i<35;i++){
    sprintf(devpts,"/dev/pts/%d", i);
    //    printf( "file = %d\n", file_exists(devpts) );
    if ( fexists(devpts)!=0 ){
      //      printf("The %s exists\n", devpts);
    }else{
      //      printf("The %s doesnot exist\n", devpts);
      system("xterm -T mut_terminal_log  cat&");
      usleep(1000*1000*3 );
      usleep(1000*1000);
      XTERM=fopen(devpts,"a");
      fprintf(XTERM,"AHOJAHOJ\n\n%s","");
      already_ex =1 ; // NOT existed before
      //      usleep(1000*1000*3);
      //      fclose(XTERM);
      break;
    }//if else fexists
  }//for
  }//XTERM stale NULL
  return already_ex; // already existed
}












//============================================= UNUSED =========

//http://www.go4expert.com/forums/showthread.php?t=871
void xprintf(const char * lpOutputString, ... )
{
    
    va_list argptr;    
    
    va_start( argptr, lpOutputString );            
    
    char OutMsg[1000];
    char format[1000];
    
    for(int i=0,j=0;lpOutputString[i] != '\0';i++) 
    {
        format[j++] = lpOutputString[i];
        // If escape character
        if(lpOutputString[i] == '\\')
        {
            i++;
            continue;
        }
        // if not a substitutal character
        if(lpOutputString[i] != '%')
            continue;
        
        format[j++] = lpOutputString[++i];
        format[j] = '\0';
        switch(lpOutputString[i])
        {
            // string
        case 's':
            {
                char* s = va_arg( argptr, char * );
                sprintf(OutMsg,format,s);
                strcpy(format,OutMsg);
                j = strlen(format);
                strcat(format," ");
                break;
            }
            // character
        case 'c':
            {
                char c = (char) va_arg( argptr, int );
                sprintf(OutMsg,format,c);
                strcpy(format,OutMsg);
                j = strlen(format);
                strcat(format," ");
                break;
            }
            // integer
        case 'd':
            {
                int d = va_arg( argptr, int );
                sprintf(OutMsg,format,d);
                strcpy(format,OutMsg);
                j = strlen(format);
                strcat(format," ");
                break;
            }
        case 'f':
            {
                double f = va_arg( argptr, double );
                sprintf(OutMsg,format,f);
                strcpy(format,OutMsg);
                j = strlen(format);
                strcat(format," ");
                break;
            }
        }
    }           
    printf("%s\n",OutMsg);
    va_end( argptr );
}



void xxxprintf( const char* Format, ... ){
      va_list Arguments;
      va_start(Arguments, Format);
      //   double FArg;
      //  int IArg;
      // char adva;
	printf("formati==<%s>\n",Format );
      for(int i = 0; Format[i] != '\0'; ++i )
      {

	if( Format[i] != '%')
            continue;
        

	    if (Format[i+1] == 'f')
            {
	      double d=(double)va_arg(Arguments, double);
                  printf("Caught a float : %.3lf\n",d);
            }
            else if (Format[i+1] == 'd')
            {
	      int ii=(int)va_arg(Arguments, int);
                  printf("Caught an integer : %d\n", ii);
            }
            else if (Format[i+1] == 'c')
            {
	      char c=(char)va_arg(Arguments, int);
                  printf("Caught an char : %c\n",c);
            }
            else if (Format[i+1] == 's')
            {
	      char *s=va_arg(Arguments, char *);
                  printf("Caught an char* : %s\n",s);
            }
      }//FOR 
      va_end(Arguments);
  printf("%15s  %15s  %15s\n", "PUSH ok" , "POP ok",  "ANALYZE ok" );
}
