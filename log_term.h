#ifndef log_term_H
#define log_term_H
 
#include <stdio.h>    // printf

// I just  declare (not define this variable)
extern    FILE* XTERM;


int fexists (char * fileName);
void table_log(int plugin, const char* text); // 0,1,2=Push,Pop,Ana  LOG TERM and FILE
int logterm();  // STARTS UP  THE TERMINAL; 0 if already existed

#endif
