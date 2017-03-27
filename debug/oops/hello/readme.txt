
(1) 由BUG: unable to handle kernel NULL pointer dereference at           (null)

知道出错的原因是使用了空指针。

(2) IP: 标红的部分确定了具体出错的函数。

(3) Modules linked in: helloworld表明了引起oops问题的具体模块。

(4) call trace列出了函数的调用信息。

由oops信息我们知道出错的地方是hello_init的地址偏移0xd。而有dump信息
知道，hello_init的地址即init_module的地址，因为hello_init即本模块的
初始化入口，如果在其他函数中出错，dump信息中就会有相应符号的地址。
由此我们得到出错的地址是0xd，
