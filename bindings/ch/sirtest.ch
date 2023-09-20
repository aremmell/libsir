#!/usr/bin/env ch

/*
 * sirtest.ch
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
#include "chsir.h"
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <chshell.h>

void report_error(void);

int
main(void) {
    /* Initialize locale data. */
    setlocale(LC_ALL, "");

    /* Read Ch version. */
    chinfo_t ch;
    if (chinfo(&ch)) {
        (void)fprintf(_stderr,
                      RED("Ch error: ") BRED("Failure reading Ch version.\n"));

        return EXIT_FAILURE;
    }

    /* Configure libsir. */
    sirinit si;
    if (!sir_makeinit(&si)) {
        report_error();

        return EXIT_FAILURE;
    }

    /* Levels for stdout: send debug, information, warning, and notice there. */
    si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_WARN | SIRL_NOTICE;

    /* Options for stdout: show everything. */
    si.d_stdout.opts = SIRO_ALL;

    /* Levels for stderr: send error and above there. */
    si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

    /* Options for stderr: don't show the timestamp, hostname, or thread ID. */
    si.d_stderr.opts = SIRO_NOTIME | SIRO_NOHOST | SIRO_NOTID;

    /* Levels for the syslog logger: don't send any output there. */
    si.d_syslog.levels = SIRL_NONE;

    /* Options for the syslog logger: use the default value. */
    si.d_syslog.opts = SIRO_DEFAULT;

    /* Configure a name to associate with our output. */
    static const char* appname = "ChDemo";
    (void)strncpy(si.name, appname, SIR_MAXNAME);

    /* Initialize libsir. */
    if (!sir_init(&si)) {
        report_error();

        return EXIT_FAILURE;
    }

    /* Set a friendly name for the current thread. */
    const char* thread_name = _argv[0] ? chsir_basename(_argv[0]) : "sirtest";
    (void)_sir_setthreadname(thread_name);

    /* Get libsir version. */
    const char* sir_versionstring = sir_getversionstring();
    if (!sir_versionstring) {
        report_error();

        return EXIT_FAILURE;
    }

    /* Print startup herald with version info. */
    (void)fprintf(_stdout,
                  WHITE("Ch") " binding test: " CYAN("libsir ") BCYAN("%s"),
                  sir_versionstring);
    (void)fprintf(_stdout,
                  BGRAY(" on ") CYAN("Ch ") BCYAN("%d.%d.%d.%d") BGRAY(".\n"),
                  ch.vermajor, ch.verminor, ch.vermicro, ch.verbuild);

    /* Log to stdout. */
    (void)fprintf(_stdout, "\nLogging four libsir messages to stdout:\n");
    (void)sir_debug("%b Testing %s output", __LINE__, ULINE("debug"));
    (void)sir_info("%b Testing %s output", __LINE__, ULINE("information"));
    (void)sir_notice("%b Testing %s output", __LINE__, ULINE("notice"));
    (void)sir_warn("%b Testing %s output", __LINE__, ULINE("warning"));

    /* Log to stderr. */
    (void)fprintf(_stderr, "\nLogging four libsir messages to stderr:\n");
    (void)sir_error("%s %b Testing %s output", __TIME__, __LINE__, ULINE("error"));
    (void)sir_crit("%s %b Testing %s output", __TIME__, __LINE__, ULINE("critical"));
    (void)sir_alert("%s %b Testing %s output", __TIME__, __LINE__, ULINE("alert"));
    (void)sir_emerg("%s %b Testing %s output", __TIME__, __LINE__, ULINE("emergency"));

    /* Clean up. */
    sir_cleanup();

    /* All done. */
    return EXIT_SUCCESS;
}

/* Print error message using libsir. */
void
report_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    (void)fprintf(_stderr, BRED("libsir error: (%" PRIu16 ", %s)") BGRAY("\n"),
                  code, message);
}