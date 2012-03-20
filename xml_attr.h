#define DEBUG 0
/*
 *  no dependence on libxml2
 *
 *  compiles in CINT
 *
 *  cannot compile with  g++
 *
 * g++  -ggdb3  `root-config --libs --cflags --glibs`   -Wall xml_attr.c
 *
 */
#include <stdio.h>

#include "TXMLEngine.h"
#include "TString.h" // good for xml
#include "unistd.h" //sleep; stFileInfo
#include <sys/stat.h> // stFileInfo
#include <string> // stFileInfo
#include <cstring>
#include <iostream>
using namespace std;


/**********************************************************************************
 *
 *   1kB +   file exists
 *
 **********************************************************************************/

bool FileExists(string strFilename, int minsize) { 
  struct stat stFileInfo; 
  bool blnReturn; 
  int intStat; 

  // Attempt to get the file attributes 
  intStat = stat(strFilename.c_str(),&stFileInfo); 
  if(intStat == 0) { 
    // We were able to get the file attributes 
    // so the file obviously exists. 
    //printf( "SIZE==%d\n", stFileInfo.st_size ); 
    if (stFileInfo.st_size<minsize){ blnReturn = false;
    }else{
    blnReturn = true; 
    }
  } else { 
    // We were not able to get the file attributes. 
    // This may mean that we don't have permission to 
    // access the folder which contains this file. If you 
    // need to do that level of checking, lookup the 
    // return values of stat which will give you 
    // more details on why stat failed. 
    printf( "Not exist ....size==%d\n", (int)stFileInfo.st_size ); 
    blnReturn = false; 
  } 
   
  return(blnReturn); 
}




/**********************************************************************************
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
 **********************************************************************************/
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

TSmallish_xml::TSmallish_xml(const char* filename){
  //  std::string addr[4];//={"telescopes", "t1", "V" , "addwith" };
  leveloffset=-1; // very upper level
  getattrlevel=3; // this is my attribute level ??
  //  DEBUG=0;
  FOUND=0;// not found.  1=found.  -1...error
  inside_UID=0;//  check that UID
   xml = new TXMLEngine;
   xmldoc = xml->ParseFile(filename);
   if (xmldoc==0) {
      delete xml;      return;  
   }
   // take access to main node   
    mainnode = xml->DocGetRootElement(xmldoc);
}
TSmallish_xml::~TSmallish_xml(){
   // Release memory before exit
   xml->FreeDoc(xmldoc);
   delete xml;
}

/*
 *  we fill addr[]  string. and at the end we put getattrlevel=last+1 
 *  - before it was address and attribute at the end.
 *  as we are sure that it is only 1st run => leveloffset=-1
 */
void TSmallish_xml::token(const char* str){// also inits!!!!
  char *pch;
  char mystr[1000];
  sprintf(mystr,"%s",str);
  int i=0;
  if (strlen(mystr)>0){
  if (DEBUG)printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (mystr,"/");
  while (pch != NULL)
  {
    addr[i]=pch; getattrlevel=i;
    if (DEBUG)printf ("(#%d)-%s- \t",i,pch);
    i++;
    pch = (char*)strtok ( NULL, "/");
   }//while
  getattrlevel++; 
  leveloffset=-1;  //reset
  FOUND=0;//reset
  inside_UID=0;// is insidde block with UID (search)
  sprintf( output, "%s", "" );
   if (DEBUG)printf("\n...........tokening done........getattrlevel==%d\n",getattrlevel);
  }//length>0
}//------------------------token----------------


void TSmallish_xml::DisplayTele( XMLNodePointer_t node, Int_t level, const char* search, const char* seq, const char* searchatt ){
  token( seq );
  /*
   * needs level=0 to enable recursive calls!
   *
   *chci aby - nasel unikatni sekci <blabla>
   *         - a pod ni (relativne) dosel az k <down> a z neho vzal attribute
   * 
   */
  TString tel;
  TString sat;
  tel.Append( xml->GetNodeName(node)  );
  if (DEBUG)printf("I am in <%s> node\n", tel.Data() );
  XMLNodePointer_t child;

  /****************************************************************************
   *  at first (leveloffset==-1) it finds the Unique section and addr[0]
   */
  if ( (leveloffset<0) ){ 
    // if I am in UID,  rise the flag.................
    // i should fall it also, but ....??

    if (tel.CompareTo( search )==0){ 
      inside_UID=1;
      if (DEBUG)printf("%*cFOUND MATCH OF UID <%s> at level .... ==%d\n",level+1,' ',search, level );
    }//rise the flag on UID  -----search is reached--------

    if ((tel.CompareTo( addr[0].c_str() )==0)&&(inside_UID==1)){//FOUND AND LEVEL
      leveloffset=level;  
      if (DEBUG)printf("%*cFOUND MATCH with <%s> at level offset.... ==%d\n\n",
		       level+1,' ',addr[0].c_str(), leveloffset );
      getattrlevel+=leveloffset-1;//  attrlevel IS RELATIVE; now I put real attr level!!!

    }else{//neni to on, hledej
      // display all child nodes   
      child = xml->GetChild(node);
      while (child!=0) {
	if (DEBUG)printf("%*c searching in children: (act.node=%s)\n", 
		       level+1,' ',
			 tel.Data()  );
	DisplayTele( child, level+1, search ,"",searchatt); 
         child = xml->GetNext(child);
      }//while
    }
  }// ------------------------leveloffset is now defined; start over and search for addr[] 




  /*
   *   mam addr[] : a/b/c   0,1,2 indexy                telescopes/t1/V  pak tam hledam attr.
   *                0 1 2   relativni levely
   *   level- leveloffset    == 0+
   *   leveloffset musim najit
   *  1/ UID    a addr[0]
   *  2/ pak najit leveloffset podle addr
   */
 


  if ( (leveloffset>=0)&&(level>=leveloffset) ){ 
    if (DEBUG)printf("%*c Level=%d ... Leveloffset=%d ,  act.node==%s\n", 
		     level+1,' ', level, leveloffset ,
		     tel.Data()  );
    //now we compare addr[0] to the current node.................
    if ( tel.CompareTo( addr[ level- leveloffset ].c_str() )==0){
            if (DEBUG)printf("%*c<%s> ....  found addr[%d] field (%s), act.node=%s:\n",
		   level+1,' ', addr[ level- leveloffset ].c_str(),
		   level- leveloffset,
			     addr[ level- leveloffset ].c_str() , tel.Data()  );
      // display all child nodes   
      if (getattrlevel==level){//>>>>>>>>>>>>>>>>>>>>>>>
	if (DEBUG)printf("%*c reached attr  level(%d) .... ==%d\n",level+1,' ',getattrlevel ,level );

	// display attributes
	XMLAttrPointer_t attr = xml->GetFirstAttr(node);
	while (attr!=0) {
	  sat=xml->GetAttrName(attr);
	  	  if (DEBUG)printf("%*c Checking  attr (seen x addr[]) =(\"%s\" x \"%s\")\n",
			 level+1,' ',
			 sat.Data(), searchatt  );
		  if ( sat.CompareTo( searchatt  )==0){//*********************
		    if (FOUND==0){//....................................
		      sprintf( output, "%s",   xml->GetAttrValue(attr) );//RESULT HERE
		      FOUND=1;
		    }//can be also error - FOUND==-1....................
	    if (DEBUG)printf("%*c ATTR <%s = %s (output)>  ---------------> FOUND\n",
			     level+1,' ',sat.Data() ,  output  );
		  }// attr OK if ( sat.CompareTo( searchatt  )==0)************
	   if (DEBUG)printf("%*c ..attr: %s value: %s\n",level+1,' ', xml->GetAttrName(attr), xml->GetAttrValue(attr));
	  attr = xml->GetNextAttr(attr);  
	}//while display attributeswhile (attr!=0) {.................
      }else{ // level not reached attrlevel.............  getattrlevel<=level

      XMLNodePointer_t child = xml->GetChild(node);// goes back up to root
      while (child!=0) {
		if (DEBUG)printf("%*c searching in children: %s.  (act.node=%s)\n", 
		       level+1,' ',
				 addr[level- leveloffset +1].c_str(), tel.Data()  );
		DisplayTele( child, level+1, addr[level- leveloffset +1].c_str(), "",searchatt ); 
         child = xml->GetNext(child);
      }//reached attrlevel
      }//else attrlevel  
    }//compare  tel.compareto  addr[0] -   ....sick?
    
  }// leveloffset>=0		

  //-----------------ON LEAVE - it can step out the GOOD 'search' UID-------------
  if (DEBUG)printf("%*c leaving node=%s) lvl=%d offs=%d, found=%d\n", 
		   level+1,' ', tel.Data(), level,leveloffset, FOUND  );
  if ((leveloffset>=0)&&(level<=leveloffset)&&(FOUND==0) ){
    // offset>=0:  already found UID
    // lvl>=offs:  still in
    if (DEBUG)printf(" ---------------------------> NOTFOUND flag!\n%s",""); FOUND=-1;
  }
  if ( tel.CompareTo( search)==0 ){
    inside_UID=0; // REMOVE FLAG !!!!
  if (DEBUG)printf("%*c leaving node=%s) removing the -------------------__>flag@@@\n", 
		   level+1,' ', tel.Data() );
  }


  return;
}
