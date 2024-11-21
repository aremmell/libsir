#!/usr/bin/env ch

/*
 * sirtest.ch
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
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
 * -----------------------------------------------------------------------------
 */

#include "chsir.h"
#include "sir/errors.h" /* SIR_E_UNAVAIL */
#include <locale.h> /* setlocale */
#include <chshell.h> /* chinfo */

void report_error(void);

int
main(void) {
    /* Initialize locale data. */
    setlocale(LC_ALL, "");

    /* Read Ch version. */
    chinfo_t ch;
    if (chinfo(&ch)) {
        (void)fprintf(_stderr,
                      SIR_BREDB("Ch error: ") SIR_RED("Failure reading Ch version.") SIR_EOL);

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

    /* Configure and add a log file. */
    sirfileid fileid = sir_addfile("libsir-chdemo.log", SIRL_ALL, SIRO_NONAME | SIRO_NOHOST);
    if (0 == fileid)
        report_error();

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
                  SIR_WHITE("Ch") " binding test: " SIR_CYAN("libsir ") SIR_BCYAN("%s"),
                  sir_versionstring);
    (void)fprintf(_stdout,
                  SIR_BGRAY(" on ") SIR_CYAN("Ch ") SIR_BCYAN("%d.%d.%d.%d") SIR_BGRAY(".") SIR_EOL SIR_EOL,
                  ch.vermajor, ch.verminor, ch.vermicro, ch.verbuild);

    /* Demo string utilities. */
    (void)fprintf(_stdout, _sir_strredact(_sir_strreplace(_sir_strsqueeze(
                  "xxx Welcome  \n  to  \f  the  Ch  \v  demo    xx!" SIR_EOL SIR_EOL),
                  '!', 'x'), "x", '*'));

    /* Log to stdout. */
    (void)fprintf(_stdout, SIR_EOL "Logging four libsir messages to stdout:" SIR_EOL);
    (void)sir_debug("%b Testing %s output", __LINE__, SIR_ULINE("debug"));
    (void)sir_info("%b Testing %s output", __LINE__, SIR_ULINE("information"));
    (void)sir_notice("%b Testing %s output", __LINE__, SIR_ULINE("notice"));
    (void)sir_warn("%b Testing %s output", __LINE__, SIR_ULINE("warning"));

    /* Log to stderr. */
    (void)fprintf(_stderr, SIR_EOL "Logging four libsir messages to stderr:" SIR_EOL);
    (void)sir_error("%s %b Testing %s output", __TIME__, __LINE__, SIR_ULINE("error"));
    (void)sir_crit("%s %b Testing %s output", __TIME__, __LINE__, SIR_ULINE("critical"));
    (void)sir_alert("%s %b Testing %s output", __TIME__, __LINE__, SIR_ULINE("alert"));
    (void)sir_emerg("%s %b Testing %s output", __TIME__, __LINE__, SIR_ULINE("emergency"));

    /* Set up for syslog output. */
    (void)sir_syslogid(appname);

    /* Check if syslog is available. */
    if (SIR_E_UNAVAIL == sir_geterror(NULL)) {
        (void)fprintf(_stderr, SIR_EOL "Logging to syslog is not supported, skipping." SIR_EOL);
    } else {
        (void)fprintf(_stderr, SIR_EOL "Logging two libsir messages to stderr and syslog:" SIR_EOL);

        /* Don't log PID; syslog has us covered. */
        if (!sir_syslogopts(SIRO_NOPID))
            report_error();

        /* Send all levels. */
        if (!sir_sysloglevels(SIRL_ALL))
            report_error();

        /* Log to syslog. */
        (void)sir_alert("%b Testing %s output", __LINE__, "alert");
        (void)sir_emerg("%b Testing %s output", __LINE__, "emergency");
    }

    /* Deregister (and close) the log file. */
    if (fileid && !sir_remfile(fileid))
        report_error();

    /* Clean up. */
    (void)sir_cleanup();

    /* We made it! */
    (void)fprintf(_stdout, SIR_EOL SIR_WHITE("Ch ") "binding test: " SIR_BGREEN("completed") SIR_BGRAY(".") SIR_EOL);

    /* All done. */
    return EXIT_SUCCESS;
}

/* Print error message. */
void
report_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    (void)fprintf(_stderr, SIR_BREDB("libsir error: ") SIR_RED("%" PRIu16 ", %s") SIR_BGRAY(".") SIR_EOL,
                  code, message);
}
