#include "xml_attr.h"

int xml_attr(){

    char ch[100];

  if (  FileExists("mut_queue.xml",1) ){  //minimum size 

    TSmallish_xml xml("mut_queue.xml");
    xml.DisplayTele( xml.mainnode, 0, "pusher","pusher","file" );
    printf( "input file ==\"%s\"\n", xml.output  );

    xml.DisplayTele( xml.mainnode, 0, "poper","poper","file" );
    printf( "output file==\"%s\"\n", xml.output  );

  }//  XML FILE EXISTS------------------------------
}//MAIN
