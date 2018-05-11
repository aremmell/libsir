/**
 * @file tests.c
 * @brief libsir test suite
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#define _CRT_RAND_S

#include "tests.h"
#include "../sir.h"
#include "../sirinternal.h"
#include "../sirfilecache.h"
#include "../sirerrors.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef _WIN32
#include <dirent.h>
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
#define BLUE(s) STRFMT("\033[1;34m", s)
#else
#define RED(s) s
#define GREEN(s) s
#define WHITE(s) s
#define BLUE(s) s
#endif

#define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    sirinit var         = {0};                            \
    var.d_stdout.opts   = o_stdout;                       \
    var.d_stdout.levels = l_stdout;                       \
    var.d_stderr.opts   = o_stderr;                       \
    var.d_stderr.levels = l_stderr;                       \
    bool var##_init = sir_init(&var);

static const sir_test sir_tests[] = {
    {"multi-thread race", sirtest_mthread_race},
    {"exceed max buffer size", sirtest_exceedmaxsize},
    {"file cache sanity", sirtest_filecachesanity},
    {"set invalid text style", sirtest_failsetinvalidstyle},    
    {"no output destination", sirtest_failnooutputdest},
    {"invalid file name", sirtest_failinvalidfilename},
    {"bad file permissions", sirtest_failfilebadpermission},
    {"null pointers", sirtest_failnulls},
    {"output without init", sirtest_failwithoutinit},
    {"superfluous init", sirtest_failinittwice},
    {"output after cleanup", sirtest_failaftercleanup},    
    {"init, cleanup, init", sirtest_initcleanupinit},
    {"duplicate file name", sirtest_faildupefile},
    {"remove nonexistent file", sirtest_failremovebadfile},
    {"roll/archive large file", sirtest_rollandarchivefile},
    {"validate error handling", sirtest_allerrorsresolve},
    {"text style sanity", sirtest_textstylesanity},
  /*  {"performance benchmark", sirtest_perf} */
};

int main(int argc, char** argv) {

    bool   allpass = true;
    size_t tests   = sizeof(sir_tests) / sizeof(sir_test);
    size_t passed  = 0;
    sirtimer_t timer = {0};

    printf(WHITE("running %lu libsir tests...\n"), tests);

    if (!startsirtimer(&timer))
        printf(RED("failed to start timer; perf won't be measured correctly!")"\n");

    for (size_t n = 0; n < tests; n++) {
        printf(WHITE("\t'%s'...") "\n", sir_tests[n].name);
        bool thispass = sir_tests[n].fn();
        allpass &= thispass;
        passed += (thispass ? 1 : 0);
        printf(WHITE("\t'%s' finished; result: ") "%s\n", sir_tests[n].name,
            thispass ? GREEN("PASS") : RED("FAIL"));
    }

    float elapsed = sirtimerelapsed(&timer);

    printf(WHITE("done; ")BLUE("%lu/%lu libsir tests passed in %.04fsec")WHITE("- enter to exit")"\n",
        passed, tests, elapsed / 1e3);

    int unused = getc(stdin);
    return allpass ? 0 : 1;
}

bool sirtest_exceedmaxsize(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    char toobig[SIR_MAXMESSAGE + 100] = {0};
    memset(toobig, 'a', SIR_MAXMESSAGE - 99);

    pass &= sir_info(toobig);

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_filecachesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    size_t numfiles = SIR_MAXFILES + 1;
    sirfileid_t ids[SIR_MAXFILES] = {0};

    sir_options even = SIRO_MSGONLY;
    sir_options odd = 0;

    for (size_t n = 0; n < numfiles - 1; n++) {
        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "test-%lu.log", n);
        rmfile(path);
        ids[n] = sir_addfile(path, SIRL_ALL, (n % 2) ? odd : even);
        pass &= NULL != ids[n] && sir_info("test %u", n);
    }

    pass &= sir_info("test test test");

    // this one should fail; max files already added
    pass &= NULL == sir_addfile("should-fail.log", SIRL_ALL, SIRO_MSGONLY);

    sir_info("test test test");

    // now remove previously added files in a different order
    int removeorder[SIR_MAXFILES];
    memset(removeorder, -1, sizeof(removeorder));

    long processed = 0;
    printf("\tcreating random file ID order...\n");

    do {
        unsigned int rnd = getrand() % SIR_MAXFILES;
        bool skip = false;

        for (size_t n = 0; n < SIR_MAXFILES; n++)
            if (removeorder[n] == rnd) {
                skip = true;
                break;
            }

        if (skip) continue;
        removeorder[processed++] = rnd;

        if (processed == SIR_MAXFILES) break;
    } while(true);

    printf("\tremove order: {");
    for (size_t n = 0; n < SIR_MAXFILES; n++)
        printf(" %d", removeorder[n]);
    printf(" }...\n");

    for (size_t n = 0; n < SIR_MAXFILES; n++) {
        pass &= sir_remfile(ids[removeorder[n]]);

        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "test-%lu.log", n);
        rmfile(path);
    }

    pass &= sir_info("test test test");

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failsetinvalidstyle(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= !sir_settextstyle(SIRL_INFO, 0xfefe);
    pass &= !sir_settextstyle(SIRL_ALL, SIRS_FG_RED | SIRS_FG_DEFAULT);

    if (pass)
        printexpectederr();

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failnooutputdest(void) {
    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    pass &= !sir_info("this goes nowhere!");

    if (pass)
        printexpectederr();

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failinvalidfilename(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= NULL == sir_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        printexpectederr();    

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failfilebadpermission(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= NULL == sir_addfile("/noperms", SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        printexpectederr();    

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failnulls(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= !sir_info(NULL);
    pass &= NULL == sir_addfile(NULL, SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        printexpectederr();

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failwithoutinit(void) {
    bool pass = !sir_info("sir isn't initialized; this needs to fail");

    if (pass)
        printexpectederr();

    return printerror(pass);        
}

bool sirtest_failinittwice(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    INIT(si2, SIRL_ALL, 0, 0, 0);
    pass &= !si2_init;

    if (pass)
        printexpectederr();

    sir_cleanup();
    return printerror(pass);      
}

bool sirtest_failaftercleanup() {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    sir_cleanup();
    pass &= !sir_info("already cleaned up; this needs to fail");

    if (pass)
        printexpectederr();

    return printerror(pass);
}

bool sirtest_initcleanupinit(void) {
    INIT(si1, SIRL_ALL, 0, 0, 0);
    bool pass = si1_init;

    pass &= sir_info("init called once; testing output...");
    sir_cleanup();

    INIT(si2, SIRL_ALL, 0, 0, 0);
    pass &= si2_init;

    pass &= sir_info("init called again after re-init; testing output...");
    sir_cleanup();

    return printerror(pass);
}

bool sirtest_faildupefile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= NULL != sir_addfile("foo.log", SIRL_ALL, SIRO_DEFAULT);
    pass &= NULL == sir_addfile("foo.log", SIRL_ALL, SIRO_DEFAULT);

    rmfile("foo.log");
    sir_cleanup();
    return printerror(pass);
}

bool sirtest_failremovebadfile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    int invalidid = 9999999;
    pass &= !sir_remfile(&invalidid);

    sir_cleanup();
    return printerror(pass);
}

bool sirtest_rollandarchivefile(void) {

    /* roll size minus 1KB so we can write until it maxes. */
    const long deltasize = 1024;
    const long fillsize = SIR_FROLLSIZE - deltasize;
    const sirchar_t * const logfilename = "rollandarchive";
    const sirchar_t * const line = "hello, i am some data. nice to meet you.";

    unsigned delcount = 0;
    if (!enumfiles(logfilename, deletefiles, &delcount)) {
        fprintf(stderr, "failed to delete existing log files! error: %d\n", getoserr());
        return false;
    }

    if (delcount > 0)
        printf("\tfound and removed %u log file(s)\n", delcount);

    FILE* f = fopen(logfilename, "w");

    if (!f) {
        fprintf(stderr, "fopen failed! error: %d\n", errno);
        return false;
    }

    if (0 != fseek(f, fillsize, SEEK_SET)) {
        fprintf(stderr, "fseek failed! error: %d\n", errno);
        fclose(f);
        return false;
    }

    if (EOF == fputc('\0', f)) {
        fprintf(stderr, "fputc failed! error: %d\n", errno);
        fclose(f);
        return false;
    }

    fclose(f);

    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    sirfileid_t fileid = sir_addfile(logfilename, SIRL_DEBUG, SIRO_MSGONLY | SIRO_NOHDR);

    if (pass &= NULL != fileid) {
        /* write an (approximately) known quantity until we should have rolled */
        size_t written = 0;
        size_t linesize = strlen(line);

        do {
            pass &= sir_debug("%s", line);
            if (!pass) break;
                
            written += linesize;
        } while (written < deltasize + (linesize * 50));

        /* Look for files matching the original name. */
        unsigned foundlogs = 0;
        if (!enumfiles(logfilename, countfiles, &foundlogs)) {
            fprintf(stderr, "failed to count log files! error: %d\n", errno);
            pass = false;
        }
        
        /* If two are present, the test is a pass. */
        pass &= foundlogs == 2;
    }

    pass &= sir_remfile(fileid);

    delcount = 0;
    if (!enumfiles(logfilename, deletefiles, &delcount)) {   
        fprintf(stderr, "failed to delete log files! error: %d\n", getoserr());
        return false;
    }

    if (delcount > 0)
        printf("\tfound and removed %u log file(s)\n", delcount);

    sir_cleanup(); 
    return printerror(pass);
}

bool sirtest_allerrorsresolve(void) {

    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

#pragma message "TODO: implement me"

    sir_cleanup();
    return printerror(pass);
}


bool sirtest_textstylesanity(void) {

    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    if (pass) {
        pass &= sir_debug("default style");
        pass &= sir_settextstyle(SIRL_DEBUG, SIRS_FG_YELLOW | SIRS_BG_DGRAY);
        pass &= sir_debug("override style");

        pass &= sir_info("default style");
        pass &= sir_settextstyle(SIRL_INFO, SIRS_FG_GREEN | SIRS_BG_MAGENTA);
        pass &= sir_info("override style");

        pass &= sir_notice("default style");
        pass &= sir_settextstyle(SIRL_NOTICE, SIRS_FG_BLACK | SIRS_BG_LYELLOW);
        pass &= sir_notice("override style");

        pass &= sir_warn("default style");
        pass &= sir_settextstyle(SIRL_WARN, SIRS_FG_BLACK | SIRS_BG_WHITE);
        pass &= sir_warn("override style");

        pass &= sir_error("default style");
        pass &= sir_settextstyle(SIRL_ERROR, SIRS_FG_WHITE | SIRS_BG_BLUE);
        pass &= sir_error("override style");

        pass &= sir_crit("default style");
        pass &= sir_settextstyle(SIRL_CRIT, SIRS_FG_DGRAY | SIRS_BG_LGREEN);
        pass &= sir_crit("override style");

        pass &= sir_alert("default style");
        pass &= sir_settextstyle(SIRL_ALERT, SIRS_BRIGHT | SIRS_FG_LBLUE);
        pass &= sir_alert("override style");

        pass &= sir_emerg("default style");
        pass &= sir_settextstyle(SIRL_EMERG, SIRS_BRIGHT | SIRS_FG_DGRAY);
        pass &= sir_emerg("override style");                                            
    }

    printf("\tcleanup to reset styles...\n");
    sir_cleanup();

    INIT(si2, SIRL_ALL, 0, 0, 0);
    pass &= si2_init;

    pass &= sir_debug("default style");
    pass &= sir_info("default style");
    pass &= sir_notice("default style");
    pass &= sir_warn("default style");
    pass &= sir_error("default style");
    pass &= sir_crit("default style");
    pass &= sir_alert("default style");
    pass &= sir_emerg("default style");
        
    sir_cleanup();

    return printerror(pass);
}

bool sirtest_perf(void) {
    const sirchar_t* logfilename = "sirperf";
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    if (pass) {
        float stdioelapsed = 0.0f;
        float fileelapsed = 0.0f;
        
        printf("\t1mm lines stdio...\n");

        sirtimer_t stdiotimer = {0};
        startsirtimer(&stdiotimer);

        for (size_t n = 0; n < 1e6; n++) {
            sir_debug("lorem ipsum foo bar blah");
        }

        stdioelapsed = sirtimerelapsed(&stdiotimer);

        sir_cleanup();

        INIT(si2, 0, 0, 0, 0);
        pass &= si2_init;

        sirfileid_t logid = sir_addfile(logfilename, SIRL_ALL, SIRO_MSGONLY);
        pass &= NULL != logid;

        if (pass) {
            printf("\t1mm lines log file...\n");

            sirtimer_t filetimer = {0};
            startsirtimer(&filetimer);

            for (size_t n = 0; n < 1e6; n++) {
                sir_debug("lorem ipsum foo bar blah");
            }

            fileelapsed = sirtimerelapsed(&filetimer);

            pass &= sir_remfile(logid);
        }

        if (pass) {
            printf("\t"WHITE("1mm lines stdio: %.04fsec (%.02f lines/s)")"\n",
                stdioelapsed / 1e3, 1e6 / (stdioelapsed / 1e3));
            printf("\t"WHITE("1mm lines log file: %.04fsec (%.02f lines/s)")"\n",
                fileelapsed / 1e3, 1e6 / (fileelapsed / 1e3));
        }
    }

    unsigned deleted = 0;
    enumfiles(logfilename, deletefiles, &deleted);

    if (deleted > 0)
        printf("\tdeleted %d log file(s)\n", deleted);    

    sir_cleanup();
    return printerror(pass);    
}

/*
bool sirtest_XXX(void) {

    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;


    sir_cleanup();
    return printerror(pass);
}
*/

#ifndef _WIN32
static void* sirtest_thread(void* arg);
#else
static unsigned sirtest_thread(void* arg);
#endif

#define NUM_THREADS 2

bool sirtest_mthread_race(void) {
#ifndef _WIN32
    pthread_t thrds[NUM_THREADS];
#else
    uintptr_t thrds[NUM_THREADS];
#endif

    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

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
            pass = false;
        }
    }

    if (pass) {
        for (size_t j = 0; j < NUM_THREADS; j++) {
#ifndef _WIN32
            pthread_join(thrds[j], NULL);
#else
            WaitForSingleObject((HANDLE)thrds[j], INFINITE);
#endif
        }
    }

    sir_cleanup();
    return printerror(pass);
}

#ifndef _WIN32
static void* sirtest_thread(void* arg) {
#else
unsigned sirtest_thread(void* arg) {
#endif
    pid_t threadid = _sir_gettid();

    char mypath[SIR_MAXPATH] = {0};
    strncpy(mypath, (const char*)arg, SIR_MAXPATH);
    free(arg);

    rmfile(mypath);
    sirfileid_t id = sir_addfile(mypath, SIRL_ALL, SIRO_MSGONLY);

    if (NULL == id) {
        fprintf(stderr, "\t"RED("Failed to add file %s!")"\n", mypath);
#ifndef _WIN32
        return NULL;
#else
        return 0;
#endif
    }

    printf("hi, i'm thread #%d, log file: '%s'\n", threadid, mypath);

#pragma message "TODO: refactor me"
    for (size_t n = 0; n < 100; n++) {
        for (size_t i = 0; i < 10; i++) {
            sir_debug("thread %lu: hello, how do you do? %d", threadid, (n * i) + i);

            int r = getrand() % 15;

            if (r % 2 == 0) {
                if (!sir_remfile(id))
                    printerror(false);

                id  = sir_addfile(mypath, SIRL_ALL, SIRO_MSGONLY);

                if (NULL == id)
                    printerror(false);
                if (!sir_settextstyle(SIRL_DEBUG, SIRS_FG_RED | SIRS_BG_DEFAULT))
                    printerror(false);
            } else {
                if (!sir_settextstyle(SIRL_DEBUG, SIRS_FG_CYAN | SIRS_BG_YELLOW))
                    printerror(false);
            }
        }
    }

    rmfile(mypath);

#ifndef _WIN32
    return NULL;
#else
    return 0;
#endif
}

bool printerror(bool pass) {
    if (!pass) {
        sirchar_t message[SIR_MAXERROR] = {0};
        uint16_t code = sir_geterror(message);
        printf("\t"RED("!! Unexpected (%hu, %s)")"\n", code, message);
    }
    return pass;
}

void printexpectederr(void) {
    sirchar_t message[SIR_MAXERROR] = {0};
    uint16_t code = sir_geterror(message);    
    printf("\t"GREEN("Expected (%hu, %s")"\n", code, message);
}

int getoserr(void) {
#ifndef _WIN32
    return errno;
#else
    return (int)GetLastError();
#endif           
}

unsigned int getrand(void) {
    static unsigned int seed = 0;
#ifndef _WIN32
    return (unsigned int)rand_r(&seed);
#else
    if (0 == rand_s(&seed)) {
        return seed;
    } else {
        srand(seed);
        return (unsigned int)rand();
    }
#endif  
}

bool rmfile(const char* filename) {
#ifndef _WIN32   
    return 0 == remove(filename);
#else
    return FALSE != DeleteFile(filename);
#endif    
}

bool deletefiles(const char* search, const char* filename, unsigned* data) {
    if (strstr(filename, search)) {
        if (!rmfile(filename))
            fprintf(stderr, "failed to delete %s! error: %d\n", filename, getoserr());
        (*data)++;
    }
    return true;
}

bool countfiles(const char* search, const char* filename, unsigned* data) {
    if (strstr(filename, search))
        (*data)++;
    return true;
}

bool enumfiles(const char* search, fileenumproc cb, unsigned* data) {
    
#ifndef _WIN32    
    DIR* d = opendir(".");
    if (!d) return false;

    rewinddir(d);
    struct dirent* di = readdir(d);
    if (!di) return false;

    while (NULL != di) {
        if (!cb(search, di->d_name, data))
            break;                 
        di = readdir(d);
    };

    closedir(d);
    d = NULL;
#else
    WIN32_FIND_DATA finddata = {0};
    HANDLE enumerator = FindFirstFile("./*", &finddata);

    if (INVALID_HANDLE_VALUE == enumerator)
        return false;

    do {
        if (!cb(search, finddata.cFileName, data))
            break;
    } while (FindNextFile(enumerator, &finddata) > 0);

    FindClose(enumerator);
    enumerator = NULL;
#endif

    return true;
}

bool startsirtimer(sirtimer_t* timer) {
#ifndef _WIN32
    int gettime = clock_gettime(CLOCK_MONOTONIC, &timer->ts);
    return 0 == gettime;
#else
    BOOL gettime = GetSystemTimeAsPreciseFileTime(&timer->ft);
    return 0 != gettime;
#endif
}

float sirtimerelapsed(const sirtimer_t* timer) {
#ifndef _WIN32
    struct timespec now;
    if (!clock_gettime(CLOCK_MONOTONIC, &now)) {
        return (float)((now.tv_sec * 1e3) + (now.tv_nsec / 1e6) -
            (timer->ts.tv_sec * 1e3) + (timer->ts.tv_nsec / 1e6));
    }
    return 0;
#else
    FILETIME now;
    if (GetSystemTimeAsPreciseFileTime(&now)) {
#pragma message "impl timer elapsed on win32"        
    }
    return 0;
#endif
}
