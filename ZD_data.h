#define DEBUG 0
#ifndef zd_data_H
#define zd_data_H

const int EOE= 0xf0000000;//end of event
const int BOR= 0xEFFFFFFF;//begin of run
const int BOE= 0xe0000000;//begin of event
const int BOEm=0xe000ffff;//begin of event

const int MAXCHAN=2048; // time is 1024+

void fillbuffer();// service: --- read buffer from disk----

int fillOEB(int pos);// fill ONE EVENT into OEbuf buffer

int channels_in_event(int word);// mask Exxx0000 and shift by two nibbles

void reset_chan_table();//  PREPARE CLEAN TABLE
void load_chan_table(const char *str2k); // LOAD channel properties into the table
void process_chan(int ch,  int val);// MAIN PART: fill propper histos,counters,time
void process_BOE();// begin of event - do cleaning
void process_EOE(); // end of event - do filling

int process_ONE_EVENT(int *arr);// translate buffer with one event to data

void ZD_data(int events);  // TEST

#endif
