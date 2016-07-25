
#if 1
int b = 100;
#else
static int b = 100; //防止外部文件引用本文件的全局变量
/*
   加上static 编译报错信息:
   3_04_extern-part1.c:(.text+0x11): undefined reference to `b'
   */
#endif


