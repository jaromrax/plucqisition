########################################################################
# 
########################################################################
OUTDIR  =       .
EXE	=	$(OUTDIR)/zmq_pub
########################################################################
CC	=	gcc
COPTS	=	-fPIC -DLINUX -Wall -ggdb -lpthread -fpermissive
#COPTS	=	-g -fPIC -DLINUX -Wall 
FLAGS	=	-Wall -s 
#FLAGS	=	-Wall
DEPLIBS	=       -lCAENDigitizer -pthread -lCAENVME  -lc -lm -lzmq
LIBS	=	
INCLUDEDIR =	-Iinclude
OBJS	=	zmq_publisher.o 


##################### from root ########################
# --new first ; to use  TMapFile
ROOTCC=`root-config  --cxx --cflags` -fPIC 
# 64bit system needs  -lstdc++    FLAG; else 
#/usr/bin/ld: xml_attr.o: undefined reference to symbol '_ZNKSs5c_strEv@@GLIBCXX_3.4'
#/usr/lib/x86_64-linux-gnu/libstdc++.so.6: error adding symbols: DSO missing from command line


LDFLAGS= `root-config --new --glibs`  -lstdc++    -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO 

#########################################################################
all	:	$(EXE) 

test	:	
		time dd if=/dev/urandom of=test.bin count=1024 bs=1024; rm tcpy.bin; ./zmq_pub client& time ./zmq_pub server; ls -l *bin;sleep 1;killall zmq_pub; diff tcpy.bin test.bin -s


clean	:
		/bin/rm -f $(OBJS) $(EXE) 

$(EXE)	:	$(OBJS) 
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) -lNew $(DEPLIBS) $(LDFLAGS) 
#		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)
		@/bin/ls -l $(EXE)

#nejaka finta pri zmene Makefilu nebo co
#$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(ROOTCC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<
#		g++ -c  -Iinclude  keyboard.c
#		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

keyboard.o:	keyboard.c
		g++ -c  -Iinclude  keyboard.c
