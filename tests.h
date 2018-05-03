#ifndef _SIR_TESTS_H_INCLUDED
#define _SIR_TESTS_H_INCLUDED

#include <stdbool.h>

typedef bool (*sir_test_fn)(void);
typedef struct {
    const char* name;
    sir_test_fn fn;
} sir_test;

bool sirtest_mthread_race();
bool sirtest_exceedmaxsize();
bool sirtest_addremovefiles();
bool sirtest_failsetinvalidstyle();
bool sirtest_failnooutputdest();
bool sirtest_failinvalidfilename();
bool sirtest_failfilebadpermission();
bool sirtest_failnulls();
bool sirtest_failwithoutinit();
bool sirtest_failaftercleanup();
bool sirtest_initcleanupinit();

void printclr(int clr, const char* fmt, ...);

#endif /* !_SIR_TESTS_H_INCLUDED */
