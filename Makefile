# ##################################################################
#
#	Makefile for the SIR library
# 	
#	Copyright (C) 2003-2018. All Rights Reserved 
#
#	Author: Ryan M. Lederman <lederman@gmail.com>
#	Version: 1.1.0
#
# ##################################################################

CC       = gcc
BUILDDIR = build
DOCSDIR = docs
INTERDIR = $(BUILDDIR)/obj
LIBDIR   = $(BUILDDIR)/lib

LIBS   = -pthread
CFLAGS = -Wpedantic -std=c99 -I.

TUS     = sir.c sirmutex.c
DEPS    = sir.h sirmutex.h

_OBJ = sirmutex.o sir.o
OBJ  = $(patsubst %,$(INTERDIR)/%,$(_OBJ))

# console debug app
_OBJ_DEBUG   = main.o $(_OBJ)
OBJ_DEBUG    = $(patsubst %.o, $(INTERDIR)/%.do, $(_OBJ_DEBUG))
OUT_DEBUG    = $(BUILDDIR)/sirdebug
CFLAGS_DEBUG = $(CFLAGS) -g -DDEBUG -DSIR_SELFLOG
DEBUGTU = $(TUS) main.c

# shared library
OBJ_SHARED    = $(patsubst %.o, $(INTERDIR)/%.lo, $(_OBJ))
OUT_SHARED	  = $(LIBDIR)/libsir.so
CFLAGS_SHARED = $(CFLAGS) -fPIC -O3

# static library
OBJ_STATIC    = $(OBJ_SHARED)
OUT_STATIC    = $(LIBDIR)/libsir.a
CFLAGS_STATIC = $(CFLAGS) -O3

# ##########
# targets
# ##########

$(DOCSDIR): prep
$(BUILDDIR): prep
$(INTERDIR) : $(BUILDDIR)
$(LIBDIR): $(BUILDDIR)

default: $(LIBDIR) $(INTERDIR) $(DOCSDIR)

$(INTERDIR)/%.do: %.c
	$(CC) -c -o $@ $< $(CFLAGS_DEBUG)

$(INTERDIR)/%.lo: %.c
	$(CC) -c -o $@ $< $(CFLAGS_SHARED)

all: debug static shared docs

prep:
	$(shell mkdir -p $(INTERDIR) >/dev/null && \
	        mkdir -p $(LIBDIR) >/dev/null && \
			mkdir $(DOCSDIR) >/dev/null)
	@echo directories prepared.

debug: $(OBJ_DEBUG) prep
	$(CC) -o $(OUT_DEBUG) $^ $(CFLAGS_DEBUG) $(LIBS)
	@echo built $(OUT_DEBUG) successfully.

shared: $(OBJ_SHARED) prep
	$(CC) -shared -o $(OUT_SHARED) $(OBJ_SHARED) $(CFLAGS_SHARED) $(LIBS)
	@echo built $(OUT_SHARED) successfully.

static: $(OBJ_SHARED) prep
#	$(CC) -o $(OUT_STATIC) $(OBJ_SHARED) $(CFLAGS_STATIC) $(LIBS)
	ar crf $(OUT_STATIC) $(OBJ_SHARED)
	@echo built $(OUT_STATIC) successfully.

docs: $(DOCSDIR)
	@doxygen
	@echo built documentation successfully.

.PHONY: clean

clean:
	$(shell rm -f $(BUILDDIR)/*.* >/dev/null && \
	        rm -f $(LIBDIR)/* >/dev/null && \
			rm -f $(INTERDIR)/* >/dev/null && \
			rm -f $(DOCSDIR)/* >/dev/null)
	@echo cleared directories.