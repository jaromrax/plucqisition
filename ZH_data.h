#define DEBUG 0
#ifndef zh_data_H
#define zh_data_H

// *.C is in Makefile as C_FILES_LIBS, so it will be linked as *.o to *.so

#include "TH1F.h"
#include "TTree.h"

#include <inttypes.h>    //  int64_t was not known as a type...?!?

//TIME OFFSET IN ROOT
const double ROOT_offset=7.889112e+08;

const int EOE= 0xf0000000;//end of event
const int BOR= 0xEFFFFFFF;//begin of run
const int BOE= 0xe0000000;//begin of event
const int BOEm=0xe000ffff;//begin of event

const int MAXCHAN=2048; // time is 1024+;  maximum channels



extern int ZHbuffer[99000000];//="ahoj\0";  // I MUST USE int!!!!!!????
extern int OEbuf[1000];//="Ahoj\0";

extern int OEBmax;// ONE EVENT LIMIT !!!!!!!!!!
extern int DataRead; // HowMuch was read to buffer

extern int64_t cnt_evt; // event number
//extern int64_t cnt_evt_data; // event number, data not time
extern Long64_t cnt_evt_data; // event number, data not time// in TTree

extern   int       T_yn[MAXCHAN];//  4 levels
extern   TH1F*     Thist;
extern   int       C_yn[MAXCHAN]; //lo hi
extern   int64_t   COUN[MAXCHAN];// total
extern   TH1F*     COUNhist[MAXCHAN];//histo to fill
extern   int64_t   COUNtmp[MAXCHAN]; // values
extern   TH1F*     HIST[MAXCHAN];
extern   UShort_t  TREE[MAXCHAN];
extern   UShort_t  ZERO[MAXCHAN];  
// - s : a 16 bit unsigned integer (UShort_t)
extern   TTree *ZH_tree;

extern double cTIME; // current data TIME (always>0)
extern double cTIME_root; // current data TIME (always>0)
extern double bTIME; // buffered time (mostly 0)
extern double sTIME; // startup time
extern double dTIME; // difference




void fillbuffer();// service: --- read buffer from disk----

int fillOEB(int pos);// fill ONE EVENT into OEbuf buffer

int channels_in_event(int word);// mask Exxx0000 and shift by two nibbles

void reset_chan_table();//  PREPARE CLEAN TABLE
void load_chan_table(const char *str2k); // LOAD channel properties into the table
void process_chan(int ch,  int val);// MAIN PART: fill propper histos,counters,time
void process_BOE();// begin of event - do cleaning
void process_EOE(); // end of event - do filling

int process_ONE_EVENT(int *arr);// translate buffer with one event to data

void ZH_data(int events);  // TEST




//=================================================END======
#endif
