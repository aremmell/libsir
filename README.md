# libsir documentation

## libsir: not just any logging library.

## Synopsis

---

libsir is a cross-platform, thread-safe C (C11) logging library that is designed to simplify and streamline the generation of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, _each with their own individual preference for format and levels_. See Formatting below for more information.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

## Highlights

Why should you use libsir instead of another logging library? I don't know, why should you? _Just kidding_:

- _No dependencies_ other than `pthreads`. On Windows, libsir uses the native sychronization API.
- Accompanied by a robust _test suite_ to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently&mdash;C library or platform level errors are captured and stored for posterity, on a per-thread basis. The _function, file, and line number from which it orginated_ are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened&mdash;every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use &ndash;_Wpedantic_, too.
- Full **[documentation](https://ryanlederman.github.io/sir)**, thanks to [Doxygen](http://www.stack.nl/~dimitri/doxygen/manual/index.html).

## Details

---

### Destinations

Currently, the following destinations are supported:

- `stdout` and `stderr`
- `syslog`
- _n_ number of log files (_archived automatically_)

### Decoration

libsir computes certain data that may be useful in output (_particularly for debugging postmortem_). Each destination may be configured to receive any, all, or none of these.

_Note: the appearance of this data may be customized for your application. See Customization for more information._


Data             |  Description                      | stdio | syslog<sup>†</sup> | files
---------------- | --------------------------------  | :---: | :-----:            | :----:
Timestamp        | The current time (_hour, min, sec_) | x     |                    | x
Milliseconds     | 1/1000<sup>ths</sup> of the current second    | x     |                    | x
Level            | Human-readable level              | x     |                    | x
Name             | The name of the process           | x     | x                  | x
Process ID       | The current process identifier    | x     | x                  | x
Thread ID        | The current thread identifier     | x     |                    | x

<sup>†</sup> _`syslog` already includes many of these categories of data on its own._

### Levels

In addition to decoration options, each destination may be registered to receive any, none, or all priority levels of output. The available levels may be overly granular, but it's better to have unused levels rather than need another one and not have it.

Level       | Description
----------- | --------------------------------------
Debug       | Debugging/diagnostic output.
Information | Informational messages.
Notice      | Normal but significant.
Warning     | Warnings that could likely be ignored.
Error       | Errors.
Critical    | Critical errors.
Alert       | Action required ASAP.
Emergency   | Nuclear war, Armageddon, the sky is falling.

## Getting started

Clone or download this repository to your machine, then choose how you'd like to integrate libsir into your project.

### make recipies

The following recipies are already prepared for you:

#### For tinkering and testing

- Test suite: `make tests` -> _build/sirtests[.exe]_
- Example app: `make example` -> _build/sirexample[.exe]_

#### For production

- Static library: `make static` -> _build/lib/libsir.a_
- Shared library: `make shared` -> _build/lib/libsir.so_

_If you'd like to debug libsir, just use `DEBUGCFLAGS` instead of `NDEBUGCFLAGS` in [the makefile](Makefile)._

### Including the source

Simply modify your Makefile (or project file of whatever type) to include all of the source files in the root directory of this repository, and include [sir.h](../sir.h).

### Preprocessor options

There are a few things one should be aware of when compiling libsir that control its behavior.

## Customization

## Documentation

Just meander right on over to <https://ryanlederman.github.io/sir>.
