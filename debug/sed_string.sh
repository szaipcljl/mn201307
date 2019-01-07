#!/bin/bash

var_ori="BUILD_AMBARELLA_IT66121_HDMI"
var_new="BUILD_DS90UB925_DISPLAY"

sed -i "s/$var_ori/$var_new/g" $1
