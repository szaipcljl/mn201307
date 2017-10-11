#!/bin/bash
HOST_NUM_CPUS=`cat /proc/cpuinfo | grep processor | wc -l`
echo $HOST_NUM_CPUS

BUILD_NUM_CPUS=`expr $HOST_NUM_CPUS \* 2`
echo $BUILD_NUM_CPUS
