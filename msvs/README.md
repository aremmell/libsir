<!-- Version: 2.2.6 -->
<!-- -->
<!-- SPDX-License-Identifier: MIT -->
<!-- -->
<!-- Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
<!-- Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com> -->
<!-- -->
<!-- Permission is hereby granted, free of charge, to any person obtaining a copy of -->
<!-- this software and associated documentation files (the "Software"), to deal in -->
<!-- the Software without restriction, including without limitation the rights to -->
<!-- use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of -->
<!-- the Software, and to permit persons to whom the Software is furnished to do so, -->
<!-- subject to the following conditions: -->
<!-- -->
<!-- The above copyright notice and this permission notice shall be included in all -->
<!-- copies or substantial portions of the Software. -->
<!-- -->
<!-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR -->
<!-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS -->
<!-- FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR -->
<!-- COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER -->
<!-- IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN -->
<!-- CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. -->

# About the libsir Visual Studio solution

## Project layout

- **libsir:** The library itself. This is a static library whose target is at `../build/lib/libsir.lib`, but could be converted into a DLL project if required. To build libsir as a DLL, see [libsir as a DLL](#libsir-as-a-dll).
- **testsuite:** The collection of libsir integrity tests compiled as a console application at `../build/bin/sirtests.exe`
- **example:** A small sample console application that demonstrates basic usage of libsir. Its target is at `../build/bin/sirexample.exe`
- **plugins**: The projects contained within this solution folder are primarily for use by the test suite application and for reference purposes.
  - **sample_plugin** is intended to be used as a template when creating your own libsir plugin. It is not included in the build configurations for this solution&mdash;it's there for reference only.
  - **dummy_plugin** is a plugin that can be compiled to be well-formed and well-behaved, or by using preprocessor macros, it can be made to compile into an invalid plugin module that will be rejected by libsir, or as a valid plugin which behaves in such a way that it will also be rejected during the loading process. The remaining projects under `plugins` are simply wrappers around **dummy_plugin** which define the appropriate preprocessor macros to cause the desired bad behavior. These are created to ensure that the plugin loader is working properly while executing the test suite.
  - These projects are set to compile as DLLs with their targets at `../build/lib`

## Recommended configuration

Unless you are experiencing issues with libsir and want to step through in a debugger, we recommend that you build the Release configuration (x64 or arm64), even when running the test suite.

In the event that you *do* wish to start a debug session, you should first add `SIR_SELFLOG=1` to the Preprocessor Definitions in the libsir project under project properties -> C++ -> Preprocessor. This will cause a ton of additional diagnostic output to be produced.

Note that if you start the test suite with Ctrl+F5, it will scroll by very quickly and exit unless you pass the `--wait` argument on the command line (this is already set for Debug configurations). This can be done through project settings under Debugging -> Command Arguments. Alternatively, you can just open your own console window, change directories to the root of this repository, and run `build/bin/sirtests.exe`.

## Full documentation

Don't forget that the entire set of Doxygen-generated documentation is in `../docs`. Simply open [../docs/index.html](../docs/index.html) in your browser.

## libsir as a DLL

To convert the libsir project into a dynamically loaded library, perform the following steps:

  1. Unload the project by right-clicking and choosing 'Unload project'.
  2. Locate all instances of `<ConfigurationType>StaticLibrary</ConfigurationType>` and change the 'StaticLibrary' to 'DynamicLibrary'.
  3. Change all instances of `_LIB` to `_USRDLL`.
  4. Open `sir.h` and somewhere near the top of the file, put `#define LIBSIR_EXPORT __declspec(dllexport)`
  5. Place the `LIBSIR_EXPORT` macro before each function declaration in the file. Switch over to `sir.c` and do the same there.
  6. Implement a bare-bones `DllMain` in `sir.c` that always returns `TRUE`.
  7. Reload the project and build!

The projects under `plugins` can be a useful guide for this process, since they are already DLLs.

## Questions?

Feel free to [open a Discussion]() on GitHub if you have questions, suggestions, or feedback&mdash;no matter what kind of feedback, we'd love to hear it.
