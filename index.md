Home      {#mainpage}
====

[TOC]

# libsir {#libsir}

---

libsir is a cross-platform, thread-safe C (C11) logging library that is designed to simplify and streamline the generation of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, _each with their own individual preference for format and levels_. See Formatting below for more information.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

## Highlights ##       {#highlights}

---

Why should you use libsir instead of another logging library? I don't know, why should you? _Just kidding_:

- _No dependencies_ other than `pthreads`. On Windows, libsir uses the native sychronization API.
- Accompanied by a robust _test suite_ to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently&mdash;C library or platform level errors are captured and stored for posterity, on a per-thread basis. The _function, file, and line number from which it orginated_ are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened&mdash;every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use &ndash;_Wpedantic_, too.
- Full **documentation**, thanks to [Doxygen](http://www.stack.nl/~dimitri/doxygen/manual/index.html).

## Supported platforms ##     {#platforms}

I've successfully compiled and verified the test suite in these environments:

| Toolset  | Version       | OS               | Architecture
| -------  | :-----------: | ---------------- | :------------
| gcc      | 5.4.0         | Ubuntu 16.04     | x64
| gcc      | 7.3.0 (MinGW) | Windows 10       | x64
| clang    | 9.1.0         | macOS 10.13.4    | x64

## Details ##      {#details}

---

### Destinations ###        {#destinations}

Currently, the following destinations are supported:

- `stdout` and `stderr`
- `syslog`
- _n_ number of log files (_archived automatically_)

### Decoration ###      {#decoration}

libsir computes certain data that may be useful in output (_particularly for debugging postmortem_). Each destination may be configured to receive any, all, or none of these.

_Note: the appearance of this data may be customized for your application. See [Customization](@ref customization) for more information._


Data             |  Description                               | stdio   | syslog<sup>†</sup> | files
----             | ------------                               | :-----: | :------:           | :-----:
Timestamp        | The current time (_hour, min, sec_)        | x       |                    | x
Milliseconds     | 1/1000<sup>ths</sup> of the current second | x       |                    | x
Level            | Human-readable level                       | x       |                    | x
Name             | The name of the process                    | x       | x                  | x
Process ID       | The current process identifier             | x       | x                  | x
Thread ID        | The current thread identifier              | x       |                    | x

<sup>†</sup> `syslog` _already includes many of these categories of data on its own._

### Levels ###      {#levels}

In addition to decoration options, each destination may be registered to receive any, none, or all priority levels of output. The available levels may be overly granular, but it's better to have unused levels rather than need another one and not have it.

Level       | Description
-----       | -----------
Debug       | Debugging/diagnostic output.
Information | Informational messages.
Notice      | Normal but significant.
Warning     | Warnings that could likely be ignored.
Error       | Errors.
Critical    | Critical errors.
Alert       | Action required ASAP.
Emergency   | Nuclear war, Armageddon, the sky is falling.

## Getting started ##              {#getstarted}

---

Clone or download this repository to your machine, then choose how you'd like to integrate libsir into your project.

### make recipies ###               {#make}

The following recipies are already prepared for you:

#### For tinkering and testing ####  {#testing}

Recipe        | Command        | Output file
------        | -------        | -----------
Test suite    | `make tests`   | _build/sirtests[.exe]_
Example use   | `make example` | _build/sirexample[.exe]_

#### For production ####             {#prod}

Recipe         | Command        | Output file
------         | -------        | -----------
Static library | `make static`  | _build/lib/libsir_s.a_
Shared library | `make shared`  | _build/lib/libsir.so_
Install        | `make install` | /usr/local/lib/libsir.so, /usr/local/include/&lt;headers&gt;

_If you'd like to debug libsir, just swap out the commented debug flags in [the makefile](../Makefile)._

### Including the source ###        {#src}

Simply modify your Makefile (or project file of whatever type) to include all of the source files in the root directory of this repository, and include [sir.h](@ref sir.h).

## Customization ##                {#customization}

---

### Strings and Limits ###          {#config}

The following can be modified to alter behavior, and are located in [sirconfig.h](@ref sirconfig.h). This is not an exhaustive list, and you should read the header for more information:

Entry             | Description                       | Default
-----             | -----------                       | :------:
`SIR_TIMEFORMAT`  | The time stamp format string at the beginning of log messages. | `"%H:%M:%S"`
`SIR_MSECFORMAT`  | The format for the current millisecond in time stamps. | `".%03ld"`
`SIR_LEVELFORMAT` | The format for the human-readable logging level. | `"[%s]"`
`SIR_PIDFORMAT`   | The format for the current process/thread ID. | `"%d"`
`SIR_FOPENMODE`   | The string passed to fopen/fopen_s for log files. | `"a"`
`SIR_FROLLSIZE` | The size, in bytes, at which a log file will be rolled/archived. | `(1024L * 1024L * 5L)`
`SIR_FHTIMEFORMAT` | The time format string in file headers. | `"%H:%M:%S %a %d %b %y (%z)"`
`SIR_FHFORMAT` | The format string written to a log file when logging begins or the file is rolled/archived. | `"\n\n----- %s %s -----\n\n"`
`SIR_FHBEGIN` | The string included in `SIR_FHFORMAT` when a file is rolled/archived due to size. | `"session begin @"`
`SIR_FHROLLED` | The string included in `SIR_FHFORMAT` when a file is rolled/archived due to size. | `"archived as %s due to size @"`
`SIR_FNAMEFORMAT` | The format string for rolled/archived log file names. | `"%s-%s%s"`
`SIRL_S_EMERG` | The human-readable form of the `SIRL_EMERG` level. | `"emrg"`
`SIRL_S_ALERT` | The human-readable form of the `SIRL_ALERT` level. | `"alrt"`
`SIRL_S_CRIT` | The human-readable form of the `SIRL_CRIT` level. | `"crit"`
`SIRL_S_ERROR` | The human-readable form of the `SIRL_ERROR` level. | `"erro"`
`SIRL_S_WARN` | The human-readable form of the `SIRL_WARN` level. | `"warn"`
`SIRL_S_NOTICE` | The human-readable form of the `SIRL_NOTICE` level. | `"noti"`
`SIRL_S_INFO` | The human-readable form of the `SIRL_INFO` level. | `"info"`
`SIRL_S_DEBUG` | The human-readable form of the `SIRL_DEBUG` level. | `"debg"`
`SIR_MAXFILES` | The maximum number of log files that may be registered.  | `16`
`SIR_MAXMESSAGE` | The maximum number of characters that may be included in one message, not including other parts of the output, like the timestamp and level. | `2048`

### Default options, levels, and text styles ###   {#defaults}

The following are the defaults used when `SIRL_DEFAULT` (@ref sir_level) or `SIRO_DEFAULT` (@ref sir_option) are used. These are located in [sirdefaults.h](@ref sirdefaults.h):

Destination      |  Levels     | Options
-----------      | :-------:   | -------
`stdout`         | <code>SIRL_DEBUG \| SIRL_INFO \| SIRL_NOTICE \| SIRL_WARN</code> | <code>SIRO_NOTIME \| SIRO_NOPID \| SIRO_NOTID</code>
`stderr`         | <code>SIRL_ERROR \| SIRL_CRIT \| SIRL_EMERG</code> | <code>SIRO_NOTIME \| SIRO_NOPID \| SIRO_NOTID</code>
`syslog`         | <code>SIRL_WARN  \| SIRL_CRIT \| SIRL_ALERT \| SIRL_EMERG</code> | `N/A (_syslog has its own decoration_)`
`log files`      | `SIRL_ALL`  | `0 (_all output_)`

These define the default text styling to use for `stdio` (@ref sir_textstyle) on a per-level basis (_can be changed at runtime with_ @ref sir_set_textstyle) and are also located in [sirdefaults.h](@ref sirdefaults.h):

Level         | Style                                           | Sample 
-----         | :-----:                                         | ------
`SIRL_DEBUG`  | `SIRS_FG_DGRAY`                                 | ![debug](http://gbtw.us/pub/img/debug.png)
`SIRL_INFO`   | `SIRS_FG_WHITE`                                 | ![info](http://gbtw.us/pub/img/info.png)
`SIRL_NOTICE` | `SIRS_FG_CYAN`                                  | ![notice](http://gbtw.us/pub/img/notice.png)
`SIRL_WARN`   | `SIRS_FG_YELLOW`                                | ![warning](http://gbtw.us/pub/img/warning.png)
`SIRL_ERROR`  | `SIRS_FG_RED`                                   | ![error](http://gbtw.us/pub/img/error.png)
`SIRL_CRIT`   | <code>SIRS_BRIGHT \| SIRS_FG_RED</code>         | ![critical](http://gbtw.us/pub/img/critical.png)
`SIRL_ALERT`  | <code>SIRS_BRIGHT \| SIRS_FG_BLACK \| SIRS_BG_LYELLOW</code>  | ![alert](http://gbtw.us/pub/img/alert.png)
`SIRL_EMERG`  | <code>SIRS_BRIGHT \| SIRS_FG_LYELLOW \| SIRS_BG_RED</code> | ![emergency](http://gbtw.us/pub/img/emergency.png)

### Preprocessor ###    {#preproc}

- For my own development purposes, I included a preprocesor define that causes libsir to log certain events to `stderr` that may be useful in diagnosing problems if/when they arise: `-DSIR_SELFLOG`.
- You should also be aware that `assert` is used in libsir. If you build _without_ `-DNDEBUG`, and an `assert` triggers, your program will halt (`assert` calls `abort` if the expression is `false`.

## Why  ## {#why}

---

libsir was originally written in 2003, and I rediscovered it recently. I looked at it and realized that it was woefully inadequate and decided to just compeltely rewrite it. This was in the context of R&D for a new embedded application for Sparkage Inc. We specialize in cloud-based mobile-first [loyalty and rewards software](https://getsparkage.com).