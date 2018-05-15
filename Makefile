#####################################################
#                                                   #
#              libsir make recipes                  #
#                                                   #
#       https://github.com/ryanlederman/sir         #
#                                                   #
#                                                   #
# Note:                                             #
#  Set the environment variable 'SIR_DEBUG' to 1    #
#  in order to cause the compiler flags to include  #
#  debug flags.                                     #
#                                                   #
#####################################################

CC         = gcc
CXX		   = g++
BUILDDIR   = build
DOCSDIR    = docs
TESTSDIR   = tests
EXAMPLEDIR = example
CXXDIR     = cppexample
INTERDIR   = $(BUILDDIR)/obj
LIBDIR     = $(BUILDDIR)/lib
INSTALLDIR = /usr/local/lib
INSTALLINC = /usr/local/include

LIBS         = -pthread

ifeq ($(SIR_DEBUG),1)
CFLAGS   = -Wpedantic -std=c11 -I. -g -DNDEBUG -fPIC -DSIR_SELFLOG
CXXFLAGS = -Wpedantic -std=c++14 -I. -g -DNDEBUG -fPIC -DSIR_SELFLOG
@echo warning: SIR_DEBUG=1, using -g.
else
CFLAGS   = -Wpedantic -std=c11 -I. -DNDEBUG -fPIC -O3
CXXFLAGS = -Wpedantic -std=c++14 -I. -DNDEBUG -fPIC -O3
endif

ifeq ($(OS),Windows_NT)
CFLAGS += -D_WIN32
CXXFLAGS += -D_WIN32
endif

# link with static library, not shared
LDFLAGS = $(LIBS) -L$(LIBDIR) -lsir_s

# translation units and headers
TUS = sir.c sirmutex.c sirinternal.c sirfilecache.c sirconsole.c sirtextstyle.c sirerrors.c sirhelpers.c
DEPS = sir.h sirmutex.h sirconfig.h sirinternal.h sirhelpers.h sirplatform.h sirfilecache.h sirtypes.h \
	   sirconsole.h sirtextstyle.h sirerrors.h

# intermediate files
_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTERDIR)/%, $(_OBJ))

# shared library
OBJ_SHARED     = $(patsubst %.o, $(INTERDIR)/%.lo, $(_OBJ))
OUT_SHARED	   = $(LIBDIR)/libsir.so
LDFLAGS_SHARED = $(LIBS)

# static library
OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC     = $(LIBDIR)/libsir_s.a

# console example
_OBJ_EXAMPLE    = example.o
OBJ_EXAMPLE     = $(patsubst %.o, $(INTERDIR)/%.eo, $(_OBJ_EXAMPLE))
OUT_EXAMPLE     = $(BUILDDIR)/sirexample
EXAMPLETU       = $(TESTSDIR)/example.c

# c++ example
_OBJ_CXXEXAMPLE  = example.o
OBJ_CXXEXAMPLE   = $(patsubst %.o, $(INTERDIR)/%.o++, $(_OBJ_EXAMPLE))
OUT_CXXEXAMPLE   = $(BUILDDIR)/sircppexample
CXXEXAMPLETU     = $(CXXDIR)/example.cc

# console test rig
_OBJ_TESTS    = tests.o
OBJ_TESTS     = $(patsubst %.o, $(INTERDIR)/%.to, $(_OBJ_TESTS))
OUT_TESTS     = $(BUILDDIR)/sirtests
TESTSTU       = $(TESTSDIR)/tests.c

# ##########
# targets
# ##########

$(BUILDDIR): prep
$(INTERDIR) : $(BUILDDIR)
$(LIBDIR): $(BUILDDIR)

$(OBJ_EXAMPLE): $(INTERDIR)
$(OBJ_CXXEXAMPLE): $(INTERDIR)
$(OBJ_SHARED): $(INTERDIR) $(LIBDIR)
$(OBJ_TESTS): $(OBJ_SHARED)

$(INTERDIR)/%.eo: $(EXAMPLEDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(INTERDIR)/%.o++: $(CXXDIR)/%.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(INTERDIR)/%.to: $(TESTSDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)	

$(INTERDIR)/%.lo: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

default: example

all: shared static example cppexample tests

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
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS) $(LDFLAGS_SHARED)
	@echo built $(OUT_SHARED) successfully.

static: shared
	ar cr $(OUT_STATIC) $(OBJ_SHARED)
	@echo built $(OUT_STATIC) successfully.

example: static $(OBJ_EXAMPLE)
	$(CC) -o $(OUT_EXAMPLE) $(OUT_STATIC) $(OBJ_EXAMPLE) $(CFLAGS) $(LDFLAGS)
	@echo built $(OUT_EXAMPLE) successfully.

cppexample: static $(OBJ_CXXEXAMPLE)
	$(CXX) -o $(OUT_CXXEXAMPLE) $(OUT_STATIC) $(OBJ_CXXEXAMPLE) $(CXXFLAGS) $(LDFLAGS)
	@echo build $(OUT_CXXEXAMPLE) successfully.

tests: static $(OBJ_TESTS)
	$(CC) -o $(OUT_TESTS) $(OUT_STATIC) $(OBJ_TESTS) $(CFLAGS) $(LDFLAGS)
	echo built $(OUT_TESTS) successfully.

docs: static
	@doxygen Doxyfile
	@echo built documentation successfully.

install: shared
ifeq ($(OS),Windows_NT)
	@echo no install support for windows.
else
	@echo copying $(OUT_SHARED) to $(INSTALLDIR) and headers to $(INSTALLINC)...
	$(shell cp -f $(OUT_SHARED) "$(INSTALLDIR)/" && \
	        cp -f *.h *.hh "$(INSTALLINC)/")
	@echo installed libsir successfully.
endif

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