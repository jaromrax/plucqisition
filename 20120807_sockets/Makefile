#  THIS IS ANALYZE   Makefile ONLY
#
#
#  root -n -b -q  compile.C
rs   := $(shell echo $$ROOTSYS)
date := $(shell date +%Y%m%d_%H%M%S)
mpath := $(shell cat ~/.rootrc | grep -v -e "^\#" | grep Unix | grep MacroPath | awk '{print $$2}')
mpath2 := $(shell echo  $(mpath) | sed  's/:/ /g')



all: plug_analyze.so
	@echo "make:";\
	echo "   the trick is - i include the .. directory:  -I.. "


plugins: plug_analyze.so

plug_analyze.so: plug_analyze.cpp 
	`root-config --cxx --cflags` -fPIC -shared -o plug_analyze.so plug_analyze.cpp -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs` -I..


clean:
	  rm plug_analyze.so 
