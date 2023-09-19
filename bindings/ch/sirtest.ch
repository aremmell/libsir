#!/usr/bin/env ch

#include "chsir.h"

void report_error(void);

int
main(void) {
    sirinit si;
    if (!sir_makeinit(&si)) {
        report_error();
        return EXIT_FAILURE;
    }

    /* Levels for stdout: send debug, information, warning, and notice there. */
    si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_WARN | SIRL_NOTICE;

    /* Options for stdout: don't show the timestamp, hostname, or thread ID. */
    si.d_stdout.opts = SIRO_NOTIME | SIRO_NOHOST | SIRO_NOTID;

    /* Levels for stderr: send error and above there. */
    si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

    /* Options for stderr: don't show the timestamp, hostname, or thread ID. */
    si.d_stderr.opts = SIRO_NOTIME | SIRO_NOHOST | SIRO_NOTID;

    /* Levels for the system logger: don't send any output there. */
    si.d_syslog.levels = SIRL_NONE;

    /* Options for the system logger: use the default value. */
    si.d_syslog.opts = SIRO_DEFAULT;

    /* Configure a name to associate with our output. */
    static const char* appname = "ChDemo";
    (void)strcpy(si.name, appname);

    /* Initialize libsir. */
    if (!sir_init(&si)) {
        report_error();
        return EXIT_FAILURE;
    }

    /* Get libsir version. */
    const char* sir_versionstring = sir_getversionstring();
    if (!sir_versionstring) {
        report_error();
        return EXIT_FAILURE;
    }

    (void)fprintf(_stdout,
            (CYAN("libsir ") BCYAN("%s") CYAN(" Ch bindings") BGRAY(" ...\n")),
            sir_versionstring);

    (void)sir_debug  ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_info   ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_notice ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_warn   ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_error  ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_crit   ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_alert  ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);
    (void)sir_emerg  ("Testing output %b %s %s", __LINE__, __TIME__, __DATE__);

    sir_cleanup();

    return EXIT_SUCCESS;
}

void
report_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    (void)fprintf(_stderr, (BRED("libsir error: (%" PRIu16 ", %s)") BGRAY("\n")),
                  code, message);
}
