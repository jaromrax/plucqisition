#  THIS IS ANALYZE   Makefile ONLY
#
#
#  root -n -b -q  compile.C

C_FILES_LIBS = ../xml_attr.C ../log_term.C ../mut_queue.C 
OBJS_LIBS := $(C_FILES_LIBS:.C=.o)
ROOTCC=`root-config --cxx --cflags` -fPIC 
LDFLAGS= -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`

rs   := $(shell echo $$ROOTSYS)
date := $(shell date +%Y%m%d_%H%M%S)
mpath := $(shell cat ~/.rootrc | grep -v -e "^\#" | grep Unix | grep MacroPath | awk '{print $$2}')
mpath2 := $(shell echo  $(mpath) | sed  's/:/ /g')



all: 	plug_analyze.so
	@echo "make:";\
	echo "      this help";\
	echo " TRICK -  I incluce  ..  directory "

plug: plugins

plugins: plug_analyze.so

plug_analyze.so: plug_analyze.cpp plug_analyze_actions.cpp plug_analyze_definitions.cpp
	$(ROOTCC)  $(OBJS_LIBS) -fPIC -I.. -shared -o plug_analyze.so plug_analyze.cpp  $(LDFLAGS)


clean:
	  rm plug_analyze.so 
