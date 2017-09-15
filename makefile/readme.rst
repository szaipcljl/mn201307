
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

