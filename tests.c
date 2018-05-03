#include "tests.h"
#include "sir.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

#define _CRT_RAND_S
#include <stdlib.h>

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <unistd.h>
#else
#define _WIN32_LEAN_AND_MEANq
#include <windows.h>
#include <process.h>
#endif

#define NUM_THREADS 2

#ifndef _WIN32
static void* sirtest_thread(void* arg);
#else
static unsigned sirtest_thread(void* arg);
#endif

int sirtest_mthread_race() {
#ifndef _WIN32
    pthread_t thrds[NUM_THREADS];
#else
    uintptr_t thrds[NUM_THREADS];
#endif    
    
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
            fprintf(stderr, "failed to create thread; err: %d\n", errno);
            return 1;
        }
    }

#ifndef _WIN32
    pthread_join(thrds[0], NULL);
    pthread_join(thrds[1], NULL);
#else
    WaitForSingleObject((HANDLE)thrds[0], INFINITE);
    WaitForSingleObject((HANDLE)thrds[1], INFINITE);
#endif

    return 0;
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

    fprintf(stderr, "Hi, I'm thread %lu, path: '%s' and I'm about to break your shit.\n",
        threadid, mypath);
        
    unsigned int seed = threadid * time(NULL);

#ifdef _WIN32
    srand(seed);
#endif

    remove(mypath);
    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file %s!\n", mypath);
#ifndef _WIN32        
        return NULL;
#else
        return 0;
#endif
    }    

    for (size_t n = 0; n < 100; n++) {
        for(size_t i = 0; i < 100; i++) {
            sir_debug("thread %lu: hello, how do you do? %d", threadid, (n*i) + i);
        }

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

#ifndef _WIN32
    return NULL;
#else
    return 0;
#endif
}