#!/bin/bash
date | awk '{print $5}'
date | awk -F ':' '{print $2}'
awk -F '%' '{print $2}' date.sh

