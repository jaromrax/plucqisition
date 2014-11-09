#define DEBUGXML 0
#ifndef xml_attr_H
#define xml_attr_H
 
/*
 *  no dependence on libxml2
 *  compiles in CINT
 *  cannot compile with  g++
 * g++  -ggdb3  `root-config --libs --cflags --glibs`   -Wall xml_attr.c
 */

#include "TXMLEngine.h"
#include <string>         // std::string
// NOT IN HEADER using namespace std;

double TokenGet( const char* tok, const char* remote, char* result);
void TokenReplace( const char* tok, const char* newtok, const char* remote, char* remoteNEW);

//avoid using namespace in .h string =>  std::string
bool FileExists1k(std::string strFilename, int minsize) ;


class TSmallish_xml
{

private:
  TXMLEngine* xml;
  XMLDocPointer_t xmldoc;
  //  bool DEBUG;
  int FOUND;
  int inside_UID;
public:
  int leveloffset;//=-1;
  XMLNodePointer_t mainnode ;
  std::string addr[14];//={"telescopes", "t1", "V" , "addwith" };//14 levels now!
  char output[1024];
  int getattrlevel;//=3;

  TSmallish_xml(const char* filename);
  ~TSmallish_xml();

  void DisplayTele( XMLNodePointer_t node, Int_t level, const char* search, const char* seq, const char* searchatt );
  void token(const char* str);

};



/**********************************************************************
 *
 *   XML           from mkmat.C
 *
 *<!--
A COMMENT MUST BE HERE
-->
<xmlconfig>
 <experiment date="20110928" reaction="o17_3hed"> </experiment> 
 <dir> /home/mrazek/OJR/20111028_o17_3he </dir>
 <telescopes>
    <t1>
    <V addwith="1.0" dechan="17" echan="1">      </V>
    <O addwith="1.0"  dechan="2"  echan="1">      </O>
    <L addwith="1.0" dechan="0"  echan="0">      </L>
  </t1>
  <t2>
    <V addwith="1.0" dechan="18" echan="2">      </V>
    <O addwith="1.0" dechan="10" echan="9">      </O>
    <L addwith="1.0" dechan="0"  echan="0">      </L>
  </t2>


    TSmallish_xml xml("config.xml");
    xml.DisplayTele( xml.mainnode, 0, "poper","B/Z","file" );
    printf( "\noutput==\"%s\"\n", xml.output  );

will look inside <poper> for B/Z sequence.  At Z it takes attribute "file"

    xml.DisplayTele( xml.mainnode, 0, "poper","poper/A/B/Z","file" );
full path is also ok 
 *  in XML:
 *  0 - top level - <xmlconfig></xmlconfig> - any name, but some is obligatory
 *  1 - 1st level, attributes can be there, whatever
 *
 *********************************************************************/

#endif
