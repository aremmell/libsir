# libsir

[![Build Status](https://travis-ci.org/ryanlederman/libsir.svg?branch=master)](https://travis-ci.org/ryanlederman/libsir)

## Synopsis

libsir is a cross-platform, thread-safe C (C11) logging library that is designed to simplify and streamline the generation of human-readable information in software.

Using libsir, you can make a single call that simultaneously sends information to multiple destinations, _each with their own individual preference for format and levels_. See the [documentation](https://ryanlederman.github.io/sir) for more information.

Each 'level' or 'priority' of output can be visually styled however you wish for `stdio` destinations. This allows for immediate visual confirmation that a message appeared in the terminal that you should pay attention to, while less pressing information is more subtle in appearance.

## Highlights

Why should you use libsir instead of another logging library? I don't know, why should you? _Just kidding_:

- _No dependencies_ other than `pthreads`. On Windows, libsir uses the native sychronization API.
- Accompanied by a robust _test suite_ to ensure dependable behavior, even if you make modifications to the source.
- Won't fail silently&mdash;C library or platform level errors are captured and stored for posterity, on a per-thread basis. The _function, file, and line number from which it orginated_ are also captured. Additionally, libsir defines its own set of error messages to aid in troubleshooting.
- Hardened&mdash;every function contains sanity checks for internal state as well as arguments passed in through the external interfaces. libsir's make recipes use &ndash;_Wpedantic_, too.
- Full **documentation**, thanks to [Doxygen](http://www.stack.nl/~dimitri/doxygen/manual/index.html).

## Supported platforms

I've successfully compiled and verified the test suite in these environments:

| Compiler | Version       | OS               | Architecture
| -------- | :-----------: | ---------------- | :------------
| gcc      | 5.4.0         | Ubuntu 16.04     | x64
| gcc      | 7.3.0         | Ubuntu 18.04     | armhf
| gcc      | 7.3.0 (MinGW) | Windows 10       | x64
| clang    | 9.1.0         | macOS 10.13.4    | x64

## Details

Please see the [documentation](https://ryanlederman.github.io/sir) for further information about libsir.
