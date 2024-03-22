/*
 * tests_shared.c
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

#include "tests_shared.h"

void print_intro(size_t tgt_tests) {
    (void)printf(SIR_EOL SIR_WHITEB(SIR_ULINE("libsir") " %s (%s) running %zu %s...") SIR_EOL,
        sir_getversionstring(), (sir_isprerelease() ? "prerelease" : "release"),
        tgt_tests, TEST_S(tgt_tests));
}

void print_test_intro(size_t num, size_t tgt_tests, const char* const name) {
    (void)printf(SIR_EOL SIR_WHITEB("(%zu/%zu) '%s'...") SIR_EOL SIR_EOL, num, tgt_tests, name);
}

void print_test_outro(size_t num, size_t tgt_tests, const char* const name, bool pass) {
    (void)printf(SIR_EOL SIR_WHITEB("(%zu/%zu) '%s' finished: ") "%s" SIR_EOL, num, tgt_tests, name,
                PRN_PASS(pass));
}

void print_test_summary(size_t tgt_tests, size_t passed, double elapsed) {
    elapsed = (elapsed / 1e3);
    if (tgt_tests == passed) {
        (void)printf(SIR_EOL SIR_WHITEB("done: ")
            SIR_GREENB("%s%zu " SIR_ULINE("libsir") " %s " SIR_EMPH("passed") " in %.03fsec!")
            SIR_EOL SIR_EOL, tgt_tests > 1 ? "all " : "", tgt_tests, TEST_S(tgt_tests), elapsed);
    } else {
        (void)printf(SIR_EOL SIR_WHITEB("done: ")
            SIR_REDB("%zu of %zu " SIR_ULINE("libsir") " %s " SIR_EMPH("failed") " in %.03fsec")
            SIR_EOL SIR_EOL, tgt_tests - passed, tgt_tests, TEST_S(tgt_tests), elapsed);
    }
}

void print_failed_test_intro(size_t tgt_tests, size_t passed) {
    (void)printf(SIR_REDB("Failed %s:") SIR_EOL SIR_EOL, TEST_S(tgt_tests - passed));
}

void print_failed_test(const char* const name) {
    (void)printf(SIR_RED(INDENT_ITEM "%s") SIR_EOL, name);
}

bool mark_test_to_run(const char* const name, sir_test* tests, size_t num_tests) {
    bool found = false;
    for (size_t t = 0; t < num_tests; t++) {
        if (_sir_strsame(name, tests[t].name, strnlen(name, SIR_MAXTESTNAME))) {
            found = tests[t].run = true;
            break;
        }
    }
    return found;
}

void print_test_list(const sir_test* tests, size_t num_tests) {
    static const size_t tab_size = 4;

    size_t longest = 0;
    for (size_t i = 0; i < num_tests; i++) {
        size_t len = strnlen(tests[i].name, SIR_MAXTESTNAME);
        if (len > longest)
            longest = len;
    }

    (void)printf(SIR_EOL SIR_WHITE("Available tests:") SIR_EOL SIR_EOL);

    for (size_t i = 0; i < num_tests; i++) {
        (void)printf("\t%s", tests[i].name);

        size_t len = strnlen(tests[i].name, SIR_MAXTESTNAME);
        if (len < longest + tab_size) {
            for (size_t n = len; n < longest + tab_size; n++)
                (void)printf(" ");
        }

        if ((i % 2) != 0 || i == num_tests - 1)
            (void)printf(SIR_EOL);
    }

    (void)printf(SIR_EOL);
}

void print_usage_info(const sir_cl_arg* args, size_t num_args) {
    size_t longest = 0;
    for (size_t i = 0; i < num_args; i++) {
        size_t len = strnlen(args[i].flag, SIR_CL_MAXFLAG);
        if (len > longest)
            longest = len;
    }

    (void)fprintf(stderr, SIR_EOL SIR_WHITE("  Usage:") SIR_EOL SIR_EOL);

    for (size_t i = 0; i < num_args; i++) {
        (void)fprintf(stderr, "\t%s ", args[i].flag);

        size_t len = strnlen(args[i].flag, SIR_CL_MAXFLAG);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                (void)fprintf(stderr, " ");

        (void)fprintf(stderr, "%s%s%s" SIR_EOL, args[i].usage,
            strnlen(args[i].usage, SIR_CL_MAXUSAGE) > 0 ? " " : "", args[i].desc);
    }

    (void)fprintf(stderr, SIR_EOL);
}

bool print_test_error(bool result, bool expected) {
    char msg[SIR_MAXERROR] = {0};
    uint16_t code          = sir_geterror(msg);
    if (SIR_E_NOERROR != code) {
        if (!expected && !result)
            TEST_MSG(SIR_RED("!! Unexpected (%" PRIu16 ", %s)"), code, msg);
        else if (expected)
            TEST_MSG(SIR_GREEN("Expected (%" PRIu16 ", %s)"), code, msg);
    }
    return result;
}

void print_libsir_version(void) {
    (void)printf(SIR_EOL SIR_ULINE("libsir") " %s (%s)" SIR_EOL SIR_EOL, sir_getversionstring(),
        sir_isprerelease() ? "prerelease" : "release");
}

const sir_cl_arg* find_cl_arg(const char* flag, const sir_cl_arg* args, size_t num_args) {
    for (size_t n = 0; n < num_args; n++) {
        const sir_cl_arg* arg = &args[n];
        if (_sir_strsame(flag, arg->flag, strnlen(arg->flag, SIR_CL_MAXFLAG)))
            return arg;
    }
    return NULL;
}

bool parse_cmd_line(int argc, char** argv, const sir_cl_arg* args, size_t num_args,
    sir_test* tests, size_t num_tests, sir_cl_config* config) {

    if (!argv || !args || !tests || !config)
        return false;

    (void)memset(config, 0, sizeof(sir_cl_config));

    for (int n = 1; n < argc; n++) {
        const sir_cl_arg* this_arg = find_cl_arg(argv[n], args, num_args);
        if (!this_arg) {
            ERROR_MSG("unknown option '%s'", argv[n]);
            print_usage_info(args, num_args);
            return false;
        }
        if (_sir_strsame(this_arg->flag, SIR_CL_PERFFLAG, strlen(SIR_CL_PERFFLAG))) {
            config->only = mark_test_to_run(SIR_CL_PERFNAME, tests, num_tests);
            if (config->only)
                config->to_run = 1;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_ONLYFLAG, strlen(SIR_CL_ONLYFLAG))) {
            while (++n < argc) {
                if (!_sir_validstrnofail(argv[n]))
                    continue;
                if (*argv[n] == '-' || !mark_test_to_run(argv[n], tests, num_tests)) {
                    ERROR_MSG("invalid argument to %s: '%s'", SIR_CL_ONLYFLAG, argv[n]);
                    print_usage_info(args, num_args);
                    return false;
                }
                config->to_run++;
            }
            if (0 == config->to_run) {
                ERROR_MSG("value expected for '%s'", SIR_CL_ONLYFLAG);
                print_usage_info(args, num_args);
                return false;
            }
            config->only = true;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_LISTFLAG, strlen(SIR_CL_LISTFLAG))) {
            print_test_list(tests, num_tests);
            return false;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_LEAVELOGSFLAG, strlen(SIR_CL_LEAVELOGSFLAG))) {
            config->leave_logs = true;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_WAITFLAG, strlen(SIR_CL_WAITFLAG))) {
            config->wait = true;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_VERSIONFLAG, strlen(SIR_CL_VERSIONFLAG))) {
            print_libsir_version();
            return false;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_HELPFLAG, strlen(SIR_CL_HELPFLAG))) {
            print_usage_info(args, num_args);
            return false;
        } else {
            ERROR_MSG("unknown option '%s'", this_arg->flag);
            return false;
        }
    }
    return true;
}

void wait_for_keypress(void) {
    (void)printf(SIR_WHITEB(SIR_EMPH("press any key to exit...")) SIR_EOL);
    char ch = '\0';
    (void)_sir_getchar(&ch);
    SIR_UNUSED(ch);
}

long sir_timer_getres(void) {
    long retval = 0L;
#if !defined(__WIN__)
    struct timespec res;
    int get = clock_getres(SIR_INTERVALCLOCK, &res);
    SIR_ASSERT(0 == get);
    if (0 == get)
        retval = res.tv_nsec;
#else /* __WIN__ */
    LARGE_INTEGER cntr_freq;
    (void)QueryPerformanceFrequency(&cntr_freq);
    if (cntr_freq.QuadPart <= 0)
        retval = 0L;
    else
        retval = (long)(ceil(((double)cntr_freq.QuadPart) / 1e9));
#endif
    return retval;
}

void sir_sleep_msec(uint32_t msec) {
    if (0U == msec)
        return;

#if !defined(__WIN__)
    struct timespec ts = { msec / 1000, (msec % 1000) * 1000000 };
    (void)nanosleep(&ts, NULL);
#else /* __WIN__ */
    (void)SleepEx((DWORD)msec, TRUE);
#endif
}

size_t sir_readline(FILE* f, char* buf, size_t size) {
    if (!f || !buf || 0 == size)
        return 0;

    size_t idx = 0;

    while (idx < size) {
        int ch = getc(f);
        if (EOF == ch || '\r' == ch || '\n' == ch)
            break;
        buf[idx++] = (char)ch;
    }

    return (0 == ferror(f)) ? idx : 0;
}

uint32_t getrand(uint32_t upper_bound) {
#if !defined(__WIN__) || (defined(__EMBARCADEROC__) && (__clang_major__ < 15))
# if defined(__MACOS__) || defined(__BSD__) || defined(__serenity__) || \
     defined(__SOLARIS__) || defined(__ANDROID__) || defined(__CYGWIN__) || \
     (defined(__linux__) && defined(__GLIBC__) && GLIBC_VERSION >= 23600)
    if (upper_bound < 2U)
        upper_bound = 2U;
    return arc4random_uniform(upper_bound);
# else
#  if defined(__EMBARCADEROC__) && (__clang_major__ < 15)
    return (uint32_t)(random(upper_bound));
#  else
    return (uint32_t)(random() % upper_bound);
#  endif
# endif
#else /* __WIN__ */
    uint32_t ctx = 0;
    if (0 != rand_s(&ctx))
        ctx = (uint32_t)rand();
    return ctx % upper_bound;
#endif
}

void rmfile(const char* filename, bool leave_logs) {
    /* skip if leave_logs is true. */
    if (leave_logs) {
        TEST_MSG(SIR_WHITE("skipping deletion of '%s' (%s)"), filename, SIR_CL_LEAVELOGSFLAG);
        return;
    }

    struct stat st;
    if (0 != stat(filename, &st)) {
        /* ignore errors if the file doesn't exist. */
        if (ENOENT == errno)
            return;
        HANDLE_OS_ERROR(true, "failed to stat %s!", filename);
        return;
    }

    if (!_sir_deletefile(filename))
        HANDLE_OS_ERROR(false, "failed to delete %s!", filename);
    else
        TEST_MSG(SIR_DGRAY("deleted %s (%ld bytes)"), filename, (long)st.st_size);
}

bool enumfiles(const char* path, const char* search, bool del, unsigned* count) {
#if !defined(__WIN__)
    DIR* d = opendir(path);
    if (!d)
        return print_test_error(false, false);

    rewinddir(d);
    const struct dirent* di = readdir(d);
    if (!di) {
        (void)closedir(d);
        return print_test_error(false, false);
    }

    while (NULL != di) {
        if (strstr(di->d_name, search)) {
            if (del) {
                char filepath[SIR_MAXPATH];
                _sir_snprintf_trunc(filepath, SIR_MAXPATH, "%s%s", path, di->d_name);
                rmfile(filepath, false);
            }
            (*count)++;
        }
        di = readdir(d);
    }

    (void)closedir(d);
    d = NULL;
#else /* __WIN__ */
    WIN32_FIND_DATA finddata = {0};
    char buf[SIR_MAXPATH]    = {0};

    (void)snprintf(buf, SIR_MAXPATH, "%s/*", path);

    HANDLE enumerator = FindFirstFile(buf, &finddata);
    if (INVALID_HANDLE_VALUE == enumerator)
        return false;

    do {
        if (strstr(finddata.cFileName, search)) {
            if (del) {
                char filepath[SIR_MAXPATH];
                _sir_snprintf_trunc(filepath, SIR_MAXPATH, "%s%s", path, finddata.cFileName);
                rmfile(filepath, false);
            }
            (*count)++;
        }
    } while (FindNextFile(enumerator, &finddata) > 0);

    FindClose(enumerator);
    enumerator = NULL;
#endif
    return true;
}
