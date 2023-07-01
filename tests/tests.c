/*
 * tests.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "tests.h"

static sir_test sir_tests[] = {
    {"performance",             sirtest_perf, false, true},
    {"thread-race",             sirtest_mthread_race, false, true},
    {"exceed-max-buffer-size",  sirtest_exceedmaxsize, false, true},
    {"no-output-destination",   sirtest_failnooutputdest, false, true},
    {"null-pointers",           sirtest_failnulls, false, true},
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
    {"cleanup-output-after",    sirtest_failaftercleanup, false, true},
    {"sanity-errors",           sirtest_errorsanity, false, true},
    {"sanity-text-styles",      sirtest_textstylesanity, false, true},
    {"sanity-options",          sirtest_optionssanity, false, true},
    {"sanity-levels",           sirtest_levelssanity, false, true},
    {"sanity-update-config",    sirtest_updatesanity, false, true},
    {"syslog",                  sirtest_syslog, false, true},
    {"os_log",                  sirtest_os_log, false, true},
    {"filesystem",              sirtest_filesystem, false, true}
};

int main(int argc, char** argv) {
#if !defined(__WIN__)
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

    bool wait  = false;
    bool only  = false;
    size_t to_run = 0;

    for (int n = 1; n < argc; n++) {
        if (_sir_strsame(argv[n], _cl_arg_list[0].flag, strlen(_cl_arg_list[0].flag))) {
            only = mark_test_to_run("performance");
            if (only)
                to_run = 1;
        } else if (_sir_strsame(argv[n], _cl_arg_list[1].flag, strlen(argv[n]))) {
            wait = true;
        } else if (_sir_strsame(argv[n], _cl_arg_list[3].flag, strlen(argv[n]))) {
            print_test_list();
            return EXIT_SUCCESS;
        } else if (_sir_strsame(argv[n], _cl_arg_list[4].flag, strlen(argv[n]))) {
            print_usage_info();
            return EXIT_SUCCESS;
        } else if (_sir_strsame(argv[n], _cl_arg_list[2].flag, strlen(argv[n]))) {
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
            only = true;
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

    printf(WHITEB("\nrunning %zu " ULINE("libsir") " %s...") "\n", tgt_tests, TEST_S(tgt_tests));
    startsirtimer(&timer);

    for (size_t n = first; n < _sir_countof(sir_tests); n++) {
        if (only && !sir_tests[n].run) {
            _sir_selflog("skipping '%s'; not marked to run", sir_tests[n].name);
            continue;
        }

        printf(WHITEB("\n\t(%zu/%zu) '%s'...") "\n\n", ran, tgt_tests, sir_tests[n].name);

        sir_tests[n].pass = sir_tests[n].fn();
        if (sir_tests[n].pass)
            passed++;

        ran++;

        printf(WHITEB("\n\t(%zu/%zu) '%s' finished: ") "%s\n", ran, tgt_tests, sir_tests[n].name,
            PRN_PASS(sir_tests[n].pass));
    }

    float elapsed = sirtimerelapsed(&timer);

    if (passed == tgt_tests) {
        printf("\n" WHITEB("done: ")
                   GREENB("%s%zu " ULINE("libsir") " %s passed in %.03fsec!") "\n\n",
            tgt_tests > 1 ? "all " : "", tgt_tests, TEST_S(tgt_tests), elapsed / 1e3);
    } else {
        printf("\n" WHITEB("done: ")
                   REDB("%zu of %zu " ULINE("libsir") " %s failed in %.03fsec") "\n\n",
            tgt_tests - passed, tgt_tests, TEST_S(tgt_tests), elapsed / 1e3);

        printf(REDB("Failed %s:") "\n\n", TEST_S(tgt_tests - passed));

        for (size_t t = 0; t < _sir_countof(sir_tests); t++)
            if (!sir_tests[t].pass)
                printf(RED(INDENT_ITEM "%s\n"), sir_tests[t].name);
        printf("\n");
    }

    if (wait) {
        printf(WHITEB(EMPH("press any key to exit...")) "\n");
        int ch = _sir_getchar();
        _SIR_UNUSED(ch);
    }

    return passed == tgt_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool sirtest_exceedmaxsize(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    char toobig[SIR_MAXMESSAGE + 100] = {0};
    memset(toobig, 'a', SIR_MAXMESSAGE - 99);

    pass &= sir_info(toobig);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failnooutputdest(void) {
    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    static const char* logfilename = "nodestination.log";

    pass &= !sir_info("this goes nowhere!");

    if (pass) {
        print_expected_error();

        pass &= sir_stdoutlevels(SIRL_INFO);
        pass &= sir_info("this goes to stdout");
        pass &= sir_stdoutlevels(SIRL_NONE);

        sirfileid fid = sir_addfile(logfilename, SIRL_INFO, SIRO_DEFAULT);
        pass &= NULL != fid;
        pass &= sir_info("this goes to %s", logfilename);
        pass &= sir_filelevels(fid, SIRL_NONE);
        pass &= !sir_info("this goes nowhere!");

        if (NULL != fid)
            pass &= sir_remfile(fid);

        rmfile(logfilename);
    }

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failnulls(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= !sir_info(NULL);
    pass &= NULL == sir_addfile(NULL, SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_filecachesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    size_t numfiles               = SIR_MAXFILES + 1;
    sirfileid ids[SIR_MAXFILES] = {0};

    sir_options even = SIRO_MSGONLY;
    sir_options odd  = SIRO_ALL;

    for (size_t n = 0; n < numfiles - 1; n++) {
        char path[SIR_MAXPATH] = {0};
        snprintf(path, SIR_MAXPATH, "test-%zu.log", n);
        rmfile(path);
        ids[n] = sir_addfile(path, SIRL_ALL, (n % 2) ? odd : even);
        pass &= NULL != ids[n] && sir_info("test %u", n);
    }

    pass &= sir_info("test test test");

    /* this one should fail; max files already added. */
    pass &= NULL == sir_addfile("should-fail.log", SIRL_ALL, SIRO_MSGONLY);

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
        snprintf(path, SIR_MAXPATH, "test-%zu.log", removeorder[n]);
        rmfile(path);
    }

    pass &= sir_info("test test test");

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failinvalidfilename(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    pass &= NULL == sir_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY);

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
    static const char* path = "C:\\Windows\\System32\\noperms";
#endif

    pass &= NULL == sir_addfile(path, SIRL_ALL, SIRO_MSGONLY);

    if (pass)
        print_expected_error();

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_faildupefile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    const char* filename = "faildupefile.log";
    sirfileid fid      = sir_addfile(filename, SIRL_ALL, SIRO_DEFAULT);

    pass &= NULL != fid;
    pass &= NULL == sir_addfile(filename, SIRL_ALL, SIRO_DEFAULT);

    if (pass)
        print_expected_error();

    pass &= sir_remfile(fid);

    rmfile(filename);
    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_failremovebadfile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    int invalidid = 9999999;
    pass &= !sir_remfile(&invalidid);

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
    snprintf(logfilename, SIR_MAXPATH, "%s%s", logbasename, logext);

    unsigned delcount = 0;
    if (!enumfiles(logbasename, deletefiles, &delcount)) {
        handle_os_error(false, "failed to enumerate log files with base name: %s!", logbasename);
        return false;
    }

    if (delcount > 0)
        printf("\tfound and removed %u log file(s)\n", delcount);

    FILE* f = NULL;
    _sir_fopen(&f, logfilename, "w");

    if (!f) {
        print_os_error();
        return false;
    }

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
    pass &= NULL != fileid;

    if (pass) {
        /* write an (approximately) known quantity until we should have rolled */
        size_t written  = 0;
        size_t linesize = strlen(line);

        do {
            pass &= sir_debug("%s", line);
            if (!pass)
                break;

            written += linesize;
        } while (written < deltasize + (linesize * 50));

        /* look for files matching the original name. */
        unsigned foundlogs = 0;
        if (!enumfiles(logbasename, countfiles, &foundlogs)) {
            handle_os_error(false, "failed to enumerate log files with base name: %s!",
                logbasename);
            pass = false;
        }

        /* if two are present, the test is a pass. */
        pass &= foundlogs == 2;
    }

    if (NULL != fileid)
        pass &= sir_remfile(fileid);

    delcount = 0;
    if (!enumfiles(logbasename, deletefiles, &delcount)) {
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
    memset(&si, 0xbadf00d, sizeof(sirinit));

    printf("\tcalling sir_init with invalid data...\n");
    bool pass = !sir_init(&si);

    if (!pass)
        sir_cleanup();
    else
        print_expected_error();

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
        {SIR_E_DUPFILE,   "SIR_E_DUPFILE"},   /**< File already managed by libsir (3) */
        {SIR_E_NOFILE,    "SIR_E_NOFILE"},    /**< File not managed by libsir (4) */
        {SIR_E_FCFULL,    "SIR_E_FCFULL"},    /**< Maximum number of files already managed (5) */
        {SIR_E_OPTIONS,   "SIR_E_OPTIONS"},   /**< Option flags are invalid (6) */
        {SIR_E_LEVELS,    "SIR_E_LEVELS"},    /**< Level flags are invalid (7) */
        {SIR_E_TEXTSTYLE, "SIR_E_TEXTSTYLE"}, /**< Text style is invalid (8) */
        {SIR_E_STRING,    "SIR_E_STRING"},    /**< Invalid string argument (9) */
        {SIR_E_NULLPTR,   "SIR_E_NULLPTR"},   /**< NULL pointer argument (10) */
        {SIR_E_INVALID,   "SIR_E_INVALID"},   /**< Invalid argument (11) */
        {SIR_E_NODEST,    "SIR_E_NODEST"},    /**< No destinations registered for level (12) */
        {SIR_E_UNAVAIL,   "SIR_E_UNAVAIL"},   /**< Feature is disabled or unavailable (13) */
        {SIR_E_INTERNAL,  "SIR_E_INTERNAL"},  /**< An internal error has occurred (14) */
        {SIR_E_PLATFORM,  "SIR_E_PLATFORM"},  /**< Platform error code %d: %s (15) */
        {SIR_E_UNKNOWN,   "SIR_E_UNKNOWN"},   /**< Error is not known (4095) */
    };

    char message[SIR_MAXERROR] = {0};
    for (size_t n = 0; n < _sir_countof(errors); n++) {
        _sir_seterror(_sir_mkerror(errors[n].code));
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

    pass &= !sir_settextstyle(SIRL_INFO, 0xbbbb);
    pass &= sir_info("I have set an invalid text style.");
    pass &= !sir_settextstyle(SIRL_DEBUG, SIRS_FG_RED | SIRS_FG_DEFAULT);
    pass &= sir_info("oops, did it again...");

    pass &= !sir_settextstyle(SIRL_ALERT, SIRS_FG_BLACK | SIRS_BG_BLACK);
    pass &= sir_info("and again.");

    pass &= sir_settextstyle(SIRL_INFO, SIRS_FG_DEFAULT | SIRS_BG_DEFAULT);
    pass &= sir_info("system default fg and bg");

    pass &= sir_settextstyle(SIRL_INFO, SIRS_BG_DEFAULT);
    pass &= sir_info("system default bg & no fg specified");

    pass &= sir_settextstyle(SIRL_INFO, SIRS_FG_DEFAULT);
    pass &= sir_info("system default fg & no bg specified");

    printf("\treset styles...\n");
    pass &= sir_resettextstyles();

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

    printf("\treset styles...\n");
    pass &= sir_resettextstyles();

    pass &= sir_debug("default style");
    pass &= sir_info("default style");
    pass &= sir_notice("default style");
    pass &= sir_warn("default style");
    pass &= sir_error("default style");
    pass &= sir_crit("default style");
    pass &= sir_alert("default style");
    pass &= sir_emerg("default style");

    /* ensure that foreground color constants match background color when
     * shifted and masked. allows prevention of unreadable text. */
    printf("\tensuring fg and bg colors align when masked and shifted...\n");
    pass &= _SIRS_SAME_COLOR(SIRS_FG_BLACK,    SIRS_BG_BLACK);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_RED,      SIRS_BG_RED);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_GREEN,    SIRS_BG_GREEN);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_YELLOW,   SIRS_BG_YELLOW);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_BLUE,     SIRS_BG_BLUE);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_MAGENTA,  SIRS_BG_MAGENTA);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_CYAN,     SIRS_BG_CYAN);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LGRAY,    SIRS_BG_LGRAY);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_DGRAY,    SIRS_BG_DGRAY);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LRED,     SIRS_BG_LRED);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LGREEN,   SIRS_BG_LGREEN);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LYELLOW,  SIRS_BG_LYELLOW);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LBLUE,    SIRS_BG_LBLUE);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LMAGENTA, SIRS_BG_LMAGENTA);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_LCYAN,    SIRS_BG_LCYAN);
    pass &= _SIRS_SAME_COLOR(SIRS_FG_WHITE,    SIRS_BG_WHITE);

    if (pass)
        printf("\t" GREEN("all fg and bg colors align") "\n");

    /* for every foreground color, OR it with each of the others.
     * same thing for background colors. none of these should be valid. */
    printf("\ttesting all combinations of (2) fg colors to create collisions...\n");
    for (size_t n = 3; n < 20; n++) {
        uint32_t style = sir_style_16color_map[n].from;
        for (size_t i = 3; i < 20; i++) {
            if (n == i)
                continue;

            uint32_t attr, fg, bg;
            if (_sir_validstyle(style | sir_style_16color_map[i].from, &attr, &fg, &bg)) {
                pass = false;
                printf("\t" RED("fg %08" PRIx32 " | %08" PRIx32 " (%08" PRIx32
                       ") is valid!") "\n", style, sir_style_16color_map[i].from,
                       style | sir_style_16color_map[i].from);
            }
        }
    }

    printf("\ttesting all combinations of (2) bg colors to create collisions...\n");
    for (size_t n = 20; n < SIR_NUM16_COLOR_MAPPINGS; n++) {
        uint32_t style = sir_style_16color_map[n].from;
        for (size_t i = 20; i < SIR_NUM16_COLOR_MAPPINGS; i++) {
            if (n == i)
                continue;

            uint32_t attr, fg, bg;
            if (_sir_validstyle(style | sir_style_16color_map[i].from, &attr, &fg, &bg)) {
                pass = false;
                printf("\t" RED("bg %08" PRIx32 " | %08" PRIx32 " (%08" PRIx32
                       ") is valid!") "\n", style, sir_style_16color_map[i].from,
                       style | sir_style_16color_map[i].from);
            }
        }
    }

    if (pass)
        printf("\t" GREEN("NO collisions in fg or bg colors") "\n");

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
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_ALL);
    pass &= _sir_validopts(SIRO_NOTIME);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOTIME);
    pass &= _sir_validopts(SIRO_NOHOST);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOHOST);
    pass &= _sir_validopts(SIRO_NOLEVEL);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOLEVEL);
    pass &= _sir_validopts(SIRO_NONAME);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NONAME);
    pass &= _sir_validopts(SIRO_NOPID);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOPID);
    pass &= _sir_validopts(SIRO_NOTID);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOTID);
    pass &= _sir_validopts(SIRO_NOHDR);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_NOHDR);
    pass &= _sir_validopts(SIRO_MSGONLY);
    printf(INDENT_ITEM WHITE("valid option: %08x") "\n", SIRO_MSGONLY);
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
        printf(INDENT_ITEM WHITE("(%zu/%zu): random valid (count: %" PRIu32
            ", options: %08" PRIx32 ")") "\n", n + 1, iterations, rand_count, opts);
    }
    PRINT_PASS(pass, "\t--- random bitmask of valid options: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- invalid values ---") "\n");

    /* the lowest byte is not valid. */
    sir_options invalid = 0x000000ff;
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("lowest byte: %08" PRIx32) "\n", invalid);

    /* gaps inbetween valid options. */
    invalid = 0x0001ff00 & ~(SIRO_NOTIME | SIRO_NOHOST | SIRO_NOLEVEL | SIRO_NONAME |
                             SIRO_NOMSEC | SIRO_NOPID | SIRO_NOTID  | SIRO_NOHDR);
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("gaps in 0x001ff00: %08" PRIx32) "\n", invalid);

    /* greater than SIRO_MSGONLY and less than SIRO_NOHDR. */
    for (sir_option o = 0x00008f00; o < SIRO_NOHDR; o += 0x1000) {
        pass &= !_sir_validopts(o);
        printf(INDENT_ITEM WHITE("SIRO_MSGONLY >< SIRO_NOHDR: %08" PRIx32) "\n", o);
    }

    /* greater than SIRO_NOHDR. */
    invalid = (0xFFFF0000 & ~SIRO_NOHDR);
    pass &= !_sir_validopts(invalid);
    printf(INDENT_ITEM WHITE("greater than SIRO_NOHDR: %08" PRIx32) "\n", invalid);

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
        printf(INDENT_ITEM WHITE("(%zu/%zu): random valid (count: %" PRIu32 ", levels:"
                                 " %04" PRIx16) ")\n", n + 1, iterations, rand_count, levels);
    }
    PRINT_PASS(pass, "\t--- random bitmask of valid levels: %s ---\n\n", PRN_PASS(pass));

    printf("\t" WHITEB("--- invalid values ---") "\n");

    /* greater than SIRL_ALL. */
    sir_levels invalid = (0xffff & ~SIRL_ALL);
    pass &= !_sir_validlevels(invalid);
    printf(INDENT_ITEM WHITE("greater than SIRL_ALL: %04" PRIx16) "\n", invalid);

    PRINT_PASS(pass, "\t--- invalid values: %s ---\n\n", PRN_PASS(pass));

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_perf(void) {
    static const char* logbasename = "libsir-perf";
    static const char* logext      = ".log";

#if !defined(__WIN__)
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
        startsirtimer(&printftimer);

        for (size_t n = 0; n < perflines; n++)
            printf("\x1b[97m%.2f: lorem ipsum foo bar %s: %zu\x1b[0m\n",
                sirtimerelapsed(&printftimer), "baz", 1234 + n);

        printfelapsed = sirtimerelapsed(&printftimer);

        printf("\t" BLUE("%zu lines libsir(stdout)...") "\n", perflines);

        sir_timer stdiotimer = {0};
        startsirtimer(&stdiotimer);

        for (size_t n = 0; n < perflines; n++)
            sir_debug("%.2f: lorem ipsum foo bar %s: %zu", sirtimerelapsed(&stdiotimer), "baz",
                1234 + n);

        stdioelapsed = sirtimerelapsed(&stdiotimer);

        sir_cleanup();

        INIT(si2, 0, 0, 0, 0);
        pass &= si2_init;

        char logfilename[SIR_MAXPATH] = {0};
        snprintf(logfilename, SIR_MAXPATH, "%s%s", logbasename, logext);

        sirfileid logid = sir_addfile(logfilename, SIRL_ALL, SIRO_NOMSEC | SIRO_NONAME);
        pass &= NULL != logid;

        if (pass) {
            printf("\t" BLUE("%zu lines libsir(log file)...") "\n", perflines);

            sir_timer filetimer = {0};
            startsirtimer(&filetimer);

            for (size_t n = 0; n < perflines; n++)
                sir_debug("lorem ipsum foo bar %s: %zu", "baz", 1234 + n);

            fileelapsed = sirtimerelapsed(&filetimer);

            pass &= sir_remfile(logid);
        }

        if (pass) {
            printf("\t" WHITEB("printf: ") CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, printfelapsed / 1e3, perflines / (printfelapsed / 1e3));
            printf("\t" WHITEB("libsir(stdout): ")
                   CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, stdioelapsed / 1e3, perflines / (stdioelapsed / 1e3));
            printf("\t" WHITEB("libsir(log file): ")
                   CYAN("%zu lines in %.3fsec (%.1f lines/sec)") "\n",
                perflines, fileelapsed / 1e3, perflines / (fileelapsed / 1e3));
            printf("\t" WHITEB("timer resolution: ") CYAN("~%ldnsec") "\n", sirtimergetres());
        }
    }

    unsigned deleted = 0;
    enumfiles(logbasename, deletefiles, &deleted);

    if (deleted > 0)
        printf("\t" DGRAY("deleted %d log file(s)") "\n", deleted);

    sir_cleanup();
    return print_result_and_return(pass);
}

bool sirtest_updatesanity(void) {
    INIT_N(si, SIRL_DEFAULT, 0, SIRL_DEFAULT, 0, "update_sanity");
    bool pass = si_init;

#define UPDATE_SANITY_ARRSIZE 10

    static const char* logfile = "update-sanity.log";
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
    pass &= NULL != id1;

    for (int i = 0; i < 10; i++) {
        if (!pass)
            break;

        /* reset to defaults*/
        pass &= sir_stdoutlevels(SIRL_DEFAULT);
        pass &= sir_stderrlevels(SIRL_DEFAULT);
        pass &= sir_stdoutopts(SIRO_DEFAULT);
        pass &= sir_stderropts(SIRO_DEFAULT);

        pass &= sir_debug("default config");
        pass &= sir_info("default config");
        pass &= sir_notice("default config");
        pass &= sir_warn("default config");
        pass &= sir_error("default config");
        pass &= sir_crit("default config");
        pass &= sir_alert("default config");
        pass &= sir_emerg("default config");

        /* pick random options to set/unset */
        uint32_t rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_stdoutlevels(levels_array[rnd]);
        pass &= sir_stdoutopts(opts_array[rnd]);
        printf("\t" WHITE("set random config #%" PRIu32 " for stdout") "\n", rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_stderrlevels(levels_array[rnd]);
        pass &= sir_stderropts(opts_array[rnd]);
        printf("\t" WHITE("set random config #%" PRIu32 " for stderr") "\n", rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        pass &= sir_filelevels(id1, levels_array[rnd]);
        pass &= sir_fileopts(id1, opts_array[rnd]);
        printf("\t" WHITE("set random config #%" PRIu32 " for %s") "\n", rnd, logfile);

        pass &= filter_error(sir_debug("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_info("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_notice("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_warn("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_error("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_crit("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_alert("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
        pass &= filter_error(sir_emerg("modified config #%" PRIu32 "", rnd), SIR_E_NODEST);
    }

    if (pass) {
        /* restore to default config and run again */
        sir_stdoutlevels(SIRL_DEFAULT);
        sir_stderrlevels(SIRL_DEFAULT);
        sir_stdoutopts(SIRO_DEFAULT);
        sir_stderropts(SIRO_DEFAULT);

        pass &= sir_debug("default config");
        pass &= sir_info("default config");
        pass &= sir_notice("default config");
        pass &= sir_warn("default config");
        pass &= sir_error("default config");
        pass &= sir_crit("default config");
        pass &= sir_alert("default config");
        pass &= sir_emerg("default config");
    }

    pass &= sir_remfile(id1);
    rmfile(logfile);
    sir_cleanup();

    return print_result_and_return(pass);
}

static bool generic_syslog_test(const char* sl_name, const char* identity, const char* category) {
    bool pass             = true;
    static const int runs = 5;

    /* repeat initializing, opening, logging, closing, cleaning up n times. */
    sir_timer timer = {0};
    pass &= startsirtimer(&timer);

    printf("\trunning %d passes of random configs (system logger: '%s', "
           "identity: '%s', category: '%s')...\n",
        runs, sl_name, identity, category);

    for (int i = 0; i < runs; i++) {
        /* randomly skip setting process name, identity/category to thoroughly
           test fallback routines; randomly update the config mid-run. */
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

        si_init = sir_init(&si);
        pass &= si_init;

        if (do_update)
            pass &= sir_sysloglevels(SIRL_ALL);

        pass &= sir_debug("%d/%d: this debug message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_info("%d/%d: this info message sent to stdout and %s.", i + 1, runs, sl_name);

        pass &= sir_notice("%d/%d: this notice message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_warn("%d/%d: this warning message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_error("%d/%d: this error message sent to stdout and %s.", i + 1, runs, sl_name);

        if (do_update)
            pass &= sir_syslogopts(SIRO_MSGONLY & ~(SIRO_NOLEVEL | SIRO_NOPID));

        pass &= sir_crit("%d/%d: this critical message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_alert("%d/%d: this alert message sent to stdout and %s.", i + 1, runs, sl_name);
        pass &= sir_emerg("%d/%d: this emergency message sent to stdout and %s.", i + 1, runs, sl_name);

#if defined(SIR_OS_LOG_ENABLED)
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
#endif

        sir_cleanup();

        if (!pass)
            break;
    }

    return print_result_and_return(pass);
}

bool sirtest_syslog(void) {
#if !defined(SIR_SYSLOG_ENABLED)
    printf("\t" DGRAY("SIR_SYSLOG_ENABLED is not defined; skipping.") "\n");
    return true;
#else
    return generic_syslog_test("syslog", "sirtests", "tests");
#endif
}

bool sirtest_os_log(void) {
#if !defined(SIR_OS_LOG_ENABLED)
    printf("\t" DGRAY("SIR_OS_LOG_ENABLED is not defined; skipping.") "\n");
    return true;
#else
    bool pass = generic_syslog_test("os_log", "com.aremmell.libsir.tests", "tests");
    return print_result_and_return(pass);
#endif
}

bool sirtest_filesystem(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

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
            char* filename2 = strdup(filename);
            pass &= NULL != filename2;

            if (NULL != filename2) {
                /* filename, stripped of directory component(s). */
                char* _basename = _sir_getbasename(filename2);
                printf("\t_sir_getbasename: '%s'\n", PRN_STR(_basename));

                /* the last strlen(_basename) chars of filename should match. */
                size_t len    = strlen(_basename);
                size_t offset = strlen(filename) - len;
                size_t n      = 0;

                while (n < len) {
                    if (filename[offset++] != _basename[n++]) {
                        pass = false;
                        break;
                    }
                };
            }

            /* directory this binary file resides in. */
            char* appdir = _sir_getappdir();
            pass &= NULL != appdir;
            printf("\t_sir_getappdir: '%s'\n", PRN_STR(appdir));

            /* _sir_get[base|dir]name can potentially modify filename,
             * so make a copy for each call. */
            char* filename3 = strdup(filename);
            pass &= NULL != filename3;

            if (NULL != appdir && NULL != filename3) {
                /* should yield the same result as _sir_getappdir(). */
                char* _dirname = _sir_getdirname(filename3);
                printf("\t_sir_getdirname: '%s'\n", PRN_STR(_dirname));

                pass &= 0 == strncmp(filename, appdir, strnlen(appdir, SIR_MAXPATH));
                pass &= 0 == strncmp(filename, _dirname, strnlen(_dirname, SIR_MAXPATH));
            }

            _sir_safefree(appdir);
            _sir_safefree(filename);
            _sir_safefree(filename2);
            _sir_safefree(filename3);
        }

        _sir_safefree(cwd);
    }

    /* this next section doesn't really yield any useful boolean pass/fail
     * information, but could be helpful to review manually. */
    char* dubious_dirnames[] = {
#if !defined(__WIN__)
        "/foo",
        "/foo/",
        "/foo/bar",
        "/foo/bar/bad:filename",
        "/"
#else /* __WIN__ */
        "C:\\foo",
        "C:\\foo\\",
        "C:\\foo\\bar",
        "C:\\foo\\bar\\bad:>filename",
        "C:\\",
        "//network-share/myfolder"
#endif
    };

    for (size_t n = 0; n < _sir_countof(dubious_dirnames); n++) {
        char* tmp = strdup(dubious_dirnames[n]);
        if (NULL != tmp) {
            printf("\t_sir_getdirname(" WHITE("'%s'") ") = " WHITE("'%s'") "\n",
                tmp, _sir_getdirname(tmp));
            _sir_safefree(tmp);
        }
    }

    char* dubious_filenames[] = {
#if !defined(__WIN__)
        "foo/bar/file-or-directory",
        "/foo/bar/file-or-directory",
        "/foo/bar/illegal:filename",
        "/"
#else /* __WIN__ */
        "foo\\bar\\file.with.many.full.stops",
        "C:\\foo\\bar\\poorly-renamed.txt.pdf",
        "C:\\foo\\bar\\illegal>filename.txt",
        "C:\\",
        "\\Program Files\\foo.bar"
#endif
    };

    for (size_t n = 0; n < _sir_countof(dubious_filenames); n++) {
        char* tmp = strdup(dubious_filenames[n]);
        if (NULL != tmp) {
            printf("\t_sir_getbasename(" WHITE("'%s'") ") = " WHITE("'%s'") "\n",
                tmp, _sir_getbasename(tmp));
            _sir_safefree(tmp);
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
            _SIR_UNUSED(unused);
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
        {"/usr/bin", true},
        {"/dev", true},
#else /* __WIN__ */
        {"\\Windows", true},
        {"\\Program Files", true},
#endif
        {"../../LICENSE", true},
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
            _SIR_UNUSED(unused);
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

    sir_cleanup();
    return print_result_and_return(pass);
}

#if !defined(__WIN__)
static void* sirtest_thread(void* arg);
#else /* __WIN__ */
static unsigned sirtest_thread(void* arg);
#endif

#define NUM_THREADS 4

bool sirtest_mthread_race(void) {
#if !defined(__WIN__)
    pthread_t thrds[NUM_THREADS] = {0};
#else /* __WIN__ */
    uintptr_t thrds[NUM_THREADS] = {0};
#endif

    INIT_N(si, SIRL_ALL, SIRO_NOPID, 0, 0, "multi-thread-race");
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
        snprintf(heap_args[n].log_file, SIR_MAXPATH, "multi-thread-race-%zu.log", n);

#if !defined(__WIN__)
        int create = pthread_create(&thrds[n], NULL, sirtest_thread, (void*)&heap_args[n]);
        if (0 != create) {
            errno = create;
            handle_os_error(true, "pthread_create() for thread #%zu failed!", n + 1);
#else /* __WIN__ */
        thrds[n] = _beginthreadex(NULL, 0, sirtest_thread, (void*)&heap_args[n], 0, NULL);
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
                handle_os_error(true, "pthread_join() for thread #%zu (%p) failed!", j + 1,
                    (void*)thrds[j]);
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

    _sir_safefree(heap_args);

    sir_cleanup();
    return print_result_and_return(pass);
}

#if !defined(__WIN__)
static void* sirtest_thread(void* arg) {
#else /* __WIN__ */
unsigned sirtest_thread(void* arg) {
#endif
    pid_t threadid       = _sir_gettid();
    thread_args* my_args = (thread_args*)arg;

    rmfile(my_args->log_file);
    sirfileid id = sir_addfile(my_args->log_file, SIRL_ALL, SIRO_MSGONLY);

    if (NULL == id) {
        bool unused = print_test_error(false, false);
        _SIR_UNUSED(unused);
#if !defined(__WIN__)
        return NULL;
#else /* __WIN__ */
        return 0;
#endif
    }

    printf("\thi, i'm thread id %d, logging to: '%s'...\n", threadid, my_args->log_file);

    for (size_t n = 0; n < 1000; n++) {
        /* choose a random level, and colors. */
        sir_textstyle style;

        if (n % 2 == 0) {
            style = SIRS_FG_CYAN | SIRS_BG_BLACK;
            sir_debug("this is log message #%zu", n);
        } else {
            style = SIRS_FG_BLACK | SIRS_BG_CYAN;
            sir_alert("this is log message #%zu", n);
        }

        /* sometimes remove and re-add the log file, and set some options/styles.
           other times, just set different options/styles. */
        if (getrand_bool((uint32_t)(n + threadid))) {
            if (!sir_remfile(id))
                my_args->pass = print_test_error(false, false);

            id = sir_addfile(my_args->log_file, SIRL_ALL, SIRO_MSGONLY);
            if (NULL == id)
                my_args->pass = print_test_error(false, false);

            if (!sir_settextstyle(SIRL_DEBUG, style))
                my_args->pass = print_test_error(false, false);

            if (!sir_stdoutopts(SIRO_NOPID))
                my_args->pass = print_test_error(false, false);
        } else {
            if (!sir_settextstyle(SIRL_DEBUG, style))
                my_args->pass = print_test_error(false, false);

            if (!sir_fileopts(id, SIRO_NOPID))
                my_args->pass = print_test_error(false, false);

            if (!sir_stdoutopts(SIRO_NOTIME | SIRO_NOLEVEL))
                my_args->pass = print_test_error(false, false);
        }

        if (!my_args->pass)
            break;
    }

    if (!sir_remfile(id))
        my_args->pass = print_test_error(false, false);

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


    sir_cleanup();
    return print_result_and_return(pass);
}
*/

/* ========================== end tests ========================== */

bool print_test_error(bool result, bool expected) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);

    if (!expected && !result && SIR_E_NOERROR != code)
        printf("\t" RED("!! Unexpected (%hu, %s)") "\n", code, message);
    else if (expected)
        printf("\t" GREEN("Expected (%hu, %s)") "\n", code, message);

    return result;
}

void print_os_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    fprintf(stderr, "\t" RED("OS error: (%hu, %s)") "\n", code, message);
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

    /* just return true if the file doesn't exist. */
    struct stat st;
    if (0 != stat(filename, &st)) {
        if (ENOENT == errno)
            return true;

        handle_os_error(true, "failed to stat %s!", filename);
        return false;
    }

#if !defined(__WIN__)
    removed = (0 == remove(filename));
#else /* __WIN__ */
    removed = FALSE != DeleteFile(filename);
#endif

    if (!removed) {
        handle_os_error(false, "failed to delete %s!", filename);
    } else {
        printf("\t" DGRAY("deleted %s (%ld bytes)...") "\n", filename, (long)st.st_size);
    }

    return removed;
}

bool deletefiles(const char* search, const char* filename, unsigned* data) {
    if (strstr(filename, search)) {
        rmfile(filename);
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
#if !defined(__WIN__)
    DIR* d = opendir(".");
    if (!d)
        return false;

    rewinddir(d);
    struct dirent* di = readdir(d);
    if (!di)
        return false;

    while (NULL != di) {
        if (!cb(search, di->d_name, data))
            break;
        di = readdir(d);
    };

    closedir(d);
    d = NULL;
#else /* __WIN__ */
    WIN32_FIND_DATA finddata = {0};
    HANDLE enumerator        = FindFirstFile("./*", &finddata);

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

bool startsirtimer(sir_timer* timer) {
#if !defined(__WIN__)
    int gettime = clock_gettime(SIRTEST_CLOCK, &timer->ts);
    if (0 != gettime) {
        handle_os_error(true, "clock_gettime(%d) failed!", SIRTEST_CLOCK);
    }

    return 0 == gettime;
#else /* __WIN__ */
    GetSystemTimePreciseAsFileTime(&timer->ft);
    return true;
#endif
}

float sirtimerelapsed(const sir_timer* timer) {
#if !defined(__WIN__)
    struct timespec now;
    if (0 == clock_gettime(SIRTEST_CLOCK, &now)) {
        return (float)((now.tv_sec * 1e3) + (now.tv_nsec / 1e6) - (timer->ts.tv_sec * 1e3) +
            (timer->ts.tv_nsec / 1e6));
    } else {
        handle_os_error(true, "clock_gettime(%d) failed!", SIRTEST_CLOCK);
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
        handle_os_error(true, "clock_getres(%d) failed!", SIRTEST_CLOCK);
    }
#else /* __WIN__ */
    retval = 100;
#endif
    return retval;
}

bool mark_test_to_run(const char* name) {
    bool found = false;
    for (size_t t = 0; t < _sir_countof(sir_tests); t++) {
        if (_sir_strsame(name, sir_tests[t].name, strlen(sir_tests[t].name))) {
            found = sir_tests[t].run = true;
            break;
        }
    }

    if (!found)
        _sir_selflog("warning: unable to locate '%s' in test array", name);

    return found;
}

void print_usage_info(void) {
    fprintf(stderr, "\n" WHITE("Usage:") "\n\n");

    for (size_t i = 0; i < _sir_countof(_cl_arg_list); i++) {
        fprintf(stderr, "\t%s%s%s%s%s\n", _cl_arg_list[i].flag,
            strlen(_cl_arg_list[i].usage) == 0 ? "" : "\t", _cl_arg_list[i].usage,
            strlen(_cl_arg_list[i].usage) == 0 ? "\t" : " ", _cl_arg_list[i].desc);
    }

    fprintf(stderr, "\n");
}

void print_test_list(void) {
    size_t longest = 0;
    for (size_t i = 0; i < _sir_countof(sir_tests); i++) {
        size_t len = strlen(sir_tests[i].name);
        if (len > longest)
            longest = len;
    }

    printf("\n" WHITE("Available tests:") "\n\n");

    for (size_t i = 0; i < _sir_countof(sir_tests); i++) {
        printf("\t%s\t", sir_tests[i].name);

        size_t len = strlen(sir_tests[i].name);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                printf(" ");

        if ((i % 2) != 0 || i == _sir_countof(sir_tests) - 1)
            printf("\n");
    }

    printf("\n");
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
