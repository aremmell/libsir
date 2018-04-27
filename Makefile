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

GLOBALFLAGS=-Wpedantic -std=c99 -I.
SHAREDFLAGS=-shared -fPIC $(GLOBALFLAGS) -O3
STATICFLAGS=$(GLOBALFLAGS) -O3
DEBUGFLAGS=-g -DDEBUG $(GLOBALFLAGS)

TUS=sir.c
TESTTU=main.c
HEADERS=sir.h
OBJS=$(BUILDDIR)/sir.o
SRCFILES=$(TUS) $(HEADERS)

STATICOUT=$(BUILDDIR)/libsir.a
SHAREDOUT=$(BUILDDIR)/libsir.so
TESTOUT=$(BUILDDIR)/sirtest

all: static shared

prep:
	mkdir -p $(BUILDDIR)

static: prep $(SRCFILES)
	$(CC) $(STATICFLAGS) -o $(OBJS) -c $(TUS)
	ar -cr $(STATICOUT) $(OBJS)

shared: prep $(SRCFILES)
	$(CC) $(SHAREDFLAGS) -o $(SHAREDOUT) -c $(TUS)

test: $(TESTTU) $(SRCFILES)
	$(CC) $(DEBUGFLAGS) -o $(TESTOUT) $(TESTTU) $(TUS)

clean:
	rm -f $(STATICOUT) $(SHAREDOUT) $(TESTOUT) $(OBJS)
