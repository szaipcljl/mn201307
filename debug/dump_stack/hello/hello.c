#include <linux/module.h>
#include <linux/init.h>

#define USE_DUMP_STACK

#ifdef USE_DUMP_STACK
#include <linux/kprobes.h>
#include <asm/traps.h>
#endif

#define mnprint(fmt, ...) printk(fmt " DEBUG: Passed %s %d\n", ##__VA_ARGS__, __func__, __LINE__)

MODULE_LICENSE("Dual BSD/GPL");

static int __init hello_init(void)
{
#ifdef USE_DUMP_STACK
	//printk(KERN_ALERT "##mn: passed %s %d \n", __FUNCTION__, __LINE__);
	printk(KERN_ALERT "###[%s %d] [dump_stack start]\n", __func__, __LINE__);
	dump_stack();
	printk(KERN_ALERT "###[%s %d] [dump_stack end]\n", __func__, __LINE__);
#endif

	return 0;
}

static void __exit hello_exit(void)
{
	//printk(KERN_ALERT "### test module exit...\n");
	mnprint(KERN_ALERT "### test module exit...");
}

module_init(hello_init);
module_exit(hello_exit);
