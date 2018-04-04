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


LDFLAGS= `root-config --new --glibs`  -lstdc++    -lHist -lNet -lCore -lRIO -lGpad -lMathCore -lPhysics -lTree -lThread -lXMLIO 

#########################################################################
all	:	$(EXE) 

test	:	
		time dd if=/dev/urandom of=test.bin count=10240 bs=1024; rm tcpy.bin; ./zmq_pub client& time ./zmq_pub server 10000; ls -l *bin;sleep 1;killall zmq_pub; diff tcpy.bin test.bin -s


clean	:
		/bin/rm -f $(OBJS) $(EXE) 

$(EXE)	:	$(OBJS) 
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) -lNew $(DEPLIBS) $(LDFLAGS) 
		@/bin/ls -l $(EXE)


%.o	:	%.C
		$(ROOTCC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

