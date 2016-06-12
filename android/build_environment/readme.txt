#
# Android build environment : install.sh
#

1.重装系统后，看能否上网
无法上网时，输入：
sudo mii-tool -r eth0
重复输入，直到能正常上网。

===================附加步骤================
1、安装飞秋,ubuntu经典桌面，然后切换桌面
sudo apt-get install iptux gnome-session-fallback

切换到经典桌面

设置feiQ：
英文环境下修改IPTUX编码：
(1)点击 Tools->Preferences
选择System 页面
(2)在Candidate network coding 栏输入
gb18030,big5
在 Preference network coding 栏输入：
gb18030
(3)OK 退出, 再点击 Tools->Update 即可

===============================================

1.将jdk-6u45-linux-x64.bin与install.sh拷贝到桌面。

2.打开终端，进入到桌面。
cd ~/Desktop/

3. 修改install.sh权限,并执行
sudo chmod a+x install.sh
sudo ./install.sh

>>开始自动安装环境，期间不用输入任何指令，直到弹出gedit编辑/etc/profile的对话框，
在最后面添加下面几行环境配置，保存后退出:
# Android4.4 java : jdk1.6
export JAVA_HOME=/usr/local/bin/jdk1.6.0_45
export JRE_HOME=$JAVA_HOME/jre
export ANDROID_JAVA_HOME=$JAVA_HOME

export CLASSPATH=.:$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH
export JAVA_PATH=$JAVA_HOME/bin:$JRE_HOME/bin
export PATH=$PATH:$JAVA_PATH

4.将id_rsa.pub提供给git服务器管理者配置git访问权限
生成git的key: ssh-keygen -t rsa
nautilus ~/.ssh/

5. 编译环境安装配置完成，可以开始拉代码编译。

===============================================================
注意：
install.sh中有jdk拷贝的jdk路径，/etc/profile中也有JAVA_HOME的路径
