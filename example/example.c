/**
 * @file example.c
 * @brief Sample usage of libsir.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
#include <sir.h>
#include <sirhelpers.h>


int report_error(void);

/**
 * @brief This is a basic example of initializing and configuring libsir for use.
 * 
 * @note You can build this CLI app yourself using `make example` or the
 * Visual Studio [Code] project files.
 * 
 * When the program is finished running, you can see the output in the console,
 * and examine the contents of 'libsir-example.log' in the current directory.
 */
int main(void) {

    /**
     * Instantiate the libsir initialization struct and customize the
     * configuration.
     * 
     * NOTE: It is not necessary to retain this structure in memory;
     * libsir makes a copy of it before returning from ::sir_init.
     */
    sirinit si = {0};

    /*
     * Configure levels for stdout. Send debug, information, warning,
     * and notice messages there.
     */
    si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_WARN | SIRL_NOTICE;

    /* Configure options for stdout. Show all available formatting. */
    si.d_stdout.opts = SIRO_NOTIME | SIRO_NOPID;

    /* Configure levels for stderr. Send error and above there. */
    si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

    /* Configure options for stderr: don't show the time stamp or process ID. */
    si.d_stderr.opts = SIRO_NOTIME | SIRO_NOPID;

    /* Configure options for the system logger: don't send any output there. */
    si.d_syslog.levels = SIRL_NONE;

    /* Configure a name to associate with our output. */
    static const char* appname = "MyFooServer";
    _sir_strncpy(si.processName, SIR_MAXNAME, appname, strnlen(appname, SIR_MAXNAME));

    /* Initialize libsir. */
    if (!sir_init(&si))
        return report_error();

    /*
     * Configure and add a log file; don't log the process name,
     * and send all levels there.
     */
    static const sirchar_t* log_file_name = "libsir-example.log";
    sirfileid_t fileid1 = sir_addfile(log_file_name, SIRL_ALL, SIRO_NONAME);
    if (NULL == fileid1)
        return report_error();

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
    sir_debug("Database connection established. Binding a TCP socket to interface '%s'"
              " (IPv4: %s) on port %u and listening for connections.", "eth0",
              "120.22.140.8", 5500);

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

    /* Let's decide we better enable syslog. Things seem to be going poorly. */

    /* Set up an identity, some options, and register for error and higher. */
    if (!sir_syslogid("MyFooServer")) {
        report_error();
    }

    if (!sir_syslogopts(SIRO_NOPID)) {
        report_error();
    }

    if (!sir_sysloglevels(SIRL_ERROR | SIRL_CRIT | SIRL_EMERG)) {
        report_error();
    }

    /* Ok, syslog should be configured now. Continue executing. */

    sir_crit("Database query failure! Ignoring incoming client requests while"
             " the database is analyzed and repaired...");

    /* Things just keep getting worse for this poor sysadmin. */

    sir_alert("Database repair attempt unsuccessful! Error: %s", "<unknown>");        

    sir_emerg("Unable to process client requests for %s!", "4m52s");

    sir_info("Begin server shutdown.");
    sir_info("Exiting with code %d", 1);

    /* 
     * Now, you can examine the terminal output, libsir-example.log, and
     * syslog to ensure that the library operated as expected.
     * 
     * The last thing we have to do is uninitialize libsir by calling sir_cleanup().
     */

    /* Clean up. */
    sir_cleanup();

    return 0;
}

int report_error(void) {
    sirchar_t message[SIR_MAXERROR] = {0};
    uint16_t code = sir_geterror(message);
    fprintf(stderr, "libsir error: (%hu, %s)\n", code, message);
    return 1;
}
