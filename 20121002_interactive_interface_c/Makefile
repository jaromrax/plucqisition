#Makefile for c04   phidgetbuster  basic
#
#  not working yet
#
CC = gcc
LIBS = 
# COMPILE PART
CFLAGS = -g -O -Wall -lpthread  
# I DIVIDED -- and LINK PART:
LDFLAGS = -g -pthread

# LIBSRC = *.c *.h

C_FILES =     gregory.c
C_FILES_LIBS =  arguments.c  commands.c 
#  I have to put   c  or C ... depends on the dest. to compile
OBJS_LIBS := $(C_FILES_LIBS:.c=.o)
OBJS := $(C_FILES:.c=.o)

all:  gregory

#	@echo  what this does === -o $@ "\n"  \
#	and params are $< "\n"   \
#	and params up are $^;  \
#	echo  OBJS ==  $(OBJS);   \
#	echo  OBJS_LIBS == $(OBJS_LIBS);   \
#	echo making really: 	$(CC)   $(C_FILES) $(LDFLAGS) -o $@  $^ ;   


# u cannot put $^ and   $(C_FILES)  at once....$(C_FILES) is twice input
gregory: $(OBJS_LIBS)  $(C_FILES) $(C_FILES_LIBS)
	$(CC)   $(C_FILES) $(LDFLAGS) -o $@ $(OBJS_LIBS)  ;   \
	ls -ltrh | tail -3

# here I create  objects for the library...OBJS_LIBS go here
# -c  means compile only
.c.o:
	$(CC) $(CFLAGS) -c  $< -o $@
clean:
	rm -f $(OBJS_LIBS) $(OBJS)  gregory
