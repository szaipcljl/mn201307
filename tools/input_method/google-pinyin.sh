#!/bin/bash

#
# 1.install
#
sudo apt install fcitx fcitx-googlepinyin im-config

#
# 2.setup
#

# (1) set keyboard input method system
im-config
# click "ok" "yes" -> choose "fcitx"

# (2)reboot, then:
# "System Settings" -> "Text Entry" -> "+" -> select "Google Pinyin(Fcitx)"
# ->"add"
