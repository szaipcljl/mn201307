#!/bin/bash

# run thist script and then run 'git status', git diff, see what happen
sed 's/drivers\/test_driver/test_driver/g' ./add_kthread.diff \
	> add_kthread_used_by_cur_dir.diff
cd ../../
patch -p1 < ./test_driver/patch/add_kthread_used_by_cur_dir.diff
rm ./test_driver/patch/add_kthread_used_by_cur_dir.diff
