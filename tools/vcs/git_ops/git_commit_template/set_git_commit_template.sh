#!/bin/bash

# 1. set git commit template
sudo mkdir /opt/git_msg_tmp/
sudo cp ./commit_template /opt/git_msg_tmp/
# 设置模板 git 提交模板：
git config commit.template /opt/git_msg_tmp/commit_template

# 2. git客户端检查提交信息的hook，放到相应的 .git/hooks下就能触发
# cp commit-msg .git/hooks
