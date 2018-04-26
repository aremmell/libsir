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

GLOBALFLAGS=-Wpedantic -std=c99 -O3
SHAREDFLAGS=-shared -fPIC $(GLOBALFLAGS)
STATICFLAGS=$(GLOBALFLAGS)

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
	$(CC) $(STATICFLAGS) -o $(OBJS) -c $(TUS) -I.
	ar -cr $(STATICOUT) $(OBJS)

shared: prep $(SRCFILES)
	$(CC) $(SHAREDFLAGS) -o $(SHAREDOUT) -c $(TUS) -I.

test: $(TESTTU) $(SRCFILES)
	$(CC) -g -o $(TESTOUT) $(TESTTU) $(TUS) -I.

clean:
	rm -f $(STATICOUT) $(SHAREDOUT) $(TESTOUT) $(OBJS)
