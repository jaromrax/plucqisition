ifneq (,)
    This makefile requires GNU Make.
    endif

PROGRAM = acq_rez
C_FILES_LIBS = xml_attr.C log_term.C mut_queue.C 
C_FILES =      xml_attr.C log_term.C mut_queue.C acq_core.C
OBJS_LIBS := $(C_FILES_LIBS:.C=.o)
OBJS := $(C_FILES:.C=.o)
###OBJS := $(patsubst $(C_FILES))
CC = echo
CFLAGS = -Wall 
LDFLAGS =	
ROOTCC=`root-config --cxx --cflags` -fPIC 
LDFLAGS= -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`
DICT = plug_push_dict.cxx



#  root -n -b -q  compile.C
rs   := $(shell echo $$ROOTSYS)
date := $(shell date +%Y%m%d_%H%M%S)
mpath := $(shell cat ~/.rootrc | grep -v -e "^\#" | grep Unix | grep MacroPath | awk '{print $$2}')
mpath2 := $(shell echo  $(mpath) | sed  's/:/ /g')


all: $(PROGRAM) $(DICT)

$(PROGRAM): $(OBJS)
	@echo USAGE:;\
	echo make ...      makes   \*.o  files;\
	echo make plug ... makes \*.so plugins;\
	echo make root ... compilation under root
#	echo ROOTCC_pg == $(ROOTCC);\
#$(ROOTCC)  $(OBJS) $(LDFLAGS) -o $(PROGRAM)


%.o: %.C
	echo ROOTCC_opc == $(ROOTCC);\
	$(ROOTCC) $(CFLAGS) -c $< -o $@

%: %.C
	echo ROOTCC_pc == $(ROOTCC);\
	$(ROOTCC) $(CFLAGS) -o $@ $<

clean:
	rm -f  *.o  *.pcm *.d  *.so 

cleanso:
	rm -f   plug_push.so plug_pop.so 

test: 
	echo AHOJ---------------------------;\
	echo rootcc== $(ROOTCC) ;\
	echo OBJS=$(OBJS);\
	echo C_FILES=$(C_FILES);\
	echo KONEC----------------------------

root:
	@echo Ahoj, zkuime root;\
	echo ./compile_root  xml_attr.C ;\
	echo  ./compile_root LogTerm.C;\
	echo   ./compile_root mut_queue.C;\
	echo  ./compile_root acq_core.C;\
	./compile_root $(C_FILES)


replug: cleanso  plug


plug: plugins
plugins:  plug_push.so plug_pop.so


plug_push.so: $(DICT) plug_push.cpp $(C_FILES_LIBS) 
	$(ROOTCC)  $(OBJS_LIBS) -fPIC -shared -o plug_push.so  $(DICT)   $(LDFLAGS)
#	$(ROOTCC)  $(OBJS_LIBS) -fPIC -shared -o plug_push.so plug_push.cpp $(DICT)  $(LDFLAGS)




plug_pop.so: plug_pop.cpp $(C_FILES_LIBS)
	$(ROOTCC)  $(OBJS_LIBS) -fPIC -shared -o plug_pop.so plug_pop.cpp    $(LDFLAGS)



#  DICT I need for plug_push.cpp 

$(DICT): 
	rootcint -f $(DICT) -c plug_push.cpp  LinkDef.h
#	@echo I have no idea how to really create it for *.so	
