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

BUILDDIR   = build
DOCSDIR    = docs
TESTSDIR   = tests
EXAMPLEDIR = example
INTERDIR   = $(BUILDDIR)/obj
LIBDIR     = $(BUILDDIR)/lib

ifeq ($(INSTALLDIR),)
	INSTALLDIR = /usr/local/lib
endif
ifeq ($(INSTALLINC),)
	INSTALLINC = /usr/local/include
endif

LIBS = -pthread

ifeq ($(SIR_DEBUG),1)
	CFLAGS   = -Wpedantic -std=c11 -I. -g -O0 -DNDEBUG -fPIC -DSIR_SELFLOG
	@echo warning: SIR_DEBUG=1, using -g.
else
	CFLAGS   = -Wpedantic -std=c11 -I. -DNDEBUG -fPIC -O3
endif

ifeq ($(OS),Windows_NT)
	CFLAGS += -D_WIN32
endif

# link with static library, not shared
LDFLAGS = $(LIBS) -L$(LIBDIR) -lsir_s

# translation units
TUS := $(wildcard ./*.c)

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

# console test rig
_OBJ_TESTS    = tests.o
OBJ_TESTS     = $(patsubst %.o, $(INTERDIR)/%.to, $(_OBJ_TESTS))
OUT_TESTS     = $(BUILDDIR)/sirtests
TESTSTU       = $(TESTSDIR)/tests.c

# ##########
# targets
# ##########

all: shared static example tests

-include $(INTERDIR)/*.d

$(BUILDDIR): prep
$(INTERDIR) : $(BUILDDIR)
$(LIBDIR): $(BUILDDIR)

$(OBJ_EXAMPLE): $(INTERDIR)
$(OBJ_SHARED): $(INTERDIR) $(LIBDIR)
$(OBJ_TESTS): $(OBJ_SHARED)

$(INTERDIR)/%.eo: $(EXAMPLEDIR)/%.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(INTERDIR)/%.to: $(TESTSDIR)/%.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(INTERDIR)/%.lo: %.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

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
	$(shell del /F /Q "$(BUILDDIR)\*.*" && \
		    del /F /Q "$(INTERDIR)\*.*" && \
			del /F /Q "$(LIBDIR)\*.*" && \
			del /F /Q "*.log")
else
	$(shell rm -f $(BUILDDIR)/* >/dev/null 2>&1 && \
	        rm -f $(LIBDIR)/* >/dev/null 2>&1 && \
			rm -f $(INTERDIR)/* >/dev/null 2>&1 && \
			rm -f *.log >/dev/null 2>&1)
endif
	@echo cleaned successfully.
