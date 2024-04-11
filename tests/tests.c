/*
 * tests.c
 *
 * Version: 2.2.5
 *
 * ----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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
 *
 * ----------------------------------------------------------------------------
 */

//-V:_sir_logv:575

#include "tests.h"

static sir_test sir_tests[] = {
    {SIR_CL_PERFNAME,           sirtest_perf, false, true},
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
    {"init-check-state",        sirtest_isinitialized, false, true},
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
    {"sanity-thread-ids",       sirtest_threadidsanity, false, true},
    {"sanity-file-write",       sirtest_logwritesanity, false, true},
    {"syslog",                  sirtest_syslog, false, true},
    {"os_log",                  sirtest_os_log, false, true},
    {"wineventlog",             sirtest_win_eventlog, false, true},
    {"filesystem",              sirtest_filesystem, false, true},
    {"squelch-spam",            sirtest_squelchspam, false, true},
    {"plugin-loader",           sirtest_pluginloader, false, true},
    {"string-utils",            sirtest_stringutils, false, true},
    {"get-cpu-count",           sirtest_getcpucount, false, true},
    {"get-version-info",        sirtest_getversioninfo, false, true}
};

/** List of available command line arguments. */
static const sir_cl_arg cl_args[] = {
    {SIR_CL_PERFFLAG,      "", SIR_CL_PERFDESC},
    {SIR_CL_ONLYFLAG,      ""  SIR_CL_ONLYUSAGE, SIR_CL_ONLYDESC},
    {SIR_CL_LISTFLAG,      "", SIR_CL_LISTDESC},
    {SIR_CL_LEAVELOGSFLAG, "", SIR_CL_LEAVELOGSDESC},
    {SIR_CL_WAITFLAG,      "", SIR_CL_WAITDESC},
    {SIR_CL_VERSIONFLAG,   "", SIR_CL_VERSIONDESC},
    {SIR_CL_HELPFLAG,      "", SIR_CL_HELPDESC}
};

static sir_cl_config cl_cfg = {0};

int main(int argc, char** argv) {
#if defined(__HAIKU__) && !defined(DEBUG)
    disable_debugger(1);
#endif

#include "tests_malloc.h"

#if !defined(__WIN__) && !defined(__HAIKU__) && !defined(__EMSCRIPTEN__)
    /* Disallow execution by root / sudo; some of the tests rely on lack of permissions. */
    if (geteuid() == 0) {
        (void)fprintf(stderr, "Sorry, but this program may not be executed by root." SIR_EOL);
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

    bool parsed = parse_cmd_line(argc, argv, cl_args, _sir_countof(cl_args),
        sir_tests, _sir_countof(sir_tests), &cl_cfg);
    if (!parsed)
        return EXIT_FAILURE;

    size_t first     = (cl_cfg.only ? 0 : 1);
    size_t tgt_tests = (cl_cfg.only ? cl_cfg.to_run : _sir_countof(sir_tests) - first);
    size_t passed    =  0;
    size_t ran       =  0;
    sir_time timer   = {0};

    print_intro(tgt_tests);
    sir_timer_start(&timer);

    for (size_t n = first; n < _sir_countof(sir_tests); n++) {
        if (cl_cfg.only && !sir_tests[n].run) {
            _sir_selflog("skipping '%s'; not marked to run", sir_tests[n].name);
            continue;
        }

        print_test_intro(ran + 1, tgt_tests, sir_tests[n].name);

        sir_tests[n].pass = sir_tests[n].fn();
        if (sir_tests[n].pass)
            passed++;

        ran++;

        print_test_outro(ran, tgt_tests, sir_tests[n].name, sir_tests[n].pass);
    }

    print_test_summary(tgt_tests, passed, sir_timer_elapsed(&timer));

    if (passed != tgt_tests) {
        print_failed_test_intro(tgt_tests, passed);

        for (size_t t = 0; t < _sir_countof(sir_tests); t++)
            if (!sir_tests[t].pass)
                print_failed_test(sir_tests[t].name);
        (void)printf(SIR_EOL);
    }

    if (cl_cfg.wait)
        wait_for_keypress();

    return passed == tgt_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool sirtest_exceedmaxsize(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    char toobig[SIR_MAXMESSAGE + 100] = {0};
    (void)memset(toobig, 'a', SIR_MAXMESSAGE + 100);
    toobig[SIR_MAXMESSAGE + 99] = '\0';

    _sir_eqland(pass, sir_info("%s", toobig));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_logwritesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const char* logfilename = MAKE_LOG_NAME("write-validate.log");
    static const char* message     = "Lorem ipsum dolor sit amet, sea ei dicit"
                                     " regione laboramus, eos cu minim putent."
                                     " Sale omnium conceptam est in, cu nam possim"
                                     " prompta eleifend. Duo purto nostrud eu."
                                     " Alia accumsan has cu, mentitum invenire"
                                     " mel an, dicta noster legendos et pro."
                                     " Solum nobis laboramus et quo, nam putant"
                                     " dolores consequuntur ex. Sit veniam eruditi"
                                     " contentiones at. Cu ponderum oporteat"
                                     " oportere mel, has et saperet accusata"
                                     " complectitur.";

    TEST_MSG("adding log file '%s' to libsir...", logfilename);
    sirfileid id = sir_addfile(logfilename, SIRL_DEBUG, SIRO_NOHDR | SIRO_NOHOST);
    _sir_eqland(pass, 0U != id);

    (void)print_test_error(pass, false);

    TEST_MSG("writing message to stdout and %s...", logfilename);

    _sir_eqland(pass, sir_debug("%s", message));

    (void)print_test_error(pass, false);

    TEST_MSG("removing %s from libsir...", logfilename);
    _sir_eqland(pass, sir_remfile(id));

    (void)print_test_error(pass, false);

    TEST_MSG("opening %s for reading...", logfilename);

    FILE* f = fopen(logfilename, "r");
    if (!f) {
        pass = false;
    } else {
        char buf[512] = {0};
        _sir_eqland(pass, 0 != sir_readline(f, buf, 512));

        bool found = NULL != strstr(buf, message);
        _sir_eqland(pass, found);

        if (found)
            TEST_MSG(SIR_GREEN("found '%s'"), message);
        else
            TEST_MSG(SIR_RED("did not find '%s'"), message);

        _sir_safefclose(&f);
        TEST_MSG("deleting %s...", logfilename);
        rmfile(logfilename, cl_cfg.leave_logs);
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_threadidsanity(void)
{
#if defined(SIR_NO_THREAD_NAMES)
    TEST_MSG_0(SIR_DGRAY("test skipped for this system configuration"));
    return true;
#endif
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const char* thread_name = "mythread";
    static const char* logfilename = MAKE_LOG_NAME("thread-id-name.log");

    TEST_MSG("adding log file '%s' to libsir...", logfilename);
    sirfileid id = sir_addfile(logfilename, SIRL_DEBUG, SIRO_NOHDR | SIRO_NOHOST);
    _sir_eqland(pass, 0U != id);

    (void)print_test_error(pass, false);

    TEST_MSG_0("logging a message normally...");
    _sir_eqland(pass, sir_debug("this is a test of the libsir system"));

    TEST_MSG("setting the thread name to '%s' and logging again...", thread_name);

    _sir_eqland(pass, _sir_setthreadname(thread_name));
    sir_sleep_msec((uint32_t)SIR_THRD_CHK_INTERVAL + 200U);

    (void)print_test_error(pass, false);

    _sir_eqland(pass, sir_debug("this is a test of the libsir system after setting thread name"));

    TEST_MSG_0("setting the thread name to '' and logging again...");

    _sir_eqland(pass, _sir_setthreadname(""));
    sir_sleep_msec((uint32_t)SIR_THRD_CHK_INTERVAL + 200U);

    (void)print_test_error(pass, false);

    _sir_eqland(pass, sir_debug("this is a test of the libsir system after clearing thread name"));

     /* remove the log file from libsir, then open it and read it line by line. */
    TEST_MSG("removing %s from libsir...", logfilename);
    _sir_eqland(pass, sir_remfile(id));

    TEST_MSG("opening %s for reading...", logfilename);

    FILE* f = fopen(logfilename, "r");
    if (!f) {
        pass = false;
    } else {
        /* look for, in order, TID, thread name, TID. */
        for (size_t n = 0; n < 3; n++) {
            char buf[256] = {0};
            _sir_eqland(pass, 0 != sir_readline(f, buf, 256));
            TEST_MSG("read line %zu: '%s'", n, buf);

            char search[SIR_MAXPID] = {0};
            switch (n) {
                case 0:
                case 2:
                    (void)snprintf(search, SIR_MAXPID, SIR_TIDFORMAT, _sir_gettid());
                break;
                case 1:
                    (void)_sir_strncpy(search, SIR_MAXPID, thread_name, strlen(thread_name));
                break;
                default: break; // GCOVR_EXCL_LINE
            }

            bool found = NULL != strstr(buf, search);
            _sir_eqland(pass, found);

            if (found)
                TEST_MSG(SIR_GREEN("line %zu: found '%s'"), n, search);
            else
                TEST_MSG(SIR_RED("line %zu: did not find '%s'"), n, search);
        }

        _sir_safefclose(&f);
        TEST_MSG("deleting %s...", logfilename);
        rmfile(logfilename, cl_cfg.leave_logs);
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failnooutputdest(void) {
    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    _sir_eqland(pass, !sir_notice("this goes nowhere!"));

    if (pass) {
        static const char* logfilename = MAKE_LOG_NAME("nodestination.log");
        PRINT_EXPECTED_ERROR();

        _sir_eqland(pass, sir_stdoutlevels(SIRL_INFO));
        _sir_eqland(pass, sir_info("this goes to stdout"));
        _sir_eqland(pass, sir_stdoutlevels(SIRL_NONE));

        sirfileid fid = sir_addfile(logfilename, SIRL_INFO, SIRO_DEFAULT);
        _sir_eqland(pass, 0U != fid);
        _sir_eqland(pass, sir_info("this goes to %s", logfilename));
        _sir_eqland(pass, sir_filelevels(fid, SIRL_NONE));
        _sir_eqland(pass, !sir_notice("this goes nowhere!"));

        if (0U != fid)
            _sir_eqland(pass, sir_remfile(fid));

        rmfile(logfilename, cl_cfg.leave_logs);
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failnulls(void) {
    INIT_BASE(si, SIRL_ALL, 0, 0, 0, "", false);
    bool pass = true;

    _sir_eqland(pass, !sir_init(NULL));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_init(&si));
    _sir_eqland(pass, !sir_info(NULL)); //-V618 //-V575

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, 0U == sir_addfile(NULL, SIRL_ALL, SIRO_MSGONLY));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, !sir_remfile(0U));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failemptymessage(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    _sir_eqland(pass, !sir_debug("%s", ""));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
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
        rmfile(path, cl_cfg.leave_logs);
        ids[n] = sir_addfile(path, SIRL_ALL, (n % 2) ? odd : even);
        _sir_eqland(pass, 0U != ids[n] && sir_info("test %zu", n));
    }

    _sir_eqland(pass, sir_info("test test test"));

    /* this one should fail; max files already added. */
    _sir_eqland(pass, 0U == sir_addfile(MAKE_LOG_NAME("should-fail.log"),
        SIRL_ALL, SIRO_MSGONLY));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_info("test test test"));

    /* now remove previously added files in a different order. */
    size_t removeorder[SIR_MAXFILES];
    (void)memset(removeorder, -1, sizeof(removeorder));

    long processed = 0L;
    TEST_MSG_0("creating random file ID order...");

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

    (void)printf("\tremove order: {");
    for (size_t n = 0; n < SIR_MAXFILES; n++)
        (void)printf(" %zu%s", removeorder[n], (n < SIR_MAXFILES - 1) ? "," : "");
    (void)printf(" }..." SIR_EOL);

    for (size_t n = 0; n < SIR_MAXFILES; n++) {
        _sir_eqland(pass, sir_remfile(ids[removeorder[n]]));

        char path[SIR_MAXPATH] = {0};
        (void)snprintf(path, SIR_MAXPATH, MAKE_LOG_NAME("test-%zu.log"), removeorder[n]);
        rmfile(path, cl_cfg.leave_logs);
    }

    _sir_eqland(pass, sir_info("test test test"));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failinvalidfilename(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    _sir_eqland(pass, 0U == sir_addfile("bad file!/name", SIRL_ALL, SIRO_MSGONLY));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
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
    if (get_wineversion()) {
        path = "Z:\\noperms";
    } else {
        path = "C:\\Windows\\System32\\noperms";
    }
# endif
#endif

    _sir_eqland(pass, 0U == sir_addfile(path, SIRL_ALL, SIRO_MSGONLY));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
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

    TEST_MSG("adding log file '%s'...", filename1);

    /* should be fine; no other files added yet. */
    sirfileid fid = sir_addfile(filename1, SIRL_ALL, SIRO_DEFAULT);
    _sir_eqland(pass, 0U != fid);

    TEST_MSG("trying again to add log file '%s'...", filename1);

    /* should fail. this is the same file we already added. */
    _sir_eqland(pass, 0U == sir_addfile(filename1, SIRL_ALL, SIRO_DEFAULT));

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG("adding log file '%s'...", filename2);

    /* should also fail. this is the same file we already added, even
     * if the path strings don't match. */
    _sir_eqland(pass, 0U == sir_addfile(filename2, SIRL_ALL, SIRO_DEFAULT));

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG("adding log file '%s'...", filename3);

    /* should pass. this is a different file. */
    sirfileid fid2 = sir_addfile(filename3, SIRL_ALL, SIRO_DEFAULT);
    _sir_eqland(pass, 0U != fid2);

    /* should also pass. */
    sirfileid fid3 = sir_addfile(filename4, SIRL_ALL, SIRO_DEFAULT);
    _sir_eqland(pass, 0U != fid3);

    _sir_eqland(pass, sir_info("hello three valid files"));

    /* should now fail since we added it earlier. */
    _sir_eqland(pass, 0U == sir_addfile(filename3, SIRL_ALL, SIRO_DEFAULT));

    if (pass)
        PRINT_EXPECTED_ERROR();

    /* don't remove all of the log files in order to also test
     * cache tear-down. */
    _sir_eqland(pass, sir_remfile(fid));
    _sir_eqland(pass, sir_cleanup());

    rmfile(filename1, cl_cfg.leave_logs);
    rmfile(filename2, cl_cfg.leave_logs);
    rmfile(filename3, cl_cfg.leave_logs);
    rmfile(filename4, cl_cfg.leave_logs);

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failremovebadfile(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    sirfileid invalidid = 9999999;
    _sir_eqland(pass, !sir_remfile(invalidid));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_rollandarchivefile(void) {
    bool pass = true;

    _sir_eqland(pass, roll_and_archive("rollandarchive1", ".log"));
    _sir_eqland(pass, roll_and_archive("rollandarchive2", ""));

    return pass;
}

bool sirtest_failwithoutinit(void) {
    bool pass = !sir_info("sir isn't initialized; this needs to fail");

    if (pass)
        PRINT_EXPECTED_ERROR();

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_isinitialized(void) {

    bool pass = true;

    TEST_MSG_0("checking sir_isinitialized before initialization...");
    _sir_eqland(pass, !sir_isinitialized());

    INIT(si, SIRL_ALL, 0, 0, 0);
    _sir_eqland(pass, si_init);

    TEST_MSG_0("checking sir_isinitialized after initialization...");
    _sir_eqland(pass, sir_isinitialized());

    _sir_eqland(pass, sir_cleanup());

    TEST_MSG_0("checking sir_isinitialized after cleanup...");
    _sir_eqland(pass, !sir_isinitialized());

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failinittwice(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    INIT(si2, SIRL_ALL, 0, 0, 0);
    _sir_eqland(pass, !si2_init);

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failinvalidinitdata(void) {
    sirinit si;

    /* fill with bad data. */
    (void)memset(&si, 0xab, sizeof(sirinit));

    TEST_MSG_0("calling sir_init with invalid data...");
    bool pass = !sir_init(&si);

    if (pass)
        PRINT_EXPECTED_ERROR();

    (void)sir_cleanup();
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_initcleanupinit(void) {
    INIT(si1, SIRL_ALL, 0, 0, 0);
    bool pass = si1_init;

    _sir_eqland(pass, sir_info("init called once; testing output..."));
    _sir_eqland(pass, sir_cleanup());

    INIT(si2, SIRL_ALL, 0, 0, 0);
    _sir_eqland(pass, si2_init);

    _sir_eqland(pass, sir_info("init called again after re-init; testing output..."));
    _sir_eqland(pass, sir_cleanup());

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_initmakeinit(void) {
    bool pass = true;

    sirinit si;
    _sir_eqland(pass, sir_makeinit(&si));
    _sir_eqland(pass, sir_init(&si));
    _sir_eqland(pass, sir_info("initialized with sir_makeinit"));
    _sir_eqland(pass, sir_cleanup());

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_failaftercleanup(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    _sir_eqland(pass, sir_cleanup());
    _sir_eqland(pass, !sir_info("already cleaned up; this needs to fail"));

    if (pass)
        PRINT_EXPECTED_ERROR();

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_errorsanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    struct {
        uint16_t code;
        const char* name;
    } errors[] = {
        {SIR_E_NOERROR,   "SIR_E_NOERROR"},   /**< The operation completed successfully (1) */
        {SIR_E_NOTREADY,  "SIR_E_NOTREADY"},  /**< libsir has not been initialized (2) */
        {SIR_E_ALREADY,   "SIR_E_ALREADY"},   /**< libsir is already initialized (3) */
        {SIR_E_DUPITEM,   "SIR_E_DUPITEM"},   /**< Item already managed by libsir (4) */
        {SIR_E_NOITEM,    "SIR_E_NOITEM"},    /**< Item not managed by libsir (5) */
        {SIR_E_NOROOM,    "SIR_E_NOROOM"},    /**< Maximum number of items already stored (6) */
        {SIR_E_OPTIONS,   "SIR_E_OPTIONS"},   /**< Option flags are invalid (7) */
        {SIR_E_LEVELS,    "SIR_E_LEVELS"},    /**< Level flags are invalid (8) */
        {SIR_E_TEXTSTYLE, "SIR_E_TEXTSTYLE"}, /**< Text style is invalid (9) */
        {SIR_E_STRING,    "SIR_E_STRING"},    /**< Invalid string argument (10) */
        {SIR_E_NULLPTR,   "SIR_E_NULLPTR"},   /**< NULL pointer argument (11) */
        {SIR_E_INVALID,   "SIR_E_INVALID"},   /**< Invalid argument (12) */
        {SIR_E_NODEST,    "SIR_E_NODEST"},    /**< No destinations registered for level (13) */
        {SIR_E_UNAVAIL,   "SIR_E_UNAVAIL"},   /**< Feature is disabled or unavailable (14) */
        {SIR_E_INTERNAL,  "SIR_E_INTERNAL"},  /**< An internal error has occurred (15) */
        {SIR_E_COLORMODE, "SIR_E_COLORMODE"}, /**< Invalid color mode (16) */
        {SIR_E_TEXTATTR,  "SIR_E_TEXTATTR"},  /**< Invalid text attributes (17) */
        {SIR_E_TEXTCOLOR, "SIR_E_TEXTCOLOR"}, /**< Invalid text color (18) */
        {SIR_E_PLUGINBAD, "SIR_E_PLUGINBAD"}, /**< Plugin module is malformed (19) */
        {SIR_E_PLUGINDAT, "SIR_E_PLUGINDAT"}, /**< Data produced by plugin is invalid (20) */
        {SIR_E_PLUGINVER, "SIR_E_PLUGINVER"}, /**< Plugin interface version unsupported (21) */
        {SIR_E_PLUGINERR, "SIR_E_PLUGINERR"}, /**< Plugin reported failure (22) */
        {SIR_E_PLATFORM,  "SIR_E_PLATFORM"},  /**< Platform error code %d: %s (23) */
        {SIR_E_UNKNOWN,   "SIR_E_UNKNOWN"},   /**< Unknown error (4095) */
    };

    char message[SIR_MAXERROR] = {0};
    for (size_t n = 0; n < _sir_countof(errors); n++) {
        if (SIR_E_PLATFORM == errors[n].code) {
            /* cause an actual platform error. */
            (void)sir_addfile("invalid/file!name", SIRL_ALL, SIRO_DEFAULT);
        } else {
            (void)_sir_seterror(_sir_mkerror(errors[n].code));
        }

        (void)memset(message, 0, SIR_MAXERROR);
        uint16_t err = sir_geterror(message);
        _sir_eqland(pass, errors[n].code == err && *message != '\0');
        TEST_MSG("%s = %s", errors[n].name, message);

        /* ensure that sir_geterrorinfo agrees with sir_geterror, and
         * that it returns sane data. */
        sir_errorinfo errinfo = {0};
        sir_geterrorinfo(&errinfo);

        TEST_MSG("errinfo = {'%s', '%s', %"PRIu32", %"PRIu16", '%s', %d, '%s'}",
            errinfo.func, errinfo.file, errinfo.line, errinfo.code, errinfo.msg,
            errinfo.os_code, errinfo.os_msg);

        _sir_eqland(pass, errinfo.code == err);
        _sir_eqland(pass, errinfo.line != 0U);
        _sir_eqland(pass, _sir_validstrnofail(errinfo.msg));

        if (errinfo.code == SIR_E_PLATFORM) {
            _sir_eqland(pass, errinfo.os_code != 0);
            _sir_eqland(pass, _sir_validstrnofail(errinfo.os_msg));
            _sir_eqland(pass, _sir_validstrnofail(errinfo.func));
            _sir_eqland(pass, _sir_validstrnofail(errinfo.file));
        } else {
            _sir_eqland(pass, _sir_strsame(__func__, errinfo.func, strlen(__func__)));
            _sir_eqland(pass, _sir_strsame(__file__, errinfo.file, SIR_MAXPATH));
        }
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_textstylesanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

#if !defined(SIR_NO_TEXT_STYLING)
    TEST_MSG_0(SIR_WHITEB("--- explicitly invalid ---"));
    _sir_eqland(pass, !sir_settextstyle(SIRL_INFO, (sir_textattr)0xbbb, 800, 920));
    _sir_eqland(pass, sir_info("I have set an invalid text style."));

    _sir_eqland(pass, !sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_BLACK));
    _sir_eqland(pass, sir_info("oops, did it again..."));

    _sir_eqland(pass, !sir_settextstyle(SIRL_ALERT, SIRTA_NORMAL, 0xff, 0xff));
    _sir_eqland(pass, sir_info("and again."));
    PASSFAIL_MSG(pass, "\t--- explicitly invalid: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- unusual but valid ---"));
    _sir_eqland(pass, sir_settextstyle(SIRL_INFO, SIRTA_NORMAL, SIRTC_DEFAULT, SIRTC_DEFAULT));
    _sir_eqland(pass, sir_info("system default fg and bg"));
    PASSFAIL_MSG(pass, "\t--- unusual but valid: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- override defaults ---"));
    _sir_eqland(pass, sir_resettextstyles());

    _sir_eqland(pass, sir_debug("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, SIRTC_BYELLOW, SIRTC_DGRAY));
    _sir_eqland(pass, sir_debug("override style"));

    _sir_eqland(pass, sir_info("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_INFO, SIRTA_NORMAL, SIRTC_GREEN, SIRTC_MAGENTA));
    _sir_eqland(pass, sir_info("override style"));

    _sir_eqland(pass, sir_notice("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_NOTICE, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_BYELLOW));
    _sir_eqland(pass, sir_notice("override style"));

    _sir_eqland(pass, sir_warn("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_WARN, SIRTA_NORMAL, SIRTC_BLACK, SIRTC_WHITE));
    _sir_eqland(pass, sir_warn("override style"));

    _sir_eqland(pass, sir_error("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_ERROR, SIRTA_NORMAL, SIRTC_WHITE, SIRTC_BLUE));
    _sir_eqland(pass, sir_error("override style"));

    _sir_eqland(pass, sir_crit("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_CRIT, SIRTA_EMPH, SIRTC_DGRAY, SIRTC_BGREEN));
    _sir_eqland(pass, sir_crit("override style"));

    _sir_eqland(pass, sir_alert("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_ALERT, SIRTA_ULINE, SIRTC_BBLUE, SIRTC_DEFAULT));
    _sir_eqland(pass, sir_alert("override style"));

    _sir_eqland(pass, sir_emerg("default style"));
    _sir_eqland(pass, sir_settextstyle(SIRL_EMERG, SIRTA_BOLD, SIRTC_DGRAY, SIRTC_DEFAULT));
    _sir_eqland(pass, sir_emerg("override style"));
    PASSFAIL_MSG(pass, "\t--- override defaults: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- reset to defaults ---"));
    _sir_eqland(pass, sir_resettextstyles());

    _sir_eqland(pass, sir_debug("default style (debug)"));
    _sir_eqland(pass, sir_info("default style (info)"));
    _sir_eqland(pass, sir_notice("default style (notice)"));
    _sir_eqland(pass, sir_warn("default style (warning)"));
    _sir_eqland(pass, sir_error("default style (error)"));
    _sir_eqland(pass, sir_crit("default style (crit)"));
    _sir_eqland(pass, sir_alert("default style (alert)"));
    _sir_eqland(pass, sir_emerg("default style (emergency)"));
    PASSFAIL_MSG(pass, "\t--- reset to defaults: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- change mode: 256-color ---"));
    _sir_eqland(pass, sir_setcolormode(SIRCM_256));

    for (sir_textcolor fg = 0, bg = 255; (fg < 256 && bg > 0); fg++, bg--) {
        if (fg != bg) {
            _sir_eqland(pass, sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg));
            _sir_eqland(pass, sir_debug("this is 256-color mode (fg: %"PRIu32", bg: %"PRIu32")",
                fg, bg));
        }
    }

    PASSFAIL_MSG(pass, "\t--- change mode: 256-color: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- change mode: RGB-color ---"));
    _sir_eqland(pass, sir_setcolormode(SIRCM_RGB));

    for (size_t n = 0; n < 256; n++) {
        sir_textcolor fg = sir_makergb(getrand(255U), getrand(255U), getrand(255U));
        sir_textcolor bg = sir_makergb(getrand(255U), getrand(255U), getrand(255U));
        _sir_eqland(pass, sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg));
        _sir_eqland(pass, sir_debug("this is RGB-color mode (fg: %"PRIu32", %"PRIu32", %"PRIu32
            ", bg: %"PRIu32", %"PRIu32", %"PRIu32")", _sir_getredfromcolor(fg),
            _sir_getgreenfromcolor(fg), _sir_getbluefromcolor(fg), _sir_getredfromcolor(bg),
            _sir_getgreenfromcolor(bg), _sir_getbluefromcolor(bg)));
    }
    PASSFAIL_MSG(pass, "\t--- change mode: RGB-color: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- change mode: invalid mode ---"));
    _sir_eqland(pass, !sir_setcolormode(SIRCM_INVALID));
    sir_textcolor fg = sir_makergb(255, 0, 0);
    sir_textcolor bg = sir_makergb(0, 0, 0);
    _sir_eqland(pass, sir_settextstyle(SIRL_DEBUG, SIRTA_NORMAL, fg, bg));
    _sir_eqland(pass, sir_debug("this is still RGB color mode"));
    PASSFAIL_MSG(pass, "\t--- change mode: invalid mode %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- change mode: 16-color ---"));
    _sir_eqland(pass, sir_setcolormode(SIRCM_16));
    _sir_eqland(pass, sir_settextstyle(SIRL_DEBUG, SIRTA_EMPH, SIRTC_BMAGENTA, SIRTC_DEFAULT));
    _sir_eqland(pass, sir_debug("this is 16-color mode (fg: %"PRId32", bg: default)",
        SIRTC_BMAGENTA));
    PASSFAIL_MSG(pass, "\t--- change mode: 16-color: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));
#else /* SIR_NO_TEXT_STYLING */
    TEST_MSG_0("SIR_NO_TEXT_STYLING is defined; skipping");
#endif

    _sir_eqland(pass, sir_cleanup());

    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_optionssanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t iterations = 10;

    /*
     * TODO(aremmell): Update printf -> TEST_MSG,
     * rename INDENT_ITEM to BULLETED_ITEM.
     */

    /* these should all be valid. */
    TEST_MSG_0(SIR_WHITEB("--- individual valid options ---"));
    _sir_eqland(pass, _sir_validopts(SIRO_ALL));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_ALL);
    _sir_eqland(pass, _sir_validopts(SIRO_NOTIME));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOTIME);
    _sir_eqland(pass, _sir_validopts(SIRO_NOHOST));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOHOST);
    _sir_eqland(pass, _sir_validopts(SIRO_NOLEVEL));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOLEVEL);
    _sir_eqland(pass, _sir_validopts(SIRO_NONAME));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NONAME);
    _sir_eqland(pass, _sir_validopts(SIRO_NOPID));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOPID);
    _sir_eqland(pass, _sir_validopts(SIRO_NOTID));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOTID);
    _sir_eqland(pass, _sir_validopts(SIRO_NOHDR));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_NOHDR);
    _sir_eqland(pass, _sir_validopts(SIRO_MSGONLY));
    (void)printf(INDENT_ITEM SIR_WHITE("valid option: %08"PRIx32) SIR_EOL, SIRO_MSGONLY);
    PASSFAIL_MSG(pass, "\t--- individual valid options: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    /* any combination these bitwise OR'd together
     * to form a bitmask should also be valid. */
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

    TEST_MSG_0(SIR_WHITEB("--- random bitmask of valid options ---"));
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

        _sir_eqland(pass, _sir_validopts(opts));
        (void)printf(INDENT_ITEM SIR_WHITE("(%zu/%zu): random valid (count: %"PRIu32
            ", options: %08"PRIx32")") SIR_EOL, n + 1, iterations, rand_count, opts);
    }
    PASSFAIL_MSG(pass, "\t--- random bitmask of valid options: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- invalid values ---"));

    /* the lowest byte is not valid. */
    sir_options invalid = 0x000000ff;
    _sir_eqland(pass, !_sir_validopts(invalid));
    (void)printf(INDENT_ITEM SIR_WHITE("lowest byte: %08"PRIx32) SIR_EOL, invalid);

    /* gaps inbetween valid options. */
    invalid = 0x0001ff00U & ~(SIRO_NOTIME | SIRO_NOHOST | SIRO_NOLEVEL | SIRO_NONAME |
                             SIRO_NOMSEC | SIRO_NOPID | SIRO_NOTID  | SIRO_NOHDR);
    _sir_eqland(pass, !_sir_validopts(invalid));
    (void)printf(INDENT_ITEM SIR_WHITE("gaps in 0x001ff00U: %08"PRIx32) SIR_EOL, invalid);

    /* greater than SIRO_MSGONLY and less than SIRO_NOHDR. */
    for (sir_option o = 0x00008f00U; o < SIRO_NOHDR; o += 0x1000U) {
        _sir_eqland(pass, !_sir_validopts(o));
        (void)printf(INDENT_ITEM SIR_WHITE("SIRO_MSGONLY >< SIRO_NOHDR: %08"PRIx32) SIR_EOL, o);
    }

    /* greater than SIRO_NOHDR. */
    invalid = (0xFFFF0000 & ~SIRO_NOHDR);
    _sir_eqland(pass, !_sir_validopts(invalid));
    (void)printf(INDENT_ITEM SIR_WHITE("greater than SIRO_NOHDR: %08"PRIx32) SIR_EOL, invalid);

    PASSFAIL_MSG(pass, "\t--- invalid values: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_levelssanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t iterations = 10;

    /* these should all be valid. */
    TEST_MSG_0(SIR_WHITEB("--- individual valid levels ---"));
    _sir_eqland(pass, _sir_validlevel(SIRL_INFO) && _sir_validlevels(SIRL_INFO));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_INFO);
    _sir_eqland(pass, _sir_validlevel(SIRL_DEBUG) && _sir_validlevels(SIRL_DEBUG));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_DEBUG);
    _sir_eqland(pass, _sir_validlevel(SIRL_NOTICE) && _sir_validlevels(SIRL_NOTICE));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_NOTICE);
    _sir_eqland(pass, _sir_validlevel(SIRL_WARN) && _sir_validlevels(SIRL_WARN));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_WARN);
    _sir_eqland(pass, _sir_validlevel(SIRL_ERROR) && _sir_validlevels(SIRL_ERROR));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_ERROR);
    _sir_eqland(pass, _sir_validlevel(SIRL_CRIT) && _sir_validlevels(SIRL_CRIT));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_CRIT);
    _sir_eqland(pass, _sir_validlevel(SIRL_ALERT) && _sir_validlevels(SIRL_ALERT));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_ALERT);
    _sir_eqland(pass, _sir_validlevel(SIRL_EMERG) && _sir_validlevels(SIRL_EMERG));
    (void)printf(INDENT_ITEM SIR_WHITE("valid level: %04x") SIR_EOL, SIRL_EMERG);
    _sir_eqland(pass, _sir_validlevels(SIRL_ALL));
    (void)printf(INDENT_ITEM SIR_WHITE("valid levels: %04x") SIR_EOL, SIRL_ALL);
    _sir_eqland(pass, _sir_validlevels(SIRL_NONE));
    (void)printf(INDENT_ITEM SIR_WHITE("valid levels: %04x") SIR_EOL, SIRL_NONE);
    PASSFAIL_MSG(pass, "\t--- individual valid levels: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    /* any combination these bitwise OR'd together
     * to form a bitmask should also be valid. */
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

    TEST_MSG_0(SIR_WHITEB("--- random bitmask of valid levels ---"));
    uint32_t last_count = SIR_NUMLEVELS;
    for (size_t n = 0; n < iterations; n++) {
        sir_levels levels   = 0U;
        uint32_t rand_count = 0U;
        size_t last_idx     = 0UL;

        do {
            rand_count = getrand(SIR_NUMLEVELS);
        } while (rand_count == last_count || rand_count <= 1U);

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

        _sir_eqland(pass, _sir_validlevels(levels));
        (void)printf(INDENT_ITEM SIR_WHITE("(%zu/%zu): random valid (count: %"PRIu32", levels:"
                                 " %04"PRIx16) ")" SIR_EOL, n + 1, iterations, rand_count, levels);
    }
    PASSFAIL_MSG(pass, "\t--- random bitmask of valid levels: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- invalid values ---"));

    /* greater than SIRL_ALL. */
    sir_levels invalid = (0xffffU & ~SIRL_ALL);
    _sir_eqland(pass, !_sir_validlevels(invalid));
    (void)printf(INDENT_ITEM SIR_WHITE("greater than SIRL_ALL: %04"PRIx16) SIR_EOL, invalid);

    /* individual invalid level. */
    sir_level invalid2 = 0x1337U;
    _sir_eqland(pass, !_sir_validlevel(invalid2));
    (void)printf(INDENT_ITEM SIR_WHITE("individual invalid level: %04"PRIx16) SIR_EOL, invalid2);

    PASSFAIL_MSG(pass, "\t--- invalid values: %s ---" SIR_EOL, PRN_PASS(pass));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_mutexsanity(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    TEST_MSG_0(SIR_WHITEB("create, lock, unlock, destroy"));
    (void)printf(INDENT_ITEM SIR_WHITE("creating mutex...") SIR_EOL);

#if !defined(__IMPORTC__)
    sir_mutex m1 = SIR_MUTEX_INIT;
#else
    sir_mutex m1 = {0};
#endif
    _sir_eqland(pass, _sir_mutexcreate(&m1));

    (void)print_test_error(pass, pass);

    if (pass) {
        (void)printf(INDENT_ITEM SIR_WHITE("locking (wait)...") SIR_EOL);
        _sir_eqland(pass, _sir_mutexlock(&m1));

        (void)print_test_error(pass, pass);

        (void)printf(INDENT_ITEM SIR_WHITE("entered; unlocking...") SIR_EOL);
        _sir_eqland(pass, _sir_mutexunlock(&m1));

        (void)print_test_error(pass, pass);

        (void)printf(INDENT_ITEM SIR_WHITE("locking (without wait)...") SIR_EOL);
        _sir_eqland(pass, _sir_mutextrylock(&m1));

        (void)print_test_error(pass, pass);

        (void)printf(INDENT_ITEM SIR_WHITE("unlocking...") SIR_EOL);
        _sir_eqland(pass, _sir_mutexunlock(&m1));

        (void)print_test_error(pass, pass);

        (void)printf(INDENT_ITEM SIR_WHITE("destryoing...") SIR_EOL);
        _sir_eqland(pass, _sir_mutexdestroy(&m1));

        (void)print_test_error(pass, pass);

    }
    PASSFAIL_MSG(pass, "\t--- create, lock, unlock, destroy: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("invalid arguments"));
    (void)printf(INDENT_ITEM SIR_WHITE("create with NULL pointer...") SIR_EOL);
    _sir_eqland(pass, !_sir_mutexcreate(NULL));
    (void)printf(INDENT_ITEM SIR_WHITE("lock with NULL pointer...") SIR_EOL);
    _sir_eqland(pass, !_sir_mutexlock(NULL));
    (void)printf(INDENT_ITEM SIR_WHITE("trylock with NULL pointer...") SIR_EOL);
    _sir_eqland(pass, !_sir_mutextrylock(NULL));
    (void)printf(INDENT_ITEM SIR_WHITE("unlock with NULL pointer...") SIR_EOL);
    _sir_eqland(pass, !_sir_mutexunlock(NULL));
    (void)printf(INDENT_ITEM SIR_WHITE("destroy with NULL pointer...") SIR_EOL);
    _sir_eqland(pass, !_sir_mutexdestroy(NULL));
    PASSFAIL_MSG(pass, "\t--- pass invalid arguments: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass); //-V1020
}

bool sirtest_perf(void) {
    static const char* logbasename = "libsir-perf";
    static const char* logext      = "";

#if !defined(DUMA)
# if !defined(SIR_PERF_PROFILE)
#  if !defined(__WIN__)
    static const size_t perflines = 1000000;
#  else
    static const size_t perflines = 100000;
#  endif
# else
    static const size_t perflines = 4000000;
# endif
#else /* DUMA */
    static const size_t perflines = 100000;
#endif

    INIT_N(si, SIRL_ALL, SIRO_NOMSEC | SIRO_NOHOST, 0, 0, "perf");
    bool pass = si_init;

    if (pass) {
        double stdioelapsed  = 0.0;
        double fileelapsed   = 0.0;
#if !defined(SIR_PERF_PROFILE)
        double printfelapsed = 0.0;

        TEST_MSG(SIR_BLUE("%zu lines printf..."), perflines);

        sir_time printftimer = {0};
        sir_timer_start(&printftimer);

        for (size_t n = 0; n < perflines; n++)
            (void)printf(SIR_WHITE("%.2f: lorem ipsum foo bar %s: %zu") SIR_EOL,
                sir_timer_elapsed(&printftimer), "baz", 1234 + n);

        printfelapsed = sir_timer_elapsed(&printftimer);
#endif

        TEST_MSG(SIR_BLUE("%zu lines libsir (stdout)..."), perflines);

        sir_time stdiotimer = {0};
        sir_timer_start(&stdiotimer);

        for (size_t n = 0; n < perflines; n++)
            (void)sir_debug("%.2f: lorem ipsum foo bar %s: %zu",
                sir_timer_elapsed(&stdiotimer), "baz", 1234 + n);

        stdioelapsed = sir_timer_elapsed(&stdiotimer);

        _sir_eqland(pass, sir_cleanup());

        INIT(si2, 0, 0, 0, 0);
        _sir_eqland(pass, si2_init);

        char logfilename[SIR_MAXPATH] = {0};
        (void)snprintf(logfilename, SIR_MAXPATH, MAKE_LOG_NAME("%s%s"), logbasename, logext);

        sirfileid logid = sir_addfile(logfilename, SIRL_ALL, SIRO_NOMSEC | SIRO_NONAME);
        _sir_eqland(pass, 0 != logid);

        if (pass) {
            TEST_MSG(SIR_BLUE("%zu lines libsir (file)..."), perflines);

            sir_time filetimer = {0};
            sir_timer_start(&filetimer);

            for (size_t n = 0; n < perflines; n++)
                (void)sir_debug("lorem ipsum foo bar %s: %zu", "baz", 1234 + n);

            fileelapsed = sir_timer_elapsed(&filetimer);

            _sir_eqland(pass, sir_remfile(logid));
        }

        if (pass) {
#if !defined(SIR_PERF_PROFILE)
            TEST_MSG(SIR_WHITEB("printf: ") SIR_CYAN("%zu lines in %.3fsec (%.1f lines/sec)"),
                perflines, printfelapsed / 1e3, (double)perflines / (printfelapsed / 1e3));
#endif
            TEST_MSG(SIR_WHITEB("libsir (stdout): ")
                   SIR_CYAN("%zu lines in %.3fsec (%.1f lines/sec)"), perflines,
                    stdioelapsed / 1e3, (double)perflines / (stdioelapsed / 1e3));

            TEST_MSG(SIR_WHITEB("libsir (file): ")
                   SIR_CYAN("%zu lines in %.3fsec (%.1f lines/sec)"), perflines,
                    fileelapsed / 1e3, (double)perflines / (fileelapsed / 1e3));

            TEST_MSG(SIR_WHITEB("timer resolution: ") SIR_CYAN("~%ldnsec"), sir_timer_getres());
        }
    }

    unsigned deleted = 0U;
    (void)enumfiles(SIR_TESTLOGDIR, logbasename, !cl_cfg.leave_logs, &deleted);

    if (deleted > 0U)
        TEST_MSG(SIR_DGRAY("deleted %u log file(s)"), deleted);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_updatesanity(void) {
    INIT_N(si, SIRL_DEFAULT, 0, SIRL_DEFAULT, 0, "update_sanity");
    bool pass = si_init;

#define UPDATE_SANITY_ARRSIZE 10

    static const char* logfile = MAKE_LOG_NAME("update-sanity.log");
    static const sir_options opts_array[UPDATE_SANITY_ARRSIZE] = {
        SIRO_NOHOST | SIRO_NOTIME | SIRO_NOLEVEL,
        SIRO_MSGONLY,
        SIRO_NONAME | SIRO_NOTID,
        SIRO_NOPID | SIRO_NOTIME,
        SIRO_NOTIME | SIRO_NOLEVEL | SIRO_NONAME,
        SIRO_NOTIME,
        SIRO_NOMSEC | SIRO_NOHOST,
        SIRO_NOPID | SIRO_NOTID,
        SIRO_NOHOST | SIRO_NOTID,
        SIRO_ALL
    };

    static const sir_levels levels_array[UPDATE_SANITY_ARRSIZE] = {
        SIRL_NONE,
        SIRL_ALL,
        SIRL_EMERG,
        SIRL_ALERT,
        SIRL_CRIT,
        SIRL_ERROR,
        SIRL_WARN,
        SIRL_NOTICE,
        SIRL_INFO,
        SIRL_DEBUG
    };

    rmfile(logfile, cl_cfg.leave_logs);
    sirfileid id1 = sir_addfile(logfile, SIRL_DEFAULT, SIRO_DEFAULT);
    _sir_eqland(pass, 0 != id1);

    for (int i = 0; i < 10; i++) {
        if (!pass)
            break;

        /* reset to defaults*/
        _sir_eqland(pass, sir_stdoutlevels(SIRL_DEFAULT));
        _sir_eqland(pass, sir_stderrlevels(SIRL_DEFAULT));
        _sir_eqland(pass, sir_stdoutopts(SIRO_DEFAULT));
        _sir_eqland(pass, sir_stderropts(SIRO_DEFAULT));

        _sir_eqland(pass, sir_debug("default config (debug)"));
        _sir_eqland(pass, sir_info("default config (info)"));
        _sir_eqland(pass, sir_notice("default config (notice)"));
        _sir_eqland(pass, sir_warn("default config (warning)"));
        _sir_eqland(pass, sir_error("default config (error)"));
        _sir_eqland(pass, sir_crit("default config (critical)"));
        _sir_eqland(pass, sir_alert("default config (alert)"));
        _sir_eqland(pass, sir_emerg("default config (emergency)"));

        /* pick random options to set/unset */
        uint32_t rnd = getrand(UPDATE_SANITY_ARRSIZE);
        _sir_eqland(pass, sir_stdoutlevels(levels_array[rnd]));
        _sir_eqland(pass, sir_stdoutopts(opts_array[rnd]));
        TEST_MSG(SIR_WHITE("set random config #%"PRIu32" for stdout"), rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        _sir_eqland(pass, sir_stderrlevels(levels_array[rnd]));
        _sir_eqland(pass, sir_stderropts(opts_array[rnd]));
        TEST_MSG(SIR_WHITE("set random config #%"PRIu32" for stderr"), rnd);

        rnd = getrand(UPDATE_SANITY_ARRSIZE);
        _sir_eqland(pass, sir_filelevels(id1, levels_array[rnd]));
        _sir_eqland(pass, sir_fileopts(id1, opts_array[rnd]));
        TEST_MSG(SIR_WHITE("set random config #%"PRIu32" for %s"), rnd, logfile);

        _sir_eqland(pass, filter_error(sir_debug("modified config #%"PRIu32" (debug)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_info("modified config #%"PRIu32" (info)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_notice("modified config #%"PRIu32" (notice)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_warn("modified config #%"PRIu32" (warning)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_error("modified config #%"PRIu32" (error)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_crit("modified config #%"PRIu32" (critical)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_alert("modified config #%"PRIu32" (alert)", rnd), SIR_E_NODEST));
        _sir_eqland(pass, filter_error(sir_emerg("modified config #%"PRIu32" (emergency)", rnd), SIR_E_NODEST));
    }

    if (pass) {
        /* restore to default config and run again */
        _sir_eqland(pass, sir_stdoutlevels(SIRL_DEFAULT));
        _sir_eqland(pass, sir_stderrlevels(SIRL_DEFAULT));
        _sir_eqland(pass, sir_stdoutopts(SIRO_DEFAULT));
        _sir_eqland(pass, sir_stderropts(SIRO_DEFAULT));

        _sir_eqland(pass, sir_debug("default config (debug)"));
        _sir_eqland(pass, sir_info("default config (info)"));
        _sir_eqland(pass, sir_notice("default config (notice)"));
        _sir_eqland(pass, sir_warn("default config (warning)"));
        _sir_eqland(pass, sir_error("default config (error)"));
        _sir_eqland(pass, sir_crit("default config (critical)"));
        _sir_eqland(pass, sir_alert("default config (alert)"));
        _sir_eqland(pass, sir_emerg("default config (emergency)"));
    }

    _sir_eqland(pass, sir_remfile(id1));
    rmfile(logfile, cl_cfg.leave_logs);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

#if defined(SIR_SYSLOG_ENABLED) || defined(SIR_OS_LOG_ENABLED) || \
    defined(SIR_EVENTLOG_ENABLED)
static
bool generic_syslog_test(const char* sl_name, const char* identity, const char* category) {
    static const int runs = 5;

    /* repeat initializing, opening, logging, closing, cleaning up n times. */
    (void)printf("\trunning %d passes of random configs (system logger: '%s', "
                 "identity: '%s', category: '%s')..." SIR_EOL, runs, sl_name, identity, category);

# if !defined(__WIN__)
    uint32_t rnd = (uint32_t)(_sir_getpid() + _sir_gettid());
# else
    uint32_t rnd = (uint32_t)GetTickCount64();
# endif

    bool pass = true;
    for (int i = 1; i <= runs; i++) {
        /* randomly skip setting process name, identity/category to thoroughly
         * test fallback routines; randomly update the config mid-run. */
        bool set_procname = getrand_bool(rnd ^ 0x5a5a5a5aU);
        bool set_identity = getrand_bool(rnd ^ 0xc9c9c9c9U);
        bool set_category = getrand_bool(rnd ^ 0x32323232U);
        bool do_update    = getrand_bool(rnd ^ 0xe7e7e7e7U);

        TEST_MSG("set_procname: %d, set_identity: %d, set_category: %d, do_update: %d",
            set_procname, set_identity, set_category, do_update);

        INIT_SL(si, SIRL_ALL, SIRO_NOHOST | SIRO_NOTID, 0, 0, (set_procname ? "sir_sltest" : ""));
        si.d_syslog.opts   = SIRO_DEFAULT;
        si.d_syslog.levels = SIRL_DEFAULT;

        if (set_identity)
            (void)_sir_strncpy(si.d_syslog.identity, SIR_MAX_SYSLOG_CAT, identity, SIR_MAX_SYSLOG_ID);

        if (set_category)
            (void)_sir_strncpy(si.d_syslog.category, SIR_MAX_SYSLOG_CAT, category, SIR_MAX_SYSLOG_CAT);

        _sir_eqland(pass, sir_init(&si));

        if (do_update)
            _sir_eqland(pass, sir_sysloglevels(SIRL_ALL));

        _sir_eqland(pass, sir_debug("%d/%d: this debug message sent to stdout and %s.", i, runs, sl_name));
        _sir_eqland(pass, sir_info("%d/%d: this info message sent to stdout and %s.", i, runs, sl_name));

        _sir_eqland(pass, sir_notice("%d/%d: this notice message sent to stdout and %s.", i, runs, sl_name));
        _sir_eqland(pass, sir_warn("%d/%d: this warning message sent to stdout and %s.", i, runs, sl_name));
        _sir_eqland(pass, sir_error("%d/%d: this error message sent to stdout and %s.", i, runs, sl_name));

        if (set_identity) {
            _sir_eqland(pass, sir_syslogid("my test ID"));
            _sir_eqland(pass, sir_syslogid("my test ID")); /* test deduping. */
        }

        if (set_category) {
            _sir_eqland(pass, sir_syslogcat("my test category"));
            _sir_eqland(pass, sir_syslogcat("my test category")); /* test deduping. */
        }

        if (do_update)
            _sir_eqland(pass, sir_syslogopts(SIRO_MSGONLY & ~(SIRO_NOLEVEL | SIRO_NOPID)));

        _sir_eqland(pass, sir_crit("%d/%d: this critical message sent to stdout and %s.", i, runs, sl_name));
        _sir_eqland(pass, sir_alert("%d/%d: this alert message sent to stdout and %s.", i, runs, sl_name));
        _sir_eqland(pass, sir_emerg("%d/%d: this emergency message sent to stdout and %s.", i, runs, sl_name));

# if defined(SIR_OS_LOG_ENABLED)
#  if defined(__MACOS__) && !defined(__INTEL_COMPILER)
        if (i == runs -1 && 0 == strncmp(sl_name, "os_log", 6)) {
            TEST_MSG_0("testing os_log activity feature...");

            /* also test activity grouping in Console. there's only one way to validate
             * this and that's by manually viewing the log. */
            os_activity_t parent = os_activity_create("flying to the moon", //-V530
                OS_ACTIVITY_NONE, OS_ACTIVITY_FLAG_DETACHED);

            /* execution now passes to os_log_parent_activity(), where some logging
             * will occur, then a sub-activity will be created, and more logging. */
            os_activity_apply_f(parent, (void*)parent, os_log_parent_activity);
        }
#  endif
# endif

        _sir_eqland(pass, sir_cleanup());

        if (!pass)
            break;
    }

    return PRINT_RESULT_RETURN(pass);
}
#endif

#if defined(SIR_NO_SYSTEM_LOGGERS)
static bool generic_disabled_syslog_test(const char* sl_name, const char* identity,
    const char* category) {
    INIT_SL(si, SIRL_ALL, SIRO_NOHOST | SIRO_NOTID, 0U, 0U, "sir_disabled_sltest");
    si.d_syslog.opts   = SIRO_DEFAULT;
    si.d_syslog.levels = SIRL_DEFAULT;
    bool pass = true;

    SIR_UNUSED(sl_name);

    TEST_MSG_0("SIR_NO_SYSTEM_LOGGERS is defined; expecting calls to fail...");

    /* init should just ignore the syslog settings. */
    _sir_eqland(pass, sir_init(&si));

    /* these calls should all fail. */
    TEST_MSG_0("setting levels...");
    _sir_eqland(pass, !sir_sysloglevels(SIRL_ALL));

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG_0("setting options...");
    _sir_eqland(pass, !sir_syslogopts(SIRO_DEFAULT));

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG_0("setting identity...");
    _sir_eqland(pass, !sir_syslogid(identity));

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG_0("setting category...");
    _sir_eqland(pass, !sir_syslogcat(category));

    if (pass)
        PRINT_EXPECTED_ERROR();

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}
#endif

bool sirtest_syslog(void) {
#if !defined(SIR_SYSLOG_ENABLED)
# if defined(SIR_NO_SYSTEM_LOGGERS)
    bool pass = generic_disabled_syslog_test("syslog", "sirtests", "tests");
    return PRINT_RESULT_RETURN(pass);
# else
    TEST_MSG_0(SIR_DGRAY("SIR_SYSLOG_ENABLED is not defined; skipping"));
    return true;
# endif
#else
    bool pass = generic_syslog_test("syslog", "sirtests", "tests");
    return PRINT_RESULT_RETURN(pass);
#endif
}

bool sirtest_os_log(void) {
#if !defined(SIR_OS_LOG_ENABLED)
# if defined(SIR_NO_SYSTEM_LOGGERS)
    bool pass = generic_disabled_syslog_test("os_log", "com.aremmell.libsir.tests", "tests");
    return PRINT_RESULT_RETURN(pass);
# else
    TEST_MSG_0(SIR_DGRAY("SIR_OS_LOG_ENABLED is not defined; skipping"));
    return true;
# endif
#else
    bool pass = generic_syslog_test("os_log", "com.aremmell.libsir.tests", "tests");
    return PRINT_RESULT_RETURN(pass);
#endif
}

bool sirtest_win_eventlog(void) {
#if !defined(SIR_EVENTLOG_ENABLED)
# if defined(SIR_NO_SYSTEM_LOGGERS)
    bool pass = generic_disabled_syslog_test("eventlog", "sirtests", "tests");
    return PRINT_RESULT_RETURN(pass);
# else
    TEST_MSG_0(SIR_DGRAY("SIR_EVENTLOG_ENABLED is not defined; skipping"));
    return true;
# endif
#else
    bool pass = generic_syslog_test("eventlog", "sirtests", "tests");
    return PRINT_RESULT_RETURN(pass);
#endif
}

bool sirtest_filesystem(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    /* Wine version */
    TEST_MSG("Running under Wine: %s",
            get_wineversion() ? get_wineversion() : "no");

    /* current working directory. */
    char* cwd = _sir_getcwd();
    _sir_eqland(pass, NULL != cwd);
    TEST_MSG("_sir_getcwd: '%s'", PRN_STR(cwd));

    if (NULL != cwd) {
        /* path to this binary file. */
        char* filename = _sir_getappfilename();
        _sir_eqland(pass, NULL != filename);
        TEST_MSG("_sir_getappfilename: '%s'", PRN_STR(filename));

        if (NULL != filename) {
            /* _sir_get[base|dir]name() can potentially modify filename,
             * so make a copy for each call. */
            char* filename2 = strndup(filename, strnlen(filename, SIR_MAXPATH));
            _sir_eqland(pass, NULL != filename2);

            if (NULL != filename2) {
                /* filename, stripped of directory component(s). */
                char* _basename = _sir_getbasename(filename2);
                TEST_MSG("_sir_getbasename: '%s'", PRN_STR(_basename));

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
                    }
                }
            }

            /* directory this binary file resides in. */
            char* appdir = _sir_getappdir();
            _sir_eqland(pass, NULL != appdir);
            TEST_MSG("_sir_getappdir: '%s'", PRN_STR(appdir));

            /* _sir_get[base|dir]name can potentially modify filename,
             * so make a copy for each call. */
            char* filename3 = strndup(filename, strnlen(filename, SIR_MAXPATH));
            _sir_eqland(pass, NULL != filename3);

            if (NULL != appdir && NULL != filename3) {
                /* should yield the same result as _sir_getappdir(). */
                char* _dirname = _sir_getdirname(filename3);
                TEST_MSG("_sir_getdirname: '%s'", PRN_STR(_dirname));

                _sir_eqland(pass, 0 == strncmp(filename, appdir, strnlen(appdir, SIR_MAXPATH)));
                _sir_eqland(pass, NULL != _dirname &&
                    0 == strncmp(filename, _dirname, strnlen(_dirname, SIR_MAXPATH)));
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
            TEST_MSG("_sir_getdirname(" SIR_WHITE("'%s'") ") = " SIR_WHITE("'%s'") "",
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
            TEST_MSG("_sir_getbasename(" SIR_WHITE("'%s'") ") = " SIR_WHITE("'%s'") "",
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

        if (relative == abs_or_rel_paths[n].abs) {
            pass = false;
            TEST_MSG(SIR_RED("_sir_ispathrelative('%s') = %s"), abs_or_rel_paths[n].path,
                relative ? "true" : "false");
        } else {
            TEST_MSG(SIR_GREEN("_sir_ispathrelative('%s') = %s"), abs_or_rel_paths[n].path,
                relative ? "true" : "false");
        }

        _sir_eqland(pass, ret);
        if (!ret) {
            bool unused = print_test_error(false, false);
            SIR_UNUSED(unused);
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
        {"\\", true},
        {".\\", true},
        {"..\\", true},
#endif
        {"../../LICENSES/MIT.txt", true},
        {"../../msvs/libsir.sln", true},
        {"./", true},
        {"../", true},
        {"file.exists", true}
    };

    for (size_t n = 0; n < _sir_countof(real_or_not); n++) {
        bool exists = false;
        bool ret    = _sir_pathexists(real_or_not[n].path, &exists, SIR_PATH_REL_TO_APP);

        if (exists != real_or_not[n].exists) {
            pass = false;
            TEST_MSG(SIR_RED("_sir_pathexists('%s') = %s"), real_or_not[n].path,
                exists ? "true" : "false");
        } else {
            TEST_MSG(SIR_GREEN("_sir_pathexists('%s') = %s"), real_or_not[n].path,
                exists ? "true" : "false");
        }

        _sir_eqland(pass, ret);
        if (!ret) {
            bool unused = print_test_error(false, false);
            SIR_UNUSED(unused);
        }
    }

    /* checking file descriptors. */
    static int bad_fds[] = {
        0,
        1,
        2,
        1234
    };

    if (get_wineversion()) {
        bad_fds[3] = 0;
    }

    for (size_t n = 0; n < _sir_countof(bad_fds); n++) {
        if (_sir_validfd(bad_fds[n])) {
            pass = false;
            TEST_MSG(SIR_RED("_sir_validfd(%d) = true"), bad_fds[n]);
        } else {
            TEST_MSG(SIR_GREEN("_sir_validfd(%d) = false"), bad_fds[n]);
        }
    }

    FILE* f = NULL;
    bool ret = _sir_openfile(&f, "file.exists", "r", SIR_PATH_REL_TO_APP);
    if (!ret) {
        pass = false;
        HANDLE_OS_ERROR(true, "fopen(%s) failed!", "file.exists");
    } else {
        int fd = fileno(f);
        if (!_sir_validfd(fd)) {
            pass = false;
            TEST_MSG(SIR_RED("_sir_validfd(%d) = false"), fd);
        } else {
            TEST_MSG(SIR_GREEN("_sir_validfd(%d) = true"), fd);
        }
    }

    _sir_safefclose(&f);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_squelchspam(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    static const size_t alternate  = 50;
    static const size_t sequence[] = {
        1000, /* non-repeating messages. */
        1000, /* repeating messages. */
        1000, /* alternating repeating and non-repeating messages. */
        100   /* repeating messages, but on different levels. */
    };

    sir_time timer;
    sir_timer_start(&timer);

    TEST_MSG(SIR_BLUE("%zu non-repeating messages..."), sequence[0]);

    for (size_t n = 0, ascii_idx = 33; n < sequence[0]; n++, ascii_idx++) {
        _sir_eqland(pass, sir_debug("%c%c a non-repeating message", (char)ascii_idx,
            (char)ascii_idx + 1));

        if (ascii_idx == 125)
            ascii_idx = 33;
    }

    TEST_MSG(SIR_BLUE("%zu repeating messages..."), sequence[1]);

    for (size_t n = 0; n < sequence[1]; n++) {
        bool ret = sir_debug("a repeating message");

        if (n >= SIR_SQUELCH_THRESHOLD - 1)
            _sir_eqland(pass, !ret);
        else
            _sir_eqland(pass, ret);
    }

    TEST_MSG(SIR_BLUE("%zu alternating repeating and non-repeating messages..."),
        sequence[2]);

    bool repeating   = false;
    size_t counter   = 0;
    size_t repeat_id = 0;
    for (size_t n = 0, ascii_idx = 33; n < sequence[2]; n++, counter++, ascii_idx++) {
        if (!repeating) {
            _sir_eqland(pass, sir_debug("%c%c a non-repeating message", (char)ascii_idx,
                (char)ascii_idx + 1));
        } else {
            bool ret = sir_debug("%zu a repeating message", repeat_id);

            if (counter - 1 >= SIR_SQUELCH_THRESHOLD - 1)
                _sir_eqland(pass, !ret);
            else
                _sir_eqland(pass, ret);
        }

        if (counter == alternate) {
            repeating = !repeating;
            counter = 0;
            repeat_id++;
        }

        if (ascii_idx == 125)
            ascii_idx = 33;
    }

    TEST_MSG(SIR_BLUE("%zu repeating messages, but on different levels..."), sequence[3]);

    for (size_t n = 0; n < sequence[3]; n++) {
        if (n % 2 == 0)
            _sir_eqland(pass, sir_debug("a repeating message"));
        else
            _sir_eqland(pass, sir_info("a repeating message"));
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
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

    TEST_MSG_0("SIR_NO_PLUGINS is defined; expecting calls to fail");

    TEST_MSG("loading good plugin: '%s'...", plugin1);
    /* load a valid, well-behaved plugin. */
    sirpluginid id = sir_loadplugin(plugin1);
    _sir_eqland(pass, 0 == id);

    if (pass)
        PRINT_EXPECTED_ERROR();

    TEST_MSG("unloading good plugin: '%s'...", plugin1);
    /* also try the unload function. */
    _sir_eqland(pass, !sir_unloadplugin(id));

    if (pass)
        PRINT_EXPECTED_ERROR();
#else
    /* load a valid, well-behaved plugin. */
    TEST_MSG("loading good plugin: '%s'...", plugin1);
    sirpluginid id = sir_loadplugin(plugin1);
    _sir_eqland(pass, 0 != id);

    (void)print_test_error(pass, pass);

    _sir_eqland(pass, sir_info("this message will be dispatched to the plugin."));
    _sir_eqland(pass, sir_warn("this message will *not* be dispatched to the plugin."));

    /* re-loading the same plugin should fail. */
    TEST_MSG("loading duplicate plugin: '%s'...", plugin1);
    sirpluginid badid = sir_loadplugin(plugin1);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    /* the following are all invalid or misbehaved, and should all fail. */
    TEST_MSG("loading bad plugin: '%s'...", plugin2);
    badid = sir_loadplugin(plugin2);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    TEST_MSG("loading bad plugin: '%s'...", plugin3);
    badid = sir_loadplugin(plugin3);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    TEST_MSG("loading bad plugin: '%s'...", plugin4);
    badid = sir_loadplugin(plugin4);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    TEST_MSG("loading bad plugin: '%s'...", plugin5);
    badid = sir_loadplugin(plugin5);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    TEST_MSG("loading bad plugin: '%s'...", plugin6);
    badid = sir_loadplugin(plugin6);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    TEST_MSG("loading bad plugin: '%s'...", plugin7);
    badid = sir_loadplugin(plugin7);
    _sir_eqland(pass, 0 != badid); /* this one should load, just return false from write */

    _sir_eqland(pass, !sir_info("should fail; a plugin failed to process the message."));

    (void)print_test_error(pass, pass);

    TEST_MSG("loading nonexistent plugin: '%s'...", plugin8);
    badid = sir_loadplugin(plugin8);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);

    /* unload the good plugin manually. */
    TEST_MSG("unloading good plugin: '%s'...", plugin1);
    _sir_eqland(pass, sir_unloadplugin(id));

    (void)print_test_error(pass, pass);

    /* try to unload the plugin again. */
    TEST_MSG("unloading already unloaded plugin '%s'...", plugin1);
    _sir_eqland(pass, !sir_unloadplugin(id));

    (void)print_test_error(pass, pass);

    /* test bad paths. */
    TEST_MSG_0("trying to load plugin with NULL path...");
    badid = sir_loadplugin(NULL);
    _sir_eqland(pass, 0 == badid);

    (void)print_test_error(pass, pass);
#endif
    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_stringutils(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    char str[] = "Kneel  \f \n  before  \t \r \v  Zod!?";

    TEST_MSG_0(SIR_WHITEB("--- valid string utility usage ---"));

    _sir_eqland(pass, _sir_strsqueeze(str) && 0 == strncmp(str, "Kneel before Zod!?", 18));
    TEST_MSG("_sir_strsqueeze:            '%s'", str);

    _sir_eqland(pass, _sir_strremove(str, "!") && 0 == strncmp(str, "Kneel before Zod?", 17));
    TEST_MSG("_sir_strremove(\"!\"):        '%s'", str);

    _sir_eqland(pass, _sir_strreplace(str, '?', '.') && 0 == strncmp(str, "Kneel before Zod.", 17));
    TEST_MSG("_sir_strreplace(\"?\", \".\"):  '%s'", str);

    _sir_eqland(pass, 1 == _sir_strcreplace(str, '.', '!', 1) && 0 == strncmp(str, "Kneel before Zod!", 17));
    TEST_MSG("_sir_strcreplace(\".\", \"!\"): '%s'", str);

    _sir_eqland(pass, _sir_strredact(str, "e", '*') && 0 == strncmp(str, "Kn**l b*for* Zod!", 17));
    TEST_MSG("_sir_strredact(\"e\", \"*\"):   '%s'", str);

    _sir_eqland(pass, _sir_strredact(str, "X", 'Y') && 0 == strncmp(str, "Kn**l b*for* Zod!", 17));
    TEST_MSG("_sir_strredact(\"X\", \"Y\"):   '%s'", str);

    PASSFAIL_MSG(pass, "\t--- valid string utility usage: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- invalid string utility usage - NULL pointer ---"));

    TEST_MSG_0("_sir_strsqueeze:  NULL pointer");
    _sir_eqland(pass, !_sir_strsqueeze(NULL));

    TEST_MSG_0("_sir_strremove:   NULL pointer");
    _sir_eqland(pass, !_sir_strremove(NULL, "sub"));

    TEST_MSG_0("_sir_strreplace:  NULL pointer");
    _sir_eqland(pass, !_sir_strreplace(NULL, 'c', 'n'));

    TEST_MSG_0("_sir_strcreplace: NULL pointer");
    _sir_eqland(pass, !_sir_strcreplace(NULL, 'c', 'n', -1));

    TEST_MSG_0("_sir_strredact:   NULL pointer");
    _sir_eqland(pass, !_sir_strredact(NULL, "s", '*'));

    PASSFAIL_MSG(pass, "\t--- invalid string utility usage - NULL pointer: %s ---" SIR_EOL SIR_EOL, PRN_PASS(pass));

    TEST_MSG_0(SIR_WHITEB("--- invalid string utility usage - bad parameters ---"));

    TEST_MSG_0("_sir_strremove:   bad parameter \"sub\"");
    _sir_eqland(pass, _sir_strremove(str, NULL));

    TEST_MSG_0("_sir_strreplace:  bad parameter 'c'");
    _sir_eqland(pass, _sir_strreplace(str, 0, 'n'));

    TEST_MSG_0("_sir_strreplace:  bad parameter 'n'");
    _sir_eqland(pass, _sir_strreplace(str, 'c', 0));

    TEST_MSG_0("_sir_strcreplace: bad parameter 'c'");
    _sir_eqland(pass, !_sir_strcreplace(str, 0, 'n', -1));

    TEST_MSG_0("_sir_strcreplace: bad parameter 'n'");
    _sir_eqland(pass, !_sir_strcreplace(str, 'c', 0, -1));

    TEST_MSG_0("_sir_strcreplace: bad parameter 'max'");
    _sir_eqland(pass, !_sir_strcreplace(str, 'c', 'n', 0));

    TEST_MSG_0("_sir_strredact:   bad parameter \"sub\"");
    _sir_eqland(pass, _sir_strredact(str, NULL, '*'));

    TEST_MSG_0("_sir_strredact:   bad parameter 'c'");
    _sir_eqland(pass, _sir_strredact(str, "sub", 0));

    PASSFAIL_MSG(pass, "\t--- invalid string utility usage - bad parameters: %s ---" SIR_EOL, PRN_PASS(pass));

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_getcpucount(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    TEST_MSG_0("checking processor counting function...");

    const long cpus = _sir_nprocs_test();
    _sir_eqland(pass, 0 < cpus);

    TEST_MSG("processor(s) detected: %ld", cpus);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

bool sirtest_getversioninfo(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    TEST_MSG_0("checking version retrieval functions...");

    const char* str = sir_getversionstring();
    _sir_eqland(pass, _sir_validstrnofail(str));

    TEST_MSG("version as string: '%s'", _SIR_PRNSTR(str));

    uint32_t hex = sir_getversionhex();
    _sir_eqland(pass, 0 != hex);

    TEST_MSG("version as hex: 0x%08"PRIx32"", hex);

    bool prerel = sir_isprerelease();
    TEST_MSG("prerelease: %s", prerel ? "true" : "false");

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

enum {
    NUM_THREADS = 4
};

static bool threadpool_pseudojob(void* arg) {
    char thread_name[SIR_MAXPID] = {0};

    _sir_snprintf_trunc(thread_name, SIR_MAXPID, "pool.%p", arg);
    (void)_sir_setthreadname(thread_name);

    (void)sir_debug("start of pseudo job that does nothing (arg: %p)", arg);
    sir_sleep_msec(1000);
    (void)sir_debug("end of pseudo job that does nothing (arg: %p)", arg);

    return true;
}

bool sirtest_threadpool(void) {
    INIT(si, SIRL_ALL, SIRO_NOTIME | SIRO_NOHOST | SIRO_NONAME, 0, 0);
    bool pass = si_init;

    static const size_t num_jobs = 30;
    sir_threadpool* pool         = NULL;

    _sir_eqland(pass, _sir_threadpool_create(&pool, NUM_THREADS));
    if (pass) {
        /* dispatch a whole bunch of jobs. */
        for (size_t n = 0; n < num_jobs; n++) {
            sir_threadpool_job* job = calloc(1, sizeof(sir_threadpool_job));
            _sir_eqland(pass, NULL != job);
            if (job) {
                job->fn = &threadpool_pseudojob;
                job->data = (void*)(n + 1);
                _sir_eqland(pass, _sir_threadpool_add_job(pool, job));
                _sir_eqland(pass, sir_info("dispatched job (fn: %"PRIxPTR", data: %p)",
                    (uintptr_t)job->fn, job->data));
            }
        }

        sir_sleep_msec(1000);

        _sir_eqland(pass, sir_info("destroying thread pool..."));
        _sir_eqland(pass, _sir_threadpool_destroy(&pool));
    }

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

#if !defined(__WIN__)
static void* threadrace_thread(void* arg);
#else /* __WIN__ */
static unsigned __stdcall threadrace_thread(void* arg);
#endif

typedef struct {
    char log_file[SIR_MAXPATH];
    bool pass;
} thread_args;

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
    if (!heap_args) {
        HANDLE_OS_ERROR(true, "calloc() %zu bytes failed!", NUM_THREADS * sizeof(thread_args));
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
            HANDLE_OS_ERROR(true, "pthread_create() for thread #%zu failed!", n + 1);
#else /* __WIN__ */
        thrds[n] = _beginthreadex(NULL, 0, threadrace_thread, (void*)&heap_args[n], 0, NULL);
        if (0 == thrds[n]) {
            HANDLE_OS_ERROR(true, "_beginthreadex() for thread #%zu failed!", n + 1);
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
            TEST_MSG("waiting for thread %zu/%zu...", j + 1, last_created + 1);
#if !defined(__WIN__)
            int join = pthread_join(thrds[j], NULL);
            if (0 != join) {
                joined = false;
                errno  = join;
                HANDLE_OS_ERROR(true, "pthread_join() for thread #%zu failed!", j + 1);
            }
#else /* __WIN__ */
            DWORD wait = WaitForSingleObject((HANDLE)thrds[j], INFINITE);
            if (WAIT_OBJECT_0 != wait) {
                joined = false;
                HANDLE_OS_ERROR(false, "WaitForSingleObject() for thread #%zu (%p) failed!", j + 1,
                    (HANDLE)thrds[j]);
            }
#endif
            _sir_eqland(pass, joined);
            if (joined) {
                TEST_MSG("thread %zu/%zu joined", j + 1, last_created + 1);

                _sir_eqland(pass, heap_args[j].pass);
                if (heap_args[j].pass)
                    TEST_MSG(SIR_GREEN("thread #%zu returned pass = true"), j + 1);
                else
                    TEST_MSG(SIR_RED("thread #%zu returned pass = false!"), j + 1);
            }
        }
    }

    _sir_safefree(&heap_args);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}

#if !defined(__WIN__)
static void* threadrace_thread(void* arg) {
#else /* __WIN__ */
unsigned __stdcall threadrace_thread(void* arg) {
#endif
    pid_t threadid       = _sir_gettid();
    thread_args* my_args = (thread_args*)arg;

    rmfile(my_args->log_file, cl_cfg.leave_logs);
    sirfileid id = sir_addfile(my_args->log_file, SIRL_ALL, SIRO_MSGONLY);

    if (0U == id) {
        bool unused = print_test_error(false, false);
        SIR_UNUSED(unused);
#if !defined(__WIN__)
        return NULL;
#else /* __WIN__ */
        return 0;
#endif
    }

    TEST_MSG("hi, i'm thread (id: " SIR_TIDFORMAT "), logging to: '%s'...",
            PID_CAST threadid, my_args->log_file);

#if !defined(DUMA)
# if !defined(__EMSCRIPTEN__)
#  define NUM_ITERATIONS 400
# else
#  define NUM_ITERATIONS 200
# endif
#else
# define NUM_ITERATIONS 100
#endif

    for (size_t n = 0; n < NUM_ITERATIONS; n++) {
        /* choose a random level, and colors. */
        sir_textcolor fg = SIRTC_DEFAULT;
        sir_textcolor bg = SIRTC_DEFAULT;

        if (n % 2 == 0) {
            fg = SIRTC_CYAN;
            bg = SIRTC_BLACK;
            (void)sir_debug("log message #%zu", n);
        } else {
            fg = SIRTC_BLACK;
            bg = SIRTC_CYAN;
            (void)sir_info("log message #%zu", n);
        }

        /* sometimes remove and re-add the log file, and set some options/styles.
         * other times, just set different options/styles. */
        uint32_t rnd = (uint32_t)(n + threadid);
        if (getrand_bool(rnd > 1U ? rnd : 1U)) {
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

    rmfile(my_args->log_file, cl_cfg.leave_logs);

#if !defined(__WIN__)
    return NULL;
#else /* __WIN__ */
    return 0U;
#endif
}

/*
bool sirtest_XXX(void) {
    INIT(si, SIRL_ALL, 0, 0, 0);
    bool pass = si_init;

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}
*/

#if defined(SIR_OS_LOG_ENABLED)
void os_log_parent_activity(void* ctx) {
    (void)sir_debug("confirming with ground control that we are a go...");
    (void)sir_info("all systems go; initiating launch sequence");
    (void)sir_warn("getting some excessive vibration here");
    (void)sir_info("safely reached escape velocity. catch you on the flip side");
    (void)sir_info("(3 days later) we have landed on the lunar surface");
    (void)sir_notice("beginning rock counting...");

    os_activity_t parent = (os_activity_t)ctx;
    os_activity_t child = os_activity_create("counting moon rocks", parent, //-V530
        OS_ACTIVITY_FLAG_DEFAULT);

    float rock_count = 0.0f;
    os_activity_apply_f(child, (void*)&rock_count, os_log_child_activity);
    (void)sir_info("astronauts safely back on board. official count: ~%.02f moon rocks",
        (double)rock_count);
}

void os_log_child_activity(void* ctx) {
    (void)sir_info("there are a lot of rocks here; we're going to be here a while");

    for (size_t n = 0; n < 10; n++) {
        (void)sir_info("counting rocks in sector %zu...", n);
    }

    float* rock_count = (float*)ctx;
    *rock_count = 1e12f;
    (void)sir_info("all sectors counted; heading back to the lunar lander");
}
#endif

/* ========================== end tests ========================== */

bool filter_error(bool pass, uint16_t err) {
    if (!pass) {
        char msg[SIR_MAXERROR] = {0};
        if (sir_geterror(msg) != err)
            return false;
    }
    return true;
}

char *get_wineversion(void) {
#if !defined(__WIN__)
    return NULL;
#else /* __WIN__ */
    typedef char* (__stdcall *get_wine_ver_proc)(void);
    static get_wine_ver_proc _p_wine_get_version = NULL;

    HMODULE _h_ntdll = GetModuleHandle("ntdll.dll");
    if (_h_ntdll != NULL) {
        _p_wine_get_version = (get_wine_ver_proc)GetProcAddress(_h_ntdll, "wine_get_version");
        if (_p_wine_get_version) {
            char *wine_version = _p_wine_get_version();
            if (wine_version)
                return wine_version;
        }
    }
    return NULL;
#endif
}

bool roll_and_archive(const char* filename, const char* extension) {
    /* roll size minus 1KiB so we can write until it maxes. */
    static const long deltasize = 1024L;
    const long fillsize         = SIR_FROLLSIZE - deltasize;

    char logfilename[SIR_MAXPATH] = {0};
    (void)snprintf(logfilename, SIR_MAXPATH, MAKE_LOG_NAME("%s%s"), filename, extension);

    TEST_MSG_0("deleting any stale logs from a previous run...");

    unsigned delcount = 0U;
    if (!enumfiles(SIR_TESTLOGDIR, filename, !cl_cfg.leave_logs, &delcount)) {
        HANDLE_OS_ERROR(false, "failed to enumerate log files with base name: %s!", filename);
        return false;
    }

    if (delcount > 0U)
        TEST_MSG("found and removed %u log file(s)", delcount);

    FILE* f = NULL;
    _sir_fopen(&f, logfilename, "w");

    if (NULL == f)
        return print_test_error(false, false);

    TEST_MSG("filling %s nearly to SIR_FROLLSIZE...", logfilename);

    if (0 != fseek(f, fillsize, SEEK_SET)) {
        HANDLE_OS_ERROR(true, "fseek in file %s failed!", logfilename);
        _sir_safefclose(&f);
        return false;
    }

    if (EOF == fputc('\0', f)) {
        HANDLE_OS_ERROR(true, "fputc in file %s failed!", logfilename);
        _sir_safefclose(&f);
        return false;
    }

    _sir_safefclose(&f);

    INIT(si, 0, 0, 0, 0);
    bool pass = si_init;

    TEST_MSG("adding %s to libsir...", logfilename);

    sirfileid fileid = sir_addfile(logfilename, SIRL_DEBUG, SIRO_MSGONLY | SIRO_NOHDR);
    _sir_eqland(pass, 0U != fileid);

    (void)print_test_error(pass, false);

    if (pass) {
        static const char* line = "hello, i am some data. nice to meet you.";
        TEST_MSG("writing to %s until SIR_FROLLSIZE has been exceeded...", logfilename);

        /* write an (approximately) known quantity until we should have rolled */
        size_t written  = 0;
        size_t linesize = strnlen(line, SIR_MAXMESSAGE);

        do {
            _sir_eqland(pass, sir_debug("%zu %s", written, line));
            written += linesize;
        } while (pass && (written < deltasize + (linesize * 50)));

        TEST_MSG_0("looking for two log files, since it should have been rolled...");

        /* look for files matching the original name. */
        unsigned foundlogs = 0U;
        if (!enumfiles(SIR_TESTLOGDIR, filename, false, &foundlogs)) {
            HANDLE_OS_ERROR(false, "failed to enumerate log files with base name: %s!", filename);
            pass = false;
        }

        /* if two (or more) are present, the test is a pass. */
        TEST_MSG("found %u log files with base name: %s", foundlogs, filename);
        _sir_eqland(pass, foundlogs >= 2U);
    }

    _sir_eqland(pass, sir_remfile(fileid));

    delcount = 0U;
    if (!enumfiles(SIR_TESTLOGDIR, filename, !cl_cfg.leave_logs, &delcount)) {
        HANDLE_OS_ERROR(false, "failed to enumerate log files with base name: %s!", filename);
        pass = false;
    }

    if (delcount > 0U)
        TEST_MSG("found and removed %u log file(s)", delcount);

    _sir_eqland(pass, sir_cleanup());
    return PRINT_RESULT_RETURN(pass);
}
