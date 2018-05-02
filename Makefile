# ##################################################
#                                                  #
#	Makefile for the SIR library                   #
#                                                  #
#	Copyright (C) 2003-2018. All Rights Reserved   # 
#	Author: Ryan M. Lederman <lederman@gmail.com>  #
#	Version: 1.1.0                                 #
#                                                  #
# ##################################################

CC       = gcc
BUILDDIR = build
DOCSDIR = docs
INTERDIR = $(BUILDDIR)/obj
LIBDIR   = $(BUILDDIR)/lib

LIBS   = -pthread
CFLAGS = -Wpedantic -std=c11 -I.

ifeq ($(OS),Windows_NT)
CFLAGS += -D_WIN32
endif

TUS     = sir.c sirmutex.c sirinternal.c sirfilecache.c sirconsole.c \
		  sirtextstyle.c
DEPS    = sir.h sirmutex.h sirconfig.h sirinternal.h sirmacros.h \
		  sirplatform.h sirfilecache.h sirtypes.h sirconsole.h \
		  sirtextstyle.h

_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTERDIR)/%, $(_OBJ))

# console debug app
_OBJ_DEBUG   = main.o tests.o $(_OBJ)
OBJ_DEBUG    = $(patsubst %.o, $(INTERDIR)/%.do, $(_OBJ_DEBUG))
OUT_DEBUG    = $(BUILDDIR)/sirdebug
CFLAGS_DEBUG = $(CFLAGS) -g -DDEBUG -DSIR_SELFLOG
DEBUGTU = $(TUS) main.c tests.c

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

$(BUILDDIR): prep
$(INTERDIR) : $(BUILDDIR)
$(LIBDIR): $(BUILDDIR)

$(OBJ_DEBUG): $(INTERDIR)
$(OBJ_SHARED): $(LIBDIR)

$(INTERDIR)/%.do: %.c
	$(CC) -c -o $@ $< $(CFLAGS_DEBUG)

$(INTERDIR)/%.lo: %.c
	$(CC) -c -o $@ $< $(CFLAGS_SHARED)

default: debug

all: debug static shared docs

# thanks to the windows folks
prep:
ifeq ($(OS),Windows_NT)
	$(shell if not exist "$(BUILDDIR)\NUL" mkdir "$(BUILDDIR)")
	$(shell if not exist "$(INTERDIR)\NUL" mkdir "$(INTERDIR)")
	$(shell if not exist "$(LIBDIR)\NUL" mkdir "$(LIBDIR)")
	$(shell if not exist "$(DOCSDIR)\NUL" mkdir "$(DOCSDIR)")
	@echo directories prepared.	
else
	$(shell mkdir -p $(BUILDDIR) && \
			mkdir -p $(INTERDIR) && \
	        mkdir -p $(LIBDIR) && \
			mkdir -p$(DOCSDIR))
	@echo directories prepared.
endif

debug: $(OBJ_DEBUG)
	$(CC) -o $(OUT_DEBUG) $^ $(CFLAGS_DEBUG) $(LIBS)
	@echo built $(OUT_DEBUG) successfully.

shared: $(OBJ_SHARED)
	$(CC) -shared -o $(OUT_SHARED) $(OBJ_SHARED) $(CFLAGS_SHARED) $(LIBS)
	@echo built $(OUT_SHARED) successfully.

static: shared
	ar crf $(OUT_STATIC) $(OBJ_SHARED)
	@echo built $(OUT_STATIC) successfully.

docs: static
	@doxygen $(DOCSDIR)/Doxyfile
	@echo built documentation successfully.

.PHONY: clean

clean:
	$(shell rm -f $(BUILDDIR)/*.* >/dev/null && \
	        rm -f $(LIBDIR)/* >/dev/null && \
			rm -f $(INTERDIR)/* >/dev/null)
	@echo cleared directories.