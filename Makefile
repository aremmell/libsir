#
# Makefile for the SIR library.
# Copyright (C) 2003-2018 Ryan M. Lederman <lederman@gmail.com>
#
#  Last modified:
#    April 25, 2018
#
#  Version: 1.1.0
#

CC=gcc
BUILDDIR=build

LIBS=
GLOBALFLAGS=-Wpedantic -std=c99 -I.
SHAREDFLAGS=$(GLOBALFLAGS) -shared -fPIC -O3
STATICFLAGS=$(GLOBALFLAGS) -O3
DEBUGFLAGS=$(GLOBALFLAGS) -g -DDEBUG 
TESTFLAGS=$(GLOBALFLAGS)-DNDEBUG -DSIR_SELFLOG -O3

TUS=sir.c
DEBUGTESTTU=main.c
HEADERS=sir.h
OBJS=$(BUILDDIR)/sir.o
SRCFILES=$(TUS) $(HEADERS)

STATICOUT=$(BUILDDIR)/libsir.a
SHAREDOUT=$(BUILDDIR)/libsir.so
DEBUGOUT=$(BUILDDIR)/sirdebug
TESTOUT=$(BUILDDIR)/sirtest

all: static shared

prep:
	mkdir -p $(BUILDDIR)

static: prep $(SRCFILES)
	$(CC) $(STATICFLAGS) -o $(OBJS) -c $(TUS)
	ar -cr $(STATICOUT) $(OBJS)

shared: prep $(SRCFILES)
	$(CC) $(SHAREDFLAGS) -o $(SHAREDOUT) -c $(TUS)

debug: $(DEBUGTESTTU) $(SRCFILES)
	$(CC) $(DEBUGFLAGS) -o $(DEBUGOUT) $(DEBUGTESTTU) $(TUS) $(LIBS)

test: $(DEBUGTESTTU) $(SRCFILES)
	$(CC) $(TESTFLAGS) -o $(TESTOUT) $(DEBUGTESTTU) $(TUS)	

clean:
	rm -f $(STATICOUT) $(SHAREDOUT) $(DEBUGOUT) $(TESTOUT) $(OBJS)
