#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>

/*
 *  什么情况下会产生core dump呢？
 *
 *  以下情况会出现应用程序崩溃导致产生core dump：

 *  内存访问越界 （数组越界、字符串无\n结束符、字符串读写越界）
 *  多线程程序中使用了线程不安全的函数，如不可重入函数
 *  多线程读写的数据未加锁保护（临界区资源需要互斥访问）
 *  非法指针（如空指针异常或者非法地址访问）
 *  堆栈溢出
*/

// 通过除零操作产生core dump
int main(int argc, const char *argv[])
{
	int a, b, result;

	a =2;
	b = 0;
	result = a/b;

	return 0;
}

/*
 *  当前目录下产生了core文件，使用file命令查看core文件类型:
 *  file core
 *
 *  发现core文件类型为ELF格式，即可执行文件，使用readelf查看ELF文件头部信息:
 *  readelf -h core
 *
 * core dump可以查看应用程序崩溃时的现场信息，需要gdb命令辅助实现，
 * 使用gdb a.out core（即a.out可执行文件和core文件）
 *
 * 其中打印出导致core dump产生的原因，即Arithmetic exception，
 * 同时打印出了出问题的代码行result = a/b;
 * 当前还处在gdb调试环境中。 可以通过bt –n (backtrace)显示函数
 * 调用栈信息，n表示显示的调用栈层数。
 *
 * dissemble命令可以打印出错时的汇编代码片段，其中箭头指向的是错误行。
 * 可以看到调用了div指令做除法操作，被除数是-0x8(%ebp)，指当前栈基
 * 址向下偏移8个字节所在内存单元的数值，EBP是栈基址寄存器。同时
 * 可以看到前面通过movl $0x0, -0x8(%ebp)将0保存到该内存单元，
 * 证明被除数为0。
 *
 **/
