/**
 * @file example.c
 * @brief A simple example of SIR usage.
 * @page examples Examples
 */
#include "../sir.h"

/**
 * @example example.c
 * This is a basic example of initializing, configuring destinations,
 * and outputting messages.
 * 
 * @note You can build this CLI app yourself using `make example`. The binary
 * is generated at build/sirexample.
 * 
 * When it is finished running, you can see the output in the console, and
 * example the contents of 'sir-example.log' in the current directory.
 */

int report_error();

int main(int argc, char** argv) {

    /*
     * Instantiate the initialization structure.
     * 
     * Note: It is not necessary to retain this structure in memory;
     * SIR makes a copy of it before returning from  sir_init.
     */
    sirinit si = {0};

    /*
     * Configure levels for stdout.
     * Send debug, information, warning, and notice messages there.
     */
    si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_WARN | SIRL_NOTICE;

    /*
     * Configure options for stdout.
     * Don't show the time stamp or process ID.
     */
    si.d_stdout.opts = SIRO_NOTIME | SIRO_NOPID;

    /*
     * Configure levels for stderr.
     * Send error and above there.
     */
    si.d_stderr.levels = SIRL_ERROR | SIRL_CRIT | SIRL_ALERT | SIRL_EMERG;

    /*
     * Configure options for stderr.
     * Don't show the time stamp or process ID.
     */
    si.d_stderr.opts = SIRO_NOTIME | SIRO_NOPID;

    /*
     * Configure options for syslog.
     * Don't send any output there.
     */
    si.d_syslog.levels = 0;

    /* Configure a name to associate with our output. */
    strcpy(si.processName, "example");

    /* Initialize SIR. */
    if (!sir_init(&si)) {
        return report_error();
    }

    /*
     * Configure and add a log file.
     * Don't show the process name.
     * Send all severity levels there.
     */
    int fileid1 = sir_addfile("sir-example.log", SIRL_ALL, SIRO_NONAME);

    if (SIR_INVALID == fileid1) {
        return report_error();
    }

    /* Now we're ready to start generating output. */
    int n = 12345;
    const char* somestr = "my string contents";
    float f = 0.0009f;

    /*
     * This will be sent to all destinations registered for SIRL_DEBUG.
     * Notice that it is not necessary to add a newline at the end. SIR does
     * this automatically.
     */ 
    sir_debug("debug-level message: {n=%d, somestr='%s', f=%.04f}",
        n, somestr, f);

    /* Do the same for the rest of available severity levels. */
    sir_info("info-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    sir_notice("notice-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    sir_warn("warning-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    sir_error("error-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    sir_crit("critical error-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    sir_alert("alert-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);        

    sir_emerg("emergency-level message: {n=%d, somestr='%s', f=%.04f}", n, somestr, f);

    /* Clean up. */
    sir_cleanup();

    return 0;
}

int report_error() {
    sirchar_t message[SIR_MAXERROR] = {0};
    uint16_t code = sir_geterror(message);
    fprintf(stderr, "SIR error: (%hu, '%s')\n", code, message);
    return 1;
}
