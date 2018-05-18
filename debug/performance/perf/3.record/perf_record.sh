#!/bin/bash
sudo perf record -e cpu-clock ./t1.out
sudo perf report
