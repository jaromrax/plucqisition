ifneq (,)
    This makefile requires GNU Make.
    endif

PROGRAM = acq_rez
C_FILES_LIBS = xml_attr.C log_term.C mut_queue.C ZH_data.C
C_FILES =      xml_attr.C log_term.C mut_queue.C ZH_data.C acq_core.C
OBJS_LIBS := $(C_FILES_LIBS:.C=.o)
OBJS := $(C_FILES:.C=.o)
#this is extra to save one compilation under root .L +
#OBJS_SO := $(C_FILES:.C=.so)
CC = echo
CFLAGS = -Wall 
LDFLAGS =
ROOTCC=`root-config --cxx --cflags` -fPIC 
LDFLAGS= -ldl -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`


#  root -n -b -q  compile.C
rs   := $(shell echo $$ROOTSYS)
date := $(shell date +%Y%m%d_%H%M%S)
mpath := $(shell cat ~/.rootrc | grep -v -e "^\#" | grep Unix | grep MacroPath | awk '{print $$2}')
mpath2 := $(shell echo  $(mpath) | sed  's/:/ /g')
#########this is from plug-Makefile
plugcpp := $(wildcard plug_*.cpp)
plugsos := $(patsubst %.cpp, %.so, $(plugcpp) )


all: $(PROGRAM) 

$(PROGRAM): $(OBJS) $(plugsos)
	@echo .;\
	esc=`echo "\033"`;cc_red="$${esc}[0;31m";cc_green="$${esc}[0;32m";cc_mag="$${esc}[0;35m";cc_normal=`echo "$${esc}[m\017"`;\
	echo $${cc_green};ls -1 *.o;echo $${cc_mag};ls -1 *.so;echo $${cc_normal}
	@echo USAGE:      i need .o files to build .so and  .so themselves for root;\
	echo "make      ... makes \*.o files and \*.so plugins";\
	echo "make plug ... makes \*.so plugins";\
	echo "make root ... compilation under root"
#	echo ROOTCC_pg == $(ROOTCC);\
#$(ROOTCC)  $(OBJS) $(LDFLAGS) -o $(PROGRAM)


#--------------I need .o   for later  plug____.so  build, and .so is used by root CINT
%.o: %.C
	$(ROOTCC) $(CFLAGS)  -c $< -o $@

#%.so: %.C
#	@echo =================== preparing .so =====================
#	$(ROOTCC) $(CFLAGS)  -fPIC -shared  -c $< -o $@

clean:
	@esc=`echo "\033"`;cc_red="$${esc}[0;31m";cc_green="$${esc}[0;32m";cc_normal=`echo "$${esc}[m\017"`;\
	echo $${cc_red};ls -1 *.o;echo .;ls -1 *.so;echo $${cc_normal};\
	rm -f  *.o  *.pcm *.d  *.so 

cleanso:
	rm -f  $(plugsos) 

root:
	@echo Ahoj, zkuime root;\
	echo ./compile_root  xml_attr.C ;\
	echo  ./compile_root LogTerm.C;\
	echo   ./compile_root mut_queue.C;\
	echo  ./compile_root acq_core.C;\
	./compile_root $(C_FILES)


replug: cleanso  plug

plug: plugins
plugins:  $(plugsos)

%.so: %.cpp $(C_FILES_LIBS) 
	@echo ... /$</ file searched for the int* PLUG\(\) function;\
	cat $< | grep PLUG\(int >/dev/null;\
	if [ $$? = 0 ]; then \
	echo COMPILING $<;\
	$(ROOTCC)  $(OBJS_LIBS) -fPIC -shared -o $@ $<  $(LDFLAGS) ;\
	fi;\

