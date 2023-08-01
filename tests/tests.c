/*
 * tests.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.2
 * License:   The MIT License (MIT)
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
 *p
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
//-V:_sir_logv:575
#include "tests.h"

static sir_test sir_tests[] = {
    {"performance",             sirtest_perf, false, true},
    {"thread-race",             sirtest_threadrace, false, true},
    {"thread-pool",             sirtest_threadpool, false, true},
    {"exceed-max-buffer-size",  sirtest_exceedmaxsize, false, true},
    {"no-output-destination",   sirtest_failnooutputdest, false, true},
    {"null-pointers",           sirtest_failnulls, false, true},
    {"empty-message",           sirtest_failemptymessage, false, true},
    {"file-cache-sanity",       sirtest_filecachesanity, false, true},
    {"file-invalid-name",       sirtest_failinvalidfilename, false, true},
    {"file-bad-permissions",    sirtest_failfilebadpermission, false, true},
    {"file-duplicate-name",     sirtest_faildupefile, false, true},
    {"file-remove-nonexistent", sirtest_failremovebadfile, false, true},
    {"file-archive-large",      sirtest_rollandarchivefile, false, true},
    {"init-output-before",      sirtest_failwithoutinit, false, true},
    {"init-superfluous",        sirtest_failinittwice, false, true},
    {"init-bad-data",           sirtest_failinvalidinitdata, false, true},
    {"init-cleanup-init",       sirtest_initcleanupinit, false, true},
    {"init-with-makeinit",      sirtest_initmakeinit, false, true},
    {"cleanup-output-after",    sirtest_failaftercleanup, false, true},
    {"sanity-errors",           sirtest_errorsanity, false, true},
    {"sanity-text-styles",      sirtest_textstylesanity, false, true},
    {"sanity-options",          sirtest_optionssanity, false, true},
    {"sanity-levels",           sirtest_levelssanity, false, true},
    {"sanity-mutexes",          sirtest_mutexsanity, false, true},
    {"sanity-update-config",    sirtest_updatesanity, false, true},
    {"syslog",                  sirtest_syslog, false, true},
    {"os_log",                  sirtest_os_log, false, true},
    {"filesystem",              sirtest_filesystem, false, true},
    {"squelch-spam",            sirtest_squelchspam, false, true},
    {"plugin-loader",           sirtest_pluginloader, false, true},
    {"get-version-info",        sirtest_getversioninfo, false, true}
};

bool leave_logs = false;

int main(int argc, char** argv) {
#if defined(__HAIKU__) && !defined(DEBUG)
    disable_debugger(1);
#endif

#if defined(MTMALLOC)
# include <mtmalloc.h>
# if !defined(DEBUG)
mallocctl(MTDOUBLEFREE, 0);
# else
mallocctl(MTDOUBLEFREE, 1);
mallocctl(MTINITBUFFER, 1);
mallocctl(MTDEBUGPATTERN, 1);
# endif
#endif

#if defined(__OpenBSD__) && defined(DEBUG)
extern char *malloc_options;
malloc_options = "CFGRSU";
#endif

#if defined(DUMA)
# if defined(DUMA_EXPLICIT_INIT)
duma_init();
# endif
# if defined(DUMA_MIN_ALIGNMENT)
#  if DUMA_MIN_ALIGNMENT > 0
DUMA_SET_ALIGNMENT(DUMA_MIN_ALIGNMENT);
#  endif
# endif
DUMA_SET_FILL(0x2E);
#endif

#if !defined(__WIN__) && !defined(__HAIKU__)
    /* Disallow execution by root / sudo; some of the tests rely on lack of permissions. */
    if (geteuid() == 0) {
        fprintf(stderr, "Sorry, but this program may not be executed by root.\n");
        return EXIT_FAILURE;
    }
#else /* __WIN__ */
# if defined(_DEBUG) && defined(SIR_ASSERT_ENABLED)
    /* Prevents assert() from calling abort() before the user is able to:
     * a.) break into the code and debug (Retry button)
     * b.) ignore the assert() and continue. */
    _set_error_mode(_OUT_TO_MSGBOX);
# endif
#endif

    bool wait     = false;
    bool only     = false;
    size_t to_run = 0;

    for (int n = 1; n < argc; n++) {
        if (_sir_strsame(argv[n], _cl_arg_list[0].flag,
            strnlen(_cl_arg_list[0].flag, SIR_MAXCLIFLAG))) { /* --perf */
            only = mark_test_to_run("performance");
            if (only)
                to_run = 1;
        } else if (_sir_strsame(argv[n], _cl_arg_list[1].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --only */
            while (++n < argc) {
                if (_sir_validstrnofail(argv[n])) {
                    if (*argv[n] == '-' || !mark_test_to_run(argv[n])) {
                        fprintf(stderr, RED("invalid argument: '%s'") "\n", argv[n]);
                        print_usage_info();
                        return EXIT_FAILURE;
                    }
                    to_run++;
                }
            };
            if (0 == to_run) {
                fprintf(stderr, RED("value expected for '%s'") "\n",
                    _cl_arg_list[1].flag);
                print_usage_info();
                return EXIT_FAILURE;
            }
            only = true;
        } else if (_sir_strsame(argv[n], _cl_arg_list[2].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --list */
            print_test_list();
            return EXIT_SUCCESS;
        } else if (_sir_strsame(argv[n], _cl_arg_list[3].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --leave-logs */
            leave_logs = true;
        } else if (_sir_strsame(argv[n], _cl_arg_list[4].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --wait */
            wait = true;
        }  else if (_sir_strsame(argv[n], _cl_arg_list[5].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --version */
            print_libsir_version();
            return EXIT_SUCCESS;
        }else if (_sir_strsame(argv[n], _cl_arg_list[6].flag,
            strnlen(_cl_arg_list[1].flag, SIR_MAXCLIFLAG))) { /* --help */
            print_usage_info();
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "unknown argument: '%s'", argv[n]);
            print_usage_info();
            return EXIT_FAILURE;
        }
    }

    size_t first     = (only ? 0 : 1);
    size_t tgt_tests = (only ? to_run : _sir_countof(sir_tests) - first);
    size_t passed    = 0;
    size_t ran       = 0;
    sir_timer timer  = {0};

    printf(WHITEB("\n" ULINE("libsir") " %s (%s) running %zu %s...") "\n",
        sir_getversionstring(), (sir_isprerelease() ? "prerelease" : "release"),
        tgt_tests, TEST_S(tgt_tests));
    sirtimerstart(&timer);

    for (size_t n = first; n < _sir_countof(sir_tests); n++) {
        if (only && !sir_tests[n].run) {
            _sir_selflog("skipping '%s'; not marked to run", sir_tests[n].name);
            continue;
        }

        printf(WHITEB("\n(%zu/%zu) '%s'...") "\n\n", ran + 1, tgt_tests, sir_tests[n].name);

        sir_tests[n].pass = sir_tests[n].fn();
        if (sir_tests[n].pass)
            passed++;

        ran++;

        printf(WHITEB("\n(%zu/%zu) '%s' finished: ") "%s\n", ran, tgt_tests, sir_tests[n].name,
            PRN_PASS(sir_tests[n].pass));
    }

    float elapsed = sirtimerelapsed(&timer);

    if (passed == tgt_tests) {
        printf("\n" WHITEB("done: ")
                   GREENB("%s%zu " ULINE("libsir") " %s passed in %.03fsec!") "\n\n",
            tgt_tests > 1 ? "all " : "", tgt_tests, TEST_S(tgt_tests), (double)elapsed / (double)1e3);
    } else {
        printf("\n" WHITEB("done: ")
                   REDB("%zu of %zu " ULINE("libsir") " %s failed in %.03fsec") "\n\n",
            tgt_tests - passed, tgt_tests, TEST_S(tgt_tests), (double)elapsed / (double)1e3);

        printf(REDB("Failed %s:") "\n\n", TEST_S(tgt_tests - passed));

        for (size_t t = 0; t < _sir_countof(sir_tests); t++)
            if (!sir_tests[t].pass)
                printf(RED(INDENT_ITEM "%s\n"), sir_tests[t].name);
        printf("\n");
    }

    if (wait) {
        printf(WHITEB(EMPH("press any key to exit...")) "\n");
        char ch = '\0';
        (void)_sir_getchar(&ch);
        SIR_UNUSED(ch);
    }

    return passed == tgt_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool sirtest_exceedmaxsize(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    char toobig[SIR_MAXMESSAGE + 100] = {0};
    memset(toobig, 'a', SIR_MAXMESSAGE + 100);
    toobig[SIR_MAXMESSAGE + 99] = '\0';

    pass &= sir_info("%s", toobig);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failnooutputdest(void) {
    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    static const char* logfilename = MAKE_LOG_NAME("nodestination.log");

    pass &= !sir_notice("this goes nowhere!");

    if (pass) {
        print_expected_error();

        pass &= sir_stdoutlevels(SIRL_INFO);
        pass &= sir_info("this goes to stdout");
        pass &= sir_stdoutlevels(SIRL_NONE);

        sirfileid fid = sir_addfile(logfilename, SIRL_INFO, SIRO_DEFAULT);
        pass &= 0 != fid;
        pass &= sir_info("this goes to %s", logfilename);
        pass &= sir_filelevels(fid, SIRL_NONE);
        pass &= !sir_notice("this goes nowhere!");

        if (0 != fid)
            pass &= sir_remfile(fid);

        rmfile(logfilename);
    }

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failnulls(void) {
    INIT_BASE(si, SIRL_ALL, 0, 0, 0, "", false);
    bool pass = true;

    pass &= !sir_init(NULL);

    if (pass)
        print_expected_error();

    pass &= sir_init(&si);
    pass &= !sir_info(NULL); //-V575 //-V618

    if (pass)
        print_expected_error();

    pass &= 0 == sir_addfile(NULL, SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    pass &= !sir_remfile(0);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failemptymessage(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= !sir_debug("%s", "");

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_filecachesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    size_t numfiles             = SIR_MAXFILES + 1;
    sirfileid ids[SIR_MAXFILES] = {0};

    sir_options even = SIRO_MSGONLY;
    sir_options odd  = SIRO_ALL;

    for (size_t n = 0; n < numfiles - 1; n++) {
        char path[SIR_MAXPATH] = {0};
        (void)snprintf(path, SIR_MAXPATH, MAKE_LOG_NAME("test-%zu.log"), n);
        rmfile(path);
        ids[n] = sir_addfile(path, SIRL_ALL, (n % 2) ? odd : even);
        pass &= 0 != ids[n] && sir_info("test %zu", n);
    }

    pass &= sir_info("test test test");

    /* this one should fail; max files already added. */
    pass &= 0 == sir_addfile(MAKE_LOG_NAME("should-fail.log"), SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    sir_info("test test test");

    /* now remove previously added files in a different order. */
    size_t removeorder[SIR_MAXFILES];
    memset(removeorder, -1, sizeof(removeorder));

    long processed = 0;
    printf("\tcreating random file ID order...\n");

    do {
        size_t rnd = (size_t)getrand(SIR_MAXFILES);
        bool skip  = false;

        for (size_t n = 0; n < SIR_MAXFILES; n++)
            if (removeorder[n] == rnd) {
                skip = true;
                break;
            }

        if (skip)
            continue;

        removeorder[processed++] = rnd;

        if (processed == SIR_MAXFILES)
            break;
    } while (true);

    printf("\tremove order: {");
    for (size_t n = 0; n < SIR_MAXFILES; n++)
        printf(" %zu%s", removeorder[n], (n < SIR_MAXFILES - 1) ? "," : "");
    printf(" }...\n");

    for (size_t n = 0; n < SIR_MAXFILES; n++) {
        pass &= sir_remfile(ids[removeorder[n]]);

        char path[SIR_MAXPATH] = {0};
        (void)snprintf(path, SIR_MAXPATH, MAKE_LOG_NAME("test-%zu.log"), removeorder[n]);
        rmfile(path);
    }

    pass &= sir_info("test test test");

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failinvalidfilename(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= 0 == sir_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failfilebadpermission(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

#if !defined(__WIN__)
    static const char* path = "/noperms";
#else /* __WIN__ */
# if defined(__CYGWIN__)
    static const char* path = "/cygdrive/c/Windows/System32/noperms";
# else
    static const char* path;
    if (_sirtest_wine()) {
        path = "Z:\\noperms";
    } else {
        path = "C:\\Windows\\System32\\noperms";
    }
# endif
#endif

    pass &= 0 == sir_addfile(path, SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_faildupefile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

#if !defined(__WIN__)
    static const char* filename1 = "./logs/faildupefile.log";
    static const char* filename2 = "logs/faildupefile.log";
#else
    static const char* filename1 = "logs\\faildupefile.log";
    static const char* filename2 = "logs/faildupefile.log";
#endif

    static const char* filename3 = "logs/not-a-dupe.log";
    static const char* filename4 = "logs/also-not-a-dupe.log";

    printf("\tadding log file '%s'...\n", filename1);

    /* should be fine; no other files added yet. */
    sirfileid fid = sir_addfile(filename1, SIRL_ALL, SIRO_DEFAULT);
    pass &= 0 != fid;

    printf("\ttrying again to add log file '%s'...\n", filename1);

    /* should fail. this is the same file we already added. */
    pass &= 0 == sir_addfile(filename1, SIRL_ALL, SIRO_DEFAULT);

    if (pass)
        print_expected_error();

    printf("\tadding log file '%s'...\n", filename2);

    /* should also fail. this is the same file we already added, even
     * if the path strings don't match. */
    pass &= 0 == sir_addfile(filename2, SIRL_ALL, SIRO_DEFAULT);

    if (pass)
        print_expected_error();

    printf("\tadding log file '%s'...\n", filename3);

    /* should pass. this is a different file. */
    sirfileid fid2 = sir_addfile(filename3, SIRL_ALL, SIRO_DEFAULT);
    pass &= 0 != fid2;

    /* should also pass. */
    sirfileid fid3 = sir_addfile(filename4, SIRL_ALL, SIRO_DEFAULT);
    pass &= 0 != fid3;

    pass &= sir_info("hello three valid files");

    /* should now fail since we added it earlier. */
    pass &= 0 == sir_addfile(filename3, SIRL_ALL, SIRO_DEFAULT);

    if (pass)
        print_expected_error();

    /* don't remove all of the log files in order to also test
     * cache tear-down. */
    pass &= sir_remfile(fid);

    rmfile(filename1);
    rmfile(filename2);
    rmfile(filename3);
    rmfile(filename4);

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failremovebadfile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    sirfileid invalidid = 9999999;
    pass &= !sir_remfile(invalidid);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_rollandarchivefile(void) {
    /* roll size minus 1KiB so we can write until it maxes. */
    static const long deltasize    = 1024L;
    const long fillsize            = SIR_FROLLSIZE - deltasize;
    static const char* logbasename = "rollandarchive";
    static const char* logext      = ".log";
    static const char* line        = "hello, i am some data. nice to meet you.";

    char logfilename[SIR_MAXPATH] = {0};
    (void)snprintf(logfilename, SIR_MAXPATH, MAKE_LOG_NAME("%s%s"), logbasename, logext);

    unsigned delcount = 0;
    if (!enumfiles(SIR_TESTLOGDIR, logbasename, deletefiles, &delcount)) {
        handle_os_error(false, "failed to enumerate log files with base name: %s!",
            logbasename);
        return false;
    }

    if (delcount > 0)
        printf("\tfound and removed %u log file(s)\n", delcount);

    FILE* f = NULL;
    _sir_fopen(&f, logfilename, "w");

    if (!f)
        return print_os_error();

    if (0 != fseek(f, fillsize, SEEK_SET)) {
        handle_os_error(true, "fseek in file %s failed!", logfilename);
        fclose(f);
        return false;
    }

    if (EOF == fputc('\0', f)) {
        handle_os_error(true, "fputc in file %s failed!", logfilename);
        fclose(f);
        return false;
    }

    fclose(f);

    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    sirfileid fileid = sir_addfile(logfilename, SIRL_DEBUG, SIRO_MSGONLY | SIRO_NOHDR);
    pass &= 0 != fileid;

    if (pass) {
        /* write an (approximately) known quantity until we should have rolled */
        size_t written  = 0;
        size_t linesize = strnlen(line, SIR_MAXMESSAGE);

        do {
            pass &= sir_debug("%zu %s", written, line);
            written += linesize;
        } while (pass && (written < deltasize + (linesize * 50)));

        /* look for files matching the original name. */
        unsigned foundlogs = 0;
        if (!enumfiles(SIR_TESTLOGDIR, logbasename, countfiles, &foundlogs)) {
            handle_os_error(false, "failed to enumerate log files with base name: %s!",
                logbasename);
            pass = false;
        }

        /* if two (or more) are present, the test is a pass. */
        printf("\tfound %u log files with base name: %s\n", foundlogs, logbasename);
        pass &= foundlogs >= 2;
    }

    pass &= sir_remfile(fileid);

    delcount = 0;
    if (!enumfiles(SIR_TESTLOGDIR, logbasename, deletefiles, &delcount)) {
        handle_os_error(false, "failed to enumerate log files with base name: %s!", logbasename);
        return false;
    }

    if (delcount > 0)
        printf("\tfound and removed %u log file(s)\n", delcount);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failwithoutinit(void) {
    bool pass = !sir_info("sir isn't initialized; this needs to fail");

    if (pass)
        print_expected_error();

    return print_result_and_return(pass);
}

bool sirtest_failinittwice(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    INIT(si2, SIRL_ALL, 0, 0, 0);
    pass &= !si2_init;

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failinvalidinitdata(void) {
    sirinit si;

    /* fill with bad data. */
    memset(&si, 0xab, sizeof(sirinit));

    printf("\tcalling sir_init with invalid data...\n");
    bool pass = !sir_init(&si);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
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

    return print_result_and_return(pass);
}

bool sirtest_initmakeinit(void) {
    bool pass = true;

    sirinit si;
    pass &= sir_makeinit(&si);
    pass &= sir_init(&si);
    pass &= sir_info("initialized with sir_makeinit");
    pass &= sir_cleanup();

    return print_result_and_return(pass);
}

bool sirtest_failaftercleanup(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    sir_cleanup();
    pass &= !sir_info("already cleaned up; this needs to fail");

    if (pass)
        print_expected_error();

    return print_result_and_return(pass);
}

bool sirtest_errorsanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    struct {
        uint16_t code;
        const char* name;
    } errors[] = {
        {SIR_E_NOERROR,   "SIR_E_NOERROR"},   /**< The operation completed successfully (0) */
        {SIR_E_NOTREADY,  "SIR_E_NOTREADY"},  /**< libsir has not been initialized (1) */
        {SIR_E_ALREADY,   "SIR_E_ALREADY"},   /**< libsir is already initialized (2) */
        {SIR_E_DUPITEM,   "SIR_E_DUPITEM"},   /**< Item already managed by libsir (3) */
        {SIR_E_NOITEM,    "SIR_E_NOITEM"},    /**< Item not managed by libsir (4) */
        {SIR_E_NOROOM,    "SIR_E_NOROOM"},    /**< Maximum number of items already stored (5) */
        {SIR_E_OPTIONS,   "SIR_E_OPTIONS"},   /**< Option flags are invalid (6) */
        {SIR_E_LEVELS,    "SIR_E_LEVELS"},    /**< Level flags are invalid (7) */
        {SIR_E_TEXTSTYLE, "SIR_E_TEXTSTYLE"}, /**< Text style is invalid (8) */
        {SIR_E_STRING,    "SIR_E_STRING"},    /**< Invalid string argument (9) */
        {SIR_E_NULLPTR,   "SIR_E_NULLPTR"},   /**< NULL pointer argument (10) */
        {SIR_E_INVALID,   "SIR_E_INVALID"},   /**< Invalid argument (11) */
        {SIR_E_NODEST,    "SIR_E_NODEST"},    /**< No destinations registered for level (12) */
        {SIR_E_UNAVAIL,   "SIR_E_UNAVAIL"},   /**< Feature is disabled or unavailable (13) */
        {SIR_E_INTERNAL,  "SIR_E_INTERNAL"},  /**< An internal error has occurred (14) */
        {SIR_E_COLORMODE, "SIR_E_COLORMODE"}, /**< Invalid color mode (15) */
        {SIR_E_TEXTATTR,  "SIR_E_TEXTATTR"},  /**< Invalid text attributes (16) */
        {SIR_E_TEXTCOLOR, "SIR_E_TEXTCOLOR"}, /**< Invalid text color (17) */
        {SIR_E_PLUGINBAD, "SIR_E_PLUGINBAD"}, /**< Plugin module is malformed (18) */
        {SIR_E_PLUGINDAT, "SIR_E_PLUGINDAT"}, /**< Data produced by plugin is invalid (19) */
        {SIR_E_PLUGINVER, "SIR_E_PLUGINVER"}, /**< Plugin interface version unsupported (20) */
        {SIR_E_PLUGINERR, "SIR_E_PLUGINERR"}, /**< Plugin reported failure (21) */
        {SIR_E_PLATFORM,  "SIR_E_PLATFORM"},  /**< Platform error code %d: %s (22) */
        {SIR_E_UNKNOWN,   "SIR_E_UNKNOWN"},   /**< Unknown error (4095) */
    };

    char message[SIR_MAXERROR] = {0};
    for (size_t n = 0; n < _sir_countof(errors); n++) {
        (void)_sir_seterror(_sir_mkerror(errors[n].code));
        memset(message, 0, SIR_MAXERROR);
        uint16_t err = sir_geterror(message);
        pass &= errors[n].code == err && *message != '\0';
        printf("\t%s = %s\n", errors[n].name, message);
    }

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_textstylesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    printf("\t" WHITEB("--- explicitly invalid ---") "\n");
    pass &= !sir_settextstyle(SIRL_INFO, 0xbbb, 800, 920);
    pass &= sir_info("I have set an invalid text style.");

    pass &= !sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_BLACK);
    pass &= sir_info("oops, did it again...");

    pass &= !sir_settextstyle(SIRL_ALERT, SIRTA_NORMAL, 0xff, 0xff);
    pass &= sir_info("and again.");
    PRINT_PASS(pass, "\t--- explicitly invalid: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- unusual but valid ---") "\n");
    pass &= sir_settextstyle(SIRL_INFO, SIRTA_NORMAL, SIRTC_DEFAULT, SIRTC_DEFAULT);
    pass &= sir_info("system default fg and bg");
    PRINT_PASS(pass, "\t--- unusual but valid: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- override defaults ---") "\n");
    pass &= sir_resettextstyles();

    pass &= sir_debug("default style");
    pass &= sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, SIRTC_BYELLOW, SIRTC_DGRAY);
    pass &= sir_debug("override style");

    pass &= sir_info("default style");
    pass &= sir_settextstyle(SIRL_INFO, SIRTA_NORMAL, SIRTC_GREEN, SIRTC_MAGENTA);
    pass &= sir_info("override style");

    pass &= sir_notice("default style");
    pass &= sir_settextstyle(SIRL_NOTICE, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_BYELLOW);
    pass &= sir_notice("override style");

    pass &= sir_warn("default style");
    pass &= sir_settextstyle(SIRL_WARN, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_WHITE);
    pass &= sir_warn("override style");

    pass &= sir_error("default style");
    pass &= sir_settextstyle(SIRL_ERROR, SIRTA_NORMAL, SIRTC_WHITE, SIRTC_BLUE);
    pass &= sir_error("override style");

    pass &= sir_crit("default style");
    pass &= sir_settextstyle(SIRL_CRIT, SIRTA_EMPH, SIRTC_DGRAY, SIRTC_BGREEN);
    pass &= sir_crit("override style");

    pass &= sir_alert("default style");
    pass &= sir_settextstyle(SIRL_ALERT, SIRTA_ULINE, SIRTC_BBLUE, SIRTC_DEFAULT);
    pass &= sir_alert("override style");

    pass &= sir_emerg("default style");
    pass &= sir_settextstyle(SIRL_EMERG, SIRTA_BOLD, SIRTC_DGRAY, SIRTC_DEFAULT);
    pass &= sir_emerg("override style");
    PRINT_PASS(pass, "\t--- override defaults: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- reset to defaults ---") "\n");
    pass &= sir_resettextstyles();

    pass &= sir_debug("default style (debug)");
    pass &= sir_info("default style (info)");
    pass &= sir_notice("default style (notice)");
    pass &= sir_warn("default style (warning)");
    pass &= sir_error("default style (error)");
    pass &= sir_crit("default style (crit)");
    pass &= sir_alert("default style (alert)");
    pass &= sir_emerg("default style (emergency)");
    PRINT_PASS(pass, "\t--- reset to defaults: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- change mode: 256-color ---") "\n");
    pass &= sir_setcolormode(SIRCM_256);

    for (sir_textcolor fg = 0, bg = 255; fg < 256; fg++, bg--) {
        if (fg != bg) {
            pass &= sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg);
            pass &= sir_debug("this is 256-color mode (fg: %"PRIu32", bg: %"PRIu32")",
                fg, bg);
        }
    }

    PRINT_PASS(pass, "\t--- change mode: 256-color: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- change mode: RGB-color ---") "\n");
    pass &= sir_setcolormode(SIRCM_RGB);

    for (size_t n = 0; n < 256; n++) {
        sir_textcolor fg = sir_makergb(getrand(255), getrand(255), getrand(255));
        sir_textcolor bg = sir_makergb(getrand(255), getrand(255), getrand(255));
        pass &= sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg);
        pass &= sir_debug("this is RGB-color mode (fg: %"PRIu32", %"PRIu32", %"PRIu32
            ", bg: %"PRIu32", %"PRIu32", %"PRIu32")", _sir_getredfromcolor(fg),
            _sir_getgreenfromcolor(fg), _sir_getbluefromcolor(fg), _sir_getredfromcolor(bg),
            _sir_getgreenfromcolor(bg), _sir_getbluefromcolor(bg));
    }
    PRINT_PASS(pass, "\t--- change mode: RGB-color: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- change mode: invalid mode ---") "\n");
    pass &= !sir_setcolormode(SIRCM_INVALID);
    sir_textcolor fg = sir_makergb(255, 0, 0);
    sir_textcolor bg = sir_makergb(0, 0, 0);
    pass &= sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg);
    pass &= sir_debug("this is still RGB color mode");
    PRINT_PASS(pass, "\t--- change mode: invalid mode %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- change mode: 16-color ---") "\n");
    pass &= sir_setcolormode(SIRCM_16);
    pass &= sir_settextstyle(SIRL_DEBUG, SIRTA_EMPH, SIRTC_BMAGENTA, SIRTC_DEFAULT);
    pass &= sir_debug("this is 16-color mode (fg: %"PRId32", bg: default)",
        SIRTC_BMAGENTA);
    PRINT_PASS(pass, "\t--- change mode: 16-color: %s ---\n\n", PRN_PASS(pass));

    sir_cleanup();

    return print_result_and_return(pass);
}

bool sirtest_optionssanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t iterations = 10;

    /* these should all be valid. */
    printf("\t" WHITEB("--- individual valid options ---") "\n");
    pass &= _sir_validopts(SIRO_ALL);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_ALL);
    pass &= _sir_validopts(SIRO_NOTIME);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOTIME);
    pass &= _sir_validopts(SIRO_NOHOST);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOHOST);
    pass &= _sir_validopts(SIRO_NOLEVEL);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOLEVEL);
    pass &= _sir_validopts(SIRO_NONAME);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NONAME);
    pass &= _sir_validopts(SIRO_NOPID);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOPID);
    pass &= _sir_validopts(SIRO_NOTID);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOTID);
    pass &= _sir_validopts(SIRO_NOHDR);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_NOHDR);
    pass &= _sir_validopts(SIRO_MSGONLY);
    printf(INDENT_ITEM WHITE("valid option: %08"PRIx32) "\n", SIRO_MSGONLY);
    PRINT_PASS(pass, "\t--- individual valid options: %s ---\n\n", PRN_PASS(pass));

    /* any combination these bitwise OR'd together
       to form a bitmask should also be valid. */
    static const sir_option option_arr[SIR_NUMOPTIONS] = {
        SIRO_NOTIME,
        SIRO_NOHOST,
        SIRO_NOLEVEL,
        SIRO_NONAME,
        SIRO_NOMSEC,
        SIRO_NOPID,
        SIRO_NOTID,
        SIRO_NOHDR
    };

    printf("\t" WHITEB("--- random bitmask of valid options ---") "\n");
    uint32_t last_count = SIR_NUMOPTIONS;
    for (size_t n = 0; n < iterations; n++) {
        sir_options opts    = 0;
        uint32_t rand_count = 0;
        size_t last_idx     = 0;

        do {
            rand_count = getrand(SIR_NUMOPTIONS);
        } while (rand_count == last_count || rand_count <= 1);

        last_count = rand_count;

        for (size_t i = 0; i < rand_count; i++) {
            size_t rand_idx = 0;
            size_t tries    = 0;

            do {
                if (++tries > SIR_NUMOPTIONS - 2)
                    break;
                rand_idx = (size_t)getrand(SIR_NUMOPTIONS);

            } while (rand_idx == last_idx || _sir_bittest(opts, option_arr[rand_idx]));

            last_idx = rand_idx;
            opts |= option_arr[rand_idx];
        }

        pass &= _sir_validopts(opts);
        printf(INDENT_ITEM WHITE("(%zu/%zu): random valid (count: %"PRIu32
            ", options: %08"PRIx32")") "\n", n + 1, iterations, rand_count, opts);
    }
    PRINT_PASS(pass, "\t--- random bitmask of valid options: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- invalid values ---") "\n");

    /* the lowest byte is not valid. */
    sir_options invalid = 0x000000ff;
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("lowest byte: %08"PRIx32) "\n", invalid);

    /* gaps inbetween valid options. */
    invalid = 0x0001ff00 & ~(SIRO_NOTIME | SIRO_NOHOST | SIRO_NOLEVEL | SIRO_NONAME |
                             SIRO_NOMSEC | SIRO_NOPID | SIRO_NOTID  | SIRO_NOHDR);
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("gaps in 0x001ff00: %08"PRIx32) "\n", invalid);

    /* greater than SIRO_MSGONLY and less than SIRO_NOHDR. */
    for (sir_option o = 0x00008f00; o < SIRO_NOHDR; o += 0x1000) {
        pass &= !_sir_validopts(o);
        printf(INDENT_ITEM WHITE("SIRO_MSGONLY >< SIRO_NOHDR: %08"PRIx32) "\n", o);
    }

    /* greater than SIRO_NOHDR. */
    invalid = (0xFFFF0000 & ~SIRO_NOHDR);
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("greater than SIRO_NOHDR: %08"PRIx32) "\n", invalid);

    PRINT_PASS(pass, "\t--- invalid values: %s ---\n\n", PRN_PASS(pass));

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_levelssanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t iterations = 10;

    /* these should all be valid. */
    printf("\t" WHITEB("--- individual valid levels ---") "\n");
    pass &= _sir_validlevel(SIRL_INFO) && _sir_validlevels(SIRL_INFO);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_INFO);
    pass &= _sir_validlevel(SIRL_DEBUG) && _sir_validlevels(SIRL_DEBUG);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_DEBUG);
    pass &= _sir_validlevel(SIRL_NOTICE) && _sir_validlevels(SIRL_NOTICE);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_NOTICE);
    pass &= _sir_validlevel(SIRL_WARN) && _sir_validlevels(SIRL_WARN);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_WARN);
    pass &= _sir_validlevel(SIRL_ERROR) && _sir_validlevels(SIRL_ERROR);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_ERROR);
    pass &= _sir_validlevel(SIRL_CRIT) && _sir_validlevels(SIRL_CRIT);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_CRIT);
    pass &= _sir_validlevel(SIRL_ALERT) && _sir_validlevels(SIRL_ALERT);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_ALERT);
    pass &= _sir_validlevel(SIRL_EMERG) && _sir_validlevels(SIRL_EMERG);
    printf(INDENT_ITEM WHITE("valid level: %04x") "\n", SIRL_EMERG);
    pass &= _sir_validlevels(SIRL_ALL);
    printf(INDENT_ITEM WHITE("valid levels: %04x") "\n", SIRL_ALL);
    pass &= _sir_validlevels(SIRL_NONE);
    printf(INDENT_ITEM WHITE("valid levels: %04x") "\n", SIRL_NONE);
    PRINT_PASS(pass, "\t--- individual valid levels: %s ---\n\n", PRN_PASS(pass));

    /* any combination these bitwise OR'd together
       to form a bitmask should also be valid. */
    static const sir_levels levels_arr[SIR_NUMLEVELS] = {
       SIRL_EMERG,
       SIRL_ALERT,
       SIRL_CRIT,
       SIRL_ERROR,
       SIRL_WARN,
       SIRL_NOTICE,
       SIRL_INFO,
       SIRL_DEBUG
    };

    printf("\t" WHITEB("--- random bitmask of valid levels ---") "\n");
    uint32_t last_count = SIR_NUMLEVELS;
    for (size_t n = 0; n < iterations; n++) {
        sir_levels levels   = 0;
        uint32_t rand_count = 0;
        size_t last_idx     = 0;

        do {
            rand_count = getrand(SIR_NUMLEVELS);
        } while (rand_count == last_count || rand_count <= 1);

        last_count = rand_count;

        for (size_t i = 0; i < rand_count; i++) {
            size_t rand_idx = 0;
            size_t tries    = 0;

            do {
                if (++tries > SIR_NUMLEVELS - 2)
                    break;
                rand_idx = (size_t)getrand(SIR_NUMLEVELS);
            } while (rand_idx == last_idx || _sir_bittest(levels, levels_arr[rand_idx]));

            last_idx = rand_idx;
            levels |= levels_arr[rand_idx];
        }

        pass &= _sir_validlevels(levels);
        printf(INDENT_ITEM WHITE("(%zu/%zu): random valid (count: %"PRIu32", levels:"
                                 " %04"PRIx16) ")\n", n + 1, iterations, rand_count, levels);
    }
    PRINT_PASS(pass, "\t--- random bitmask of valid levels: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- invalid values ---") "\n");

    /* greater than SIRL_ALL. */
    sir_levels invalid = (0xffff & ~SIRL_ALL);
    pass &= !_sir_validlevels(invalid);
    printf(INDENT_ITEM WHITE("greater than SIRL_ALL: %04"PRIx16) "\n", invalid);

    /* individual invalid level. */
    sir_level invalid2 = 0x1337;
    pass &= !_sir_validlevel(invalid2);
    printf(INDENT_ITEM WHITE("individual invalid level: %04"PRIx32) "\n", invalid2);

    PRINT_PASS(pass, "\t--- invalid values: %s ---\n\n", PRN_PASS(pass));

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_mutexsanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    printf("\t" WHITEB("create, lock, unlock, destroy") "\n");
    printf(INDENT_ITEM WHITE("creating mutex...") "\n");

    sir_mutex m1 = SIR_MUTEX_INIT;
    pass &= _sir_mutexcreate(&m1);

    print_test_error(pass, pass);

    if (pass) {
        printf(INDENT_ITEM WHITE("locking (wait)...") "\n");
        pass &= _sir_mutexlock(&m1);

        print_test_error(pass, pass);

        printf(INDENT_ITEM WHITE("entered; unlocking...") "\n");
        pass &= _sir_mutexunlock(&m1);

        print_test_error(pass, pass);

        printf(INDENT_ITEM WHITE("locking (without wait)...") "\n");
        pass &= _sir_mutextrylock(&m1);

        print_test_error(pass, pass);

        printf(INDENT_ITEM WHITE("unlocking...") "\n");
        pass &= _sir_mutexunlock(&m1);

        print_test_error(pass, pass);

        printf(INDENT_ITEM WHITE("destryoing...") "\n");
        pass &= _sir_mutexdestroy(&m1);

        print_test_error(pass, pass);

    }
    PRINT_PASS(pass, "\t--- create, lock, unlock, destroy: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("invalid arguments") "\n");
    printf(INDENT_ITEM WHITE("create with NULL pointer...") "\n");
    pass &= !_sir_mutexcreate(NULL);
    printf(INDENT_ITEM WHITE("lock with NULL pointer...") "\n");
    pass &= !_sir_mutexlock(NULL);
    printf(INDENT_ITEM WHITE("trylock with NULL pointer...") "\n");
    pass &= !_sir_mutextrylock(NULL);
    printf(INDENT_ITEM WHITE("unlock with NULL pointer...") "\n");
    pass &= !_sir_mutexunlock(NULL);
    printf(INDENT_ITEM WHITE("destroy with NULL pointer...") "\n");
    pass &= !_sir_mutexdestroy(NULL);
    PRINT_PASS(pass, "\t--- pass invalid arguments: %s ---\n\n", PRN_PASS(pass));

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_perf(void) {
    static const char* logbasename = "libsir-perf";
    static const char* logext      = "";

#if !defined(__WIN__) && !defined(DUMA)
    static const size_t perflines = 1000000;
#else /* __WIN__ */
    static const size_t perflines = 100000;
#endif

    INIT_N(si, SIRL_ALL, SIRO_NOMSEC | SIRO_NOHOST, 0, 0, "perf");
    bool pass = si_init;

    if (pass) {
        float printfelapsed = 0.0f;
        float stdioelapsed  = 0.0f;
        float fileelapsed   = 0.0f;

        printf("\t" BLUE("%zu lines printf...") "\n", perflines);

        sir_timer printftimer = {0};
        sirtimerstart(&printftimer);

        for (size_t n = 0; n < perflines; n++)
            printf(WHITE("%.2f: lorem ipsum foo bar %s: %zu") "\n",
                (double)sirtimerelapsed(&printftimer), "baz", 1234 + n);

        printfelapsed = sirtimerelapsed(&printftimer);

        printf("\t" BLUE("%zu lines libsir(stdout)...") "\n", perflines);

        sir_timer stdiotimer = {0};
        sirtimerstart(&stdiotimer);

        for (size_t n = 0; n < perflines; n++)
            sir_debug("%.2f: lorem ipsum foo bar %s: %zu",
                (double)sirtimerelapsed(&stdiotimer), "baz", 1234 + n);

        stdioelapsed = sirtimerelapsed(&stdiotimer);

        sir_cleanup();

        INIT(si2, 0, 0, 0, 0);
        pass &= si2_init;

        char logfilename[SIR_MAXPATH] = {0};
        (void)snprintf(logfilename, SIR_MAXPATH, MAKE_LOG_NAME("%s%s"), logbasename, logext);

        sirfileid logid = sir_addfile(logfilename, SIRL_ALL, SIRO_NOMSEC | SIRO_NONAME);
        pass &= 0 != logid;

        if (pass) {
            printf("\t" BLUE("%zu lines libsir(log file)...") "\n", perflines);

            sir_timer filetimer = {0};
            sirtimerstart(&filetimer);

            for (size_t n = 0; n < perflines; n++)
                sir_debug("lorem ipsum foo bar %s: %zu", "baz", 1234 + n);

            fileelapsed = sirtimerelapsed(&filetimer);

            pass &= sir_remfile(logid);
        }

        if (pass) {
            printf("\t" WHITEB("printf: ") CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, (double)printfelapsed / (double)1e3,
                (double)perflines / (double)((double)printfelapsed / (double)1e3));
            printf("\t" WHITEB("libsir(stdout): ")
                   CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, (double)stdioelapsed / (double)1e3,
                (double)perflines / (double)((double)stdioelapsed / (double)1e3));
            printf("\t" WHITEB("libsir(log file): ")
                   CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, (double)fileelapsed / (double)1e3,
                (double)perflines / (double)((double)fileelapsed / (double)1e3));
            printf("\t" WHITEB("timer resolution: ") CYAN("~%ldnsec") "\n", sirtimergetres());
        }
    }

    unsigned deleted = 0;
    enumfiles(SIR_TESTLOGDIR, logbasename, deletefiles, &deleted);

    if (deleted > 0)
        printf("\t" DGRAY("deleted %u log file(s)") "\n", deleted);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_updatesanity(void) {
    INIT_N(si, SIRL_DEFAULT, 0, SIRL_DEFAULT, 0, "update_sanity");
    bool pass = si_init;

#define UPDATE_SANITY_ARRSIZE 10

    static const char* logfile = MAKE_LOG_NAME("update-sanity.log");
    static const sir_options opts_array[UPDATE_SANITY_ARRSIZE] = {
        SIRO_NOHOST | SIRO_NOTIME | SIRO_NOLEVEL,
        SIRO_MSGONLY, SIRO_NONAME | SIRO_NOTID,
        SIRO_NOPID | SIRO_NOTIME,
        SIRO_NOTIME | SIRO_NOLEVEL | SIRO_NONAME,
        SIRO_NOTIME, SIRO_NOMSEC | SIRO_NOHOST,
        SIRO_NOPID | SIRO_NOTID,
        SIRO_NOHOST | SIRO_NOTID, SIRO_ALL
    };

    static const sir_levels levels_array[UPDATE_SANITY_ARRSIZE] = {
        SIRL_NONE, SIRL_ALL, SIRL_EMERG, SIRL_ALERT,
        SIRL_CRIT, SIRL_ERROR, SIRL_WARN, SIRL_NOTICE,
        SIRL_INFO, SIRL_DEBUG
    };

    rmfile(logfile);
    sirfileid id1 = sir_addfile(logfile, SIRL_DEFAULT, SIRO_DEFAULT);
    pass &= 0 != id1;

    for (int i = 0; i < 10; i++) {
        if (!pass)
            break;

        /* reset to defaults*/
        pass &= sir_stdoutlevels(SIRL_DEFAULT);
        pass &= sir_stderrlevels(SIRL_DEFAULT);
        pass &= sir_stdoutopts(SIRO_DEFAULT);
        pass &= sir_stderropts(SIRO_DEFAULT);

        pass &= sir_debug("default config (debug)");
        pass &= sir_info("default config (info)");
        pass &= sir_notice("default config (notice)");
        pass &= sir_warn("default config (warning)");
        pass &= sir_error("default config (error)");
        pass &= sir_crit("default config (critical)");
        pass &= sir_alert("default config (alert)");
        pass &= sir_emerg("default config (emergency)");

        /* pick random options to set/unset */
        uint32_t rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_stdoutlevels(levels_array[rnd]);
        pass &= sir_stdoutopts(opts_array[rnd]);
        printf("\t" WHITE("set random config #%"PRIu32" for stdout") "\n", rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_stderrlevels(levels_array[rnd]);
        pass &= sir_stderropts(opts_array[rnd]);
        printf("\t" WHITE("set random config #%"PRIu32" for stderr") "\n", rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_filelevels(id1, levels_array[rnd]);
        pass &= sir_fileopts(id1, opts_array[rnd]);
        printf("\t" WHITE("set random config #%"PRIu32" for %s") "\n", rnd, logfile);

        pass &= filter_error(sir_debug("modified config #%"PRIu32" (debug)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_info("modified config #%"PRIu32" (info)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_notice("modified config #%"PRIu32" (notice)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_warn("modified config #%"PRIu32" (warning)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_error("modified config #%"PRIu32" (error)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_crit("modified config #%"PRIu32" (critical)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_alert("modified config #%"PRIu32" (alert)", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_emerg("modified config #%"PRIu32" (emergency)", rnd), SIR_E_NODEST);
    }

    if (pass) {
        /* restore to default config and run again */
        sir_stdoutlevels(SIRL_DEFAULT);
        sir_stderrlevels(SIRL_DEFAULT);
        sir_stdoutopts(SIRO_DEFAULT);
        sir_stderropts(SIRO_DEFAULT);

        pass &= sir_debug("default config (debug)");
        pass &= sir_info("default config (info)");
        pass &= sir_notice("default config (notice)");
        pass &= sir_warn("default config (warning)");
        pass &= sir_error("default config (error)");
        pass &= sir_crit("default config (critical)");
        pass &= sir_alert("default config (alert)");
        pass &= sir_emerg("default config (emergency)");
    }

    pass &= sir_remfile(id1);
    rmfile(logfile);
    sir_cleanup();

    return print_result_and_return(pass);
}

#if defined(SIR_SYSLOG_ENABLED) || defined(SIR_OS_LOG_ENABLED)
static bool generic_syslog_test(const char* sl_name, const char* identity, const char* category) {
    bool pass             = true;
    static const int runs = 5;

    /* repeat initializing, opening, logging, closing, cleaning up n times. */
    sir_timer timer = {0};
    pass &= sirtimerstart(&timer);

    printf("\trunning %d passes of random configs (system logger: '%s', "
           "identity: '%s', category: '%s')...\n",
        runs, sl_name, identity, category);

    for (int i = 0; i < runs; i++) {
        /* randomly skip setting process name, identity/category to thoroughly
         * test fallback routines; randomly update the config mid-run. */
        bool set_procname = getrand_bool((uint32_t)sirtimerelapsed(&timer));
        bool set_identity = getrand_bool((uint32_t)sirtimerelapsed(&timer));
        bool set_category = getrand_bool((uint32_t)sirtimerelapsed(&timer));
        bool do_update    = getrand_bool((uint32_t)sirtimerelapsed(&timer));

        printf("\tset_procname: %d, set_identity: %d, set_category: %d, do_update: %d\n",
            set_procname, set_identity, set_category, do_update);

        INIT_SL(si, SIRL_ALL, SIRO_NOHOST | SIRO_NOTID, 0, 0, (set_procname ? "sir_sltest" : ""));
        si.d_syslog.opts   = SIRO_DEFAULT;
        si.d_syslog.levels = SIRL_DEFAULT;

        if (set_identity)
            _sir_strncpy(si.d_syslog.identity, SIR_MAX_SYSLOG_CAT, identity, SIR_MAX_SYSLOG_ID);

        if (set_category)
            _sir_strncpy(si.d_syslog.category, SIR_MAX_SYSLOG_CAT, category, SIR_MAX_SYSLOG_CAT);

        si_init = sir_init(&si); //-V519
        pass &= si_init;

        if (do_update)
            pass &= sir_sysloglevels(SIRL_ALL);

        pass &= sir_debug("%d/%d: this debug message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_info("%d/%d: this info message sent to stdout and %s.", i + 1, runs, sl_name);

        pass &= sir_notice("%d/%d: this notice message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_warn("%d/%d: this warning message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_error("%d/%d: this error message sent to stdout and %s.", i + 1, runs, sl_name);

        if (set_identity) {
            pass &= sir_syslogid("my test ID");
            pass &= sir_syslogid("my test ID"); /* test deduping. */
        }

        if (set_category) {
            pass &= sir_syslogcat("my test category");
            pass &= sir_syslogcat("my test category"); /* test deduping. */
        }

        if (do_update)
            pass &= sir_syslogopts(SIRO_MSGONLY & ~(SIRO_NOLEVEL | SIRO_NOPID));

        pass &= sir_crit("%d/%d: this critical message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_alert("%d/%d: this alert message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_emerg("%d/%d: this emergency message sent to stdout and %s.", i + 1, runs, sl_name);

# if defined(SIR_OS_LOG_ENABLED)
#  if defined(__APPLE__) && !defined(__INTEL_COMPILER)
        if (i == runs -1 && 0 == strncmp(sl_name, "os_log", 6)) {
            printf("\ttesting os_log activity feature...\n");

            /* also test activity grouping in Console. there's only one way to validate
             * this and that's by manually viewing the log. */
             os_activity_t parent = os_activity_create("flying to the moon",
                OS_ACTIVITY_NONE, OS_ACTIVITY_FLAG_DETACHED);

            /* execution now passes to os_log_parent_activity(), where some logging
            * will occur, then a sub-activity will be created, and more logging. */
            os_activity_apply_f(parent, (void*)parent, os_log_parent_activity);
        }
#  endif
# endif

        sir_cleanup();

        if (!pass)
            break;
    }

    return print_result_and_return(pass);
}
#endif

#if defined(SIR_NO_SYSTEM_LOGGERS)
static bool generic_disabled_syslog_test(const char* sl_name, const char* identity,
    const char* category) {
    INIT_SL(si, SIRL_ALL, SIRO_NOHOST | SIRO_NOTID, 0, 0, "sir_disabled_sltest");
    si.d_syslog.opts   = SIRO_DEFAULT;
    si.d_syslog.levels = SIRL_DEFAULT;
    bool pass = true;

    SIR_UNUSED(sl_name);

    printf("\tSIR_NO_SYSTEM_LOGGERS is defined; expecting calls to fail...\n");

    /* init should just ignore the syslog settings. */
    pass &= sir_init(&si);

    /* these calls should all fail. */
    printf("\tsetting levels...\n");
    pass &= !sir_sysloglevels(SIRL_ALL);

    if (pass)
        print_expected_error();

    printf("\tsetting options...\n");
    pass &= !sir_syslogopts(SIRO_DEFAULT);

    if (pass)
        print_expected_error();

    printf("\tsetting identity...\n");
    pass &= !sir_syslogid(identity);

    if (pass)
        print_expected_error();

    printf("\tsetting category...\n");
    pass &= !sir_syslogcat(category);

    if (pass)
        print_expected_error();

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}
#endif

bool sirtest_syslog(void) {
#if !defined(SIR_SYSLOG_ENABLED)
# if defined(SIR_NO_SYSTEM_LOGGERS)
    bool pass = generic_disabled_syslog_test("syslog", "sirtests", "tests");
    return print_result_and_return(pass);
# else
    printf("\t" DGRAY("SIR_SYSLOG_ENABLED is not defined; skipping") "\n");
    return true;
# endif
#else
    bool pass = generic_syslog_test("syslog", "sirtests", "tests");
    return print_result_and_return(pass);
#endif
}

bool sirtest_os_log(void) {
#if !defined(SIR_OS_LOG_ENABLED)
    printf("\t" DGRAY("SIR_OS_LOG_ENABLED is not defined; skipping") "\n");
    return true;
#else
    bool pass = generic_syslog_test("os_log", "com.aremmell.libsir.tests", "tests");
    return print_result_and_return(pass);
#endif
}

char *_sirtest_wine(void) {
#if defined(__WIN__)
# if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4152)
# endif
    static const char* (CDECL* _p_wine_get_version)(void);
    HMODULE _h_ntdll;

    _h_ntdll = GetModuleHandle("ntdll.dll");
    if (_h_ntdll != NULL) {
# if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
# endif
        _p_wine_get_version = (void *)GetProcAddressA(_h_ntdll, "wine_get_version");
# if defined(__GNUC__)
#  pragma GCC diagnostic pop
# endif
        char *wine_version = (char *)_p_wine_get_version();
        if (wine_version)
            return wine_version;
    }
# if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
# endif
#endif
    return NULL;
}

bool sirtest_filesystem(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    /* Wine version */
    printf("\t_sirtest_wine: '%s'\n", PRN_STR(_sirtest_wine()));

    /* current working directory. */
    char* cwd = _sir_getcwd();
    pass &= NULL != cwd;
    printf("\t_sir_getcwd: '%s'\n", PRN_STR(cwd));

    if (NULL != cwd) {
        /* path to this binary file. */
        char* filename = _sir_getappfilename();
        pass &= NULL != filename;
        printf("\t_sir_getappfilename: '%s'\n", PRN_STR(filename));

        if (NULL != filename) {
            /* _sir_get[base|dir]name() can potentially modify filename,
             * so make a copy for each call. */
            char* filename2 = strndup(filename, strnlen(filename, SIR_MAXPATH));
            pass &= NULL != filename2;

            if (NULL != filename2) {
                /* filename, stripped of directory component(s). */
                char* _basename = _sir_getbasename(filename2);
                printf("\t_sir_getbasename: '%s'\n", PRN_STR(_basename));

                if (!_basename) {
                    pass = false;
                } else {
                    /* the last strlen(_basename) chars of filename should match. */
                    size_t len    = strnlen(_basename, SIR_MAXPATH);
                    size_t offset = strnlen(filename, SIR_MAXPATH) - len;
                    size_t n      = 0;

                    while (n < len) {
                        if (filename[offset++] != _basename[n++]) {
                            pass = false;
                            break;
                        }
                    };
                }
            }

            /* directory this binary file resides in. */
            char* appdir = _sir_getappdir();
            pass &= NULL != appdir;
            printf("\t_sir_getappdir: '%s'\n", PRN_STR(appdir));

            /* _sir_get[base|dir]name can potentially modify filename,
             * so make a copy for each call. */
            char* filename3 = strndup(filename, strnlen(filename, SIR_MAXPATH));
            pass &= NULL != filename3;

            if (NULL != appdir && NULL != filename3) {
                /* should yield the same result as _sir_getappdir(). */
                char* _dirname = _sir_getdirname(filename3);
                printf("\t_sir_getdirname: '%s'\n", PRN_STR(_dirname));

                pass &= 0 == strncmp(filename, appdir, strnlen(appdir, SIR_MAXPATH));
                pass &= NULL != _dirname &&
                    0 == strncmp(filename, _dirname, strnlen(_dirname, SIR_MAXPATH));
            }

            _sir_safefree(&appdir);
            _sir_safefree(&filename);
            _sir_safefree(&filename2);
            _sir_safefree(&filename3);
        }

        _sir_safefree(&cwd);
    }

    /* this next section doesn't really yield any useful boolean pass/fail
     * information, but could be helpful to review manually. */
    char* dubious_dirnames[] = {
#if !defined(__WIN__)
        "/foo",
        "/foo/",
        "/foo/bar",
        "/foo/bar/bad:filename",
        "/",
        ""
#else /* __WIN__ */
        "C:\\foo",
        "C:\\foo\\",
        "C:\\foo\\bar",
        "C:\\foo\\bar\\bad:>filename",
        "C:\\",
        "//network-share/myfolder",
        ""
#endif
    };

    for (size_t n = 0; n < _sir_countof(dubious_dirnames); n++) {
        char* tmp = strndup(dubious_dirnames[n], strnlen(dubious_dirnames[n], SIR_MAXPATH));
        if (NULL != tmp) {
            printf("\t_sir_getdirname(" WHITE("'%s'") ") = " WHITE("'%s'") "\n",
                tmp, _sir_getdirname(tmp));
            _sir_safefree(&tmp);
        }
    }

    char* dubious_filenames[] = {
#if !defined(__WIN__)
        "foo/bar/file-or-directory",
        "/foo/bar/file-or-directory",
        "/foo/bar/illegal:filename",
        "/",
        ""
#else /* __WIN__ */
        "foo\\bar\\file.with.many.full.stops",
        "C:\\foo\\bar\\poorly-renamed.txt.pdf",
        "C:\\foo\\bar\\illegal>filename.txt",
        "C:\\",
        "\\Program Files\\foo.bar",
        ""
#endif
    };

    for (size_t n = 0; n < _sir_countof(dubious_filenames); n++) {
        char* tmp = strndup(dubious_filenames[n], strnlen(dubious_filenames[n], SIR_MAXPATH));
        if (NULL != tmp) {
            printf("\t_sir_getbasename(" WHITE("'%s'") ") = " WHITE("'%s'") "\n",
                tmp, _sir_getbasename(tmp));
            _sir_safefree(&tmp);
        }
    }

    /* absolute/relative paths. */
    static const struct {
        const char* const path;
        bool abs;
    } abs_or_rel_paths[] = {
        {"this/is/relative", false},
        {"relative", false},
        {"./relative", false},
        {"../../relative", false},
#if !defined(__WIN__)
        {"/usr/local/bin", true},
        {"/", true},
        {"/home/foo/.config", true},
        {"~/.config", true}
#else /* __WIN__ */
        {"D:\\absolute", true},
        {"C:\\Program Files\\FooBar", true},
        {"C:\\", true},
        {"\\absolute", true},
#endif
    };

    for (size_t n = 0; n < _sir_countof(abs_or_rel_paths); n++) {
        bool relative = false;
        bool ret      = _sir_ispathrelative(abs_or_rel_paths[n].path, &relative);

        pass &= ret;
        if (!ret) {
            bool unused = print_test_error(false, false);
            SIR_UNUSED(unused);
            continue;
        }

        if (relative == abs_or_rel_paths[n].abs) {
            pass = false;
            printf("\t" RED("_sir_ispathrelative('%s') = %s") "\n", abs_or_rel_paths[n].path,
                relative ? "true" : "false");
        } else {
            printf("\t" GREEN("_sir_ispathrelative('%s') = %s") "\n", abs_or_rel_paths[n].path,
                relative ? "true" : "false");
        }
    }

    /* file existence. */
    static const struct {
        const char* const path;
        bool exists;
    } real_or_not[] = {
        {"../foobarbaz", false},
        {"foobarbaz", false},
#if !defined(__WIN__)
        {"/", true},
# if !defined(__HAIKU__)
        {"/usr/bin", true},
# else
        {"/bin", true},
# endif
        {"/dev", true},
#else /* __WIN__ */
        {"C:\\Windows", true},
        {"C:\\Program Files", true},
#endif
        {"../../LICENSES/MIT.txt", true},
        {"../../msvs/libsir.sln", true},
        {"../", true},
        {"file.exists", true}
    };

    for (size_t n = 0; n < _sir_countof(real_or_not); n++) {
        bool exists = false;
        bool ret    = _sir_pathexists(real_or_not[n].path, &exists, SIR_PATH_REL_TO_APP);

        pass &= ret;
        if (!ret) {
            bool unused = print_test_error(false, false);
            SIR_UNUSED(unused);
            continue;
        }

        if (exists != real_or_not[n].exists) {
            pass = false;
            printf("\t" RED("_sir_pathexists('%s') = %s") "\n", real_or_not[n].path,
                exists ? "true" : "false");
        } else {
            printf("\t" GREEN("_sir_pathexists('%s') = %s") "\n", real_or_not[n].path,
                exists ? "true" : "false");
        }
    }

    /* checking file descriptors. */
    static const int bad_fds[] = {
        0,
        1,
        2,
        1234
    };

    for (size_t n = 0; n < _sir_countof(bad_fds); n++) {
        if (_sir_validfd(bad_fds[n])) {
            pass = false;
            printf("\t" RED("_sir_validfd(%d) = true") "\n", bad_fds[n]);
        } else {
            printf("\t" GREEN("_sir_validfd(%d) = false") "\n", bad_fds[n]);
        }
    }

    FILE* f = NULL;
    bool ret = _sir_openfile(&f, "file.exists", "r", SIR_PATH_REL_TO_APP);
    if (!ret) {
        pass = false;
        handle_os_error(true, "fopen(%s) failed!", "file.exists");
    } else {
        int fd = fileno(f);
        if (!_sir_validfd(fd)) {
            pass = false;
            printf("\t" RED("_sir_validfd(%d) = false") "\n", fd);
        } else {
            printf("\t" GREEN("_sir_validfd(%d) = true") "\n", fd);
        }
    }

    _sir_safefclose(&f);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_squelchspam(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t alternate   = 50;
    static const size_t sequence[3] = {
        1000, /* non-repeating messages. */
        1000, /* repeating messages. */
        1000  /* alternating repeating and non-repeating messages. */
    };

    sir_timer timer;
    sirtimerstart(&timer);

    printf("\t" BLUE("%zu non-repeating messages...") "\n", sequence[0]);

    for (size_t n = 0, ascii_idx = 33; n < sequence[0]; n++, ascii_idx++) {
        pass &= sir_debug("%c%c a non-repeating message", (char)ascii_idx,
            (char)ascii_idx + 1);

        if (ascii_idx == 125)
            ascii_idx = 33;
    }

    printf("\t" BLUE("%zu repeating messages...") "\n", sequence[1]);

    for (size_t n = 0; n < sequence[1]; n++) {
        bool ret = sir_debug("a repeating message");

        if (n >= SIR_SQUELCH_THRESHOLD - 1)
            pass &= !ret;
        else
            pass &= ret;
    }

    printf("\t" BLUE("%zu alternating repeating and non-repeating messages...")
           "\n", sequence[2]);

    bool repeating   = false;
    size_t counter   = 0;
    size_t repeat_id = 0;
    for (size_t n = 0, ascii_idx = 33; n < sequence[2]; n++, counter++, ascii_idx++) {
        if (!repeating) {
            pass &= sir_debug("%c%c a non-repeating message", (char)ascii_idx,
                (char)ascii_idx + 1);
        } else {
            bool ret = sir_debug("%zu a repeating message", repeat_id);

            if (counter - 1 >= SIR_SQUELCH_THRESHOLD - 1)
                pass &= !ret;
            else
                pass &= ret;
        }

        if (counter == alternate) {
            repeating = !repeating;
            counter = 0;
            repeat_id++;
        }

        if (ascii_idx == 125)
            ascii_idx = 33;
    }

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_pluginloader(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

#if !defined(__WIN__)
# define PLUGIN_EXT "so"
#else
# define PLUGIN_EXT "dll"
#endif

    static const char* plugin1 = "build/lib/plugin_dummy."PLUGIN_EXT;
    static const char* plugin2 = "build/lib/plugin_dummy_bad1."PLUGIN_EXT;
    static const char* plugin3 = "build/lib/plugin_dummy_bad2."PLUGIN_EXT;
    static const char* plugin4 = "build/lib/plugin_dummy_bad3."PLUGIN_EXT;
    static const char* plugin5 = "build/lib/plugin_dummy_bad4."PLUGIN_EXT;
    static const char* plugin6 = "build/lib/plugin_dummy_bad5."PLUGIN_EXT;
    static const char* plugin7 = "build/lib/plugin_dummy_bad6."PLUGIN_EXT;
    static const char* plugin8 = "build/lib/i_dont_exist."PLUGIN_EXT;

#if defined(SIR_NO_PLUGINS)
    SIR_UNUSED(plugin2);
    SIR_UNUSED(plugin3);
    SIR_UNUSED(plugin4);
    SIR_UNUSED(plugin5);
    SIR_UNUSED(plugin6);
    SIR_UNUSED(plugin7);
    SIR_UNUSED(plugin8);

    printf("\tSIR_NO_PLUGINS is defined; expecting calls to fail\n");

    printf("\tloading good plugin: '%s'...\n", plugin1);
    /* load a valid, well-behaved plugin. */
    sirpluginid id = sir_loadplugin(plugin1);
    pass &= 0 == id;

    if (pass)
        print_expected_error();

    printf("\tunloading good plugin: '%s'...\n", plugin1);
    /* also try the unload function. */
    pass &= !sir_unloadplugin(id);

    if (pass)
        print_expected_error();
#else
    /* load a valid, well-behaved plugin. */
    printf("\tloading good plugin: '%s'...\n", plugin1);
    sirpluginid id = sir_loadplugin(plugin1);
    pass &= 0 != id;

    print_test_error(pass, pass);

    pass &= sir_info("this message will be dispatched to the plugin.");
    pass &= sir_warn("this message will *not* be dispatched to the plugin.");

    /* re-loading the same plugin should fail. */
    printf("\tloading duplicate plugin: '%s'...\n", plugin1);
    sirpluginid badid = sir_loadplugin(plugin1);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    /* the following are all invalid or misbehaved, and should all fail. */
    printf("\tloading bad plugin: '%s'...\n", plugin2);
    badid = sir_loadplugin(plugin2);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    printf("\tloading bad plugin: '%s'...\n", plugin3);
    badid = sir_loadplugin(plugin3);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    printf("\tloading bad plugin: '%s'...\n", plugin4);
    badid = sir_loadplugin(plugin4);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    printf("\tloading bad plugin: '%s'...\n", plugin5);
    badid = sir_loadplugin(plugin5);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    printf("\tloading bad plugin: '%s'...\n", plugin6);
    badid = sir_loadplugin(plugin6);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    printf("\tloading bad plugin: '%s'...\n", plugin7);
    badid = sir_loadplugin(plugin7);
    pass &= 0 != badid; /* this one should load, just return false from write */

    pass &= !sir_info("this should fail, because one plugin failed to process"
                      " the message.");

    print_test_error(pass, pass);

    printf("\tloading nonexistent plugin: '%s'...\n", plugin8);
    badid = sir_loadplugin(plugin8);
    pass &= 0 == badid;

    print_test_error(pass, pass);

    /* unload the good plugin manually. */
    printf("\tunloading good plugin: '%s'...\n", plugin1);
    pass &= sir_unloadplugin(id);

    print_test_error(pass, pass);

    /* try to unload the plugin again. */
    printf("\nunloading already unloaded plugin '%s'...\n", plugin1);
    pass &= !sir_unloadplugin(id);

    print_test_error(pass, pass);

    /* test bad paths. */
    printf("\ntrying to load plugin with NULL path...\n");
    badid = sir_loadplugin(NULL);
    pass &= 0 == badid;

    print_test_error(pass, pass);
#endif
    pass &= sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_getversioninfo(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    printf("\tchecking version retrieval functions...\n");

    const char* str = sir_getversionstring();
    pass &= _sir_validstrnofail(str);

    printf("\tversion as string: '%s'\n", _SIR_PRNSTR(str));

    uint32_t hex = sir_getversionhex();
    pass &= 0 != hex;

    printf("\tversion as hex: 0x%08"PRIx32"\n", hex);

    bool prerel = sir_isprerelease();
    printf("\tprerelease: %s\n", prerel ? "true" : "false");

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}

enum {
    NUM_THREADS = 4
};

static bool threadpool_pseudojob(void* arg) {
    sir_debug("this is a pseudo job that actually does nothing (arg: %p)", arg);
#if !defined(__WIN__)
    sleep(1);
#else
    Sleep(1000);
#endif
    return true;
}

bool sirtest_threadpool(void) {
    INIT(si, SIRL_ALL, SIRO_NOTIME | SIRO_NOHOST | SIRO_NONAME, 0, 0);
    bool pass = si_init;

    static const size_t num_jobs = 30;
    sir_threadpool* pool         = NULL;

    pass &= _sir_threadpool_create(&pool, NUM_THREADS);
    if (pass) {
        /* dispatch a whole bunch of jobs. */
        for (size_t n = 0; n < num_jobs; n++) {
            sir_threadpool_job* job = calloc(1, sizeof(sir_threadpool_job));
            pass &= NULL != job;
            if (job) {
                job->fn = &threadpool_pseudojob;
                job->data = (void*)(n + 1);
                pass &= _sir_threadpool_add_job(pool, job);
                pass &= sir_info("dispatched job (fn: %"PRIxPTR", data: %p)",
                    (uintptr_t)job->fn, job->data);
            }
        }

#if !defined(__WIN__)
        sleep(1);
#else
        Sleep(1000);
#endif

        pass &= sir_info("destroying thread pool...");
        pass &= _sir_threadpool_destroy(&pool);
    }

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}

#if !defined(__WIN__)
static void* threadrace_thread(void* arg);
#else /* __WIN__ */
static unsigned threadrace_thread(void* arg);
#endif

bool sirtest_threadrace(void) {
#if !defined(__WIN__)
    pthread_t thrds[NUM_THREADS] = {0};
#else /* __WIN__ */
    uintptr_t thrds[NUM_THREADS] = {0};
#endif

    INIT_N(si, SIRL_DEFAULT, SIRO_NOPID | SIRO_NOHOST, 0, 0, "thread-race");
    bool pass           = si_init;
    bool any_created    = false;
    size_t last_created = 0;

    thread_args* heap_args = (thread_args*)calloc(NUM_THREADS, sizeof(thread_args));
    pass &= NULL != heap_args;
    if (!heap_args) {
        handle_os_error(true, "calloc(%zu) bytes failed!", NUM_THREADS * sizeof(thread_args));
        return false;
    }

    for (size_t n = 0; n < NUM_THREADS; n++) {
        if (!pass)
            break;

        heap_args[n].pass = true;
        (void)snprintf(heap_args[n].log_file, SIR_MAXPATH,
            MAKE_LOG_NAME("multi-thread-race-%zu.log"), n);

#if !defined(__WIN__)
        int create = pthread_create(&thrds[n], NULL, threadrace_thread, (void*)&heap_args[n]);
        if (0 != create) {
            errno = create;
            handle_os_error(true, "pthread_create() for thread #%zu failed!", n + 1);
#else /* __WIN__ */
        thrds[n] = _beginthreadex(NULL, 0, threadrace_thread, (void*)&heap_args[n], 0, NULL);
        if (0 == thrds[n]) {
            handle_os_error(true, "_beginthreadex() for thread #%zu failed!", n + 1);
#endif
            pass = false;
            break;
        }

        last_created = n;
        any_created  = true;
    }

    if (any_created) {
        for (size_t j = 0; j < last_created + 1; j++) {
            bool joined = true;
            printf("\twaiting for thread %zu/%zu...\n", j + 1, last_created + 1);
#if !defined(__WIN__)
            int join = pthread_join(thrds[j], NULL);
            if (0 != join) {
                joined = false;
                errno  = join;
                handle_os_error(true, "pthread_join() for thread #%zu failed!", j + 1);
            }
#else /* __WIN__ */
            DWORD wait = WaitForSingleObject((HANDLE)thrds[j], INFINITE);
            if (WAIT_OBJECT_0 != wait) {
                joined = false;
                handle_os_error(false, "WaitForSingleObject() for thread #%zu (%p) failed!", j + 1,
                    (HANDLE)thrds[j]);
            }
#endif
            pass &= joined;
            if (joined) {
                printf("\tthread %zu/%zu joined\n", j + 1, last_created + 1);

                pass &= heap_args[j].pass;
                if (heap_args[j].pass)
                    printf("\t" GREEN("thread #%zu returned pass = true") "\n", j + 1);
                else
                    printf("\t" RED("thread #%zu returned pass = false!") "\n", j + 1);
            }
        }
    }

    _sir_safefree(&heap_args);

    sir_cleanup();
    return print_result_and_return(pass);
}

#if !defined(__WIN__)
static void* threadrace_thread(void* arg) {
#else /* __WIN__ */
unsigned threadrace_thread(void* arg) {
#endif
    pid_t threadid       = _sir_gettid();
    thread_args* my_args = (thread_args*)arg;

    rmfile(my_args->log_file);
    sirfileid id = sir_addfile(my_args->log_file, SIRL_ALL, SIRO_MSGONLY);

    if (0 == id) {
        bool unused = print_test_error(false, false);
        SIR_UNUSED(unused);
#if !defined(__WIN__)
        return NULL;
#else /* __WIN__ */
        return 0;
#endif
    }

    printf("\thi, i'm thread (id: %d), logging to: '%s'...\n",
            PID_CAST threadid, my_args->log_file);

#if !defined(DUMA)
# define NUM_ITERATIONS 1000
#else
# define NUM_ITERATIONS 100
#endif

    for (size_t n = 0; n < NUM_ITERATIONS; n++) {
        /* choose a random level, and colors. */
        sir_textcolor fg = SIRTC_INVALID;
        sir_textcolor bg = SIRTC_INVALID;

        if (n % 2 == 0) {
            fg = SIRTC_CYAN;
            bg = SIRTC_BLACK;
            sir_debug("log message #%zu", n);
        } else {
            fg = SIRTC_BLACK;
            bg = SIRTC_CYAN;
            sir_info("log message #%zu", n);
        }

        /* sometimes remove and re-add the log file, and set some options/styles.
         * other times, just set different options/styles. */
        uint32_t rnd = (uint32_t)(n + threadid);
        if (getrand_bool((uint32_t)(rnd > 1 ? rnd : 1))) {
            my_args->pass = print_test_error(sir_remfile(id), false);
            my_args->pass = print_test_error(0 != sir_addfile(my_args->log_file,
                SIRL_ALL, SIRO_MSGONLY), false);

            bool test = sir_settextstyle(SIRL_DEBUG, SIRTA_EMPH, fg, bg) &&
                        sir_settextstyle(SIRL_INFO, SIRTA_BOLD, fg, bg);
            my_args->pass = print_test_error(test, false);

            test = sir_stdoutopts(SIRO_NONAME | SIRO_NOHOST | SIRO_NOMSEC);
            my_args->pass = print_test_error(test, false);
        } else {
            bool test = sir_settextstyle(SIRL_DEBUG, SIRTA_ULINE, fg, bg) &&
                        sir_settextstyle(SIRL_INFO, SIRTA_NORMAL, fg, bg);
            my_args->pass = print_test_error(test, false);

            test = sir_fileopts(id, SIRO_NOPID | SIRO_NOHOST);
            my_args->pass = print_test_error(test, false);

            test = sir_stdoutopts(SIRO_NOTIME | SIRO_NOLEVEL);
            my_args->pass = print_test_error(test, false);
        }

        if (!my_args->pass)
            break;
    }

    my_args->pass = print_test_error(sir_remfile(id), false);

    rmfile(my_args->log_file);

#if !defined(__WIN__)
    return NULL;
#else /* __WIN__ */
    return 0;
#endif
}

/*
bool sirtest_XXX(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= sir_cleanup();
    return print_result_and_return(pass);
}
*/

/* ========================== end tests ========================== */

bool print_test_error(bool result, bool expected) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);

    if (!expected && !result && SIR_E_NOERROR != code)
        printf("\t" RED("!! Unexpected (%"PRIu16", %s)") "\n", code, message);
    else if (expected && SIR_E_NOERROR != code)
        printf("\t" GREEN("Expected (%"PRIu16", %s)") "\n", code, message);

    return result;
}

bool print_os_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    fprintf(stderr, "\t" RED("OS error: (%"PRIu16", %s)") "\n", code, message);
    return false;
}

bool filter_error(bool pass, uint16_t err) {
    if (!pass) {
        char message[SIR_MAXERROR] = {0};
        uint16_t code              = sir_geterror(message);
        if (code != err)
            return false;
    }
    return true;
}

uint32_t getrand(uint32_t upper_bound) {
#if !defined(__WIN__)
# if defined(__MACOS__) || defined(__BSD__)
    return arc4random_uniform(upper_bound);
# else
    return (uint32_t)(random() % upper_bound);
# endif
#else /* __WIN__ */
    uint32_t ctx = 0;
    if (0 != rand_s(&ctx))
        ctx = (uint32_t)rand();
    return ctx % upper_bound;
#endif
}

bool rmfile(const char* filename) {
    bool removed = false;

    /* return true if leave_logs is true. */
    if (leave_logs) {
        printf("\t" WHITE("not deleting '%s' due to '%s'") "\n",
            filename, _cl_arg_list[3].flag);
        return true;
    }

    /* return true if the file doesn't exist. */
    struct stat st;
    if (0 != stat(filename, &st)) {
        if (ENOENT == errno)
            return true;

        handle_os_error(true, "failed to stat %s!", filename);
        return false;
    }

    if (!_sir_deletefile(filename)) {
        handle_os_error(false, "failed to delete %s!", filename);
    } else {
        printf("\t" DGRAY("deleted %s (%ld bytes)...") "\n", filename,
            (long)st.st_size);
    }

    return removed;
}

void deletefiles(const char* search, const char* path, const char* filename, unsigned* data) {
    if (strstr(filename, search)) {
        char filepath[SIR_MAXPATH];
        (void)snprintf(filepath, SIR_MAXPATH, "%s%s", path, filename);

        rmfile(filepath);
        (*data)++;
    }
}

void countfiles(const char* search, const char* path, const char* filename, unsigned* data) {
    SIR_UNUSED(path);
    if (strstr(filename, search))
        (*data)++;
}

bool enumfiles(const char* path, const char* search, fileenumproc cb, unsigned* data) {
#if !defined(__WIN__)
    DIR* d = opendir(path);
    if (!d)
        return print_os_error();

    rewinddir(d);
    struct dirent* di = readdir(d);
    if (!di) {
        closedir(d);
        return print_os_error();
    }

    while (NULL != di) {
        cb(search, path, di->d_name, data);
        di = readdir(d);
    };

    closedir(d);
    d = NULL;
#else /* __WIN__ */
    WIN32_FIND_DATA finddata = {0};
    char buf[SIR_MAXPATH]    = {0};

    (void)snprintf(buf, SIR_MAXPATH, "%s/*", path);

    HANDLE enumerator = FindFirstFile(buf, &finddata);

    if (INVALID_HANDLE_VALUE == enumerator)
        return false;

    do {
        cb(search, path, finddata.cFileName, data);
    } while (FindNextFile(enumerator, &finddata) > 0);

    FindClose(enumerator);
    enumerator = NULL;
#endif

    return true;
}

bool sirtimerstart(sir_timer* timer) {
#if !defined(__WIN__) || defined(__ORANGEC__)
    int gettime = clock_gettime(SIRTEST_CLOCK, &timer->ts);
    if (0 != gettime) {
        handle_os_error(true, "clock_gettime(%d) failed!", CLOCK_CAST SIRTEST_CLOCK);
    }

    return 0 == gettime;
#else /* __WIN__ */
    GetSystemTimePreciseAsFileTime(&timer->ft);
    return true;
#endif
}

float sirtimerelapsed(const sir_timer* timer) {
#if !defined(__WIN__) || defined(__ORANGEC__)
    struct timespec now;
    if (0 == clock_gettime(SIRTEST_CLOCK, &now)) {
        return (float)(((double)now.tv_sec * (double)1e3)
            + ((double)now.tv_nsec / (double)1e6)
            - ((double)timer->ts.tv_sec * (double)1e3)
            + ((double)timer->ts.tv_nsec / (double)1e6));
    } else {
        handle_os_error(true, "clock_gettime(%d) failed!", CLOCK_CAST SIRTEST_CLOCK);
    }
    return 0.0f;
#else /* __WIN__ */
    FILETIME now;
    GetSystemTimePreciseAsFileTime(&now);
    ULARGE_INTEGER start   = {0};
    start.LowPart          = timer->ft.dwLowDateTime;
    start.HighPart         = timer->ft.dwHighDateTime;
    ULARGE_INTEGER n100sec = {0};
    n100sec.LowPart        = now.dwLowDateTime;
    n100sec.HighPart       = now.dwHighDateTime;
    return (float)((n100sec.QuadPart - start.QuadPart) / 1e4);
#endif
}

long sirtimergetres(void) {
    long retval = 0;
#if !defined(__WIN__)
    struct timespec res;
    if (0 == clock_getres(SIRTEST_CLOCK, &res)) {
        retval = res.tv_nsec;
    } else {
        handle_os_error(true, "clock_getres(%d) failed!", CLOCK_CAST SIRTEST_CLOCK); // GCOVR_EXCL_LINE
    }
#else /* __WIN__ */
    retval = 100;
#endif
    return retval;
}

#if defined(SIR_OS_LOG_ENABLED)
void os_log_parent_activity(void* ctx) {
    sir_debug("confirming with ground control that we are a go...");
    sir_info("all systems go; initiating launch sequence");
    sir_warn("getting some excessive vibration here");
    sir_info("safely reached escape velocity. catch you on the flip side");
    sir_info("(3 days later) we have landed on the lunar surface");
    sir_notice("beginning rock counting...");

    os_activity_t parent = (os_activity_t)ctx;
    os_activity_t child = os_activity_create("counting moon rocks", parent,
        OS_ACTIVITY_FLAG_DEFAULT);

    float rock_count = 0.0f;
    os_activity_apply_f(child, (void*)&rock_count, os_log_child_activity);
    sir_info("astronauts safely back on board. official count: ~%.02f moon rocks",
        rock_count);
}

void os_log_child_activity(void* ctx) {
    sir_info("there are a lot of rocks here; we're going to be here a while");

    for (size_t n = 0; n < 10; n++) {
        sir_info("counting rocks in sector %zu...", n);
    }

    float* rock_count = (float*)ctx;
    *rock_count = 1e12;
    sir_info("all sectors counted; heading back to the lunar lander");
}
#endif

bool mark_test_to_run(const char* name) {
    bool found = false;
    for (size_t t = 0; t < _sir_countof(sir_tests); t++) {
        if (_sir_strsame(name, sir_tests[t].name,
            strnlen(sir_tests[t].name, SIR_MAXTESTNAME))) {
            found = sir_tests[t].run = true;
            break;
        }
    }

    if (!found)
        _sir_selflog("warning: unable to locate '%s' in test array", name); // GCOVR_EXCL_LINE

    return found;
}

void print_usage_info(void) {
    size_t longest = 0;
    for (size_t i = 0; i < _sir_countof(_cl_arg_list); i++) {
        size_t len = strnlen(_cl_arg_list[i].flag, SIR_MAXCLIFLAG);
        if (len > longest)
            longest = len;
    }

    fprintf(stderr, "\n" WHITE("Usage:") "\n\n");

    for (size_t i = 0; i < _sir_countof(_cl_arg_list); i++) {
        fprintf(stderr, "\t%s ", _cl_arg_list[i].flag);

        size_t len = strnlen(_cl_arg_list[i].flag, SIR_MAXCLIFLAG);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                fprintf(stderr, " ");

        fprintf(stderr, "%s%s%s\n", _cl_arg_list[i].usage,
            strnlen(_cl_arg_list[i].usage, SIR_MAXUSAGE) > 0 ? " " : "",
            _cl_arg_list[i].desc);
    }

    fprintf(stderr, "\n");
}

void print_test_list(void) {
    size_t longest = 0;
    for (size_t i = 0; i < _sir_countof(sir_tests); i++) {
        size_t len = strnlen(sir_tests[i].name, SIR_MAXTESTNAME);
        if (len > longest)
            longest = len;
    }

    printf("\n" WHITE("Available tests:") "\n\n");

    for (size_t i = 0; i < _sir_countof(sir_tests); i++) {
        printf("\t%s\t", sir_tests[i].name);

        size_t len = strnlen(sir_tests[i].name, SIR_MAXTESTNAME);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                printf(" ");

        if ((i % 2) != 0 || i == _sir_countof(sir_tests) - 1)
            printf("\n");
    }

    printf("\n");
}

void print_libsir_version(void) {
    printf("\n"ULINE("libsir") " %s (%s)\n\n", sir_getversionstring(),
        sir_isprerelease() ? "prerelease" : "release");
}
