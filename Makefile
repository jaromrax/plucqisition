#  root -n -b -q  compile.C
rs   := $(shell echo $$ROOTSYS)
date := $(shell date +%Y%m%d_%H%M%S)
mpath := $(shell cat ~/.rootrc | grep -v -e "^\#" | grep Unix | grep MacroPath | awk '{print $$2}')
mpath2 := $(shell echo  $(mpath) | sed  's/:/ /g')



all: mut plugins
	@echo "make:";\
	echo "      this help";\
	echo "make plugins:";\
	echo "            compiles plugins";\
	echo "make mut";\
	echo "            compiles main code using root -n batch";\
	echo "make install";\
	echo "            coppies the .so file to MACRO path";

mut:mut_queue.h mut_queue.C cuts_manip.h nano_acquis_pureconvert.C xml_attr.c xml_attr.h logterm.C
	root -n -b -q  compile.C  ; echo ECHO $?


install: mut_queue_C.so nano_acquis_pureconvert_C.so
	@echo I TRY TO FIND ROOTSYS AND MACRO;\
	echo MACRO AT  $(mpath2);\
	for i in $(mpath2); do \
	if [ -d $$i ]; then \
	 	if [ $$i = "." ]; then echo "  ";else \
		TARG=$$i;\
	 	echo $$i "  " IS OK; \
	 	break;\
	 	fi \
	fi; \
	done;\
	echo "TESTING <$$TARG>";\
	if [ "$$TARG" != "" ];then \
		echo cp mut_queue_C.so nano_acquis_pureconvert_C.so $$TARG/ ;\
		cp mut_queue_C.so nano_acquis_pureconvert_C.so $$TARG/ ;\
		ls -l $$TARG/*.so;\
	else\
		echo ERROR; echo "  " NO PATH FOR MACRO. CREATE SOME IN ~/.rootrc;\
	fi;


plugins: plug_push.so plug_pop.so 


plug_push.so: plug_push.cpp mut_queue.h cuts_manip.h nano_acquis_pureconvert.C xml_attr.c xml_attr.h logterm.C
	`root-config --cxx --cflags` -fPIC -shared -o plug_push.so plug_push.cpp     -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`


plug_pop.so: plug_pop.cpp mut_queue.h cuts_manip.h nano_acquis_pureconvert.C xml_attr.c xml_attr.h logterm.C
	`root-config --cxx --cflags` -fPIC -shared -o plug_pop.so plug_pop.cpp     -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`


###not here in main;analyze is always down  plug_analyze.so: plug_analyze.cpp mut_queue.h cuts_manip.h nano_acquis_pureconvert.C xml_attr.c xml_attr.h logterm.C
####	`root-config --cxx --cflags` -fPIC -shared -o plug_analyze.so plug_analyze.cpp -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO `root-config --glibs`


clean:
	  rm plug_pop.so plug_push.so

##	g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o
##	g++ -fPIC -c plug_analyze.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_analyze.so plug_analyze.o
