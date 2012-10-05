#Makefile for c04   phidgetbuster  basic
#
#  not working yet
#
CC = gcc
LIBS = 
CFLAGS = -g -O -Wall -lpthread  
LDFLAGS = -g

LIBSRC = *.c *.h

all:  gregory


gregory:  $(LIBSRC)
	$(CC) $(CFLAGS) $(LDFLAGS)  gregory.c -o gregory

clean:
	rm -f $(LIBOBJ) gregory
