####################################################
#                                                  #
#              libsir make recipes                 #
#                                                  #
#       https://github.com/ryanlederman/sir        #
#                                                  #
####################################################

CC         = gcc
BUILDDIR   = build
DOCSDIR    = docs
TESTSDIR   = tests
EXAMPLEDIR = example
INTERDIR   = $(BUILDDIR)/obj
LIBDIR     = $(BUILDDIR)/lib

LIBS         = -pthread
CFLAGS       = -Wpedantic -std=c11 -I.
DEBUGCFLAGS  = $(CFLAGS) -g -DNDEBUG -fPIC -DSIR_SELFLOG
NDEBUGCFLAGS = $(CFLAGS) -DNDEBUG -fPIC -O3

ifeq ($(OS),Windows_NT)
CFLAGS += -D_WIN32
endif

TUS = sir.c sirmutex.c sirinternal.c sirfilecache.c sirconsole.c sirtextstyle.c sirerrors.c sirhelpers.c
DEPS = sir.h sirmutex.h sirconfig.h sirinternal.h sirhelpers.h sirplatform.h sirfilecache.h sirtypes.h \
	   sirconsole.h sirtextstyle.h sirerrors.h

_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTERDIR)/%, $(_OBJ))

# shared library
OBJ_SHARED     = $(patsubst %.o, $(INTERDIR)/%.lo, $(_OBJ))
OUT_SHARED	   = $(LIBDIR)/libsir.so
CFLAGS_SHARED  = $(NDEBUGCFLAGS)
LDFLAGS_SHARED = $(LIBS)

# static library
OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC     = $(LIBDIR)/libsir.a

# console example
_OBJ_EXAMPLE    = example.o
OBJ_EXAMPLE     = $(patsubst %.o, $(INTERDIR)/%.eo, $(_OBJ_EXAMPLE))
OUT_EXAMPLE     = $(BUILDDIR)/sirexample
CFLAGS_EXAMPLE  = $(DEBUGCFLAGS)
LDFLAGS_EXAMPLE = $(LIBS) -L$(LIBDIR) -l:libsir.a
EXAMPLETU       = $(TESTSDIR)/example.c

# console test rig
_OBJ_TESTS    = tests.o
OBJ_TESTS     = $(patsubst %.o, $(INTERDIR)/%.to, $(_OBJ_TESTS))
OUT_TESTS     = $(BUILDDIR)/sirtests
CFLAGS_TESTS  = $(NDEBUGCFLAGS)
LDFLAGS_TESTS = $(LDFLAGS_EXAMPLE)
TESTSTU       = $(TESTSDIR)/tests.c

# ##########
# targets
# ##########

$(BUILDDIR): prep
$(INTERDIR) : $(BUILDDIR)
$(LIBDIR): $(BUILDDIR)

$(OBJ_EXAMPLE): $(INTERDIR)
$(OBJ_SHARED): $(INTERDIR) $(LIBDIR)
$(OBJ_TESTS): $(OBJ_SHARED)

$(INTERDIR)/%.eo: $(EXAMPLEDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS_EXAMPLE)

$(INTERDIR)/%.to: $(TESTSDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS_TESTS)	

$(INTERDIR)/%.lo: %.c
	$(CC) -c -o $@ $< $(CFLAGS_SHARED)

default: example

all: shared static example tests

# thanks to the windows folks
prep:
ifeq ($(OS),Windows_NT)
	$(shell if not exist "$(BUILDDIR)\NUL" mkdir "$(BUILDDIR)" && \
		    if not exist "$(INTERDIR)\NUL" mkdir "$(INTERDIR)" && \
			if not exist "$(LIBDIR)\NUL" mkdir "$(LIBDIR)")
else
	$(shell mkdir -p $(BUILDDIR) && \
			mkdir -p $(INTERDIR) && \
	        mkdir -p $(LIBDIR))
endif
	@echo directories prepared.

shared: $(OBJ_SHARED)
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS_SHARED) $(LDFLAGS_SHARED)
	@echo built $(OUT_SHARED) successfully.

static: shared
	ar cr $(OUT_STATIC) $(OBJ_SHARED)
	@echo built $(OUT_STATIC) successfully.

example: static $(OBJ_EXAMPLE)
	$(CC) -o $(OUT_EXAMPLE) $(OUT_STATIC) $(OBJ_EXAMPLE) $(CFLAGS_EXAMPLE) $(LDFLAGS_EXAMPLE)
	@echo built $(OUT_EXAMPLE) successfully.

tests: static $(OBJ_TESTS)
	$(CC) -o $(OUT_TESTS) $(OUT_STATIC) $(OBJ_TESTS) $(CFLAGS_TESTS) $(LDFLAGS_TESTS)
	echo built $(OUT_TESTS) successfully.

docs: static
	@doxygen Doxyfile
	@echo built documentation successfully.

.PHONY: clean

clean:
ifeq ($(OS),Windows_NT)
	@echo using del /F /Q...
	$(shell del /F /Q "$(BUILDDIR)\*.*" && \
		    del /F /Q "$(INTERDIR)\*.*" && \
			del /F /Q "$(LIBDIR)\*.*" && \
			del /F /Q "*.log")
else
	@echo using rm -f...
	$(shell rm -f $(BUILDDIR)/* >/dev/null 2>&1 && \
	        rm -f $(LIBDIR)/* >/dev/null 2>&1 && \
			rm -f $(INTERDIR)/* >/dev/null 2>&1 && \
			rm -f *.log >/dev/null 2>&1)
endif
	@echo cleaned successfully.