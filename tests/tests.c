/**
 * @file tests.c
 * @brief Implementation of test rig app.
 */
#include "tests.h"
#include "../sir.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <unistd.h>
#else
#define _WIN32_LEAN_AND_MEAN
#include <process.h>
#include <windows.h>
#endif

#ifndef _WIN32
#define STRFMT(clr, s) clr s "\033[0m"
#define RED(s) STRFMT("\033[1;91m", s)
#define GREEN(s) STRFMT("\033[1;92m", s)
#define WHITE(s) STRFMT("\033[1;97m", s)
#else
#define RED(s) s
#define GREEN(s) s
#define WHITE(s) s
#endif

#pragma message "add test to archive/roll log files."
/*    // Fill a log until it's rolled
    const sirchar_t* line = "sups, gonna fill this log right up!";
    size_t write = 0;
    size_t linesize = strlen(line);

    do {
        sir_info("%s", line);
        write += linesize;
    } while (write < SIR_FROLLSIZE);*/

#define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    sirinit var         = {0};                            \
    var.d_stdout.opts   = o_stdout;                       \
    var.d_stdout.levels = l_stdout;                       \
    var.d_stderr.opts   = o_stderr;                       \
    var.d_stderr.levels = l_stderr;                       \
    if (!sir_init(&var)) {                                \
        printf(RED("failed to initialize!") "\n");        \
        return false;                                     \
    }

static const sir_test sir_tests[] = {
    {"multi-thread race", sirtest_mthread_race},
    {"exceed max buffer size", sirtest_exceedmaxsize},
    {"add and remove files", sirtest_addremovefiles},
    {"set invalid text style", sirtest_failsetinvalidstyle},    
    {"no output destination", sirtest_failnooutputdest},
    {"invalid file name", sirtest_failinvalidfilename},
    {"bad file permissions", sirtest_failfilebadpermission},
    {"null pointers", sirtest_failnulls},
    {"output without init", sirtest_failwithoutinit},
    {"output after cleanup", sirtest_failaftercleanup},    
    {"init, cleanup, init", sirtest_initcleanupinit},
};

int main(int argc, char** argv) {

    bool   allpass = true;
    size_t tests   = sizeof(sir_tests) / sizeof(sir_test);
    size_t passed  = 0;

    printf(WHITE("running %lu sir library tests...\n"), tests);

    for (size_t n = 0; n < tests; n++) {
        printf(WHITE("\t'%s'...") "\n", sir_tests[n].name);
        bool thispass = sir_tests[n].fn();
        allpass &= thispass;
        passed += (thispass ? 1 : 0);
        printf(WHITE("\t'%s' finished; result: ") "%s\n", sir_tests[n].name,
            thispass ? GREEN("OK") : RED("FAIL"));
    }

    printf(WHITE("done; %lu/%lu tests passed  - press enter to continue") "\n", passed, tests);

    int unused = getc(stdin);
    return allpass ? 0 : 1;
}


bool sirtest_exceedmaxsize() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);

    char toobig[SIR_MAXMESSAGE + 100] = {0};
    memset(toobig, 'a', SIR_MAXMESSAGE - 99);

    pass &= sir_info(toobig);

    sir_cleanup();
    return pass;    
}

bool sirtest_addremovefiles() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);

    size_t numfiles = SIR_MAXFILES + 1;
    int ids[SIR_MAXFILES] = {SIR_INVALID};

    for (size_t n = 0; n < numfiles - 1; n++) {
        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "test-%lu.log", n);
        ids[n] = sir_addfile(path, SIRL_ALL, SIRO_MSGONLY);
        pass &= SIR_INVALID != ids[n];
    }

    pass &= sir_info("test test test");

    // this one should fail; max files already added
    pass &= SIR_INVALID == sir_addfile("should-fail.log", SIRL_ALL, SIRO_MSGONLY);

    sir_info("test test test");

    for (size_t j = 0; j < numfiles - 1; j++) {

        pass &= sir_remfile(ids[j]);

        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "test-%lu.log", j);
#ifndef _WIN32
        remove(path);
#else
        DeleteFile(path);
#endif
    }

    pass &= sir_info("test test test");

    sir_cleanup();

    return pass;
}

bool sirtest_failsetinvalidstyle() {
    bool pass = true;
    INIT(si, 0, 0, 0, 0);

    pass &= !sir_settextstyle(SIRL_INFO, 0xfefe);
    pass &= !sir_settextstyle(SIRL_ALL, SIRS_FG_RED | SIRS_FG_DEFAULT);

    sir_cleanup();
    return pass;    
}

bool sirtest_failnooutputdest() {
    bool pass = true;
    INIT(si, 0, 0, 0, 0);

    pass &= !sir_info("this goes nowhere!");

    sir_cleanup();
    return pass;
}

bool sirtest_failinvalidfilename() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);

    pass &= SIR_INVALID == sir_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY);

    sir_cleanup();
    return pass;
}

bool sirtest_failfilebadpermission() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);

    pass &= SIR_INVALID == sir_addfile("/noperms", SIRL_ALL, SIRO_MSGONLY);

    sir_cleanup();
    return pass;
}

bool sirtest_failnulls() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);

    pass &= !sir_info(NULL);
    pass &= SIR_INVALID == sir_addfile(NULL, SIRL_ALL, SIRO_MSGONLY);

    sir_cleanup();
    return pass;
}

bool sirtest_failwithoutinit() {
    return !sir_info("sir isn't initialized; this needs to fail");
}

bool sirtest_failaftercleanup() {
    bool pass = true;
    INIT(si, SIRL_ALL, 0, 0, 0);
    sir_cleanup();
    pass &= !sir_info("already cleaned up; this needs to fail");

    return pass;    
}

bool sirtest_initcleanupinit() {

    bool pass = true;
    INIT(si1, SIRL_ALL, 0, 0, 0);

    pass &= sir_info("init called once; testing output...");
    sir_cleanup();

    INIT(si2, SIRL_ALL, 0, 0, 0);

    pass &= sir_info("init called again after cleanup; testing output...");
    sir_cleanup();

    return pass;
}

#ifndef _WIN32
static void* sirtest_thread(void* arg);
#else
static unsigned sirtest_thread(void* arg);
#endif

#define NUM_THREADS 2

bool sirtest_mthread_race() {
#ifndef _WIN32
    pthread_t thrds[NUM_THREADS];
#else
    uintptr_t thrds[NUM_THREADS];
#endif

    INIT(si, SIRL_ALL, 0, 0, 0);

    for (size_t n = 0; n < NUM_THREADS; n++) {
        char* path = (char*)calloc(SIR_MAXPATH, sizeof(char));
        snprintf(path, SIR_MAXPATH, "%lu.log", n);

#ifndef _WIN32
        int create = pthread_create(&thrds[n], NULL, sirtest_thread, (void*)path);
        if (0 != create) {
            errno = create;
#else
        thrds[n] = _beginthreadex(NULL, 0, sirtest_thread, (void*)path, 0, NULL);
        if (0 == thrds[n]) {
#endif
            printf(RED("failed to create thread; err: %d") "\n", errno);
            sir_cleanup();
            return false;
        }
    }

    for (size_t j = 0; j < NUM_THREADS; j++) {
#ifndef _WIN32
        pthread_join(thrds[j], NULL);
#else
        WaitForSingleObject((HANDLE)thrds[j], INFINITE);
#endif
        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "%lu.log", j);
        remove(path);
    }

    sir_cleanup();
    return true;
}

#ifndef _WIN32
static void* sirtest_thread(void* arg) {
    long threadid = syscall(SYS_gettid);
#else
static unsigned sirtest_thread(void* arg) {
    long threadid = (long)GetCurrentThreadId();
#endif

    char mypath[SIR_MAXPATH] = {0};
    strncpy(mypath, (const char*)arg, SIR_MAXPATH);
    free(arg);

    printf("Hi, I'm thread %lu, path: '%s' and I'm about to break your shit.\n", threadid, mypath);

    unsigned int seed = threadid * time(NULL);

#ifdef _WIN32
    srand(seed);
#endif

    remove(mypath);
    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        printf(RED("Failed to add file %s!") "\n", mypath);
#ifndef _WIN32
        return NULL;
#else
        return 0;
#endif
    }

    for (size_t n = 0; n < 100; n++) {
        for (size_t i = 0; i < 10; i++) {
            sir_debug("thread %lu: hello, how do you do? %d", threadid, (n * i) + i);

#ifndef _WIN32
            int r = rand_r(&seed) % 15;
#else
            int r = rand() % 15;
#endif
            if (r % 2 == 0) {
                sir_remfile(id1);
                sir_options file1opts = SIRO_MSGONLY;
                int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);
                sir_settextstyle(SIRL_DEBUG, SIRS_FG_RED | SIRS_BG_DEFAULT);
            } else {
                sir_settextstyle(SIRL_DEBUG, SIRS_FG_CYAN | SIRS_BG_YELLOW);
            }
        }
    }

#ifndef _WIN32
    return NULL;
#else
    return 0;
#endif
}
