#
# libsir
# https://github.com/aremmell/libsir
#

BUILDDIR   = build
DOCSDIR    = docs
TESTS      = tests
EXAMPLE    = example
INTDIR     = $(BUILDDIR)/obj
LIBDIR     = $(BUILDDIR)/lib
BINDIR	   = $(BUILDDIR)/bin
INSTALLLIB = /usr/local/lib
INSTALLINC = /usr/local/include

# base CFLAGS
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -I. -fPIC -D_FORTIFY_SOURCE=2

# debug/non-debug CFLAGS
ifeq ($(SIR_DEBUG),1)
	CFLAGS += -g -O0 -DDEBUG
else
	CFLAGS += -O3 -DNDEBUG
endif

# enable internal diagnostic logging
ifeq ($(SIR_SELFLOG),1)
	CFLAGS += -DSIR_SELFLOG
endif

ifeq ($(SIR_ASSERT_ENABLED),1)
	CFLAGS += -DSIR_ASSERT_ENABLED
endif

# on Windows, automatically defined by the preprocessor.
ifeq ($(SIR_NO_SYSTEM_LOGGERS),1)
	CFLAGS += -DSIR_NO_SYSTEM_LOGGERS
endif

# dependencies
LIBS = -pthread

# for test rig and example:
# link with static library, not shared
LDFLAGS = $(LIBS) -L$(LIBDIR) -lsir_s

# translation units
TUS := $(wildcard *.c)

# intermediate files
_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTDIR)/%, $(_OBJ))

# shared library
OBJ_SHARED     = $(patsubst %.o, $(INTDIR)/%.o, $(_OBJ))
OUT_SHARED	   = $(LIBDIR)/libsir.so
LDFLAGS_SHARED = $(LIBS)

# static library
OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC     = $(LIBDIR)/libsir_s.a

# console example
OBJ_EXAMPLE	   = $(INTDIR)/$(EXAMPLE)/$(EXAMPLE).o
OUT_EXAMPLE    = $(BINDIR)/sirexample

# console test rig
OBJ_TESTS      = $(INTDIR)/$(TESTS)/$(TESTS).o
OUT_TESTS      = $(BINDIR)/sirtests

# ##########
# targets
# ##########

all: prep shared static example tests

-include $(INTDIR)/*.d

$(BUILDDIR)   : prep
$(INTDIR)     : $(BUILDDIR)
$(LIBDIR)     : $(BUILDDIR)
$(BINDIR)     : $(BUILDDIR)
$(OBJ_SHARED) : $(INTDIR)
$(OBJ_TESTS)  : $(OBJ_SHARED)
$(OBJ_EXAMPLE): $(OBJ_SHARED)

$(OBJ_EXAMPLE): $(EXAMPLE)/$(EXAMPLE).c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS) -I..

$(OBJ_TESTS): $(TESTS)/$(TESTS).c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS) -I..

$(INTDIR)/%.o: %.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

prep:
	$(shell mkdir -p $(BUILDDIR) && \
			mkdir -p $(INTDIR)/$(EXAMPLE) && \
			mkdir -p $(INTDIR)/$(TESTS) && \
			mkdir -p $(LIBDIR) && \
	        mkdir -p $(BINDIR))
	@echo directories prepared successfully.

shared: $(OBJ_SHARED)
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS) $(LDFLAGS_SHARED)
	@echo built $(OUT_SHARED) successfully.

static: shared
	ar -cr $(OUT_STATIC) $(OBJ_SHARED)
	@echo built $(OUT_STATIC) successfully.

example: static $(OBJ_EXAMPLE)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) $(CFLAGS) -I.. $(LDFLAGS)
	@echo built $(OUT_EXAMPLE) successfully.

tests: static $(OBJ_TESTS)
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS) $(CFLAGS) -I.. $(LDFLAGS)
	$(shell touch $(BINDIR)/file.exists)
	@echo built $(OUT_TESTS) successfully.

docs: static
	@doxygen Doxyfile
	@echo built documentation successfully.

install: shared
	@echo copying $(OUT_SHARED) to $(INSTALLLIB) and headers to $(INSTALLINC)...
	$(shell cp -f $(OUT_SHARED) "$(INSTALLLIB)/" && \
	        cp -f sir.h "$(INSTALLINC)/")
	@echo installed libsir successfully.

.PHONY: clean

clean:
	$(shell rm -rf "$(BUILDDIR)/" >/dev/null 2>&1 ; \
			rm -f *.log >/dev/null 2>&1)
	@echo build directory and log files cleaned successfully.
