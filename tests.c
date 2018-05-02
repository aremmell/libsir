#include "tests.h"
#include "sir.h"

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef _WIN32
#include <pthread.h>
#else
#endif

static void* sirtest_thread(void* arg);

int sirtest_mthread_race() {
    pthread_t thrds[2];

    for (size_t n = 0; n < 2; n++) {
        char* path = (char*)calloc(SIR_MAXPATH, sizeof(char));
        snprintf(path, SIR_MAXPATH, "%lu.log", n);
        int create = pthread_create(&thrds[n], NULL, sirtest_thread, (void*)path);
        if (0 != create) {
            fprintf(stderr, "pthread_create failed; err: %d\n", create);
            return 1;
        }

    }

    pthread_join(thrds[0], NULL);
    pthread_join(thrds[1], NULL);

    return 0;
}

static void* sirtest_thread(void* arg) {

    long threadid = syscall(SYS_gettid);
    char mypath[SIR_MAXPATH] = {0};
    strncpy(mypath, (const char*)arg, SIR_MAXPATH);
    free(arg);

    fprintf(stderr, "Hi, I'm thread %lu, path: '%s' and I'm about to break your shit.\n",
        threadid, mypath);
        
    uint seed = threadid * time(NULL);

    remove(mypath);
    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file %s!\n", mypath);
        return NULL;
    }    

    for (size_t n = 0; n < 100; n++) {
        for(size_t i = 0; i < 100; i++) {
            sir_debug("thread %lu: I'm here to wreck shop! %s, %d", threadid, "sups", 
                (n*i) + i);
        }

        int r = rand_r(&seed) % 15;
        
        if (r % 2 == 0) {
            sir_remfile(id1);
            sir_options file1opts = SIRO_MSGONLY;
            int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);                
            sir_settextstyle(SIRL_DEBUG, SIRS_FG_RED | SIRS_BG_DEFAULT);
        } else {
            sir_settextstyle(SIRL_DEBUG, SIRS_FG_CYAN | SIRS_BG_YELLOW);
        }
        
    }

    return NULL;
}