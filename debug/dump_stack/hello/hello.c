#include <linux/module.h>
#include <linux/init.h>

#define USE_DUMP_STACK

#ifdef USE_DUMP_STACK
#include <linux/kprobes.h>
#include <asm/traps.h>
#endif

MODULE_LICENSE("Dual BSD/GPL");

static int __init hello_init(void)
{
#ifdef USE_DUMP_STACK
	printk(KERN_ALERT "### dump_stack start\n");
	dump_stack();
	printk(KERN_ALERT "### dump_stack over\n");
#endif

	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "### test module\n");
}

module_init(hello_init);
module_exit(hello_exit);
