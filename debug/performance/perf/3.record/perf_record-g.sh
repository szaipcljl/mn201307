#!/bin/bash
sudo perf record -e cpu-clock -g ./t1.out
sudo perf report -g
