Java通过-jni调用c语言

Ubuntu14.04
已安装过JDK
===========
（1）首先编写一个简单的Java程序。
	TestJNI.java

（2）用javac编译TestJNI.java文件生成TestJNI.class文件
	javac TestJNI.java

（3）用javah带-jni参数编译TestJNI.class文件生成TestJNI.h文件。
	该文件中定义了c的函数原型。在实现c函数的时候需要。
	javah -jni TestJNI

（4）编写c语言去实现这些方法
	TestJNI.c

（5）生成对象文件TestJNI.o。
	/usr/lib/jvm/java/是JDk安装的路径,根据自己的JDK路径设置.
	Jni_md.h和Jni.h分别在JDK的/include/linux与/include/目录下
	-Wall：打开警告开关。
	-fPIC：表示编译为位置独立的代码，不用此选项的话编译后的代码是位置相关的所以动态载入时是
	通过代码拷贝的方式来满足不同进程的需要，而不能达到真正代码段共享的目的。

	gcc -Wall -fPIC -c TestJNI.c -I ./ -I /usr/lib/jvm/java-7-openjdk-amd64/include/linux/ -I /usr/lib/jvm/java-7-openjdk-amd64/include/

（6）生成动态链接库libdiaoyong.so文件，
	注意：动态链接库的名字必须是 lib*.so，因为编译器查找动态连接库时有隐含的命名规则，即在给出的
	名字前面加上lib，后面加上.so来确定库的名称。这里是libdiaoyong.so对应于Java程序里的diaoyong
	选项 -rdynamic 用来通知链接器将所有符号添加到动态符号表中。
	-shared指编译后会链接成共享对象。

	gcc -Wall -rdynamic -shared -o libdiaoyong.so TestJNI.o

（7）配置环境变量,装载动态库的路径
	.bashrc:
	 export LD_LIBRARY_PATH=./

	 让.bashrc中设置立即加载
	 source ~/.bashrc

（8）运行Java程序，实现去调用c语言。
	Java文件所在目录运行

	java TestJNI

print:
ok!You have successfully passed the Java call c
888
