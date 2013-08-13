# @(#) Makefile for sir library.


# This is for normal GCC stuff.
CC=gcc
DLFLAGS=-shared -fPIC -Wall -ansi
SLFLAGS=-Wall -ansi


# This is for the solaris compiler.
#CC=cc
#DLFLAGS=-G -Kpic -errwarn=%all -Xc
#SLFLAGS=-errwarn=%all -Xc


SRCS=sir.c
OBJS=sir.o


all:		static dynamic


static:		sir.c sir.h
		$(CC) $(SLFLAGS) -o sir.o -c sir.c
		ar -cr libsir.a sir.o

dynamic:	sir.c sir.h 
		$(CC) $(DLFLAGS) -o libsir.so.1.0.4 -c sir.c



install:	
		install -f /usr/lib -m 444 -u root -g bin -s libsir.so.1.0.4
		install -f /usr/lib -m 444 -u root -g bin -s libsir.a
		ln -s /usr/lib/libsir.so.1.0.4 /usr/lib/libsir.so.1
		ln -s /usr/lib/libsir.so.1.0.4 /usr/lib/libsir.so
		mkdir -f /usr/include/sir
		install -f /usr/include/sir -m 444 -u root -g bin -s sir.h
