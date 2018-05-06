# Main Page                {#mainpage}
[TOC]

## About

### What is SIR?
SIR is a cross-platform, thread-safe C library that was created to alleviate the hassles associated with implementing useful, readable, and great-looking diagnostic output facilities.

Using SIR, you can make a single call that, depending on configuration, simultaneously sends output to many destinations, _each with their own individual formatting options_.

#### Highlights

- Includes support for text stying for _stdout/stderr_ output, including foreground color, background color, and brightness/dim.
- Output to log files (and automatically archive when they get too big).
- Thread-safe (both _pthreads_ and native Windows API)
- Lightweight&mdash;I designed SIR for an embedded application, so it's as easy on resources as possible.
- Can automatically map levels to _syslog_ levels and send output there.
- _stdout_ and _stderr_ are configured separately.

#### Formatting
The following are the available categories of data available in SIR's output. Some of them only apply to certain types of destinations:

Data             |  Description                      | stdio | syslog<sup>†</sup>  | files
---------------- | --------------------------------  | :---: | :-----:             | :----:
Timestamp        | The current time (hour, min, sec) | x     |                     | x
Milliseconds     | The current millisecond           | x     |                     | x
Level            | Human-readable level              | x     |                     | x
Name             | The name of the process           | x     | x                   | x
Process ID       | The current process identifier    | x     | x                   | x
Thread ID        | The current thread identifier     | x     |                     | x

<sup>†</sup> _syslog already includes many of these categories of data on its own._

#### Levels
In addition to formatting options for each destination, each destination may opt in or out of any level of severity. These are designed to provide the most flexibility possible, and the use of each one is optional. Lowest severity to highest:

Level       | Description                           
----------- | --------------------------------------
Debug       | Debugging/troubleshooting output.
Information | Informational messages.
Warning     | Warnings that could likely be ignored.
Error       | Errors.                                 
Critical    | Critical errors.
Alert       | Action required ASAP.
Emergency   | Nuclear war, Armageddon, etc.

## Getting started
Clone or download this repository to your machine, then choose how you'd like to integrate SIR into your C/C++/whatever project.

### Supplied make recipies
I have included make recipies for building as a static library, and as a shared library.

- As a static library: `make static`
  + The output will be located at build/lib/libsir.a.

- As a shared library: `make shared`
  + The output will be located at build/lib/libsir.so.

### Including the source
Simply modify your Makefile (or project file of whatever type) to include all of the source files in the root directory of this repository, and include sir.h.

### Preprocessor options
There are a few things one should be aware of when compiling SIR that control its behavior.

- _SIR\_SELFLOG_: 

### Customization

### Usage

