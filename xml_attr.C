#include "xml_attr.h"


#include <stdio.h>

#include "TString.h" // good for xml
#include "unistd.h" //sleep; stFileInfo
#include <sys/stat.h> // stFileInfo
#include <string> // stFileInfo
#include <cstring>
#include <iostream>

#include <stdlib.h>     /* atof */

using namespace std;


//=============================================== Special: TokenGet

double TokenGet( const char* tok , const char* remote , char* result ){
  char str1[4096];
  strcpy( str1, remote );
  char *pch, *pch2;
  pch = strtok(str1," ,\n");
  int pos=0;
  while (pch != NULL) {
    //printf("tok  /%s/\n" , pch  );
    if (  strstr( pch, tok)!=NULL ){
      pch2= strstr( pch, tok) + strlen(tok);
      //printf("found  /%s/ and cut is /%s/\n" , pch , pch2 );
      strcpy( result, pch2  );
      return atof( pch2) ;
    }
    pos=pos+strlen(pch);
    pch = strtok( NULL , " ,\n");
  }
  strcpy( result, ""  );   //  strlen ==0
  return 0.0;
}// TokenGet returns rest of remote (blabla from file=blabla) + value



void TokenReplace( const char* tok , const char* newtok, const char* remote, char* remoteNEW ){

  char str1[4096];
  char strnew[4096];
  strcpy( str1, remote );
  char *pch;
  pch = strtok(str1," ,\n");
  int pos=0;
  while (pch != NULL) {
    //printf("tok  /%s/\n" , pch  );
    if (  strstr( pch, tok)!=NULL ){ // FOUND MATCH
      //printf("found  /%s/ and new is /%s/\n" , pch, newtok  );
      strcpy( strnew+ pos, newtok );
      pos=pos+strlen(newtok);
      strcpy( strnew+ pos, "\n" );
      pos++;
    }else{// NO MATCH
      strcpy( strnew+ pos, pch );
      pos=pos+strlen(pch);
      strcpy( strnew+ pos, "\n"  );
      pos++;
    }
    pch = strtok( NULL , " ,\n");
    //printf("\n=============\n%s\n------------/%d/\n",  strnew, pos );
  }
  //not neccessary strcpy( result, strnew  );   //  
  strcpy( remoteNEW,  strnew );
  return;
}// TokenReplace replcs whole new for a fragment (file=bla2 for file=)




//===================class declarations==========TSmallish_xml======

TSmallish_xml::TSmallish_xml(const char* filename){
  //  std::string addr[4];//={"telescopes", "t1", "V" , "addwith" };
  leveloffset=-1; // very upper level
  getattrlevel=3; // this is my attribute level ??
  //  DEBUG=0;
  FOUND=0;// not found.  1=found.  -1...error
  inside_UID=0;//  check that UID  

  if (  FileExists1k( filename ,1) ){ 
    printf("xml file of some size  exists <%s>\n", filename ); 
  }else{
    printf("ERROR  file  <%s> DOESNOT EXIST \n", filename ); 
    //    return 1;
  }
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

       child = xml->GetChild(node);// goes back up to root
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
//=======================================end of class definitions====










/**********************************************************************
 *   1kB +   file exists
 **********************************************************************/
bool FileExists1k(string strFilename, int minsize) { 
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







/**********************************************************************
 *   xml attr                   TEST
 **********************************************************************/
//---------------------------------------------------------
void xml_attr(){


  if (  FileExists1k("mut_queue.xml",1) ){  //minimum size 

    TSmallish_xml xml("mut_queue.xml");
    xml.DisplayTele( xml.mainnode, 0, "pusher","pusher","file" );
    printf( "input file ==\"%s\"\n", xml.output  );

    xml.DisplayTele( xml.mainnode, 0, "poper","poper","file" );
    printf( "output file==\"%s\"\n", xml.output  );

  }//  XML FILE EXISTS------------------------------
  return;
}//xml_attr
