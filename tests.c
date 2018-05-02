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
    char path[PATH_MAX] = {0};

    for (size_t n = 0; n < 4; n++) {
        snprintf(path, PATH_MAX, "%lu.log", n);
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
    const char* mypath = (const char*)arg;

    fprintf(stderr, "Hi, I'm thread %lu, and I'm about to break your shit.\n",
        pthread_self());

    uint seed = pthread_self() * time(NULL);

    remove(mypath);
    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile(mypath, SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file %s!\n", mypath);
        return NULL;
    }    

    for (size_t n = 0; n < 100; n++) {
        for(size_t i = 0; i < 100; i++) {
            sir_debug("thread %lu: I'm here to wreck shop! %s, %d", pthread_self(), "sups", 
                (n*i) + i);
        }

        int r = rand_r(&seed) % 10;
        
        if (r % 2 == 0) {
            sir_remfile(id1);
        }
    }


    return NULL;
}