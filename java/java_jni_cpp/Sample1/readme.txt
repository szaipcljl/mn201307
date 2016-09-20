
Java通过-jni调用c++语言

Ubuntu14.04
已安装过JDK
===========
（1）首先编写一个简单的Java程序。
	Sample1.java

（2）用javac编译Sample1.java文件生成Sample1.class文件
	javac Sample1.java

（3）用javah带-jni参数编译Sample1.class文件生成Sample1.h文件。
	该文件中定义了c++的函数原型。在实现c++函数的时候需要。
	javah -jni Sample1

（4）编写c++语言去实现这些方法
	Sample1.cpp

（5）生成对象文件Sample1.o。
	/usr/lib/jvm/java/是JDk安装的路径,根据自己的JDK路径设置.
	Jni_md.h和Jni.h分别在JDK的/include/linux与/include/目录下
	-Wall：打开警告开关。
	-fPIC：表示编译为位置独立的代码，不用此选项的话编译后的代码是位置相关的所以动态载入时是
	通过代码拷贝的方式来满足不同进程的需要，而不能达到真正代码段共享的目的。

	g++ -Wall -fPIC -c Sample1.cpp -I ./ -I /usr/lib/jvm/java-7-openjdk-amd64/include/linux/ -I /usr/lib/jvm/java-7-openjdk-amd64/include/

	--------
	多个cpp文件的编译:
	生成对象文件Sample1.o 和 myprint.o
	g++ -Wall -fPIC -c Sample1.cpp myprint.cpp -I ./ -I /usr/lib/jvm/java-7-openjdk-amd64/include/linux/ -I /usr/lib/jvm/java-7-openjdk-amd64/include/

（6）生成动态链接库libSample1.so文件，
	注意：动态链接库的名字必须是 lib*.so，因为编译器查找动态连接库时有隐含的命名规则，即在给出的
	名字前面加上lib，后面加上.so来确定库的名称。这里是libSample1.so对应于Java程序里的Sample1
	-rdynamic 通知链接器将所有符号添加到动态符号表中。
	-shared 编译后会链接成共享对象。

	g++ -Wall -rdynamic -shared -o libSample1.so Sample1.o

	--------
	多个cpp文件的编译:
	g++ -Wall -rdynamic -shared -o libSample1.so Sample1.o myprint.o

（7）配置环境变量,装载动态库的路径
	bash:
	export LD_LIBRARY_PATH=./

（8）运行Java程序，实现去调用c++语言。
	Java文件所在目录运行

	java Sample1

========================================================================================
运行结果:
intMethod: 25
booleanMethod: false
stringMethod: JAVA
intArrayMethod: 36

--------
多个cpp文件:
hello from myprint.cpp
intMethod: 25
booleanMethod: false
stringMethod: JAVA
intArrayMethod: 36

========================================================================================
只有一个不同点
C代码: (*env)->GetStringUTFChars(env, string, 0);

C++代码: env->GetStringUTFChars(string, 0);

C语言中使用的是结构体的函数指针,
在C++中使用的还是struct, struct在C++中和class的功能几乎一样, struct也可以用来定义类, 所以
env在C++中是个类对象的指针.
