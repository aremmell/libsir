/**
 * @file example.c
 *
 * A simple demonstration of initialization, configuration, and basic usage of
 * libsir.
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.0
 * @copyright The MIT License (MIT)
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
#include <sir.h>
#include <sirhelpers.h>

int report_error(void);

/**
 * @brief This is a basic example of initializing and configuring libsir for use.
 *
 * @note You can build this command-line app yourself using `make example` or the
 * Visual Studio [Code] project files.
 *
 * When the program is finished running, you can see the output in the console,
 * and examine the contents of 'libsir-example.log' in the current working
 * directory.
 *
 * @returns EXIT_SUCCESS if execution completes successfully, or EXIT_FAILURE
 * if an error occurs.
 */
int main(void) {
    /**
     * Instantiate the libsir initialization struct and customize the
     * configuration.
     *
     * NOTE: It is not necessary to retain this structure in memory;
     * libsir makes a copy of it before returning from ::sir_init.
     */
    sirinit si;
    if (!sir_makeinit(&si))
        return report_error();

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
    static const char* appname = "MyFooServer";
    _sir_strncpy(si.name, SIR_MAXNAME, appname, strnlen(appname, SIR_MAXNAME));

    /* Initialize libsir. */
    if (!sir_init(&si))
        return report_error();

    /*
     * Configure and add a log file; don't log the process name or hostname,
     * and send all levels there.
     */
    sirfileid fileid = sir_addfile("libsir-example.log", SIRL_ALL, SIRO_NONAME | SIRO_NOHOST);
    if (NULL == fileid)
        report_error();

    /*
     * Ready to start logging. The messages passed to sir_debug() will be sent
     * to all destinations registered for the ::SIRL_DEBUG level. In our example,
     * that is stdout and 'libsir-example.log'.
     */

    /* Notice that it is not necessary to include a newline */
    sir_debug("Reading config file %s...", "/usr/local/myapp/myapp.conf");

    /* Pretend to read a config file */
    sir_debug("Config file successfully parsed; connecting to database...");

    /* pretend to connect to a database */
    sir_debug("Database connection established.");
    sir_debug("Binding a TCP socket to interface '%s'"
              " (IPv4: %s) on port %u and listening for connections...",
              "eth0", "120.22.140.8", 5500);

    /*
     * Log a message for each of the remaining severity levels. Only up to
     * 'notice' will the messages be emitted from stdout; the rest will come from
     * stderr.
     */
    sir_info("MyFooServer v%d.%d.%d (amd64) started successfully in %.2fsec.",
        2, 9, 4, 1.94f);

    sir_notice("Client at %s:%u (username: %s) failed 5 authentication attempts!",
        "210.10.54.3", 43113, "bob");

    sir_warn("Detected downgraded link speed on %s: last transfer rate: %.1f KiB/s",
        "eth0", "219.4");

    /* Hmm, what else could go wrong... */
    sir_error("Failed to synchronize with node pool.846.myfooserver.io! Error:"
              " %s", "connection reset by peer. Retry in 30sec");

    /*
     * Let's decide we better set up logging to the system logger; things seem
     * to be going poorly.
     *
     * Set up an identity, some options, and register for error and higher.
     *
     * NOTE: If this platform does not have a supported system logger, or the
     * SIR_NO_SYSTEM_LOGGERS preprcessor define was set when libsir was
     * compiled, these calls will fail and have no effect.
     */

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!sir_syslogid(appname))
        report_error();

    if (!sir_syslogopts(SIRO_NOPID))
        report_error();

    if (!sir_sysloglevels(SIRL_ERROR | SIRL_CRIT | SIRL_EMERG))
        report_error();
#endif

    /*
     * Okay, syslog should be configured now. Continue executing.
     */
    sir_crit("Database query failure! Ignoring incoming client requests while"
             " the database is analyzed and repaired...");

    /*
     * Things just keep getting worse for this poor sysadmin.
     */
    sir_alert("Database repair attempt unsuccessful! Error: %s", "<unknown>");
    sir_emerg("Unable to process client requests for %s! Restarting...", "4m52s");

    sir_debug("Begin server shutdown.");
    sir_debug("Exiting with code %d.", 1);

    /* Deregister (and close) the log file. */
    if (fileid && !sir_remfile(fileid))
        report_error();

    /*
     * Now, you can examine the terminal output, libsir-example.log, and
     * syslog to ensure that the library operated as expected.
     *
     * The last thing we have to do is uninitialize libsir by calling sir_cleanup().
     */
    sir_cleanup();

    return EXIT_SUCCESS;
}

/**
 * Prints the last libsir error to stderr.
 *
 * @return EXIT_FAILURE
 */
int report_error(void) {
    char message[SIR_MAXERROR] = {0};
    uint16_t code              = sir_geterror(message);
    fprintf(stderr, "libsir error: (%hu, %s)\n", code, message);
    return EXIT_FAILURE;
}
