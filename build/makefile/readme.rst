
?=, :=, += and = 
----------------
?= indicates to set the KDIR variable only if it's not set/doesn't have a value.

For example:

KDIR ?= "foo"
KDIR ?= "bar"

test:
echo $(KDIR)
Would print "foo"

gnc manual:
http://www.gnu.org/software/make/manual/html_node/Setting.html

= 是最基本的赋值
:= 是覆盖之前的值
?= 是如果没有被赋值过就赋予等号后面的值
+= 是添加等号后面的值

1、“=”

make会将整个makefile展开后，再决定变量的值。也就是说，变量的值将会是整个makefile中最后被指定的值。看例子：

x = foo
y = $(x) bar
x = xyz

在上例中，y的值将会是 xyz bar ，而不是 foo bar 。

2、“:=”

“:=”表示变量的值决定于它在makefile中的位置，而不是整个makefile展开后的最终值。

x := foo
y := $(x) bar
x := xyz

在上例中，y的值将会是 foo bar ，而不是 xyz bar 了。

---------------------------------------------
通常，make会把其要执行的命令行在命令执行前输出到屏幕上。
用“@”字符在命令 行前，命令将不被make显示出来.
@echo 正在编译XXX模块......
当make执行时，会输出“正在编译XXX模块......”字串，但不会输出命令，
如果没有“@ ”，make将输出：
echo 正在编译XXX模块......
正在编译XXX模块......

make执行时，带入make参数“-n”或“--just-print”，只显示命令，不执行命令，
利于调试Makefile，看看命令是执行起来是什么样子或什么顺序的。

make参数“-s”或“--slient”, 全面禁止命令的显示。

------------------------------
自动化变量：
(1) $@ ——目标文件的名称；
(2) $^ ——所有的依赖文件，以空格分开，不包含重复的依赖文件；
(3) $< ——第一个依赖文件的名称。

示例:
main:main.c sort.o
    gcc main.c sort.o -o main
简洁表示:
main:main.c sort.o
    gcc $^ -o $@
