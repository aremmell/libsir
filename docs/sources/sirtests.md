# Test suite              {#testsuite}

In order to ensure that libsir is working properly on your system, the first thing you should do after compiliation is
verify that the test suite runs successfully (_in fact, we recommend making it part of your deployment workflow_).

It will exit with `0` if all test(s) ran successfully and passed, or `1` if errors occurred/any test failed.

## Basic usage

The simplest way to achieve this is simply to run `build/bin/sirtests` from the root of the repository. You should see output similar
to the following:

~~~sh
done: all 26 libsir tests passed in 0.690sec!
~~~

## Advanced usage

`sirtests` has a command-line interface that allows you to control its behavior. For the current options, run `sirtests --help`.
You should see output similar to the following:

~~~txt
Usage:

        --perf  Only run the performance measurement test.
        --wait  Wait for a keypress after running test(s) before exiting.
        --only  name [, name, ...] Only run the test(s) specified.
        --list  Prints a list of available test names for use with --only.
        --help  Shows this message.
~~~

Of note here is the obvious one, `--perf`. The perf test is only run if you explicitly specify this flag. It is a good way to ensure that libsir is compiled correctly for deployment in a production environment. If you get a very slow (_or dubiously fast_) result, you should re-examine your configuration.

The result includes a comparison to an equivalent raw `printf` call for the same number of iterations. I expect this to always be faster than libsir for many reasons, but I have seen it beaten once.

Here is an example of the results of `--perf` on this development machine (_iMac Pro, 3.2GHz 8-core Xeon, 64GB 2666MHz DDR4, SSD_):

~~~txt
printf: 1000000 lines in 7.394sec (135239.0 lines/sec)
libsir(stdout): 1000000 lines in 8.116sec (123209.8 lines/sec)
libsir(log file): 1000000 lines in 7.978sec (125342.3 lines/sec)
timer resolution: ~1nsec
~~~

---

@remark The perf test only outputs to the debug level. If level switching were introduced where formatting options varied from level to level, a much slower elapsed time could be expected, since some of libsir's internal formatting buffers would need to be recalculated each time.

The other useful flags include `--list` and `--only` if you wish to narrow down a problem test or set of tests. Please let me know if you think of additional tests that should be performed by [opening a feature request](https://github.com/aremmell/libsir/issues/new?template=Feature_request.md).
