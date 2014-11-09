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
plugcpp := $(wildcard plug_*.cpp)
plugsos := $(patsubst %.cpp, %.so, $(plugcpp) )


.PHONY:	all clean distclean

all: 	$(plugsos)
	@echo " A TRICK with compile .so here -  I have included the  ..  directory ";\
	esc=`echo "\033"`;cc_red="$${esc}[0;31m";cc_green="$${esc}[0;32m";cc_mag="$${esc}[0;35m";cc_normal=`echo "$${esc}[m\017"`;\
	echo $${cc_mag};ls -1 *.so;echo $${cc_normal}


%.so: %.cpp
	@echo ... /$</ file searched for the int* PLUG\(\) function;\
	cat $< | grep PLUG\(int >/dev/null;\
	if [ $$? = 0 ]; then \
	echo COMPILING $<;\
	$(ROOTCC)  $(OBJS_LIBS) -fPIC -I.. -shared -o $@ $<  $(LDFLAGS) ;\
	fi;\

clean:
	@esc=`echo "\033"`;cc_red="$${esc}[0;31m";cc_green="$${esc}[0;32m";cc_normal=`echo "$${esc}[m\017"`;\
	echo $${cc_red};ls -1 *.so;echo $${cc_normal};\
	$(RM) $(plugsos)

distclean: clean
