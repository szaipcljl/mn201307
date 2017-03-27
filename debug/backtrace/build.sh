#!/bin/bash
#gcc backtrace_test.c -o backtrace_test.bin -g -rdynamic -static
#can run in adb shell(x86), but no symbol
gcc backtrace_test.c -o backtrace_test.bin -g -rdynamic
