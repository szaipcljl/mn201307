Linux core dump详解
2010-08-24

有的程序可以通过编译, 但在运行时会出现Segment fault(段错误).
这通常都是指针错误引起的.以下是我们详细的对Linux core dump的调试
技术进行的介绍:

1.Linux core dump 前言:
	有的程序可以通过编译, 但在运行时会出现Segment fault(段错误).
	这通常都是指针错误引起的.但这不像编译错误一样会提示到文件->行,
	而是没有任何信息, 使得我们的调试变得困难起来.

2. Linux core dump gdb:
	有一种办法是, 我们用gdb的step, 一步一步寻找.
	这放在短小的代码中是可行的, 但要让你step一个上万行的代码,
	我想你会从此厌恶程序员这个名字, 而把他叫做调试员.
	我们还有更好的办法, 这就是core file.

3. Linux core dump ulimit:
	如果想让系统在信号中断造成的错误时产生core文件, 需要在shell中
按如下设置:

	#设置core大小为无限
		ulimit -c unlimited
	#设置文件大小为无限
		ulimit unlimited
	这些需要有root权限, 在ubuntu下每次重新打开中断都需要重新输入
上面的第一条命令, 来设置core大小为无限.

4. Linux core dump用gdb查看core文件:
	下面我们可以在发生运行时信号引起的错误时发生core dump了.
	发生core dump之后, 用gdb进行查看core文件的内容, 以定位文件中引
发core dump的行.
		gdb [exec file] [core file]

	如:
		gdb ./test test.core
	在进入gdb后, 用bt命令查看backtrace以检查发生程序运行到哪里, 来
定位core dump的文件->行.

5.要怎麽才不会让 core 文件出现？
	如果用的是tcsh的话, 以试著在 .tcshrc 里加一行：
		limit coredumpsize 0
	如果用的是bash的话, 在/etc/profile里加上(或者修改)一条:
		ulimit -c 0

6.何谓 Linux core dump？
	我们在开发（或使用）一个程序时，最怕的就是程序莫明其妙地当掉。
虽然系统没事，但我们下次仍可能遇到相同的问题。于是这时操作系统就会
把程序当掉时的内存内容 dump 出来，让我们或是debugger 做为参考。这个动作就叫作 core dump。

7.有一招, 可以让你看出 core 最好用的地方
	gdb -c core
	进去后打 where, 可以 show 出在程序哪一行当掉的, 当掉时在哪个
	function 里, 这个 function 是被哪个function 所 call 的, 而这个
	function 又是被哪个function所 call 的.... 一直到 main(), 由这个
	信息, 可以找出五六成的 bug........ 屡试不爽,
	但先决条件, 在 compile 时必须把 debug information 选项打开.
	不然, 会出现一大堆看不懂的东西，而不是源程序。

8.System Dump和Core Dump的区别?
	许多没有做过UNIX系统级软件开发的人士，可能只听说过Dump，而并不
	知道系统Dump和Core Dump的区别，甚至混为一谈。

	1) 系统Dump（System Dump）
	所有开放式操作系统，都存在系统DUMP问题。
	产生原因：
	由于系统关键/核心进程，产生严重的无法恢复的错误，为了避免系统相
	关资源受到更大损害，操作系统都会强行停止运行，并将当前内存中的
	各种结构,核心进程出错位置及其代码状态，保存下来，以便以后分析。

	最常见的原因是指令走飞，或者缓冲区溢出，或者内存访问越界。
	走飞就是说代码流有问题，导致执行到某一步指令混乱，跳转到一些不
	属于它的指令位置去执行一些莫名其妙的东西（没人知道那些地方本来
	是代码还是数据，而且是不是正确的代码开始位置），或者调用到不属
	于此进程的内存空间。写过C程序及汇编程序的人士，对这些现象应当是
	很清楚的。

	系统DUMP生成过程的特点：
	在生成DUMP过程中，为了避免过多的操作结构，导致问题所在位置正好
	也在生成DUMP过程所涉及的资源中，造成DUMP不能正常生成，操作系
	统都用尽量简单的代码来完成，所以避开了一切复杂的管理结构，如文
	件系统）LVM等等，所以这就是为什么几乎所有开放系统，都要求DUMP设
	备空间是物理连续的——不用定位一个个数据块，从DUMP设备开头一直写
	直到完成，这个过程可以只用BIOS级别的操作就可以。这也是为什么在
	企业级UNIX普遍使用LVM的现状下，DUMP设备只可能是裸设备而不可能是
	文件系统文件，而且只用作DUMP的设备，做LVM镜像是无用的——
	系统此时根本没有LVM操作，它不会管什么镜像不镜像，就用第一份连续
	写下去。

	所以UNIX系统也不例外，它会将DUMP写到一个裸设或磁带设备。在重启
	的时候，如果设置的DUMP转存目录（文件系统中的目录）有足够空间，
	它将会转存成一个文件系统文件，缺省情况下，对于AIX来说是
	/var/adm/ras/下的vmcore*这样的文件，对于HPUX来说是
	/var/adm/crash下的目录及文件。
	当然，也可以选择将其转存到磁带设备。

	会造成系统DUMP的原因主要是：
	系统补丁级别不一致或缺少）系统内核扩展有BUG（例如Oracle就会安装
	系统内核扩展））驱动程序有 BUG（因为设备驱动程序一般是工作在内
	核级别的），等等。所以一旦经常发生类似的系统DUMP，可以考虑将系
	统补丁包打到最新并一致化）升级微码）升级设备驱动程序（包括FC
	多路冗余软件））升级安装了内核扩展的软件的补丁包等等。

	2) 进程Core Dump
	进程Core
	Dump产生的技术原因，基本等同于系统DUMP，就是说从程序原理上来说
	是基本一致的。
	但进程是运行在低一级的优先级上（此优先级不同于系统中对进程定义
	的优先级，而是指CPU代码指令的优先级），被操作系统所控制，所以操
	作系统可以在一个进程出问题时，不影响其他进程的情况下，中止此进
	程的运行，并将相关环境保存下来，这就是core dump文件，可供分析。

	如果进程是用高级语言编写并编译的，且用户有源程序，那么可以通过
	在编译时带上诊断用符号表（所有高级语言编译程序都有这种功能），
	通过系统提供的分析工具，加上core文件，能够分析到哪一个源程序语
	句造成的问题，进而比较容易地修正问题，当然，要做到这样，除非一
	开始就带上了符号表进行编译，否则只能重新编译程序，并重新运行程
	序，重现错误，才能显示出源程序出错位置。

	如果用户没有源程序，那么只能分析到汇编指令的级别，难于查找问题
	所在并作出修正，所以这种情况下就不必多费心了，找到出问题的地方
	也没有办法。

	进程Core Dump的时候，操作系统会将进程异常终止掉并释放其占用的资
	源，不可能对系统本身的运行造成危害。这是与系统DUMP根本区别的一
	点，系统DUMP产生时，一定伴随着系统崩溃和停机，进程Core Dump时，
	只会造成相应的进程被终止，系统本身不可能崩溃。当然如果此进程与
	其他进程有关联，其他进程也会受到影响，至于后果是什么，就看相关
	进程对这种异常情况（与自己相关的进程突然终止）的处理机制是什么
	了，没有一概的定论。

9.关于coredump文件的其它说明：
	1）如何生成coredump文件?
	登陆LINUX服务器，任意位置键入
		echo "ulimit -c 1024" >> /etc/profile
	退出LINUX重新登陆LINUX, 键入
		ulimit -c
	如果显示 1024 那么说明coredump已经被开启。

	2）core文件的简单介绍
	在一个程序崩溃时，它一般会在指定目录下生成一个core文件。core文
	件仅仅是一个内存映象(同时加上调试信息)，主要是用来调试的。

	3) 开启或关闭core文件的生成
	阻止系统生成core文件:
		ulimit -c 0
	检查生成core文件的选项是否打开:
		ulimit -a
	该命令将显示所有的用户定制，其中选项-a代表“all”。

	也可以修改系统文件来调整core选项
	在/etc/profile通常会有这样一句话来禁止产生core文件，通常这种设 
	置是合理的:
	# No core files by default
		ulimit -S -c 0 > /dev/null 2>&1

	但是在开发过程中有时为了调试问题，还是需要在特定的用户环境下打
	开core文件产生的设置。
	在用户的~/.bash_profile里加上ulimit -c unlimited来让特定的用户
	可以产生core文件。

	如果ulimit -c 0 则也是禁止产生core文件，而ulimit -c 1024则限制
	产生的core文件的大小不能超过1024kb

	4）设置Core Dump的核心转储文件目录和命名规则
	/proc/sys/kernel/core_uses_pid
	可以控制产生的core文件的文件名中是否添加pid作为扩展，如果添加则
	文件内容为1，否则为0

	proc/sys/kernel/core_pattern
	可以设置格式化的core文件保存位置或文件名，比如原来文件内容是
	core-%e 可以这样修改:
	echo "/corefile/core-%e-%p-%t" > core_pattern
	将会控制所产生的core文件会存放到/corefile目录下，产生的文件名为
	core-命令名-pid-时间戳

	以下是参数列表:
	%p - insert pid into filename 添加pid
	%u - insert current uid into filename 添加当前uid
	%g - insert current gid into filename 添加当前gid
	%s - insert signal that caused the coredump into the filename 
		 添加导致产生core的信号
	%t - insert UNIX time that the coredump occurred into filename 
		 添加core文件生成时的unix时间
	%h - insert hostname where the coredump happened
	into filename 添加主机名
	%e - insert coredumping executable name into filename
	 	 添加命令名

	5）使用core文件
	在core文件所在目录下键入:
		gdb -c core
	它会启动GNU的调试器，来调试core文件，并且会显示生成此core文件的
	程序名，中止此程序的信号等等。
	如果你已经知道是由什么程序生成此core文件的，比如MyServer崩溃了
	生成core.12345，那么用此指令调试:
		gdb -c core MyServer

	6）一个小方法来测试产生core文件
		直接输入指令:
			kill -s SIGSEGV $$
